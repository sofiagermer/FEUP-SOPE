#include <sys/wait.h>
#include <signal.h>
#include <time.h>
#include "modes.h"
#include "options.h"
#include "signal_handling.h"


extern int errno;

ProcessInfo* pInfo;

//BIGGER FUNCS
void parse(); //Parses arguments
void executer(char* fileName); //Recursive funtion
void endProgram(); //To kill everything
void initializeProcess(char* argv[], int argc); //To initialize the struct and define signal handlers
void makeNewArgs(char* newArgs[], char* fileName); //To fabricate the arguments for the new process
void initRegister();

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
    pInfo->options.recursive = false;
    pInfo->options.vflag = NONE;
     

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

    //while(1) sleep(1);
    endProgram(0);
    

    return 0;
}
