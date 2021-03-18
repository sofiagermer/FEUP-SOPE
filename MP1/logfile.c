#include "logfile.h"


void registSignalSent(char* signalSent, int pid) {
    char info[20];
    snprintf(info,sizeof(info), "%s : %d", signalSent, pid);
    regitExecution(getpid(), "SIGNAL_SENT", info);
}

void regitExecution( pid_t pid, char* event, char* info){
    char* filename = getenv("LOG_FILENAME");
    if(filename == NULL) {
        //fprintf(stderr, "Environment variable error: no such file or directory\n");
        //endProgram(1);
        return;
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

void initRegister () {
    struct timespec t;
    clock_gettime(CLOCK_REALTIME,&t);
    char timeString[50];
    snprintf(timeString,sizeof(timeString),"%ld.%.9ld", (int64_t)t.tv_sec, t.tv_nsec);
    setenv("firstRun",timeString,1);
    char *filename = getenv("LOG_FILENAME");
    if(filename == NULL) {
        //fprintf(stderr, "Environment variable error: no such file or directory\n");
        //endProgram(1);
        return;
    }
    //Opens a text file for both reading and writing. 
    //It first truncates the file to zero length if it exists, otherwise creates a file if it does not exist.
    FILE *file = fopen(filename, "w+");
    if(fclose(file) != 0) printf("Error closing register file \n");
    
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

void registFileModf(mode_t oldMode, mode_t newMode, char* filePath) {
    char* fourDigitOldMode = (char*)malloc(10 * sizeof(char));
    fourDigitOctal(oldMode,fourDigitOldMode);
    char* fourDigitNewMode = (char*)malloc(10 * sizeof(char));
    fourDigitOctal(newMode,fourDigitNewMode);
    size_t size=strlen(filePath)+strlen(fourDigitOldMode) + strlen(fourDigitNewMode) + 20;
    char *info=(char*)malloc(size); 
    snprintf(info,size, "%s : %s : %s", filePath, fourDigitOldMode, fourDigitNewMode);
    regitExecution(getpid(), "FILE_MODF", info); 
}

double getMiliSeconds(double initialTime) {
    struct timespec t;
    clock_gettime(CLOCK_REALTIME,&t);
    char aux[20];
    snprintf(aux,sizeof(aux),"0.%ld",t.tv_nsec);
    double curTime=(double)t.tv_sec+strtod(aux,NULL);
    return (double)(curTime - initialTime)*1000;
}