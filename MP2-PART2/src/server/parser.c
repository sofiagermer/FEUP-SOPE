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

void parse(info_t * info, int argc, char * argv[]) {

    if(argc < 4){
        printf("s: error: too few arguments\n");
        exit(1);
    } else if(argc > 6) {
        printf("s: error: too many arguments\n");
        exit(1);
    }
    info->nsecs = -1;
    info->buffersize = 10;
    info->fifoname = NULL;
    for (unsigned i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-t") == 0) { //Time
            if ( i == argc - 1 || (i != argc - 1 && !is_positive_integer(argv[i+1]))) {
                printf("s: error: missing time value after -t\n"); 
                exit(1);
            }
            info->nsecs = atoi(argv[i+1]);
            i++;
        } else if(strcmp(argv[i], "-l") == 0) { //buffer size
            if ( i == argc - 1 || (i != argc - 1 && !is_positive_integer(argv[i+1]))) {
                printf("s: error: missing time value after -l\n"); 
                exit(1);
            }
            info->buffersize = atoi(argv[i+1]);
            i++;
        } else { //Fifopath
            if(info->fifoname != NULL) {
                printf("s: error: arguments aren't correct\n"); 
                exit(1);
            }
            info->fifoname = argv[i];
        }
    }
    if (info->nsecs == -1) {
        printf("s: error: missing option -t \n"); 
        exit(1);
    }
    if (info->fifoname == NULL) {
        printf("s: error: missing fifoname \n"); 
        exit(1);
    }
}
