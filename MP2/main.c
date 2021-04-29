#include <time.h>
#include "fifos.h"
#include "parser.h"
#include "linkedlist.h"

//GLOBAL VARS
info_t info; //Info from args
pthread_mutex_t lock; //Mutex to enter public fifo
bool serverFlag; //Server closed
bool timeFlag; //Time is up
extern int publicFifoDesc; //Descriptor of the public named pipe
extern struct node * head;
extern struct node * tail;


//FUNCS
void *threadHandler(void *i); //Handler for each request thread
void handleRequests(); //Create requests and threads for each



void *threadHandler(void *i) {

    //Message struct
    msg* message = (msg*) malloc(sizeof(msg));
    if(message == NULL) return NULL;
    createMessageStruct(message, *(int*) i);

    //Creates private fifo's name in format pid.tid
    char privateFifoName[200];
    snprintf(privateFifoName, sizeof(privateFifoName), "/tmp/%d.%ld", message->pid, message->tid);

    //Create Fifo
    createFifo(privateFifoName);

    //Sends the message
    writeToPublicFifo(message);

    //Receives message
    readFromPrivateFifo(message,privateFifoName);

    free(message);
    pthread_exit(NULL);
}

void handleRequests() {

    //For the time
    time_t start,end;
    time(&start);
    double sec = 0;

    //Threads and stoping conds
    unsigned int identifier = 0;
    pthread_t id;
    serverFlag = true;
    timeFlag = true;

    //Cycle to create threads
    while(sec < info.nsecs && serverFlag) {
        time(&end);
        sec = end - start;

        identifier++;

        //Create thread for requests
        if (pthread_create(&id,NULL,threadHandler,&identifier)) { 
            fprintf(stderr, "Failed to create thread: %s\n", strerror(errno));
            exit(1);
        }
        push(id);
        //To avoid race conditions
        randomWait(identifier);
    }

    //Close Fifos
    timeFlag = false;
    forcePipesClosure(identifier);

    //Wait for all threads to finish
    while(!isEmpty()){
        if (pthread_join(pop(), NULL) != 0) {
            fprintf(stderr, "Error in pthread_join: %s\n", strerror(errno));
        }
    }
}

int main(int argc, const char* argv[]) {

    //Parse arguments
    parse(&info, argc, argv);

    //Create Fifo(info.fifoname);
    //createFifo(info.fifoname); WE ASSUMED SERVER SECURES THE CREATION OF PUBLIC FIFO
    publicFifoDesc = open(info.fifoname,O_WRONLY );
    
    //Create mutex
    if (pthread_mutex_init(&lock, NULL) != 0) {
        fprintf(stderr,"Mutex init has failed: %s\n", strerror(errno));
        return 1;
    }

    //Create Requests and Threads and wait for answer
    handleRequests();

    //Destroy mutex
    if (pthread_mutex_destroy(&lock) != 0) {
        fprintf(stderr,"Mutex init has failed: %s\n",strerror(errno));
        return 1;
    }

    close(publicFifoDesc);
    free(info.fifoname);
    return 0;
}
