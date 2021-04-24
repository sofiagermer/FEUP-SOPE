#include <fcntl.h>
#include <stdbool.h>
#include <time.h>
#include <pthread.h>
#include "parser.h"


info_t info;
pthread_mutex_t lock;

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
    if(mkfifo(info.fifoname, 0666) == -1 && errno != EEXIST){
        perror("Failed to create FIFO");
        return 1;
    }
    return 0;
}

int writeToPublicFifo(msg* message) {
    pthread_mutex_lock(&lock);
    int np;
    if(createFifo() != 0) return 1;
    while ((np = open (info.fifoname,O_WRONLY )) < 0);
    write(np,message,sizeof(*message));
    regist(message->i,message->pid,message->tid,message->res,"IWANT");
    close(np);
    pthread_mutex_unlock(&lock);
    return 0;
}

void randomWait(){
    //dorme qlq coisa entre 0 e 100 milisegundos
    int time = (rand() % 100000);
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
        perror("Failed to create FIFO");
        exit(1);
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

int main(int argc, char const * argv[]) {
    if (pthread_mutex_init(&lock, NULL) != 0) {
        printf("\n mutex init has failed\n");
        return 1;
    }

    time_t start,end;
    time(&start);
    double sec,trigger = 3;

    int i = 1;
    pthread_t *ids = (pthread_t*)malloc(1 * sizeof(pthread_t));
    
    parse(&info, argc, argv);


    while(sec < trigger){
        time(&end);sec = end-start;
        
        if(pthread_create(&ids[i-1],NULL,threadHandler,&i)){
            fprintf(stderr, "Failed to create thread: %s\n", strerror(errno));
            return 1;
        }

        i++;
        ids = (pthread_t*)realloc(ids, i*sizeof(pthread_t));
        printf("i:%d mils:%f\n", i, sec);
        randomWait();
    }

    for(int j=0; j<i; j++) {
        pthread_join(ids[j], NULL);
    }
    pthread_mutex_destroy(&lock);

    return 0;
}
