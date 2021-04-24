

#include <fcntl.h>
#include <stdbool.h>
#include <time.h>
#include <pthread.h>


#include "parser.h"

info_t info;

void regist(int i, pid_t pid, pthread_t tid, int res,char* oper){
    printf("%ld ; %d ; %d ; %ld ; %d ; %s" ,time(NULL),i,pid,tid,res,oper);
}

typedef struct{
    int i;
    int t;
    pid_t pid;
    pthread_t tid;
    int res;
} msg;

int createFifo(){
    if(mkfifo(info.fifoname, 0666) == -1){
        if(errno != EEXIST){
            perror("Creating FIFO: ");
            return 1;
        }
        else{
           return 0;
        }
    }
    return 0;
}

int writeToPublicFifo(msg* message){
    int np;
    if(createFifo() != 0) return 1;
    while ((np = open (info.fifoname,O_WRONLY )) < 0);
    write(np,message,sizeof(*message));
    regist(message->i,message->pid,message->tid,message->res,"IWANT");
    close(np);
    return 0;
}

void randomWait(){
    //dorme qlq coisa entre 0 e 100 milisegundos
    int time=(rand()%100000)+1000;
    usleep(time);
}

void *threadHandler(void *i){
    msg message; 
    message.i =*(int *) i;
    message.t=(rand()%10)+1;
    message.pid=getpid();
    message.tid=pthread_self();
    message.res=-1;

    int privateFifo;

    //Creates private fifo name in format pid.tid
    char privateFifoName[200];
    snprintf(privateFifoName,200,"/tmp/%d.%ld",message.pid,message.tid);
    if(mkfifo(privateFifoName, 0666) == -1){
        if(errno != EEXIST){
            perror("Creating FIFO: ");
            //return 1;
        }
        
    }
    while ((privateFifo = open (privateFifoName, O_RDONLY)) < 0);
    read(privateFifo,&message,sizeof(message));

    if(message.res==-1){
        regist(message.i,message.pid,message.tid,message.res,"CLOSD");
    }
    else if(message.res!=-1) regist(message.i,message.pid,message.tid,message.res,"GOTRS");
    else regist(message.i,message.pid,message.tid,message.res,"FAILD");

    close(privateFifo);
    pthread_exit(NULL);
}

int main(int argc, char const * argv[])
{
    int msec = 0, trigger = 3000; /* 10ms */
    clock_t before = clock();
    int i=1;
    pthread_t *ids=(pthread_t*)malloc(1*sizeof(pthread_t));;
    parse(&info, argc, argv);
    printf("name %s\n", info.fifoname);  //just debug
    printf("nsecs %i\n", info.nsecs);
    //nsecs is validated, fifoname isn't validated yet (pipe might not exist)
    while(msec < trigger){
        clock_t difference = clock() - before;
        msec = difference * 1000 / CLOCKS_PER_SEC;
        
        if(pthread_create(&ids[i-1],NULL,threadHandler,&i)){

        }

        i++;
        ids = (pthread_t*)realloc(ids, i*sizeof(pthread_t));
        //randomWait();
    }
    printf("%d\n",getpid());
    for(int j=0; j<i; j++) {
        pthread_join(ids[j], NULL);
    }
    return 0;
}
