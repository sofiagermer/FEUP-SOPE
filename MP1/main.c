#include <sys/wait.h>
#include <signal.h>
#include <time.h>
#include "headers/modes.h"
#include "headers/options.h"
#include "headers/utils.h"

extern int errno;

typedef struct {
    Options options;
    int noFilesFound;
    int noFilesChanged;
    Arguments args;
    char* filePath;
    char* modeString;
    bool isInitial;
    bool isParent;
    bool sleep;
    int* childrenPIDs;
    int noChildren;
} ProcessInfo;

ProcessInfo* pInfo;

//BIGGER FUNCS
void parse(); //Parses arguments
void executer(char* fileName); //Recursive funtion

//AUXILIARY FUNCS
void sigHandlerSigInt(int signo); //Handles ctrlC
void sigHandlerSigIntInitial(int signo);
void setUpSigInt(); //Set up the signal handling
double getMiliSeconds(double initialTime); //Returns time in miliseconds since initialTime
void regitExecution(pid_t pid, char* event, char* info); // Writes on execution register
void endProgram(); //To kill everything
void initializeProcess(char* argv[], int argc); //To initialize the struct and define signal handlers
void makeNewArgs(char* newArgs[], char* fileName); //To fabricate the arguments for the new process
void initRegister();
void setUpOtherSigs();
void setUpSigAlrm();
void sigHandlerSigAlrm(int signo);
void setUpSigTerm();
void sigHandlerSigTerm(int signo);
void registSignalSent(char* signalSent, int pid);

double getMiliSeconds(double initialTime){
    struct timespec t;
    clock_gettime(CLOCK_REALTIME,&t);
    char aux[20];
    snprintf(aux,sizeof(aux),"0.%ld",t.tv_nsec);
    double curTime=(double)t.tv_sec+strtod(aux,NULL);
    return (double)(curTime - initialTime)*1000;
}

void sigHandlerSigInt(int signo) {
    regitExecution(getpid(), "SIGNAL_RECV", "SIGINT");
    printf("\nPID:%d FILEPATH:%s NUMBER OF FILES FOUND:%d NUMBER OF FILES MODIFIED:%d\n",
     getpid(), pInfo->filePath, pInfo->noFilesFound, pInfo->noFilesChanged);
    pInfo->sleep = true;
    while(pInfo->sleep) sleep(1);
}

void sigHandlerSigAlrm(int signo) {
    regitExecution(getpid(), "SIGNAL_RECV", "SIGALRM");
    pInfo->sleep = false;
    if (pInfo->isParent) {
        for (unsigned int i = 0; i < pInfo->noChildren; i++) {
            kill(pInfo->childrenPIDs[i], SIGALRM);   
            registSignalSent("SIGALRM", pInfo->childrenPIDs[i]);
        }
    }
}

void sigHandlerSigTerm(int signo) {
    regitExecution(getpid(), "SIGNAL_RECV", "SIGTERM");
    if (pInfo->isParent) {
        for (unsigned int i = 0; i < pInfo->noChildren; i++) {
            kill(pInfo->childrenPIDs[i], SIGTERM);   
            registSignalSent("SIGTERM", pInfo->childrenPIDs[i]);
        }
    }
    endProgram(1);
}

void registSignalSent(char* signalSent, int pid) {
    char info[20];
    snprintf(info,sizeof(info), "%s : %d", signalSent, pid);
    regitExecution(getpid(), "SIGNAL_SENT", info);
}

void sigHandlerSigIntInitial(int signo) {
    char answer[2];

    regitExecution(getpid(), "SIGNAL_RECV", "SIGINT");
    printf("\nPID:%d FILEPATH:%s NUMBER OF FILES FOUND:%d NUMBER OF FILES MODIFIED:%d \n", getpid(), pInfo->filePath, pInfo->noFilesFound, pInfo->noFilesChanged);
    sleep(0.2);
    printf("\nDO YOU WANT TO WANT THE EXECUTION TO PROCEED?(y/n)");
    scanf("%s", answer);

    while(1) {
        if(strcmp(answer,"y") == 0) {
            for (unsigned int i = 0; i < pInfo->noChildren; i++) {
                kill(pInfo->childrenPIDs[i], SIGALRM);
                registSignalSent("SIGALRM", pInfo->childrenPIDs[i]);
            }
            break;
        } else if (strcmp(answer,"n") == 0) {
            for (unsigned int i = 0; i < pInfo->noChildren; i++) {
                kill(pInfo->childrenPIDs[i], SIGTERM);
                registSignalSent("SIGTERM", pInfo->childrenPIDs[i]);
            }
            endProgram(1);
        } else {
            printf("NOT A VALID ANSWER\n");
        }
    } 
}

