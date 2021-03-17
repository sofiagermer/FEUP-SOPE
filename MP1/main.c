#include <signal.h>
#include "modes.h"
#include "options.h"
#include "utils.h"
#include <time.h>
#include<sys/wait.h>
#include <stdlib.h>
#include <string.h>

extern int errno;

typedef struct {
    Options options;
    int noFilesFound;
    int noFilesChanged;
    Arguments args;
    char* filePath;
    char* modeString;
    bool isParent;
} ProcessInfo;


ProcessInfo* pInfo;

//BIGGER FUNCS
void parse(); //Parses arguments
void executer(char* fileName); //Recursive funtion

//AUXILIARY FUNCS
void sigHandler(int signo); //Handles ctrlC
void setUpSigHandler(); //Set up the signal handling
double getMiliSeconds(double initialTime); //Returns time in miliseconds since initialTime
bool regitExecution(pid_t pid, char* event, char* info); // Writes on execution register
void endProgram(); //To kill everything
void initializeProcess(char* argv[], int argc); //To initialize the struct and define signal handlers
void makeNewArgs(char* newArgs[], char* fileName); //To fabricate the arguments for the new process
void initRegister();

double getMiliSeconds(double initialTime){
    struct timespec t;
    clock_gettime(CLOCK_REALTIME,&t);
    char aux[20];
    sprintf(aux,"0.%ld",t.tv_nsec);
    double curTime=(double)t.tv_sec+strtod(aux,NULL);
    return (double)(curTime - initialTime)*1000;
}

