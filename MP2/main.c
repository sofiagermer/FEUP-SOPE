
#include "parser.h"
#include <fcntl.h>

typedef struct{
    int i;
    int t;
    pid_t pid;
    pthread_t tid;
    int res;
} msg;

void writeToFifo(char* name,char* msg){
    int np;
    if(mkfifo(name,0666)<0)
        perror("Error creating fifo");
    while((np=open(name,O_WRONLY))<0){
        
    }
    write(np,msg,strlen(msg)+1);
    
}

void randomWait(){
    //dorme qlq coisa entre 0 e 100 milisegundos
    int time=(rand()%100000)+1000;
    usleep(time);
}
void *createRequest(void *message){

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
        
        if(pthread_create(threadIds[i-1],NULL,createRequest,i)){

        }
        
        i++;
    }
    return 0;
}
