#include "logfile.h"

void registSignalSent(char* signalSent, int pid) {

    //Building info string and writing the line in the log file
    char info[20];
    snprintf(info, sizeof(info), "%s : %d", signalSent, pid);
    regitExecution(getpid(), "SIGNAL_SENT", info);
}

void regitExecution( pid_t pid, char* event, char* info){

    if (!pInfo->regist) return;

    //Opening log file
    char* filename = getenv("LOG_FILENAME");
    if(filename == NULL) {
        pInfo->regist = false;
        return;
    }

    FILE *file = fopen(filename, "a");
    if(file == NULL) {
        fprintf(stderr, "Failed to open file %s:%s\n", filename, strerror(errno));
        endProgram(1);
    } 

    //Calculating time since beggining
    double initialTime = strtod(getenv("firstRun"),NULL);
    double time = getMiliSeconds(initialTime);

    //Preparing line buffering and writing to the file
    char* newBuffer = (char*) malloc(300 * sizeof(char));
    setvbuf(file, newBuffer, _IOLBF, 300);
    fprintf(file, "%f ; %d ; %s ; %s\n", time, pid, event, info);
    free(newBuffer);

    //Closing file
    if(fclose(file) != 0) {
        fprintf(stderr, "Error closing register file: %s\n", strerror(errno));
        endProgram(1);
    }
}

void initRegister () {

    //Initializing the process starting time
    struct timespec t;
    clock_gettime(CLOCK_REALTIME,&t);
    char timeString[50];
    snprintf(timeString,sizeof(timeString),"%ld.%.9ld", (int64_t)t.tv_sec, t.tv_nsec);
    setenv("firstRun",timeString,1);

    //Veryfying if the environment variable is set
    char *filename = getenv("LOG_FILENAME");
    if(filename == NULL) {
        fprintf(stderr, "Environment variable error: LOG_FILENAME not set\n");
        return;
    }
    
    //Opens a text file for both reading and writing. 
    //It first truncates the file to zero length if it exists, otherwise creates a file if it does not exist.
    FILE *file = fopen(filename, "w+");
    if (file == NULL) {
        fprintf(stderr, "Failed to open file %s:%s\n", filename, strerror(errno));
        endProgram(1);
    }
    if(fclose(file) != 0) {
        fprintf(stderr, "Error closing register file:%s\n", strerror(errno));
        endProgram(1);
    }
}

void registProcessExit(int exitStatus) {

    //Get the exit status and write it on the log file
    char info[10];
    snprintf(info,sizeof(info), "%d", exitStatus);
    regitExecution(getpid(), "PROC_EXIT", info);
}

void registProcessCreation(char* args[]) {

    //Build the string with the process's arguments and registers it 
    char info[100] ;
    for (unsigned int i = 0; i < pInfo->args.nArgs; i++) {
        snprintf(info,sizeof(info),"%s ,",args[i]);
    }
    regitExecution(getpid(), "PROC_CREAT" , info);
}

void registFileModf(mode_t oldMode, mode_t newMode, char* filePath) {

    //Gets a string of the old mode
    char* fourDigitOldMode = (char*) malloc(10 * sizeof(char));
    fourDigitOctal(oldMode,fourDigitOldMode);
    if(fourDigitOldMode == NULL) {
        fprintf(stderr, "Error getting old mode string\n");
        endProgram(1);
    }

    //Gets a string of the new mode
    char* fourDigitNewMode = (char*) malloc(10 * sizeof(char));
    fourDigitOctal(newMode,fourDigitNewMode);
    if(fourDigitNewMode == NULL) {
        fprintf(stderr, "Error getting new mode string\n");
        endProgram(1);
    }

    //Writes the info to the log file apropriattely
    size_t size = strlen(filePath) + strlen(fourDigitOldMode) + strlen(fourDigitNewMode) + 20;
    char *info = (char*) malloc(size); 
    snprintf(info,size, "%s : %s : %s", filePath, fourDigitOldMode, fourDigitNewMode);
    regitExecution(getpid(), "FILE_MODF", info); 
    free(fourDigitNewMode);
    free(fourDigitOldMode);
    free(info);
}
