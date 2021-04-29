#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

struct node{
   int value;
   struct node * next;
};

//insert link at the end of the list
void push(pthread_t data);

//delete first item
pthread_t pop();


//is list empty
bool isEmpty();