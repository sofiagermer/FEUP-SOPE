#include "fifos.h"


void createFifo(char* fifoName) {
    if(mkfifo(fifoName, 0666) == -1 && errno != EEXIST) {
        fprintf(stderr,"Failed to create FIFO: %s\n", strerror(errno));
        exit(1);
    }
}
