
#include "buffer.h"



void initializeBuffer(int size){
    maxSize=size;
    if(queue!=NULL){
        free(queue);
    }
    else{

        queue=malloc(sizeof(msg)*maxSize);
    }
    back=-1;
}

bool addToBuffer(msg* message){
    if (bufferFull()) return false;
    if(bufferEmpty()) back=0;
    queue[back]=message;
    back++;
    return true;
}

bool bufferEmpty(){
    if(back==-1) return true;
    return false;
}

bool bufferFull(){
    if(back+1==maxSize) return true;
    return false;
}

msg* removeFromBuffer(){
    if(bufferEmpty()) return NULL;
    back--;
    return queue[back+1];
}





