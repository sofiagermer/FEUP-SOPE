#include "fifos.h"

extern int publicFifoDesc; 
extern pthread_mutex_t lock; 
extern bool timeFlag;
extern bool serverFlag;

void createFifo(char* name) {

    if(mkfifo(name, 0666) == -1 && errno != EEXIST) {
        fprintf(stderr,"Failed to create FIFO: %s\n", strerror(errno));
        exit(1);
    }
}

void writeToPublicFifo(msg* message) {
    
    //Locks the mutex
    if (pthread_mutex_lock(&lock) != 0) {
        fprintf(stderr, "Failed to lock mutex: %s\n", strerror(errno));
        exit(1);
    } 

    //Writes to fifo
    if (write(publicFifoDesc,message,sizeof(msg)) < 0) {
        fprintf(stderr, "Failed to write to public fifo: %s\n", strerror(errno));
        exit(1);
    }

    //Logs
    regist(message->i,message->t,message->pid,message->tid,message->res,"IWANT"); 

    //Unlocks mutex
    if (pthread_mutex_unlock(&lock) != 0) {
        fprintf(stderr, "Failed to unlock mutex: %s\n", strerror(errno));
        exit(1);
    } 
}

void readFromPrivateFifo(msg* message,char *privateFifoName) {

    int result;
    int privateFifoDesc;

    //Open, read, close
    if ((privateFifoDesc = open (privateFifoName, O_RDONLY)) == -1) {
        fprintf(stderr, "Failed to open from private fifo: %s\n", strerror(errno));
        exit(1);
    }
    if ((result = read(privateFifoDesc,message,sizeof(msg))) == -1 && timeFlag) {
        fprintf(stderr, "Failed to read from private fifo: %s\n", strerror(errno));
        exit(1);
    }
    close(privateFifoDesc);

    //Logs
    if(!timeFlag && result != 0) {
        regist(message->i,message->t,message->pid,message->tid,message->res,"GAVUP");
    } else if(message->res == -1){
        regist(message->i, message->t, message->pid, message->tid, message->res,"CLOSD");
        serverFlag = false;
    } else if (message->res != -1) {
        regist(message->i,message->t,message->pid,message->tid,message->res,"GOTRS");
    } 
}

void forcePipesClosure(unsigned int identifier) {
    for (unsigned int i = 0; i < identifier; i++) 
        close(i + 3); //Close everything but 0, 1 and 2
}


