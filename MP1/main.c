#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include <sys/types.h> 
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <dirent.h>
#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
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
bool CheckIfFile(const char *path)
{
    struct stat stats;

    stat(path, &stats);

    // Check for file existence
    if (stats.st_mode & F_OK)
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
bool CheckOctalMode(char i, char j, char k, char s){
    if(i != '0') return false;
    if(j == '0' || j == '1' || j == '2' || j == '3' || j == '4' || j == '5' || j == '6' || j == '7'){
        if(k == '0' || k == '1' || k == '2' || k == '3' || k == '4' || k == '5' || k == '6' || k == '7'){
            if(s == '0' || s == '1' || s == '2' || s == '3' || s == '4' || s == '5' || s == '6' || s == '7'){
                return true;;
            }
        }
    }
    return false;
}
/*
int addPermisions(char i, char j, char k, int n){
    if
}*/
/*
int findMode(char* arguments[], char* filePath){
    int mode = getChmod(filePath);
    if(*arguments[1] == '=') return mode;
    size_t n = sizeof(arguments)/sizeof(arguments[0]);

    switch(*arguments[0]){
        case 'u': {
            switch(*arguments[1]){
                case '-':{
                    for(int i = 2; i < n; i++){
                        if(i == 2 && *arguments[2] == 'r') {mode -= 400;}
                        if(i == 3 && *arguments[3] == 'w') {mode -= 200;}
                        if(i == 4 && *arguments[4] == 'x') {mode -= 100;}
                        else return -1;
                    }
                     break;
                }
                case '+': {
                   for(int i = 2; i < n; i++){
                        if(i == 2 && *arguments[2] == 'r') {mode += 400;}
                        else if(i == 3 && *arguments[3] == 'w') {mode += 200;}
                        else if(i == 4 && *arguments[4] == 'x') {mode += 100;}
                        else return -1;
                    }
                }
                break;
            }
            break;
        }
        case 'g': {
            switch(*arguments[1]){
                case '-':{
                    for(int i = 2; i < n; i++){
                        if(i == 2 && *arguments[2] == 'r') {mode -= 40;}
                        if(i == 3 && *arguments[3] == 'w') {mode -= 20;}
                        if(i == 4 && *arguments[4] == 'x') {mode -= 10;}
                        else return -1;
                    }
                }
                case '+': {
                    for(int i = 2; i < n; i++){
                        if(i == 2 && *arguments[2] == 'r') {mode += 40;}
                        else if(i == 3 && *arguments[3] == 'w') {mode += 20;}
                        else if(i == 4 && *arguments[4] == 'x') {mode += 10;}
                        else return -1;
                    }
                    break;
                }
            }
            break;
        }
        case 'o':{
            switch(*arguments[1]){
                case '-':{
                    for(int i = 2; i < n; i++){
                        if(i == 2 && *arguments[2] == 'r') {mode -= 40;}
                        if(i == 3 && *arguments[3] == 'w') {mode -= 20;}
                        if(i == 4 && *arguments[4] == 'x') {mode -= 10;}
                        else return -1;
                    }
                }
                case '+': {
                    for(int i = 2; i < n; i++){
                        if(i == 2 && *arguments[2] == 'r') {mode += 40;}
                        else if(i == 3 && *arguments[3] == 'w') {mode += 20;}
                        else if(i == 4 &&*arguments[4] == 'x') {mode += 10;}
                        else return -1;
                    }
                }
            }
            break;
        }
        case 'a':{
            switch(*arguments[1]){
                case '-':{
                    for(int i = 2; i < n; i++){
                        if(i == 2 && *arguments[2] == 'r') {mode -= 444;}
                        if(i == 3 && *arguments[3] == 'w') {mode -= 222;}
                        if(i == 4 && *arguments[4] == 'x') {mode -= 111;}
                        else return -1;
                    }
                    break;
                }
                case '+': {
                    for(int i = 2; i < n; i++){
                        if(i == 2 && *arguments[2] == 'r') {mode -= 444;}
                        if(i == 3 && *arguments[3] == 'w') {mode -= 222;}
                        if(i == 4 && *arguments[4] == 'x') {mode -= 111;}
                        else return -1;
                    }
                    break;
                }
            }
        }
        default:
            break;
    }
    return mode;
}*/

/*
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
*/

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
    for(int i = 1; i < 4 ; i++){
        //OCTAL MODE
        if(isdigit(*arguments[i])) 
         {
            if(CheckOctalMode(arguments[i][0], arguments[i][1], arguments[i][2], arguments[i][3])){
                mode = (int) *arguments[i];
            }
            else{
                fprintf(stderr , "Octal Mode isn't correct: %s\n", strerror(errno));
                exit(1);
            }
        }
        //MODE
        else if(isalpha(*arguments[i])){
            if ((access(arguments[i], 0)) != -1) continue;
           /* else {
                mode = findMode(arguments, filePath);
                if(mode != -1){
                    //executer(mode,filePath);
                    printf("would work \n");
                }
                else{
                    fprintf(stderr , "Mode isn't correct: %s\n", strerror(errno));
                    exit(1);
                }
            }*/
        }
        else{
            if ((access(arguments[i], 0)) != -1) continue;

            //OPTIONS
            else if(arguments[i][0] == '-') {
                continue;
            }
            else exit(1);
        }
    }
    //executer(mode,filePath);
}

int main(int argc, char* argv[], char* envp[]) {
    parser(argv, argc);
    return 0;
}