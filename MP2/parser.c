#include "parser.h"

int is_positive_integer(const char * num)  //move function to utils file or together with parse()
{
    for(unsigned i = 0; i < strlen(num); i++) {
        if (isdigit(num[i])) 
        {
            continue;
        }
        return 0;
    }
    return 1;
}

void parse(info_t * info, int argc, char const * argv[]) 
{
    const int VALID_ARGC = 4;

    if (argc > VALID_ARGC)  
    {
        printf("c: error: too many arguments\n");
        exit(1);
    }
    if (argc < VALID_ARGC)
    {
        printf("c: error: too few arguments\n");
        exit(1);
    }

    char * fifoname;
    int nsecs = 0;

    for (unsigned i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-t") == 0) 
        {
            if ( i == argc - 1 || (i != argc - 1 && !is_positive_integer(argv[i+1])) ) 
            {
                printf("c: error: missing time value after -t\n"); 
                exit(1);
            }
            nsecs = atoi(argv[i+1]);
            i++;
        }
        else
        {
            fifoname = (char*)malloc(strlen(argv[i])+1);  //[1]
            strcpy(fifoname, argv[i]);
        }
    }
    info->fifoname = fifoname;
    info->nsecs = nsecs;
}