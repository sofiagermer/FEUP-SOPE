#include "linkedlist.h"

struct node * head = NULL;
struct node * tail = NULL;

void push(pthread_t data) {
    struct node * node=(struct node*)malloc(sizeof(struct node));
    if(node!=NULL){
        node->value = data;
        node->next=NULL;
    }
    
    if (head == NULL) {
        head = node;
        tail = head;
    } else {
        tail->next = node;
        tail = node;
    }
}

pthread_t pop() {
    struct node * toDelete=head;
    pthread_t retVal=head->value;
    head=toDelete->next;
    free(toDelete);
    return retVal;
}

bool isEmpty() {
    return head == NULL;
}
