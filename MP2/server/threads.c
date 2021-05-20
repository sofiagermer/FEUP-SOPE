#include "threads.h"

extern info_t info;
extern int errno;

int publicFifoDesc; 
sem_t semP, semC; // Semaphores for consumer and producer threads to access the buffer
pthread_mutex_t indexMutex = PTHREAD_MUTEX_INITIALIZER; // Mutex for the index of the buffer
msg** buffer;
int bufferIndex;
bool timeout;

void* consumerHandler(void* a) {
    msg* message;

    int index = 0;
    while (true) {    
        printf("STUCK\n");

        // Conditions to end the loop
        int result;
        if (sem_getvalue(&semC, &result) != 0) {
            fprintf(stderr, "Server: Error reading semaphore value in %s: %s\n", __func__, strerror(errno));
            exit(1);
        }
        if (result == 0 && timeout) break;

        // Semaphore to wait when buffer is empty
        sem_wait(&semC);
        message = buffer[index];
        index++;
        index = index % info.buffersize;
        sem_post(&semP);

        char privateFifoName[200];
        int privateFifoDesc;

        snprintf(privateFifoName, sizeof(privateFifoName), "/tmp/%d.%ld", message->pid, message->tid);

        if((privateFifoDesc = open(privateFifoName, O_WRONLY)) < 0) {
            regist(message->i,message->t,getpid(),pthread_self(),message->res,"FAILD");
            fprintf(stderr, "Server: Failed to open private FIFO in %s: %s\n", __func__, strerror(errno));
            printf("CONSUMER GONNA DIE NOW\n");
            pthread_exit(NULL);
        }

        printf("GOT THERE\n");
        
        if (write(privateFifoDesc,message,sizeof(msg)) < 0) {
            fprintf(stderr, "Server: Failed to write to private fifo in %s: %s\n", __func__, strerror(errno));
            exit(1);
        }
        regist(message->i,message->t,getpid(),pthread_self(),message->res,"TSKDN");
    }
    printf("CONSUMER GONNA DIE NOW\n");
    pthread_exit(NULL);
}

void* producerHandler(void* a) {
    msg* message = (msg*) a;

    // Perform task
    message->res = task(message->t);
    regist(message->i,message->t,getpid(),pthread_self(),message->res,"TSKEX");

    // Semaphore to wait when buffer is full
    sem_wait(&semP);

    // Mutex to access buffer index variable
    pthread_mutex_lock(&indexMutex);
    int localIndex = bufferIndex;
    bufferIndex++;
    bufferIndex = bufferIndex % info.buffersize;
    pthread_mutex_unlock(&indexMutex);
    
    // Write message to buffer
    buffer[localIndex] = message;
    sem_post(&semC);
    pthread_exit(NULL);
}


void createThreads() {

    printf("I am here\n");

    timeout = false;
    msg* message;
    bufferIndex = 0;

    // Allocation of the buffer
    buffer = (msg**) malloc(info.buffersize * sizeof(msg));
    bufferIndex = 0;

    // Time, semaphores and mutex
    time_t start;
    time(&start);
    sem_init(&semP, 0, info.buffersize);
    sem_init(&semC, 0, 0);

    // Try opening public fifo
    while ((publicFifoDesc = open(info.fifoname, O_RDONLY | O_NONBLOCK)) < 0 && (time(NULL) - start < info.nsecs)) {
        printf("TRYING TO OPEN\n");
    }
    if (publicFifoDesc == -1) { // Timeout
        fprintf(stderr, "Server: Error opening public fifo in %s:timeout reached\n", __func__);
        sem_destroy(&semC);
        sem_destroy(&semP);
        free(buffer);
        exit(1);
    }
    printf("FILE DESCRIPTOR:%d\n", publicFifoDesc);

    // Consumer thread
    pthread_t id;
    if (pthread_create(&id, NULL, consumerHandler, NULL) != 0) {
        fprintf(stderr, "Server: Error creating thread in %s: %s\n", __func__, strerror(errno));
        exit(1);
    }
    push(id);

    // Producer threads (read message and create thread)
    while((int) (time(NULL) - start) < info.nsecs) {
        
        message = (msg*) malloc(sizeof(msg));
        int result;
        
        while((result = read(publicFifoDesc, message, sizeof(msg))) < 0 && (int) time(NULL) - start < info.nsecs);
        if (result == -1) {
            fprintf(stderr, "Server: Error reading from public fifo in %s:%s\n", __func__, strerror(errno));
            exit(1);
        } else if (result == 0) { // Timeout reached
            free(message);
            break;
        }

        regist(message->i, message->t, getpid(), pthread_self(), message->res, "RECVD");

        if (pthread_create(&id, NULL, producerHandler, message)) {
            fprintf(stderr, "Server: Error creating thread in %s:%s\n", __func__, strerror(errno));
            exit(1);
        }
        push(id);
    }

    timeout = true;
    /* if (read(publicFifoDesc, message, sizeof(msg)) == -1) {
        fprintf(stderr, "Server: Error in %s:%s\n", __func__, strerror(errno));
        exit(1);
    }

    char privateFifoName[200];
    snprintf(privateFifoName, sizeof(privateFifoName), "/tmp/%d.%ld", message->pid, message->tid);
    
    int privateFifoDesc;

    while((privateFifoDesc = open(privateFifoName, O_WRONLY)) < 0) {
        //fprintf(stderr, "Failed to open private FIFO: %s\n", strerror(errno));
    }
    message->pid=getpid();
    message->tid=pthread_self();
    
    if (write(privateFifoDesc,message,sizeof(msg)) < 0) {
        fprintf(stderr, "Failed to write to private fifo: %s\n", strerror(errno));
        exit(1);
    }

    regist(message->i,message->t,message->pid,message->tid,message->res,"2LATE"); */

    // Wait for all threads to finish
    while(!isEmpty()){
        printf("One more\n");
        if (pthread_join(pop(), NULL) != 0) {
            fprintf(stderr, "Server: Error in pthread_join in %s:%s\n", __func__, strerror(errno));
        }
    }

    close(publicFifoDesc);
    sem_destroy(&semC);
    sem_destroy(&semP);

    free(buffer);
}