#pragma once


#include <stdio.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "utils.h"

void initRegister ();
void regitExecution( pid_t pid, char* event, char* info);
void registSignalSent(char* signalSent, int pid);
void registProcessExit(int exitStatus);
void registProcessCreation(char* args[]);
void registFileModf(mode_t oldMode, mode_t newMode, char* filePath);
double getMiliSeconds(double initialTime);

