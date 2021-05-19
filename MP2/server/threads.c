#include "threads.h"

extern info_t info;
extern int errno;

int publicFifoDesc; 
sem_t semP, semC;
pthread_mutex_t indexMutex = PTHREAD_MUTEX_INITIALIZER;
msg** buffer;
int bufferIndex;

void* consumerHandler(void* a) {
    msg* message;

    // Semaphore to wait when buffer is empty
    sem_wait(&semC);
    message=buffer[0];
    sem_post(&semP);
    
    regist(message->i,message->t,getpid(),pthread_self(),message->res,"TSKDN");
    pthread_exit(NULL);
}

void* producerHandler(void* a) {
    msg* message=(msg*)a;

    // Semaphore to wait when buffer is full
    sem_wait(&semP);
    message->res=task(message->t);
    regist(message->i,message->t,getpid(),pthread_self(),message->res,"TSKEX");

    // Mutex to access buffer index variable
    pthread_mutex_lock(&indexMutex);
    int localIndex=bufferIndex;
    bufferIndex++;
    bufferIndex=bufferIndex%info.buffersize;
    pthread_mutex_unlock(&indexMutex);
    
    // Write message to buffer
    buffer[localIndex]=message;
    sem_post(&semC);
    pthread_exit(NULL);
}


void createThreads() {

    // Allocation of the buffer
    buffer = (msg*) malloc(info.buffersize * sizeof(msg));
    bufferIndex = 0;

    // Time, semaphores and mutex
    time_t start, now;
    time(&start);
    sem_init(&semP, 0, info.buffersize);
    sem_init(&semC, 0, 0);

    // Consumer thread
    pthread_t id;
    if (pthread_create(&id, NULL, consumerHandler, NULL) != 0) {
        fprintf(stderr, "Server: Error in %s:%s\n", __func__, strerror(errno));
        exit(1);
    }
    push(id);

    while ((publicFifoDesc = open(info.fifoname, O_RDONLY|O_NONBLOCK) < 0)); // Timeout
    if (publicFifoDesc < 0) {
        fprintf(stderr, "Server: Error in %s:%s - (timeout reached)\n",__func__, strerror(errno));
        exit(1);
    }
    time(&now);

    // Producer threads (read message and create thread)
    while((int) (now - start) < info.nsecs) {
        
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

    // Wait for all threads to finish
    while(!isEmpty()){
        if (pthread_join(pop(), NULL) != 0) {
            fprintf(stderr, "Error in pthread_join: %s\n", strerror(errno));
        }
    }

    close(publicFifoDesc);
    sem_destroy(&semC);
    sem_destroy(&semP);

    free(buffer);
}