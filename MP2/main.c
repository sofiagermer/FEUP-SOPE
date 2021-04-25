#include <fcntl.h>
#include <stdbool.h>
#include <time.h>
#include "utils.h"
#include "parser.h"

info_t info;
pthread_mutex_t lock;
int publicnp;
bool serverFlag; //Server closed
bool timeFlag; //Time is up

void createFifo(char* name); //Create fifos
void writeToPublicFifo(msg* message); //Senging messages
void readFromPrivateFifo(msg* message,char *privateFifoName); //Receiving response
void *threadHandler(void *i); //Handler for each request thread
void createRequests(); //Create requests and threads for each

void createFifo(char* name) {

    if(mkfifo(name, 0666) == -1 && errno != EEXIST) {
        fprintf(stderr,"Failed to create FIFO: %s\n", strerror(errno));
        exit(1);
    }
}

void writeToPublicFifo(msg* message) {
    //Locks the mutex
    pthread_mutex_lock(&lock); 

    //Writes to fifo
    write(publicnp,message,sizeof(msg));

    //Logs
    regist(message->i,message->t,message->pid,message->tid,message->res,"IWANT"); 

    //Unlocks mutex
    pthread_mutex_unlock(&lock);
}

void readFromPrivateFifo(msg* message,char *privateFifoName) {

    int privateFifo = open (privateFifoName, O_RDONLY);

    int result = read(privateFifo,message,sizeof(msg));
    close(privateFifo);

    if(!timeFlag && result != 0) regist(message->i,message->t,message->pid,message->tid,message->res,"GAVUP");

    //Logs
    if(message->res == -1){
        regist(message->i, message->t, message->pid, message->tid, message->res, "CLOSD");
        serverFlag = false;
    } else if (message->res != -1) {
        regist(message->i,message->t,message->pid,message->tid,message->res,"GOTRS");
    } 
}

void *threadHandler(void *i) {

    //Message struct
    msg message;
    createMessageStruct(&message, *(int*) i);

    //Creates private fifo name in format pid.tid
    char privateFifoName[200];
    snprintf(privateFifoName,sizeof(privateFifoName),"/tmp/%d.%ld",message.pid,message.tid);

    //Create, Open and Read Fifo
    createFifo(privateFifoName);

    //Sends the message
    writeToPublicFifo(&message);

    //Receives message
    readFromPrivateFifo(&message,privateFifoName);
    
    pthread_exit(NULL);
}

void forcePipesClosure(int identifier, pthread_t* ids) {

    //TODO
}

void createRequests() {

    //For the time
    time_t start,end;
    time(&start);
    double sec;

    int identifier = 0;
    pthread_t *ids = (pthread_t*)malloc(1 * sizeof(pthread_t));
    serverFlag = true;

    while(sec < info.nsecs && serverFlag) {

        time(&end);
        sec = end - start;

        identifier++;
        ids = (pthread_t*)realloc(ids, identifier * sizeof(pthread_t));

        //Create thread for requests
        if(pthread_create(&ids[identifier - 1],NULL,threadHandler,&identifier)) { 
            fprintf(stderr, "Failed to create thread: %s\n", strerror(errno));
            exit(1);
        }

        //To avoid race conditions
        randomWait(identifier);
    }

    //Close Fifos
    timeFlag = false;

    //Wait for all threads to finish
    for(int j = 0; j < identifier; j++) {
        if (pthread_join(ids[j], NULL) != 0) {
            fprintf(stderr, "Error in pthread_join: %s\n", strerror(errno));
        }
    }
    free(ids);
}

int main(int argc, char* argv[]) {
    //Parse arguments
    parse(&info, argc, argv);
    
    //Create Fifo(info.fifoname);
    publicnp = open (info.fifoname,O_WRONLY );
    
    //Create mutex
    if (pthread_mutex_init(&lock, NULL) != 0) {
        fprintf(stderr,"Mutex init has failed: %s\n", strerror(errno));
        return 1;
    }

    //Create Requests and Threads and wait for answer
    createRequests();

    //Destroy mutex
    if (pthread_mutex_destroy(&lock) != 0) {
        fprintf(stderr,"Mutex init has failed: %s\n",strerror(errno));
        return 1;
    }

    return 0;
}
