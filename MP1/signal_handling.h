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

void sigHandlerSigInt(int signo); //Handler for SIGINT
void sigHandlerSigIntInitial(int signo); //Handler for SIGINT for initialprocess
void sigHandlerSigTerm(int signo); //Handler for SIGTERM
void sigHandlerSigAlrm(int signo); //Handler for SIGALRM
void sigHandlerSigUsr1(int signo); //Handler for SIGUSR1
void sigHandlerSigHuv(int signo); //Handler for SIGHUV
void sigHandlerSigPipe(int signo); //Handler for SIGPIPE
void sigHandlerSigIO(int signo); //Handler for SIGIO
void setUpSigAlrm(); //Sets up the handler
void setUpSigTerm(); //Sets up the handler
void setUpSigIO(); //Sets up the handler
void setUpSigPipe(); //Sets up the handler
void setUpSigUsr1(); //Sets up the handler
void setUpSigUsr2();; //Sets up the handler
void setUpSigHup(); //Sets up the handler
void setUpSigInt (); //Sets up the handler
void setUpSigHandlers (); //Calls all the setuppers
