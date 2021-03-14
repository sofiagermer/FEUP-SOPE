#ifndef OPTIONS_H
#define OPTIONS_H

#include <stdbool.h>

typedef enum {C, V, NONE} Verbose;

typedef struct {
    Verbose vflag;
    bool recursive;
} Options;

void processOption(const char optFlag, Options* options); //Processes an option

#endif //OPTIONS_H