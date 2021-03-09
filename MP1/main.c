#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include <sys/types.h> 
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <dirent.h>

extern int errno;
typedef enum {C, V, NONE} verbose;
bool recursive;
verbose vflag;

bool isDirectory(char* pathname){ //Checks if the path is a directory, returns true if it is else it's a file

    struct stat sb;

    if (stat(pathname, &sb) == 0 && S_ISDIR(sb.st_mode))
        return true;
    return false;
}


void findOptions(char* arguments[]) { //Get the options in a string, they can be anywhere in the line
    char *options;
    for (unsigned int i = 0; i < 4; i++) {
        if (arguments[i][0] == '-') {
            options= arguments[i];
        }
    }
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

void executer(mode_t mode, char* filePath) { //Will actually use chmod function (recursively if necessary)
    DIR *dir;
    struct dirent *ent;
    if(!recursive){
        printf("No Recursion");
        return;
    }
    else{
        if(isDirectory(filePath)){ //Verifica se é um directorio, se for muda as permissoes e chama a funçao recursivamente, se for um ficheiro muda as permissoes
            if ((dir = opendir (filePath)) != NULL) {
                while ((ent = readdir (dir)) != NULL) { //Está me a dar um erro estupido, aparecem bue pontos ao listar os ficheiro, nao percebo
                
                    printf ("%s\n", ent->d_name); //Falta juntar as 2 strings para formar um novo path e chamar a funçao com o novo path
                    printf("Call recursion\n");
                }
                closedir (dir);
            } 
            else {
                printf("Dir did not open");
                return;
            }
        }
        else{
            printf("Changed permissions");
        }
    }

}

void parser(char* arguments[], int n) {
    mode_t mode;
    int flags; //To save the result of the mode detection module (the flags to be used in chmod function)
    char* filePath="testDir";
    if (n == 3 || n == 4) { 
        //Function(s) that detect and assign the mode to 'flags' variable 
        //May also retrieve the filename and detect if the order is correct (idk?)
    }
    else { //Not enough arguments
        fprintf(stderr, "Few arguments: %s\n", strerror(errno));
        exit(1);
    }
    if (n == 4) { //With Options
        findOptions(arguments);
    }
    executer(mode,filePath);
}

int main(int argc, char* argv[], char* envp[]) {

    parser(argv, argc);

    return 0;
}