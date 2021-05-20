#include "parser.h"
#include "utils.h"
#include "threads.h"

info_t info;
int publicFifoDesc;
extern int errno;

int main(int argc, char* argv[]) {
    
    parse(&info,argc, argv);
    
    //Creates public fifo
    if(mkfifo(info.fifoname, 0666) == -1 && errno != EEXIST) {
        fprintf(stderr,"Failed to create FIFO: %s\n", strerror(errno));
        exit(1);
    }

    // Try opening public fifo
    if ((publicFifoDesc = open(info.fifoname, O_RDONLY | O_NONBLOCK)) == -1) { 
        fprintf(stderr, "Server: Error opening public fifo in %s: %s\n", __func__, strerror(errno));
        exit(1);
    }

    createThreads();

    
    return 0;
}