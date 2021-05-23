#include "utils.h"

extern int errno;

void regist(int i,int t, pid_t pid, pthread_t tid, int res,char* oper) {
    printf("%ld ; %d ; %d ; %d ; %ld ; %d ; %s\n" ,time(NULL),i,t,pid,tid,res,oper);
}

