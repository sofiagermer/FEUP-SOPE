
#include "buffer.h"

int back;
int maxSize;
msg **queue;

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

bool push(msg* message){
    //falta verificar se esta cheia antes de dar push
    if (isFull) return false;
    if(isEmpty) back=0;
    queue[back]=message;
    back++;
    return true;
}

bool isEmpty(){
    if(back==-1) return true;
    return false;
}

bool isFull(){
    if(back+1==maxSize) return true;
    return false;
}

bool pop(msg* message){
    if(isEmpty) return false;
    message=queue[back];
    back--;
}





