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

int getChmod(const char *path){
    struct stat mode;

    if (stat(path, &mode) == -1) {
        return -1;
    }

    int m = mode.st_mode;
    m &= 0xfff;
    return m;
}

//R -> 4 W -> 2 E -> 1

mode_t findMode(char* arguments[], char* filePath){
    int mode = getChmod(filePath);
    if(*arguments[1] == '=') return mode;
    switch(*arguments[0]){
        case 'u': {
            switch(*arguments[1]){
                case '-':{
                    if(*arguments[2] == 'r') {mode -= 400;}
                    if(*arguments[3] == 'w') {mode -= 200;}
                    if(*arguments[4] == 'x') {mode -= 100;}
                    break;
                }
                case '+': {
                    if(*arguments[2] == 'r') {mode += 400;}
                    if(*arguments[3] == 'w') {mode += 200;}
                    if(*arguments[4] == 'x') {mode += 100;}
                    break;
                }
            }
            break;
        }
        case 'g': {
            switch(*arguments[1]){
                case '-':{
                    if(*arguments[2] == 'r') {mode -= 40;}
                    if(*arguments[3] == 'w') {mode -= 20;}
                    if(*arguments[4] == 'x') {mode -= 10;}

                    break;
                }
                case '+': {
                    if(*arguments[2] == 'r') {mode += 40;}
                    if(*arguments[3] == 'w') {mode += 20;}
                    if(*arguments[4] == 'x') {mode += 10;}
                    break;
                }
            }
            break;
        }
        case 'o':{
            switch(*arguments[1]){
                case '-':{
                    if(*arguments[2] == 'r') {mode -= 4;}
                    if(*arguments[3] == 'w') {mode -= 2;}
                    if(*arguments[4] == 'x') {mode -= 1;}
                    break;
                }
                case '+': {
                    if(*arguments[2] == 'r') {mode += 4;}
                    if(*arguments[3] == 'w') {mode += 2;}
                    if(*arguments[4] == 'x') {mode += 1;}
                    break;
                }
            }
            break;
        }
        case 'a':{
            switch(*arguments[1]){
                case '-':{
                    if(*arguments[2] == 'r') {mode -= 444;}
                    if(*arguments[3] == 'w') {mode -= 222;}
                    if(*arguments[4] == 'x') {mode -= 111;}
                    break;
                }
                case '+': {
                    if(*arguments[2] == 'r') {mode += 444;}
                    if(*arguments[3] == 'w') {mode += 222;}
                    if(*arguments[4] == 'x') {mode += 111;}
                    break;
                }
            }
        }
        default:
            break;
    }
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
            //chmod(filePath,mode);
            if ((dir = opendir (filePath)) != NULL) {
                while ((ent = readdir (dir)) != NULL) { 
                    if(strcmp(".",ent->d_name)!=0&&strcmp("..",ent->d_name)!=0){
                        printf ("%s\n", ent->d_name); 
                        char* newPath=malloc(strlen(filePath)+strlen(ent->d_name)+1);
                        strcat(newPath,filePath);
                        strcat(newPath,"/");
                        strcat(newPath,ent->d_name);
                        executer(mode,newPath); //Calls recursively until all permissions are changed
                    }
                    
                }
                closedir (dir);
            } 
            else {
                printf("Dir did not open");
                return;
            }
        }
        else{
            //chmod(filePath,mode);
            printf("Changed permissions");
        }
    }

}

void parser(char* arguments[], int n) {
    mode_t mode=777; //random initialization because I was getting warnings
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
        //findOptions(arguments);
    }
    executer(mode,filePath);
}

int main(int argc, char* argv[], char* envp[]) {

    //parser(argv, argc);
    int mode =getChmod("/home/sofia/Desktop/SOPE/MP1/MP1/testfile.txt");
    printf("%d", mode);
    printf("\n");
    return 0;
}