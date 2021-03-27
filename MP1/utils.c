#include "utils.h"


bool isDirectory(const char* pathname) {

    struct stat sb;
    if (stat(pathname, &sb) == 0 && S_ISDIR(sb.st_mode))
        return true;
    return false;
    
}

void endProgram(int exitStatus) {

    registProcessExit(exitStatus);

    if(pInfo->isInitial){
        unsetenv("firstRun");
    }
    
    free(pInfo);
    exit(exitStatus);
}

void processOption(const char optFlag, Options* options) {
    switch (optFlag) {
        case 'R': {
            options->recursive = true;
            break;
        }
        case 'c': {
            if (options->vflag == V) break; //If v is present, c is irrelevant
            options->vflag = C;
            break;
        }
        case 'v': {
            options->vflag = V;
            break;
        }
    } 
}

int get_bit(int bits, int pos) { 
   return (bits >> pos) & 0x01;
}

void fromOctalToString(mode_t mode,char* permissions) { 
    for(int i=0;i<9;i++){
        if((i==0||i==3||i==6)&&get_bit(mode,i)==1){
            permissions[8-i]='x';
        } else if((i==1||i==4||i==7)&&get_bit(mode,i)==1){
            permissions[8-i]='w';
        } else if((i==2||i==5||i==8)&&get_bit(mode,i)==1){
            permissions[8-i]='r';
        } else{
            permissions[8-i]='-';
        }
    }
    permissions[9]='\0';
}

void fourDigitOctal(mode_t mode,char *str){ 
    size_t size = sizeof(str);
    if(mode <= 7){ // in Octal -> 0007
        snprintf(str,size, "000%o", mode);
    } else if(mode<=63){ // in Octal -> 0070
        snprintf(str,size, "00%o", mode);
    } else{     
        snprintf(str,size,"0%o", mode);
    }
}

void diagnosticPrint(const char* filePath, mode_t oldMode,mode_t newMode) { 

    //Get numerical and schematichal string of old and new modes
    char oldPermissions [10];
    fromOctalToString(oldMode,oldPermissions);
    char newPermissions[10];
    fromOctalToString(newMode,newPermissions);  
    char oldPermsFourDigit[5];
    fourDigitOctal(oldMode,oldPermsFourDigit);
    char newPermsFourDigit [5];
    fourDigitOctal(newMode,newPermsFourDigit);

    //Print the message
    if(checkChanges(oldMode, newMode) && (pInfo->options.vflag == V || pInfo->options.vflag == C))
        printf("mode of '%s' changed from %s (%s) to %s (%s)\n",filePath, oldPermsFourDigit, oldPermissions, newPermsFourDigit, newPermissions);
    else if(!checkChanges(oldMode,newMode) && pInfo->options.vflag == V)
        printf("mode of '%s' retained as %s (%s)\n",filePath, oldPermsFourDigit, oldPermissions);
}

bool checkChanges(const mode_t oldMode, const mode_t newMode) {
    if (oldMode == newMode) return false;
    else return true;
}

void makeNewArgs(char* newArgs[], char* fileName) {
    for (unsigned int i = 0; i <= pInfo->args.nArgs; i++) {
        if (i == pInfo->args.fileNameIndex)
            newArgs[i] = fileName;
        else    
            newArgs[i] = pInfo->args.arguments[i];
        if (i == pInfo->args.nArgs)
            newArgs[i] = NULL;
    }
}

void hasAChild(char* newPath) {
    
    char* args[pInfo->args.nArgs + 1];
    makeNewArgs(args, newPath);
    registProcessCreation(args);

    int id = fork();
    switch (id) {
        case 0: {
            registProcessCreation(args);
            char* args[pInfo->args.nArgs + 1];
            makeNewArgs(args, newPath);
            
            if (execvp(args[0], args) == -1) {
                fprintf(stderr, "Error with execvpe: %s", strerror(errno));
                endProgram(1);
            }
            break;
        }
        case -1:{
            fprintf(stderr, "Error with fork:%s\n", strerror(errno));
            endProgram(1);
        }
        default: {
            pInfo->noChildren++;
            break;
        }
    }
}

double getMiliSeconds(double initialTime) {
    struct timespec t;
    clock_gettime(CLOCK_REALTIME,&t);
    char aux[20];
    snprintf(aux,sizeof(aux),"0.%ld",t.tv_nsec);
    double curTime=(double)t.tv_sec+strtod(aux,NULL);
    return (double)(curTime - initialTime)*1000;
}
