#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include <sys/types.h> 
#include <errno.h>
#include <string.h>

extern int errno;



int main(int argc, char* argv[], char* envp[]) {

    char* fileName = argv[1]; //Filename


    //Chmod 
    if (chmod(fileName, 0) == -1) {
        fprintf(stderr, "Chmod funtion error: %s\n", strerror(errno));
        exit(1);
    }




    return 0;
}