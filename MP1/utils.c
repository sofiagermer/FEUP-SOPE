#include "utils.h"

extern ProcessInfo* pInfo;

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
    if (pInfo->isParent) {
        free(pInfo->childrenPIDs);
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

int get_bit(int bits, int pos) { //returns the value of the bit at position pos of the value bits
   return (bits >> pos) & 0x01;
}


void fromOctalToString(mode_t mode,char* permissions) { //converts Octal to a string with rwx notation
    for(int i=0;i<9;i++){
        if((i==0||i==3||i==6)&&get_bit(mode,i)==1){
            permissions[i]='r';
        } else if((i==1||i==4||i==7)&&get_bit(mode,i)==1){
            permissions[i]='w';
        } else if((i==2||i==5||i==8)&&get_bit(mode,i)==1){
            permissions[i]='x';
        } else{
            permissions[i]='-';
        }
    }
    permissions[9]='\0';
}

void fourDigitOctal(mode_t mode,char *str){ //adds zeros, example input: 777-> return: "0777"
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
    char oldPermissions [10];
    fromOctalToString(oldMode,oldPermissions);
    char newPermissions[10];
    fromOctalToString(newMode,newPermissions);  
    char oldPermsFourDigit[5];
    fourDigitOctal(oldMode,oldPermsFourDigit);
    char newPermsFourDigit [5];
    fourDigitOctal(newMode,newPermsFourDigit);
    if(checkChanges(oldMode, newMode) && (pInfo->options.vflag == V || pInfo->options.vflag == C))
        printf("mode of '%s' changed from %s (%s) to %s (%s)\n",filePath, oldPermsFourDigit, oldPermissions, newPermsFourDigit, newPermissions);
    else if(!checkChanges(oldMode,newMode) && pInfo->options.vflag == V)
        printf("mode of '%s' retained as %s (%s)\n",filePath, oldPermsFourDigit, oldPermissions);
}

bool checkChanges(const mode_t oldMode, const mode_t newMode) {
    if (oldMode == newMode) return false;
    else return true;
}