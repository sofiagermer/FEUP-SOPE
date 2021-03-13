#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include <sys/types.h> 
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <dirent.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>


typedef enum {C, V, NONE} Verbose;

extern int errno;

bool recursive;
Verbose vflag = NONE;

//BIGGER FUNCS
void parse(char* arguments[], int nArgs, char** filePath, char** mode); //Parses arguments
void executer(const char* mode, const char* filePath); //Recursive funtion

//AUXILIARY FUNCS RELATED TO MODE
bool checkOctalMode(const char* octalMode); //Checks if octal mode's structure is valid
mode_t findMode(const char* mode, const char* filePath, const mode_t oldMode); //Returns the mode to be applied (in case of MODE option)
mode_t getModeNum(const char* mode, const char* filePath, const mode_t oldMode); //Decides between octal and MODE and returns numerical code for mode
mode_t getFilePermissions(const char *path); //Returns the permissions of the file being treated

//AUXILIARY FUNCS RELATED TO OTHERS
mode_t convert(int octal); //To convert octal do decimal
void diagnosticPrint(const char* filePath, const mode_t oldMode, const mode_t newMode); //Verbose messages
void processOption(const char optFlag); //Processes an option
void checkFile(const char* filePath); //Checks if a file exists
bool isDirectory(const char* pathname); //Checks if file in pathname is a directory
bool checkChanges(const mode_t oldMode, const mode_t newMode); //Checks if there were changes
char* convertModeToString(mode_t mode); //Converts a mode to its string


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

bool checkOctalMode(const char* octalMode) {
    if (strlen(octalMode) > 4) return false;
    if (octalMode[0] != '0') return false;
    if (octalMode[1] == '0' || octalMode[1] == '1' || octalMode[1] == '2' || octalMode[1] == '3' || octalMode[1] == '4' || octalMode[1] == '5' || octalMode[1] == '6' || octalMode[1] == '7'){
        if (octalMode[2] == '0' || octalMode[2] == '1' || octalMode[2] == '2' || octalMode[2] == '3' || octalMode[2] == '4' || octalMode[2] == '5' || octalMode[2] == '6' || octalMode[2] == '7'){
            if (octalMode[3] == '0' || octalMode[3] == '1' || octalMode[3] == '2' || octalMode[3] == '3' || octalMode[3] == '4' || octalMode[3] == '5' || octalMode[3] == '6' || octalMode[3] == '7'){
                return true;;
            }
        }
    }
    return false;
}

mode_t findMode(const char* mode, const char* filePath, const mode_t oldMode) {

    mode_t newMode = 0;
    int length = strlen(mode);
    char target = mode[0];
    char operation = mode[1];

    switch (target) {
        case 'u': {
            for (unsigned int i = 2; i < length; i++) {
                switch (mode[i]) {
                    case 'r': {
                        newMode |= S_IRUSR;
                        break;
                    }
                    case 'w': {
                        newMode |= S_IWUSR;
                        break;
                    }
                    case 'x': {
                        newMode |= S_IXUSR;
                        break;
                    }
                }
            }
            break;
        }
        case 'g': {
            for (unsigned int i = 2; i < length; i++) {
                switch (mode[i]) {
                    case 'r': {
                        newMode |= S_IRGRP;
                        break;
                    }
                    case 'w': {
                        newMode |= S_IWGRP;
                        break;
                    }
                    case 'x': {
                        newMode |= S_IXGRP;
                        break;
                    }
                }
            }
            break;
        }
        case 'o': {
            for (unsigned int i = 2; i < length; i++) {
                switch (mode[i]) {
                    case 'r': {
                        newMode |= S_IROTH;
                        break;
                    }
                    case 'w': {
                        newMode |= S_IWOTH;
                        break;
                    }
                    case 'x': {
                        newMode |= S_IXOTH;
                        break;
                    }
                }
            }
            break;
        }
        case 'a': {
            for (unsigned int i = 2; i < length; i++) {
                switch (mode[i]) {
                    case 'r': {
                        newMode |= S_IRUSR | S_IRGRP | S_IROTH;
                        break;
                    }
                    case 'w': {
                        newMode |= S_IWUSR | S_IWGRP | S_IWOTH;
                        break;
                    }
                    case 'x': {
                        newMode |= S_IXUSR | S_IXGRP | S_IXOTH;
                        break;
                    }
                }
            }
            break;
        }
        default: {
            fprintf(stderr, "Invalid target identifier:%c\n", target);
            exit(1);
        }
    }

    switch (operation) {
        case '+': {
            return oldMode | newMode;
        }
        case '-': {
            return oldMode & ~newMode;
        }
        case '=': {
            return newMode;
        }
        default: {
            fprintf(stderr, "Invalid type of mode operation:%c\n", operation);
            exit(1);
        }
    } 
}

void diagnosticPrint(const char* filePath, const mode_t oldMode, const mode_t newMode) { 
    char* oldPermissions = "", *newPermissions = ""; 
    
    printf("mode of '%s' changed from %o (%s) to %o (%s)\n",filePath, oldMode, oldPermissions, newMode, newPermissions);
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

mode_t getModeNum(const char* mode, const char* filePath, const mode_t oldMode) {
    mode_t modeNum;
    int octal;
    if ((octal = atoi(mode)) != 0) {
        if (!checkOctalMode(mode)) {
            fprintf(stderr, "Invalid format for octal mode:%s\n", mode);
            exit(1);
        }
        modeNum = convert(octal);
    }
    else 
        modeNum = findMode(mode, filePath, oldMode);
    return modeNum;
}

bool checkChanges(const mode_t oldMode, const mode_t newMode) {
    if (oldMode == newMode) return false;
    else return true;
}

mode_t convert(int octal) {
    int decimal = 0, i = 0;

    while(octal != 0) {
        decimal += (octal%10) * pow(8,i);
        ++i;
        octal/=10;
    }

    return decimal;
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

char* convertModeToString(mode_t mode) { //NOT YET IMPLEMENTED
    int user, group, other;
    char string1[9] = "";
    user = mode / 10;
    group = (mode / 10) % 10;
    return "";
}

void executer(const char* mode, const char* filePath) { //Will actually use chmod function (recursively if necessary)

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
    if ((vflag == C && checkChanges(oldMode, newMode)) || vflag == V) 
        diagnosticPrint(filePath, oldMode, newMode);

    if (recursive && isDirectory(filePath)) { //Verifica se é um directorio

        if ((dir = opendir (filePath)) != NULL) {
            while ((entry = readdir (dir)) != NULL) { 
                if(strcmp(".", entry->d_name) != 0 && strcmp("..", entry->d_name) != 0){
                    printf ("%s\n", entry->d_name); 
                    char* newPath = malloc(strlen(filePath) + strlen(entry->d_name) + 1);
                    strcat(newPath,filePath);
                    strcat(newPath,"/");
                    strcat(newPath,entry->d_name);
                }
                
            }
        }
        
    }
    

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