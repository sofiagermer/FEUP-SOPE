#include "options.h"


void processOption(const char optFlag, Options* options) {
    switch (optFlag) {
        case 'R': {
            options->recursive = true;
            break;
        }
        case 'c': {
            if (options->vflag == V) break; //If v is present, c is irrelevant
            options->vflag = C;
            break;
        }
        case 'v': {
            options->vflag = V;
            break;
        }
    } 
}