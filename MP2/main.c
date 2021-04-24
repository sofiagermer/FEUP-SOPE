
#include "parser.h"
#include <pthread.h>
#include <fcntl.h>

typedef struct{
    int i;
    int t;
    pid_t pid;
    pthread_t tid;
    int res;
} msg;

void writeToPublicFifo(char* name,msg* message){
    int np;
    //if(mkfifo(name,0666)<0)
        //perror("Error creating fifo");
    while ((np = open (name,O_WRONLY )) < 0);
    write(np,message,sizeof(*message));
    close(np);
}

void randomWait(){
    //dorme qlq coisa entre 0 e 100 milisegundos
    int time=(rand()%100000)+1000;
    usleep(time);
}

void *threadHandler(void *i){
    msg *message;
    message->i=(int)i;
    message->t=(rand()%10)+1;
    message->pid=getpid();
    message->tid=pthread_self();
    message->res=-1;

    //Creates private fifo name in format pid.tid
    char privateFifoName[200];
    snprintf(privateFifoName,200,"/tmp/%d.%ld",message->pid,message->tid);


}

int main(int argc, char const * argv[])
{
    info_t info;  //maybe there is no need for a struct and we can just leave fifoname and nsecs separated
    int i=1;
    int *threadIds=malloc(sizeof(int)*i);
    parse(&info, argc, argv);
    printf("name %s\n", info.fifoname);  //just debug
    printf("nsecs %i\n", info.nsecs);
    //nsecs is validated, fifoname isn't validated yet (pipe might not exist)
    while(1){
        
        if(pthread_create(threadIds[i-1],NULL,threadHandler,i)){

        }
        
        i++;
        randomWait();
    }
    return 0;
}
