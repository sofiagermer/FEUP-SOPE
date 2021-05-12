#include "parser.h"

int is_positive_integer(const char * num) { 
    for(unsigned i = 0; i < strlen(num); i++) {
        if (isdigit(num[i])) {
            continue;
        }
        return 0;
    }
    return 1;
}

void parse(info_t * info, int argc, char const * argv[]) {
    const int VALID_ARGC = 6;

    //Check valid arguments number
    if (argc > VALID_ARGC) {
        printf("s: error: too many arguments\n");
        exit(1);
    }
    if (argc < VALID_ARGC) {
        printf("s: error: too few arguments\n");
        exit(1);
    }

    int nsecs = 0;

    //Retrieving arguments
    for (unsigned i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-t") == 0) { //Time
            if ( i == argc - 1 || (i != argc - 1 && !is_positive_integer(argv[i+1])) ) 
            {
                printf("s: error: missing time value after -t\n"); 
                exit(1);
            }
            nsecs = atoi(argv[i+1]);
            i++;
        }else if (strcmp(argv[i], "-l") == 0){ //buffer size
            if ( i == argc - 1 || (i != argc - 1 && !is_positive_integer(argv[i+1])) ) 
            {
                printf("s: error: missing time value after -l\n"); 
                exit(1);
            }
            info->buffersize = atoi(argv[i+1]);
            i++;
        }
        else {//Fifopath
            info->fifoname = (char*) malloc(strlen(argv[i])+1);  
            size_t size=strlen(argv[i])+1;
            snprintf(info->fifoname,size,"%s",argv[i]);
        }
    }
    //Struct
    info->nsecs = nsecs;
}