void setUpSigHandlers () {
    setUpSigAlrm();
    setUpOtherSigs();
    setUpSigInt();
    setUpSigTerm();
}

void setUpOtherSigs() {
    /*struct sigaction new, old;
    sigset_t smask;
    if (sigemptyset(&smask) == -1) {
        fprintf(stderr, "Failed to set empty signals mask: %s\n", strerror(errno));
        exit(1);
    }
    new.sa_mask = smask;
    new.sa_flags = 0;
    new.sa_handler = sigHandlerOtherSigs;
    if (sigaction(SIGIO, &new, &old) == -1) {
        fprintf(stderr, "Error with sigaction: %s", strerror(errno));
        exit(1);
    }
    if (sigaction(SIGPIPE, &new, &old) == -1) {
        fprintf(stderr, "Error with sigaction: %s", strerror(errno));
        exit(1);
    }*/

}

void setUpSigTerm() {
    struct sigaction new, old;
    sigset_t smask;
    if (sigemptyset(&smask) == -1) {
        fprintf(stderr, "Failed to set empty signals mask: %s\n", strerror(errno));
        exit(1);
    }
    new.sa_mask = smask;
    new.sa_flags = 0;
    new.sa_handler = sigHandlerSigTerm;
    if (sigaction(SIGTERM, &new, &old) == -1) {
        fprintf(stderr, "Error with sigaction: %s", strerror(errno));
        exit(1);
    }
}

void setUpSigAlrm() {
    struct sigaction new, old;
    sigset_t smask;
    if (sigemptyset(&smask) == -1) {
        fprintf(stderr, "Failed to set empty signals mask: %s\n", strerror(errno));
        exit(1);
    }
    new.sa_mask = smask;
    new.sa_flags = 0;
    if (pInfo->isInitial)
        new.sa_handler = sigHandlerSigAlrm;
    else
        new.sa_handler = sigHandlerSigAlrm;
    if (sigaction(SIGALRM, &new, &old) == -1) {
        fprintf(stderr, "Error with sigaction: %s", strerror(errno));
        exit(1);
    }
}

void setUpSigInt () {
    struct sigaction new, old;
    sigset_t smask;
    if (sigemptyset(&smask) == -1) {
        fprintf(stderr, "Failed to set empty signals mask: %s\n", strerror(errno));
        exit(1);
    }
    new.sa_mask = smask;
    new.sa_flags = 0;
    if (pInfo->isInitial)
        new.sa_handler = sigHandlerSigIntInitial;
    else
        new.sa_handler = sigHandlerSigInt;
    if (sigaction(SIGINT, &new, &old) == -1) {
        fprintf(stderr, "Error with sigaction: %s", strerror(errno));
        exit(1);
    }
}

void initRegister () {
    struct timespec t;
    clock_gettime(CLOCK_REALTIME,&t);
    char *filename = getenv("LOG_FILENAME");
    if(filename == NULL) {
        fprintf(stderr, "Environment variable error: no such file or directory\n");
        endProgram(1);
    } else{
        char timeString[50];
        snprintf(timeString,sizeof(timeString),"%lld.%.9ld", (long long)t.tv_sec, t.tv_nsec);
        setenv("firstRun",timeString,1);

        //Opens a text file for both reading and writing. 
        //It first truncates the file to zero length if it exists, otherwise creates a file if it does not exist.
        FILE *file = fopen(filename, "w+");
        if(fclose(file) != 0) printf("Error closing register file \n");
    }
}

