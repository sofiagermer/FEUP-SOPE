#pragma once


#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include "logfile.h"
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include "utils.h"

extern int errno;
extern ProcessInfo* pInfo;

void sigHandlerSigInt(int signo);
void setUpSigInt ();
void setUpSigHandlers ();
void sigHandlerSigIntInitial(int signo);
void sigHandlerSigTerm(int signo);
void sigHandlerSigAlrm(int signo);
void setUpSigAlrm();
void setUpSigTerm();
void setUpOtherSigs();