void sigHandler(int signo) {
    char answer[2];
    printf("\nPID:%d FILEPATH:%s NUMBER OF FILES FOUND:%d NUMBER OF FILES MODIFIED:%d \n", getpid(), pInfo->filePath, pInfo->noFilesFound, pInfo->noFilesChanged);
    while(1){
        if(pInfo->isParent){
            printf("\nDO YOU WANT TO WANT THE EXECUTION TO PROCEED?(y/n)\n");
            scanf("%s", answer);
            if(strcmp(answer,"y")){
                //funçaozinha
            }
            else if (strcmp(answer,"n\n")){
                exit(1);
            }
            else{
                printf("NOT A VALID ANSWER\n");
            }
        }
    }
    
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

void initRegister(){
    struct timespec t;
    clock_gettime(CLOCK_REALTIME,&t);
    char *filename=getenv("LOG_FILENAME");
    if(filename == NULL) {
        printf("Environment variable Error \n");
    }
    else{
        char timeString[50];
        sprintf(timeString,"%lld.%.9ld", (long long)t.tv_sec, t.tv_nsec);
        setenv("firstRun",timeString,1);
        //Opens a text file for both reading and writing. 
        //It first truncates the file to zero length if it exists, otherwise creates a file if it does not exist.
        FILE *file = fopen(filename, "w+");
        if(fclose(file) != 0) printf("Error closing register file \n");
    }
}

bool regitExecution( pid_t pid, char* event, char* info){
    char* filename=getenv("LOG_FILENAME");
    if(filename == NULL) {
        printf("Environment variable Error \n");
        return false;
    }
    double initialTime=strtod(getenv("firstRun"),NULL);
    double time=getMiliSeconds(initialTime);
    FILE *file = fopen(filename, "a");
    char newBuffer[300];
    setvbuf(file,newBuffer,_IOLBF,300);
    fprintf(file,"%f ; %d ; %s ; %s\n",time,pid,event,info);
    if(fclose(file) != 0) printf("Error closing register file \n");
    return true;
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

void executer(char* filePath) { 

    mode_t oldMode;
    mode_t newMode;
    DIR *dir;
    struct dirent *entry;
    
    oldMode = getFilePermissions(filePath);
    newMode = getModeNum(pInfo->modeString, filePath, oldMode);

    if (access(filePath, F_OK) == -1) {
        regitExecution( getpid(), "PROC_EXIT",  "1");
        fprintf(stderr, "File name wrong: %s", strerror(errno));
        exit(1);
    }
    else 
        pInfo->noFilesFound++;


    if (chmod(filePath, newMode) != 0) {
        regitExecution(getpid(), "PROC_EXIT",  "1");
        fprintf(stderr, "Error with chmod:%s\n", strerror(errno));
        exit(1);
    }
    else {
        char* fourDigitOldMode=(char*)malloc(sizeof(oldMode));
        fourDigitOctal(oldMode,fourDigitOldMode);
        char* fourDigitNewMode=(char*)malloc(sizeof(newMode));
        fourDigitOctal(newMode,fourDigitNewMode);
        char *info=(char*)malloc(strlen(filePath)+strlen(fourDigitOldMode)+strlen(fourDigitNewMode)+100);  //Nao funciona n percebo pq.... 
        strcpy(info,filePath);
        strcat(info," : ");
        strcat(info,fourDigitOldMode);
        strcat(info," : ");
        strcat(info,fourDigitNewMode); 
        regitExecution(getpid(), "FILE_MODF", info); 
        diagnosticPrint(filePath, oldMode, newMode, pInfo->options); 
        pInfo->noFilesChanged++;
    }
    
    if (pInfo->options.recursive && isDirectory(filePath)) { //Verifica se é um directorio

        if ((dir = opendir (filePath)) == NULL) {
            fprintf(stderr, "Error with chmod:%s\n", strerror(errno));
            regitExecution(getpid(), "PROC_EXIT",  "1");
            exit(1);
        }

        while ((entry = readdir (dir)) != NULL) { 
            if(strcmp(".", entry->d_name) != 0 && strcmp("..", entry->d_name) != 0){

                //NEW FILE
                char* newPath = (char*) malloc(strlen(entry->d_name) + 1 + strlen(filePath));
                strcpy(newPath,filePath);
                strcat(newPath,"/");
                strcat(newPath,entry->d_name);

                //VERIFY IF IT IS A DIRECTORY
                if (entry->d_type != DT_DIR) {
                    executer(newPath);
                    continue;
                }
                
                regitExecution(getpid(), "PROC_CREAT" , "GET INFO!!!");
                char* args[pInfo->args.nArgs + 1];
                makeNewArgs(args, newPath);
                

                int id = fork();
                switch (id) {
                    case 0: {
                        regitExecution(getpid(), "PROC_CREAT" , "GET INFO!!!");
                        char* args[pInfo->args.nArgs + 1];
                        makeNewArgs(args, newPath);
                        
                        if (execvp(args[0], args) == -1) {
                            fprintf(stderr, "Error with execvpe: %s", strerror(errno));
                            exit(1);
                        }
                        break;
                    }
                    case -1:{
                        fprintf(stderr, "Error with fork:%s\n", strerror(errno));
                        regitExecution(getpid(), "PROC_EXIT",  "1");
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
   regitExecution(getpid(), "PROC_EXIT",  "0");
}

void initializeProcess(char* argv[], int argc) {
    pInfo = (ProcessInfo*) malloc(sizeof(ProcessInfo));
    pInfo->isParent=false;
    if(!getenv("firstRun")){
        initRegister();
        pInfo->isParent=true;
    }
    pInfo->noFilesChanged = 0;
    pInfo->noFilesFound = 0;
    pInfo->args.arguments = argv;
    pInfo->args.nArgs = argc;  
     

    setUpSigHandler();
}

void endProgram() {

    if(pInfo->isParent){
        unsetenv("firstRun");
    }
    
    free(pInfo);
}

int main(int argc, char* argv[], char* envp[]) {
    //Initialize Process
    initializeProcess(argv, argc);

    //Parse
    parse();
    printf("%d\n",getpid());
    //Recursive function
    executer(pInfo->filePath);
    
    wait(NULL); //Waits for child processes to finish
    //("pid : %d \n", getpid());
    //while(1) sleep(1);
    endProgram();
    

    return 0;
}


//BEWARE! UNCOMMENT LINES!