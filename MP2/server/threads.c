#include "threads.h"

extern info_t info;
extern int errno;
extern int publicFifoDesc; 

sem_t semP, semC; // Semaphores for consumer and producer threads to access the buffer
pthread_mutex_t indexMutex = PTHREAD_MUTEX_INITIALIZER; // Mutex for the index of the buffer
msg** buffer;
int bufferIndex;
bool timeout, ready;

void* consumerHandler(void* a) {

    while (!ready) {
        if (timeout) pthread_exit(NULL);
    }

    msg* message;
    int index = 0;
    int result;

    while (true) {    

        // Conditions to end the loop
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

        // Fabricate message
        char privateFifoName[200];
        int privateFifoDesc;
        snprintf(privateFifoName, sizeof(privateFifoName), "/tmp/%d.%ld", message->pid, message->tid);

        // Write answer
        if((privateFifoDesc = open(privateFifoName, O_WRONLY | O_NONBLOCK)) < 0) {
            regist(message->i,message->t,getpid(),pthread_self(),message->res,"FAILD");
            pthread_exit(NULL);
        }
        if (write(privateFifoDesc,message,sizeof(msg)) == -1) {
            fprintf(stderr, "Server: Failed to write to private fifo in %s: %s\n", __func__, strerror(errno));
            exit(1);
        }

        // Register answer
        if (message->res != -1) regist(message->i,message->t,getpid(),pthread_self(),message->res,"TSKDN");
        else regist(message->i,message->t,getpid(),pthread_self(),message->res,"2LATE");
    }
    pthread_exit(NULL);
}

void* producerHandler(void* a) {
    msg* message = (msg*) a;

    // Perform task
    if (!timeout) {
        message->res = task(message->t);
        regist(message->i,message->t,getpid(),pthread_self(),message->res,"TSKEX");
    }

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
    free(message);
    pthread_exit(NULL);
}


void createThreads() {

    timeout = false;
    ready = false;
    msg* message;
    bufferIndex = 0;

    // Allocation of the buffer
    pthread_t id;
    buffer = (msg**) malloc(info.buffersize * sizeof(msg*));
    bufferIndex = 0;

    // Time, semaphores and mutex
    time_t start;
    time(&start);
    sem_init(&semP, 0, info.buffersize);
    sem_init(&semC, 0, 0);

    // Consumer thread
    if (pthread_create(&id, NULL, consumerHandler, NULL) != 0) {
        fprintf(stderr, "Server: Error creating thread in %s: %s\n", __func__, strerror(errno));
        exit(1);
    }
    push(id);

    // Producer threads (read message and create thread)
    while((time(NULL) - start) < info.nsecs) {
        
        message = (msg*) malloc(sizeof(msg));
        int result;
        
        while((result = read(publicFifoDesc, message, sizeof(msg))) <= 0 && (int) time(NULL) - start < info.nsecs);
        if (result == 0) { // Timeout reached
            free(message);
            break;
        }
        ready = true; // To start the consumer thread

        regist(message->i, message->t, getpid(), pthread_self(), message->res, "RECVD");

        if (pthread_create(&id, NULL, producerHandler, message)) {
            fprintf(stderr, "Server: Error creating thread in %s: %s\n", __func__, strerror(errno));
            exit(1);
        }
        push(id);
    }

    timeout = true; // Time of program is over
    
    // Wait for all threads to finish
    while(!isEmpty()){
        pthread_t t = pop();
        printf("Thread:%ld\n", t);
        if (pthread_join(t, NULL) != 0) {
            fprintf(stderr, "Server: Error in pthread_join in %s:%s\n", __func__, strerror(errno));
            exit(1);
        }
    }

    sem_destroy(&semC);
    sem_destroy(&semP);
    free(buffer);
}