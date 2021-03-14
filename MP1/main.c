#include <dirent.h>
#include "modes.h"

typedef enum {C, V, NONE} Verbose;

extern int errno;

bool recursive;
Verbose vflag = NONE;

//BIGGER FUNCS
void parse(char* arguments[], int nArgs, char** filePath, char** mode); //Parses arguments
void executer(const char* mode, const char* filePath); //Recursive funtion

//AUXILIARY FUNCS
void diagnosticPrint(const char* filePath, const mode_t oldMode, const mode_t newMode); //Verbose messages
void processOption(const char optFlag); //Processes an option
void checkFile(const char* filePath); //Checks if a file exists
bool isDirectory(const char* pathname); //Checks if file in pathname is a directory
bool checkChanges(const mode_t oldMode, const mode_t newMode); //Checks if there were changes
mode_t getFilePermissions(const char *path); //Returns the permissions of the file being treated
int get_bit(int bits, int pos);
char* fromOctalToString(mode_t mode); //Converts mode to a string to be printed
char* fourDigitOctal(mode_t mode);
void calculateSpaceDir(DIR* dir, int* numFiles, int* maxSizeFile); //Calculates the space to be allocated for the array of files

bool isDirectory(const char* pathname) { //Checks if the path is a directory, returns true if it is else it's a file

    struct stat sb;
    if (stat(pathname, &sb) == 0 && S_ISDIR(sb.st_mode))
        return true;
    else
        return false;
}

mode_t getFilePermissions(const char *path) {
    struct stat mode;

    if (stat(path, &mode) == -1) {
        return -1;
    }

    int m = mode.st_mode;
    m &= 0x00fff;
    return m;
}

int get_bit(int bits, int pos) {
   return (bits >> pos) & 0x01;
}

char* fromOctalToString(mode_t mode) {
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

char* fourDigitOctal(mode_t mode){
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

void diagnosticPrint(const char* filePath, const mode_t oldMode, const mode_t newMode) { 
    char* oldPermissions = fromOctalToString(oldMode), *newPermissions = fromOctalToString(newMode); 

    if(checkChanges(oldMode, newMode) && (vflag == V || vflag == C))
        printf("mode of '%s' changed from %s (%s) to %s (%s)\n",filePath, fourDigitOctal(oldMode), oldPermissions, fourDigitOctal(newMode), newPermissions);
    else if(!checkChanges(oldMode,newMode)&&vflag==V)
        printf("mode of '%s' retained as %s (%s)\n",filePath, fourDigitOctal(oldMode), oldPermissions);
}

void checkFile(const char* filePath) {
    if (access(filePath, F_OK) == -1) {
        fprintf(stderr, "File name wrong: %s", strerror(errno));
        exit(1);
    }
}

void processOption(const char optFlag) {
    switch (optFlag) {
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

bool checkChanges(const mode_t oldMode, const mode_t newMode) {
    if (oldMode == newMode) return false;
    else return true;
}

void calculateSpaceDir(DIR* dir, int* numFiles, int* maxSizeFile) { //NOT TESTED
    *maxSizeFile = 0;
    *numFiles = 0;
    struct dirent *entry;
    while ((entry = readdir (dir)) != NULL) { 
        if(strcmp(".", entry->d_name) != 0 && strcmp("..", entry->d_name) != 0) {
            if (strlen(entry->d_name) > *maxSizeFile)
                *maxSizeFile = strlen(entry->d_name);
            (*numFiles)++;
        } 
    }
}

void parse(char* arguments[], int nArgs, char** filePath, char** mode) { 

    bool isMode = true;
 
    //PARSE
    for (unsigned int i = 1; i < nArgs; i++) {
        if (arguments[i][0] == '-') {
            for (unsigned int j = 0; j < strlen(arguments[i]); j++)
                processOption(arguments[i][j]);
        }
        else if (isMode) {
            *mode = arguments[i];
            isMode = false;
        }
        else *filePath = arguments[i];      
    }    
}

void executer(const char* mode, const char* filePath) { //IMPLEMENTING...

    mode_t oldMode;
    mode_t newMode;
    DIR *dir;
    struct dirent *entry;

    checkFile(filePath);
    oldMode = getFilePermissions(filePath);
    newMode = getModeNum(mode, filePath, oldMode);

    //TESTING
    printf("Mode numerical:%o\n", newMode);

    if (chmod(filePath, newMode) != 0) {
        fprintf(stderr, "Error with chmod:%s\n", strerror(errno));
        exit(1);
    }
    else diagnosticPrint(filePath, oldMode, newMode);

    //NOT TESTED
    /*
    if (recursive && isDirectory(filePath)) { //Verifica se é um directorio

        if ((dir = opendir (filePath)) != NULL) {

            //Allocate files array
            int numFiles, maxSizeFile;
            calculateSpaceDir(dir, &numFiles, &maxSizeFile);
            char **filesArr = (char **) malloc(numFiles * sizeof(char*)); 
            for (unsigned int i = 0; i < numFiles; i++) 
                filesArr[i] = (char *) malloc(maxSizeFile * sizeof(char));

            //Fill it
            int i = 0;
            while ((entry = readdir (dir)) != NULL) { 
                if(strcmp(".", entry->d_name) != 0 && strcmp("..", entry->d_name) != 0){
                    printf ("%s\n", entry->d_name); 
                    char* newPath = malloc(strlen(filePath) + strlen(entry->d_name) + 1);
                    strcat(newPath,filePath);
                    strcat(newPath,"/");
                    strcat(newPath,entry->d_name);
                    filesArr[i] = newPath;
                    i++;
                }               
            }
            for (unsigned int i = 0; i < numFiles; i++) {
                executer(mode, filesArr[i]);
            }
        }       
    }*/
}

int main(int argc, char* argv[], char* envp[]) {
    //TESTING
    /*
    //mode_t oldMode = getChmod("textfile.txt");
    mode_t oldMode = S_IROTH | S_IWOTH;
    mode_t newMode = S_IROTH;
    printf("oldmode:%o\n", oldMode);
    printf("newmode:%o\n", newMode);
    mode_t mode = oldMode & ~newMode;
    int mode2 = (int) mode;
    printf("mode:%o\n", mode);
    printf("mode2:%o\n", mode2);
    chmod("textfile.txt", mode2);
    int i = 777;
    printf("%o\n", convertOctalToDecimal(i));
    mode_t e = convertOctalToDecimal(i);
    chmod("textfile.txt", i);
    */

    
    //Variables
    char* filePath = "";
    char* mode = "";
    
    //Parse
    parse(argv, argc, &filePath, &mode);

    //TESTING
    if (vflag == V) printf("V: ok\n");
    else if (vflag == C) printf("C: ok\n");
    else if (vflag == NONE) printf("NONE: ok\n");
    else printf("Merdou\n");
    if (recursive) printf("Recursive: ok\n");
    else printf("Non recursive\n");
    printf("Mode:%s\n", mode);
    printf("File name:%s\n", filePath);

    executer(mode, filePath);  
    return 0;
}