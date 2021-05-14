#include "threads.h"

extern info_t info;
extern int errno;

int publicFifoDesc; 

void* consumerHandler(void* a) {

}

void* threadHandler(void* a) {
    msg* message=(msg*)a;
    task(message->t);
    //message->
}


void createThreads() {

    time_t start, now;
    time(&start);

    pthread_t id;
    if (pthread_create(&id, NULL, consumerHandler, NULL) != 0) {
        fprintf(stderr, "Server: Error in %s:%s\n", __func__, strerror(errno));
        exit(1);
    }
    push(id);
    int publicFifoDesc;
    while ((publicFifoDesc = open(info.fifoname, O_RDONLY|O_NONBLOCK) < 0)); // Timeout
    if (publicFifoDesc < 0) {
        fprintf(stderr, "Server: Error in %s:%s - (timeout reached)\n",__func__, strerror(errno));
        exit(1);
    }
    while((int) (now - start) > info.nsecs) {
        msg* message = (msg*) malloc(sizeof(msg));
        if (read(publicFifoDesc, message, sizeof(msg)) != 0) {
            fprintf(stderr, "Server: Error in %s:%s\n", __func__, strerror(errno));
            exit(1);
        }
        if (pthread_create(&id, NULL, threadHandler, message) != 0) {
            fprintf(stderr, "Server: Error in %s:%s\n", __func__, strerror(errno));
            exit(1);
        }
        push(id);
    }

    close(publicFifoDesc);


     
}