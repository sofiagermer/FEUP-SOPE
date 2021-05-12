#include <stdio.h>
#include <stdlib.h>

#include "parser.h"
#include "threads.h"
#include "fifos.h"

extern struct node * head; //Linked list head
extern struct node * tail; //Linked list tail
info_t info;
//msg buffer[];



int main(int argc,const char* argv[]) {


    parse(&info,argc, argv);
    
    //Creates public fifo
    createFifo(info.fifoname);


    return 0;
}