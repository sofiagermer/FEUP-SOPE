#include "threads.h"

extern info_t info;
extern int errno;

int publicFifoDesc; 
sem_t semP, semC;

void* consumerHandler(void* a) {
    while(bufferEmpty()){
        sem_wait(&semP);
    }
    msg* message=removeFromBuffer();
    regist(message->i,message->t,getpid(),pthread_self(),message->res,"TSKDN");
    sem_post(&semC);
    pthread_exit(NULL);
}

void* producerHandler(void* a) {
    msg* message=(msg*)a;
    while(bufferFull()){
        sem_wait(&semC);
    }
    message->res=task(message->t);
    regist(message->i,message->t,getpid(),pthread_self(),message->res,"TSKEX");
    addToBuffer(message);
    sem_post(&semP);
    pthread_exit(NULL);
}


void createThreads() {

    time_t start, now;
    time(&start);
    sem_init(&semP, 0, 0);
    sem_init(&semC, 0, 0);

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
    time(&now);
    while((int) (now - start) > info.nsecs) {
        msg* message = (msg*) malloc(sizeof(msg));
        if (read(publicFifoDesc, message, sizeof(msg)) != 0) {
            fprintf(stderr, "Server: Error in %s:%s\n", __func__, strerror(errno));
            exit(1);
        }
        if (pthread_create(&id, NULL, producerHandler, message) != 0) {
            fprintf(stderr, "Server: Error in %s:%s\n", __func__, strerror(errno));
            exit(1);
        }
        push(id);
        time(&now);
    }

    close(publicFifoDesc);
    sem_destroy(&semC);
    sem_destroy(&semP);

     
}