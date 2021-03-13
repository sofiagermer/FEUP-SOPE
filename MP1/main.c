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
#include <ctype.h>
#include <math.h>

typedef enum {C, V, NONE} verbose;

extern int errno;

bool recursive;
verbose vflag;
char* filePath;

bool isDirectory(char* pathname) { //Checks if the path is a directory, returns true if it is else it's a file

    struct stat sb;
    if (stat(pathname, &sb) == 0 && S_ISDIR(sb.st_mode))
        return true;
    return false;

}

bool checkPath(const char *path) {
    printf("Path:%s\n", path);
    struct stat stats;

    stat(path, &stats);

    // Check for file existence
    if (stats.st_mode & F_OK) {

        printf("YAY\n");
        return true;
    }
    //else if (S_ISDIR(stats.st_mode))  return true;
    return false;
}

long long convert1(int oct) {
    int dec = 0, i = 0;
    long long bin = 0;

    // converting octal to decimal
    while (oct != 0) {
        dec += (oct % 10) * pow(8, i);
        ++i;
        oct /= 10;
    }
    i = 1;

   // converting decimal to binary
    while (dec != 0) {
        bin += (dec % 2) * i;
        dec /= 2;
        i *= 10;
    }
}

int convert(int bin) {
    int oct = 0, dec = 0, i = 0;

    // converting binary to decimal
    while (bin != 0) {
        dec += (bin % 10) * pow(2, i);
        ++i;
        bin /= 10;
    }
    i = 1;

    // converting to decimal to octal
    while (dec != 0) {
        oct += (dec % 8) * i;
        dec /= 8;
        i *= 10;
    }
    return oct;
}

int getChmod(const char *path) {
    struct stat mode;

    if (stat(path, &mode) == -1) {
        return -1;
    }

    int m = mode.st_mode;
    m &= 0xfff;
    return m;
}

//R -> 4 W -> 2 E -> 1
bool CheckOctalMode(char i, char j, char k, char s) {
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
int findMode(char* modes, char* filePath){
    int modeBefore = getChmod(filePath);
    int modeAfter = 0;
    int n = strlen(modes);

    for(int i = 2; i < n; i++){
        if(modes[i] == 'r') modeAfter += 4;
        if(modes[i] == 'w') modeAfter += 2;
        if(modes[i] == 'x') modeAfter += 1;
        else return -1;
    }

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
                        if(i == 3 && *arguments[3] == 'w') {mode += 200;}
                        if(i == 4 && *arguments[4] == 'x') {mode += 100;}
                        else return -1;
                    }
                }
                case '=': {
                    for (int i = 2; i < n; i++) {

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

void diagnosticPrint(char* filePath,mode_t oldMode,mode_t newMode){ //falta converter estes modos todos para a notação correcta
    char* oldPermissions="",*newPermissions=""; //inicializei so para calar warnings
    
    printf("mode of '%s' changed from %x (%s) to %x (%s)",filePath,oldMode,oldPermissions,newMode,newPermissions);

}

void findOptions(char* options) { //Get the options in a string, they can be anywhere in the line

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

            //Esta parte ainda nao esta terminada mas era mil vezes mais fácil pensar nisto se o programa já tivesse a funcionar todo ok
            /*struct stat info;
            stat(filePath,&info);
            if(chmod(filePath,mode)!=0){
                printf("chmod error");
            }
            else if(vflag==C&&(info.st_mode!=mode)){
                diagnosticPrint(filePath,info.st_mode,mode);
            }
            else if(vflag==V){
                diagnosticPrint(filePath,info.st_mode,mode);
            }*/

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
            //Esta parte ainda nao esta terminada mas era mil vezes mais fácil pensar nisto se o programa já tivesse a funcionar todo ok
            /*struct stat info;
            stat(filePath,&info);
            if(chmod(filePath,mode)!=0){
                printf("chmod error");
            }
            else if(vflag==C&&(info.st_mode!=mode)){
                diagnosticPrint(filePath,info.st_mode,mode);
            }
            else if(vflag==V){
                diagnosticPrint(filePath,info.st_mode,mode);
            }*/
        }
    }

}

void parser(char* arguments[], int n) {
    mode_t mode=777; //random initialization because I was getting warnings
    //int flags; //To save the result of the mode detection module (the flags to be used in chmod function)
    for(int i = 1; i < n ; i++){
        printf("%d\n", i);
        //OCTAL MODE
        if(isdigit(*arguments[i])) 
         {
            if(CheckOctalMode(arguments[i][0], arguments[i][1], arguments[i][2], arguments[i][3])){
                mode = atoi(arguments[i]);
                printf("Mode:%d  Mode in arguments:%s\n", mode, arguments[i]);
            }
            else{
                fprintf(stderr , "Octal Mode isn't correct: %s\n", strerror(errno));
                exit(1);
            }
        }
        //FILES
        else if (checkPath(arguments[i])) {
            filePath = arguments[i];
            printf("Filepath:%s\n", filePath);
        }
        //MODE or FILE PATH
        else if(isalpha(*arguments[i])){
            if ((access(arguments[i], F_OK)) != -1) {
                filePath = arguments[i];
                printf("Filepath:%s\n", filePath);
                continue;
            }
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
        //FULL PATHS
        else if (arguments[i][0] == '/') {
            if ((access(arguments[i], 0)) != -1) {
                filePath = arguments[i];
                printf("Filepath:%s\n", filePath);
                continue;
            }
        }
        //OPTIONS
        else if(arguments[i][0] == '-') {
            
            findOptions(arguments[i]);
            
            //OPTIONS TESTING
            /*
            if (vflag == V) printf("V ok\n");
            else if (vflag == C) printf("C ok\n");
            else if (vflag == NONE) printf("NONE ok\n");
            else printf("Merdou\n");
            if (recursive) printf("Recursive ok\n");
            else printf("Non recursive\n");*/

        }
        else {
            printf("WTF is this argument\n");
            exit(1);
        }
    }
    //executer(mode,filePath);
}

int main(int argc, char* argv[], char* envp[]) {
    //chmod("textfile.txt", 0777);
    int oct = getChmod("textfile.txt");
    printf("%l", convert1(oct));
    parser(argv, argc);
    return 0;
}