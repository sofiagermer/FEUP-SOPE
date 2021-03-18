#pragma once

#include <dirent.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

typedef struct {
    Verbose vflag;
    bool recursive;
} Options;

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
    bool isInitial;
    bool isParent;
    bool sleep;
    int* childrenPIDs;
    int noChildren;
} ProcessInfo;

typedef enum {C, V, NONE} Verbose;


bool isDirectory(const char* pathname); //Checks if file in pathname is a directory

