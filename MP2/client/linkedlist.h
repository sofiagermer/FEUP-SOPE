#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

struct node{
   pthread_t value;
   struct node * next;
};


void push(pthread_t data); //insert link at the end of the list
pthread_t pop(); //delete first item
bool isEmpty(); //is list empty