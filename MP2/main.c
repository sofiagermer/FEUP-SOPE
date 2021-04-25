#include <fcntl.h>
#include <stdbool.h>
#include <time.h>
#include "utils.h"
#include "parser.h"

info_t info;
pthread_mutex_t lock;

void createFifo(char* name); //Create fifos
void writeToPublicFifo(msg* message); //Senging messages
void readFromPrivateFifo(msg* message,char *privateFifoName); //Receiving response
void *threadHandler(void *i); //Handler for each request thread
void createRequests(); //Create requests and threads for each

void createFifo(char* name) {

    if(mkfifo(name, 0666) == -1 && errno != EEXIST) {
        fprintf(stderr,"Failed to create FIFO: %s\n", strerror(errno));
    }
}

void writeToPublicFifo(msg* message) {

    //Locks the mutex
    pthread_mutex_lock(&lock); 

    //Creates fifo
    int np;
    createFifo(info.fifoname);

    //Writes to fifo
    while ((np = open (info.fifoname,O_WRONLY )) < 0);
    write(np,message,sizeof(*message));

    //Logs
    regist(message->i,message->t,message->pid,message->tid,message->res,"IWANT"); 

    //Closes fifo
    close(np);

    //Unlocks mutex
    pthread_mutex_unlock(&lock);
}

void readFromPrivateFifo(msg* message,char *privateFifoName) {

    
    int privateFifo;
    while ((privateFifo = open (privateFifoName, O_RDONLY)) < 0);
    //ver os tamanhos ta a dar erro fsr
    read(privateFifo,message,2000);
    close(privateFifo);

    //Logs
    if(message->res==-1){
        regist(message->i,message->t,message->pid,message->tid,message->res,"CLOSD");
    } else if (message->res!=-1) {
        regist(message->i,message->t,message->pid,message->tid,message->res,"GOTRS");
    } 
}

void *threadHandler(void *i) {

    //Message struct
    msg message;
    createMessageStruct(&message, *(int*) i);

    //Creates private fifo name in format pid.tid
    char privateFifoName[2000];
    snprintf(privateFifoName,sizeof(privateFifoName),"/tmp/%d.%ld",message.pid,message.tid);

    //Create, Open and Read Fifo
    createFifo(privateFifoName);

    //Sends the message
    writeToPublicFifo(&message);

    //Receives message
    readFromPrivateFifo(&message,privateFifoName);
    
    pthread_exit(NULL);
}

void createRequests() {

    //For the time
    time_t start,end;
    time(&start);
    double sec;
    int identifier = 1;

    pthread_t *ids = (pthread_t*)malloc(1 * sizeof(pthread_t));

    while(sec < info.nsecs) {

        time(&end);
        sec = end - start;

        //Create thread for requests
        if(pthread_create(&ids[identifier-1],NULL,threadHandler,&identifier)) { //Porque está identifier - 1?
            fprintf(stderr, "Failed to create thread: %s\n", strerror(errno));
            exit(1);
        }
        
        identifier++;
        ids = (pthread_t*)realloc(ids, identifier*sizeof(pthread_t));
        //To avoid race conditions
        randomWait(identifier);
    }

    //Wait for all threads to finish
    for(int j = 0; j < identifier; j++) {
        if (pthread_join(ids[j], NULL) != 0) {
            fprintf(stderr, "Error in pthread_join: %s\n", strerror(errno));
            exit(1);
        }
    }
    free(ids);
}

int main(int argc, char const * argv[]) {

    //Parse arguments
    parse(&info, argc, argv);

    //Unlock mutex
    if (pthread_mutex_init(&lock, NULL) != 0) {
        fprintf(stderr,"Mutex init has failed: %s\n", strerror(errno));
        return 1;
    }

    //Create Requests and Threads and wait for answer
    createRequests();

    //Lock mutex
    if (pthread_mutex_destroy(&lock) != 0) {
        fprintf(stderr,"Mutex init has failed: %s\n",strerror(errno));
        return 1;
    }

    return 0;
}
