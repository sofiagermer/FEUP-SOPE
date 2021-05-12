#pragma once

#include "utils.h"
#include <fcntl.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/stat.h>


void createFifo(char* name); //Create fifos
int writeToPublicFifo(msg* message); //Senging messages
void readFromPrivateFifo(msg* message,char *privateFifoName); //Receiving response
void forcePipesClosure(unsigned int identifier); //Forces the pipes to close
void openPublicFifo(char* fifoName); //Open public FIFO