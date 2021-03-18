#pragma once


#include <stdio.h>

void initRegister ();
void regitExecution( pid_t pid, char* event, char* info);
void registSignalSent(char* signalSent, int pid);
void registProcessExit(int exitStatus);
void registProcessCreation(char* args[]);
void registFileModf(mode_t oldMode, mode_t newMode, char* filePath);
double getMiliSeconds(double initialTime);

