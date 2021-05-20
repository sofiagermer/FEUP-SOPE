#pragma once

#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct{
    int i;
    pid_t pid;
    pthread_t tid;
    int t;
    int res;
} msg;

typedef struct {  
    char * fifoname;
    int nsecs; 
    int buffersize;
} info_t;

void regist(int i,int t, pid_t pid, pthread_t tid, int res,char* oper); //Log files