#pragma once

#include <stdlib.h>
#include "utils.h"
#include <stdbool.h>

int back;
int maxSize;
msg **queue;

void initializeBuffer(int size);
bool addToBuffer(msg* message);
bool bufferEmpty();
bool bufferFull();
msg* removeFromBuffer();