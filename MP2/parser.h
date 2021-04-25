#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

typedef struct {  
    char * fifoname;
    int nsecs; 
} info_t;

int is_positive_integer(const char * num);
void parse(info_t * info, int argc, char const * argv[]);