void regitExecution( pid_t pid, char* event, char* info){
    char* filename = getenv("LOG_FILENAME");
    if(filename == NULL) {
        fprintf(stderr, "Environment variable error: no such file or directory\n");
        endProgram(1);
    }
    double initialTime = strtod(getenv("firstRun"),NULL);
    double time = getMiliSeconds(initialTime);

    FILE *file = fopen(filename, "a");
    char* newBuffer = (char*) malloc(300 * sizeof(char));
    setvbuf(file, newBuffer, _IOLBF, 300);
    fprintf(file, "%f ; %d ; %s ; %s\n", time, pid, event, info);
    free(newBuffer);

    if(fclose(file) != 0) {
        fprintf(stderr, "Error closing register file: %s\n", strerror(errno));
        endProgram(1);
    }

}

void parse() { 

    bool isMode = true;
 
    //PARSE
    for (unsigned int i = 1; i < pInfo->args.nArgs; i++) {
        if (pInfo->args.arguments[i][0] == '-') {
            for (unsigned int j = 0; j < strlen(pInfo->args.arguments[i]); j++)
                processOption(pInfo->args.arguments[i][j], &pInfo->options);
        } else if (isMode) {
            pInfo->modeString = pInfo->args.arguments[i];
            isMode = false;
        } else {
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

void becomingAParent() {
    if (!pInfo->isParent) {
        pInfo->isParent = true;
        pInfo->childrenPIDs = (int*) malloc(100 * sizeof(int));
    }
}

void registProcessExit(int exitStatus) {
    char info[10];
    snprintf(info,sizeof(info), "%d", exitStatus);
    regitExecution(getpid(), "PROC_EXIT", info);
}

void registProcessCreation(char* args[]) {
    char info[100] = "";
    for (unsigned int i = 0; i < pInfo->args.nArgs; i++) {
        snprintf(info,sizeof(info),"%s ,",args[i]);
    }
    regitExecution(getpid(), "PROC_CREAT" , info);

}

void hasAChild(char* newPath) {
    //GETTING READY TO SAVE CHILDREN'S PIDS
    becomingAParent();
    
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
            pInfo->childrenPIDs[pInfo->noChildren] = id;
            pInfo->noChildren++;
            break;
        }
    }
}

void registFileModf(mode_t oldMode, mode_t newMode, char* filePath) {
    char* fourDigitOldMode = (char*)malloc(sizeof(oldMode));
    fourDigitOctal(oldMode,fourDigitOldMode);
    char* fourDigitNewMode = (char*)malloc(sizeof(newMode));
    fourDigitOctal(newMode,fourDigitNewMode);
    size_t size=strlen(filePath)+strlen(fourDigitOldMode) + strlen(fourDigitNewMode) + 20;
    char *info=(char*)malloc(size); 
    snprintf(info,size, "%s : %s : %s", filePath, fourDigitOldMode, fourDigitNewMode);
    regitExecution(getpid(), "FILE_MODF", info); 
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
    } else {
        registFileModf(oldMode, newMode, filePath);
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
                size_t sizeNewPath=strlen(entry->d_name) + 2 + strlen(filePath); // +2, 1 because of the "/" and 1 because of the null terminator
                char* newPath = (char*) malloc(sizeNewPath);
                snprintf(newPath,sizeNewPath,"%s/%s",filePath,entry->d_name);


                //VERIFY IF IT IS A DIRECTORY
                if (entry->d_type != DT_DIR) {
                    executer(newPath);
                    continue;
                }
                hasAChild(newPath);

                free(newPath);

            }               
        }

    }
   regitExecution(getpid(), "PROC_EXIT",  "0");
}

void initializeProcess(char* argv[], int argc) {
    pInfo = (ProcessInfo*) malloc(sizeof(ProcessInfo));

    if(!getenv("firstRun")){
        initRegister();
        pInfo->isInitial = true;
    }
    else    
        pInfo->isInitial = false;

    pInfo->isParent = false;
    pInfo->noFilesChanged = 0;
    pInfo->noFilesFound = 0;
    pInfo->args.arguments = argv;
    pInfo->args.nArgs = argc;  
    pInfo->childrenPIDs = NULL;
    pInfo->noChildren = 0;
     

    setUpSigHandlers();
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

int main(int argc, char* argv[], char* envp[]) {
    //Initialize Process
    initializeProcess(argv, argc);

    //Parse
    parse();

    //Recursive function
    executer(pInfo->filePath);
    
    wait(NULL); //Waits for child processes to finish

    while(1) sleep(1);
    endProgram(0);
    

    return 0;
}


//BEWARE! UNCOMMENT LINES!