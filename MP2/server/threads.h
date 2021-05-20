#pragma once

#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>

#include "utils.h"
#include "linkedlist.h"
#include "library/lib.h"
#include "./library/lib.h"


void* consumerHandler(void* a);
void* producerHandler(void* a);
void createThreads();