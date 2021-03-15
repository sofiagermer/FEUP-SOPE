#include <dirent.h>
#include <signal.h>
#include "modes.h"
#include "options.h"
#include "time.h"
#include "string.h"

extern int errno;

typedef struct {
    char** arguments;
    int nArgs;
    int fileNameIndex;
} Arguments;

typedef struct {
    Options options;
    int noFilesFound;
    int noFilesChanged;
    Arguments args;
    char* filePath;
    char* modeString;
} ProcessInfo;

ProcessInfo* pInfo;

//BIGGER FUNCS
void parse(); //Parses arguments
void executer(char* fileName); //Recursive funtion

//AUXILIARY FUNCS
void diagnosticPrint(const char* filePath, const mode_t oldMode, const mode_t newMode); //Verbose messages
int checkFile(const char* filePath); //Checks if a file exists
bool isDirectory(const char* pathname); //Checks if file in pathname is a directory
bool checkChanges(const mode_t oldMode, const mode_t newMode); //Checks if there were changes
mode_t getFilePermissions(const char *path); //Returns the permissions of the file being treated
int get_bit(int bits, int pos);
char* fromOctalToString(mode_t mode); //Converts mode to a string to be printed
char* fourDigitOctal(mode_t mode);
void sigHandler(int signo); //Handles ctrlC
void setUpSigHandler(); //Set up the signal handling
double getMiliSeconds(clock_t initialTime); //Returns time in miliseconds since initialTime
void regitExecution(const char* filename, double time, pid_t pid, char* event, char* info); // Writes on execution register

double getMiliSeconds(clock_t initialTime){
    clock_t actualTime = clock();
    return (double)(actualTime-initialTime)/(CLOCKS_PER_SEC/1000);
}

void sigHandler(int signo) {
    printf("\nPID:%d FILEPATH:%s NUMBER OF FILES FOUND:%d NUMBER OF FILES MODIFIED:%d \n", getpid(), pInfo->filePath, pInfo->noFilesFound, pInfo->noFilesChanged);
    exit(1);
}

void setUpSigHandler() {
    struct sigaction new, old;
    sigset_t smask;
    if (sigemptyset(&smask) == -1) {
        fprintf(stderr, "Failed to set empty signals mask: %s\n", strerror(errno));
        exit(1);
    }
    new.sa_mask = smask;
    new.sa_flags = 0;
    new.sa_handler = sigHandler;
    if (sigaction(SIGINT, &new, &old) == -1) {
        fprintf(stderr, "Error with sigaction: %s", strerror(errno));
        exit(1);
    }
}

/*
char* initRegister(){
    char *filename = getenv("LOG_FILENAME");
    if(filename == NULL) printf("Environment variable Error \n");
    //Opens a text file for both reading and writing. 
    //It first truncates the file to zero length if it exists, otherwise creates a file if it does not exist.
    const char * mode = "w+";
    FILE *file = fopen(filename, mode);
    
    if(fclose(file) != 0) printf("Error closing register file \n"); 
    return filename;
}

void regitExecution(const char* filename, double time, pid_t pid, char* event, char* info){
    const char * mode = "a";
    FILE *file = fopen(filename, mode);
    fprintf(file, "%f", time);
    fputs(" ; ", file);
    fprintf(file, "%d", pid);
    fputs(" ; ", file);
    fputs(event, file);
    fputs(" ; ", file);
    fputs(info, file);
    fputs("\n",file);
    if(fclose(file) != 0) printf("Error closing register file \n"); 
}
*/

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

void diagnosticPrint(const char* filePath, const mode_t oldMode, const mode_t newMode) { 
    char* oldPermissions = fromOctalToString(oldMode), *newPermissions = fromOctalToString(newMode); 

    if(checkChanges(oldMode, newMode) && (pInfo->options.vflag == V || pInfo->options.vflag == C))
        printf("mode of '%s' changed from %s (%s) to %s (%s)\n",filePath, fourDigitOctal(oldMode), oldPermissions, fourDigitOctal(newMode), newPermissions);
    else if(!checkChanges(oldMode,newMode) && pInfo->options.vflag == V)
        printf("mode of '%s' retained as %s (%s)\n",filePath, fourDigitOctal(oldMode), oldPermissions);
}

int checkFile(const char* filePath) {
    if (access(filePath, F_OK) == -1) 
        return 1;
    return 0;
}

bool checkChanges(const mode_t oldMode, const mode_t newMode) {
    if (oldMode == newMode) return false;
    else return true;
}

