#include "parser.h"
#include "utils.h"
#include "threads.h"
#include "fifos.h"

int main(int argc, char* argv[]) {
    info_t info;
    parse(&info,argc, argv);
    
    //Creates public fifo
    createFifo(info.fifoname);

    createThreads(info);
    return 0;
}