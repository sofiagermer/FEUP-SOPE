#include <time.h>
#include "fifos.h"
#include "parser.h"

//GLOBAL VARS
info_t info; //Info from args
pthread_mutex_t lock; //Mutex to enter public fifo
bool serverFlag; //Server closed
bool timeFlag; //Time is up
extern int publicFifoDesc; //Descriptor of the public named pipe

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
    pthread_attr_t threadsAttr;
    pthread_attr_init(&threadsAttr);
    pthread_attr_setdetachstate(&threadsAttr, PTHREAD_CREATE_DETACHED); //No need to wait for other threads

    serverFlag = true;
    timeFlag = true;

    //Cycle to create threads
    while(sec < info.nsecs && serverFlag) {
        time(&end);
        sec = end - start;

        identifier++;
        pthread_t threadID;

        //Create thread for requests
        if (pthread_create(&threadID,&threadsAttr,threadHandler,&identifier)) { 
            fprintf(stderr, "Failed to create thread: %s\n", strerror(errno));
            exit(1);
        }

        //To avoid race conditions
        randomWait(identifier);
    }

    pthread_attr_destroy(&threadsAttr);

    //Close Fifos
    timeFlag = false;
    forcePipesClosure(identifier);
}

int main(int argc, const char* argv[]) {

    //Parse arguments
    parse(&info, argc, argv);

    //Open Fifo(info.fifoname);
    publicFifoDesc = open(info.fifoname,O_WRONLY );

    //Create Requests and Threads and wait for answer
    handleRequests();

    //Close Public Fifo
    close(publicFifoDesc);
    free(info.fifoname);

    pthread_exit(NULL);
}
