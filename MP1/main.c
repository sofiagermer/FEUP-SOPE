#include <sys/wait.h>

#include "modes.h"
#include "utils.h"
#include "signal_handling.h"


extern int errno;

ProcessInfo* pInfo;

void parse(); //Parses arguments
void executer(char* fileName); //Recursive funtion (Acuatlly changes the modes of the files)
void initializeProcess(char* argv[], int argc); //To initialize the struct and define signal handlers

void parse() { 

    bool isMode = true;
 
    //PARSE (get the arguments from argv to the struct)
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

void executer(char* filePath) { 
    
    mode_t oldMode = getFilePermissions(filePath);
    mode_t newMode = getModeNum(pInfo->modeString, filePath, oldMode);

    //If it can't get the old permissions
    if (oldMode == 999999) {
        printf("mode of '%s' could not be changed\n", filePath);
        return;
    }

    if (chmod(filePath, newMode) != 0) {
        fprintf(stderr, "Error with chmod:%s\n", strerror(errno));
        endProgram(1);
    } else {
        registFileModf(oldMode, newMode, filePath);
        diagnosticPrint(filePath, oldMode, newMode); 
        pInfo->noFilesChanged++;
    }
    
    if (pInfo->options.recursive && isDirectory(filePath)) { //Verifica se é um directorio

        DIR *dir;
        struct dirent *entry;

        if ((dir = opendir (filePath)) == NULL) {
            fprintf(stderr, "Error with chmod:%s\n", strerror(errno));
            endProgram(1);
        }

        while ((entry = readdir (dir)) != NULL) { 
            if (strcmp(".", entry->d_name) != 0 && strcmp("..", entry->d_name) != 0){

                //NEW FILE
                size_t sizeNewPath = strlen(entry->d_name) + 2 + strlen(filePath); // +2, 1 because of the "/" and 1 because of the null terminator
                char* newPath = (char*) malloc(sizeNewPath);
                snprintf(newPath,sizeNewPath,"%s/%s",filePath,entry->d_name);

                //VERIFY IF IT IS A DIRECTORY
                if (entry->d_type != DT_DIR) {
                    executer(newPath);
                } else {
                    hasAChild(newPath);
                }

                free(newPath);
            }               
        }
        free(dir);
    }
}

void initializeProcess(char* argv[], int argc) {

    //Allocate memory for struct with process info
    pInfo = (ProcessInfo*) malloc(sizeof(ProcessInfo));

    if(pInfo == NULL) {
        fprintf(stderr, "Failed to allocate memory for pInfo\n");
        endProgram(1);
    }

    //Check if it's an initial process
    if(!getenv("firstRun")){
        initRegister();
        pInfo->isInitial = true;
    }
    else 
        pInfo->isInitial = false;

    pInfo->noFilesChanged = 0;
    pInfo->noFilesFound = 0;
    pInfo->args.arguments = argv;
    pInfo->args.nArgs = argc;  
    pInfo->options.recursive = false;
    pInfo->options.vflag = NONE;
    pInfo->regist = true;
    pInfo->noChildren = 0;
     
    //Sets the signal handling
    setUpSigHandlers();
}

int main(int argc, char* argv[], char* envp[]) {

    //Initialize Process
    initializeProcess(argv, argc);

    //Parse
    parse();

    //Recursive function
    executer(pInfo->filePath);
    
    
    //Waits for child processes to finish
    for(int i = 0; i < pInfo->noChildren; i++)
        wait(NULL);

    endProgram(0);

    return 0;
}
