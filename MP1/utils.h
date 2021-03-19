#pragma once

#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "logfile.h"

typedef enum {C, V, NONE} Verbose;

typedef struct {
    Verbose vflag; //v ou c
    bool recursive; //R
} Options;

typedef struct {
    char** arguments; //argv
    int nArgs; //argc
    int fileNameIndex; //Index of the fileName in argv
} Arguments;

typedef struct {
    Options options;
    int noFilesFound; //Number of files found so far
    int noFilesChanged; //Number of files changed so far
    int noChildren; //Number of children
    Arguments args; 
    char* filePath; //File that initiated the process
    char* modeString; //Mode string
    bool isInitial; //If it is the first process or not
    bool regist; //If it should register in the log file or not
} ProcessInfo;

extern ProcessInfo* pInfo;

bool isDirectory(const char* pathname); //Checks if file in pathname is a directory
void diagnosticPrint(const char* filePath, mode_t oldMode, mode_t newMode); //Verbose messages
void processOption(const char optFlag, Options* options); //Processes an option
int get_bit(int bits, int pos); //Returns the value of the bit at position pos of the value bits
void fromOctalToString(mode_t mode,char* permissions); //Converts Octal to a string with rwx notation
void fourDigitOctal(mode_t mode,char *str); //Adds zeros, example input: 777-> return: "0777"
bool checkChanges(const mode_t oldMode, const mode_t newMode); //Checks if there were changes
void endProgram(int exitStatus); //Function to end the program with exitStatus
void hasAChild(char* newPath); //Creates new process and records information about it
void makeNewArgs(char* newArgs[], char* fileName); //Fabricate the arguments for the new process
double getMiliSeconds(double initialTime); //Get milliseconds since begining of execution