void parse() { 

    bool isMode = true;
 
    //PARSE
    for (unsigned int i = 1; i < pInfo->args.nArgs; i++) {
        if (pInfo->args.arguments[i][0] == '-') {
            for (unsigned int j = 0; j < strlen(pInfo->args.arguments[i]); j++)
                processOption(pInfo->args.arguments[i][j], &pInfo->options);
        }
        else if (isMode) {
            pInfo->modeString = pInfo->args.arguments[i];
            isMode = false;
        }
        else {
            pInfo->filePath = pInfo->args.arguments[i];  
            pInfo->args.fileNameIndex = i;
        }    
    }    
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

void executer(char* filePath) { //IMPLEMENTING...


    mode_t oldMode;
    mode_t newMode;
    DIR *dir;
    struct dirent *entry;

    if (checkFile(filePath) != 0) {
        //regitExecution(registFileName, getMiliSeconds(initialTime), getpid(), "PROC_EXIT",  "1");
        fprintf(stderr, "File name wrong: %s", strerror(errno));
        exit(1);
    }
    else 
        pInfo->noFilesFound++;

    oldMode = getFilePermissions(filePath);
    newMode = getModeNum(pInfo->modeString, filePath, oldMode);

    if (chmod(filePath, newMode) != 0) {
        //regitExecution(registFileName, getMiliSeconds(initialTime), getpid(), "PROC_EXIT",  "1");
        fprintf(stderr, "Error with chmod:%s\n", strerror(errno));
        exit(1);
    }
    else {

        /* char info[strlen(filePath)+8+6+1];  //Nao funciona n percebo pq....
        sprintf(info,"%s : %s : %s",filePath,fourDigitOctal(oldMode),fourDigitOctal(newMode));
        regitExecution(registFileName, getMiliSeconds(initialTime), getpid(), "FILE_MODF", info); */
        pInfo->noFilesChanged++;
        diagnosticPrint(filePath, oldMode, newMode);
    }
    
    if (pInfo->options.recursive && isDirectory(filePath)) { //Verifica se é um directorio

        if ((dir = opendir (filePath)) == NULL) {
            fprintf(stderr, "Error with chmod:%s\n", strerror(errno));
            //regitExecution(registFileName, getMiliSeconds(initialTime), getpid(), "PROC_EXIT",  "1");
            exit(1);
        }

        while ((entry = readdir (dir)) != NULL) { 
            if(strcmp(".", entry->d_name) != 0 && strcmp("..", entry->d_name) != 0){

                //NEW FILE
                char* newPath = (char*) malloc(strlen(entry->d_name) + 1 + strlen(filePath));
                strcat(newPath,filePath);
                strcat(newPath,"/");
                strcat(newPath,entry->d_name);

                //VERIFY IF IT IS A DIRECTORY
                if (entry->d_type != DT_DIR) {
                    executer(newPath);
                    continue;
                }
                
                int id = fork();
                switch (id) {
                    case 0: {
                        //regitExecution(registFileName, getMiliSeconds(initialTime), getpid(), "PROC_CREAT" , "GET INFO!!!");
                        pInfo->noFilesChanged = 0;
                        pInfo->noFilesFound = 0;
                        
                        char* args[pInfo->args.nArgs + 1];
                        makeNewArgs(args, newPath);
                        printf("New file path:%s\n", args[pInfo->args.fileNameIndex]);
                        
                        if (execvp("/home/marhc/Documents/SOPE/FEUP-SOPE/MP1/main.exe", args) == -1) {
                            fprintf(stderr, "Error with execvpe: %s", strerror(errno));
                            exit(1);
                        }
                        sleep(20);
                        break;
                    }
                    case -1:{
                        fprintf(stderr, "Error with fork:%s\n", strerror(errno));
                        //regitExecution(registFileName, getMiliSeconds(initialTime), getpid(), "PROC_EXIT",  "1");
                        exit(1);
                    }
                    default: {
                        //printf("I am the parent\n");
                        break;
                    }
                }
                free(newPath);

            }               
        }

    }
    //regitExecution(registFileName, getMiliSeconds(initialTime), getpid(), "PROC_EXIT",  "0");
}

int main(int argc, char* argv[], char* envp[]) {;
    

    printf("%s", getenv("PATH"));
    exit(0);


    //const char * registFileName = initRegister(); 
    //clock_t initialTime = clock();
    pInfo = (ProcessInfo*) malloc(sizeof(ProcessInfo));

    pInfo->noFilesChanged = 0;
    pInfo->noFilesFound = 0;
    pInfo->args.arguments = argv;
    pInfo->args.nArgs = argc;
    
    setUpSigHandler();

    //Parse
    parse();


    //Recursive function
    executer(pInfo->filePath);
    sleep(10);
    free(pInfo);

    return 0;
}