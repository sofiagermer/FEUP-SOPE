#pragma once

#include <sys/stat.h>
#include <fcntl.h>

#include "utils.h"
#include "linkedlist.h"
#include "library/lib.h"



void* consumerHandler(void* a);
void* threadHandler(void* a);
void createThreads();