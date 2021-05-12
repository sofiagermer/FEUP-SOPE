#pragma once

#include <errno.h>
#include <stdio.h>

void createFifo(char* name);
void readPublicFifo(char *name);