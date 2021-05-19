#include "parser.h"
#include "utils.h"
#include "threads.h"
#include "fifos.h"

info_t info;

int main(int argc, char* argv[]) {
    
    parse(&info,argc, argv);
    
    //Creates public fifo
    createFifo(info.fifoname);

    createThreads();
    return 0;
}