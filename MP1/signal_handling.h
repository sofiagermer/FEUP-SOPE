#pragma once


#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#include "logfile.h"
#include "utils.h"

extern int errno;
extern ProcessInfo* pInfo;

void sigHandlerSigInt(int signo);
void sigHandlerSigIntInitial(int signo);
void sigHandlerSigTerm(int signo);
void sigHandlerSigAlrm(int signo);
void sigHandlerSigUsr1(int signo);
void sigHandlerSigHuv(int signo);
void sigHandlerSigPipe(int signo);
void sigHandlerSigIO(int signo);
void sigHandlerSigSegv(int signo);
void sigHandlerSigTermInit(int signo);
void sigHandlerSigAlrmInit(int signo);
void setUpSigAlrm();
void setUpSigTerm();
void setUpSigIO();
void setUpSigPipe();
void setUpSigUsr1();
void setUpSigUsr2();
void setUpSigSegv();
void setUpSigHup();
void setUpSigInt ();
void setUpSigHandlers ();
