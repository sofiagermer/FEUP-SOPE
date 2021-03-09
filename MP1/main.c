#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include <sys/types.h> 
#include <errno.h>
#include <string.h>
#include <stdbool.h>

extern int errno;
typedef enum {C, V, NONE} verbose;

char* findOptions(char* arguments[]) { //Get the options in a string, they can be anywhere in the line
    for (unsigned int i = 0; i < 4; i++) {
        if (arguments[i][0] == '-') {
            return arguments[i];
        }
    }
}

void executer(char* options, int flag, char*filename) { //Will actually use chmod function (recursively if necessary)
    bool recursive = false;
    verbose vflag = NONE; //Detect verbose mode (c, v or none) 
    for (char* optFlag = options; *optFlag != NULL; optFlag++) {
        switch (*optFlag) {
            case 'R': {
                recursive = true;
                break;
            }
            case 'c': {
                if (vflag == V) break; //If v is present, c is irrelevant
                vflag = C;
                break;
            }
            case 'v': {
                vflag = V;
                break;
            }
        }
    }
    switch (vflag) {
        case C: {
            printf("c is present\n");
            break;
        }
        case V: {
            printf("v is present\n");
            break;
        }
        case NONE: {
            break;
        }
    }
    if (recursive) //Do recursion with forks
        printf("Not implemented\n");
}

void parser(char* arguments[], int n) {
    int flags; //To save the result of the mode detection module (the flags to be used in chmod function)
    char* options; //Options
    char* fileName;
    if (n == 3 || n == 4) { 
        //Function(s) that detect and assign the mode to 'flags' variable 
        //May also retrieve the filename and detect if the order is correct (idk?)
    }
    else { //Not enough arguments
        fprintf(stderr, "Few arguments: %s\n", strerror(errno));
        exit(1);
    }
    if (n == 4) { //With Options
        options = findOptions(arguments);
        printf("Options flags: %s\n", options);
    }
    executer(options, 777, "blyat");
}

int main(int argc, char* argv[], char* envp[]) {

    parser(argv, argc);

    return 0;
}