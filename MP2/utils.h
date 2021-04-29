#pragma once

#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include "errno.h"
#include <stdlib.h>
#include <unistd.h>

typedef struct{
    int i;
    pid_t pid;
    pthread_t tid;
    int t;
    int res;
} msg;

void regist(int i,int t, pid_t pid, pthread_t tid, int res,char* oper); //Log files
void randomWait(unsigned int i); //Avoid race conditions
void createMessageStruct(msg* message, int i); //Create struct for message