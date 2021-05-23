#pragma once


#include <stdio.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "utils.h"

void initRegister(); //Initiates the time counting process and checks if the variable required exists, as well as if it is well defined
void regitExecution( pid_t pid, char* event, char* info); //Records something on the log file chosen
void registSignalSent(char* signalSent, int pid); //Records events of type SIGNAL_SENT
void registProcessExit(int exitStatus); //Records events of type PROC_EXIT
void registProcessCreation(char* args[]); //Records events of type PROC_CREAT
void registFileModf(mode_t oldMode, mode_t newMode, char* filePath); //Records events of type FILE_MODF

