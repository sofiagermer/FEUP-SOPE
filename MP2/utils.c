#include "utils.h"


void regist(int i, pid_t pid, pthread_t tid, int res,char* oper) {
    printf("%ld ; %d ; %d ; %ld ; %d ; %s" ,time(NULL),i,pid,tid,res,oper);
}

void randomWait(unsigned int i) {
    //Sleeps between 50 and 100 milliseconds
    int time = (rand_r(&i) % 50000) + 50000;
    usleep(time);
}

void createMessageStruct(msg* message, int i) {
    message->i = i;
    unsigned seed = message->i;
    message->t = (rand_r(&seed) % 10) + 1;
    message->pid = getpid();
    message->tid = pthread_self();
    message->res = -1;
}