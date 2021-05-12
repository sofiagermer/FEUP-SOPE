#include <time.h>
#include "fifos.h"
#include "parser.h"
#include "linkedlist.h"

//GLOBAL VARS
info_t info; //Info from args
bool serverFlag; //Server closed
bool timeFlag; //Time is up
extern int publicFifoDesc; //Descriptor of the public named pipe
extern struct node * head; //Linked list head
extern struct node * tail; //Linked list tail

//FUNCS
void *threadHandler(void *i); //Handler for each request thread
void handleRequests(); //Create requests and threads for each

void *threadHandler(void *i) {

    //Message struct
    msg* message = (msg*) malloc(sizeof(msg));
    if(message==NULL){
        fprintf(stderr,"Failed to allocate memory for message struct\n");
        pthread_exit(NULL);
    }
    createMessageStruct(message, *(int*) i);

    //Creates private fifo's name in format pid.tid
    char privateFifoName[200];
    snprintf(privateFifoName, sizeof(privateFifoName), "/tmp/%d.%ld", message->pid, message->tid);

    //Create Fifo
    createFifo(privateFifoName);

    //Sends the message
    if (writeToPublicFifo(message) != 0) { //In case it does not write, it shall not read
        free(message);
        pthread_exit(NULL);
    }

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
        //Adds id to a linked list
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

    //Open Fifo(info.fifoname);
    openPublicFifo(info.fifoname);
    
    //Create Requests and Threads and wait for answer
    handleRequests();

    //Close fifo
    close(publicFifoDesc);
    free(info.fifoname);
    return 0;
}
