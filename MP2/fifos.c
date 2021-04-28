#include "fifos.h"

int publicFifoDesc; 
extern bool timeFlag;
extern bool serverFlag;

void createFifo(char* name) {

    if(mkfifo(name, 0666) == -1 && errno != EEXIST) {
        fprintf(stderr,"Failed to create FIFO: %s\n", strerror(errno));
        exit(1);
    }
}

void writeToPublicFifo(msg* message) {

    //Writes to fifo
    if (write(publicFifoDesc,message,sizeof(msg)) < 0) {
        fprintf(stderr, "Failed to write to public fifo: %s\n", strerror(errno));
        exit(1);
    }

    //Logs
    regist(message->i,message->t,message->pid,message->tid,message->res,"IWANT"); 

}

void readFromPrivateFifo(msg* message,char *privateFifoName) {

    int result;
    int privateFifoDesc;

    //Open, read, close
    if ((privateFifoDesc = open (privateFifoName, O_RDONLY)) == -1) {
        if (timeFlag) {
            fprintf(stderr, "Failed to open from private fifo: %s\n", strerror(errno));
            exit(1);
        } else {
            regist(message->i,message->t,message->pid,message->tid,message->res,"GAVUP");
            return;
        }
    }
    if ((result = read(privateFifoDesc,message,sizeof(msg))) == -1 && timeFlag) {
        if (timeFlag) {
            fprintf(stderr, "Failed to read from private fifo: %s\n", strerror(errno));
            exit(1);
        } else {
            regist(message->i,message->t,message->pid,message->tid,message->res,"GAVUP");
            return;
        }
    }
    close(privateFifoDesc);

    //Logs
    if(message->res == -1){
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


