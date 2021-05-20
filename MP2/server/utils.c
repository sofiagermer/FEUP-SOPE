#include "utils.h"

extern int errno;

void regist(int i,int t, pid_t pid, pthread_t tid, int res,char* oper) {
    printf("%ld ; %d ; %d ; %d ; %ld ; %d ; %s\n" ,time(NULL),i,t,pid,tid,res,oper);
}

void randomWait(unsigned int i) {
    //Sleeps between 50 and 100 milliseconds
    int time = (rand_r(&i) % 50000) + 50000;
    usleep(time);
}
