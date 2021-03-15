#include "options.h"


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


char* fromOctalToString(mode_t mode) { //converts Octal to a string with rwx notation
    char* permissions=malloc(10);
    for(int i=0;i<9;i++){
        if((i==0||i==3||i==6)&&get_bit(mode,i)==1){
            permissions[i]='r';
        }
        else if((i==1||i==4||i==7)&&get_bit(mode,i)==1){
            permissions[i]='w';
        }
        else if((i==2||i==5||i==8)&&get_bit(mode,i)==1){
            permissions[i]='x';
        }
        else{
            permissions[i]='-';
        }
    }
    return permissions;
    
}

char* fourDigitOctal(mode_t mode){ //adds zeros, example input: 777-> return: "0777"
    char *str;
    str=malloc(5);
    if(mode<=7){ // in Octal -> 0007
        char aux[2];
        char* zeros="000";
        sprintf(aux, "%o", mode);
        strcat(str,zeros);
        strcat(str,aux);
    }
    else if(mode<=63){ // in Octal -> 0070
        char aux[3];
        char* zeros="00";
        sprintf(aux, "%o", mode);
        strcat(str,zeros);
        strcat(str,aux);
    }
    else{ 
        char aux[4];
        char* zeros="0";
        sprintf(aux, "%o", mode);
        strcat(str,zeros);
        strcat(str,aux);
    }
    return str;
}

void diagnosticPrint(const char* filePath, const mode_t oldMode, const mode_t newMode, Options options) { 
    char* oldPermissions = fromOctalToString(oldMode), *newPermissions = fromOctalToString(newMode); 

    if(checkChanges(oldMode, newMode) && (options.vflag == V || options.vflag == C))
        printf("mode of '%s' changed from %s (%s) to %s (%s)\n",filePath, fourDigitOctal(oldMode), oldPermissions, fourDigitOctal(newMode), newPermissions);
    else if(!checkChanges(oldMode,newMode) && options.vflag == V)
        printf("mode of '%s' retained as %s (%s)\n",filePath, fourDigitOctal(oldMode), oldPermissions);
}

bool checkChanges(const mode_t oldMode, const mode_t newMode) {
    if (oldMode == newMode) return false;
    else return true;
}