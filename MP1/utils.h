#pragma once

#include <dirent.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

typedef struct {
    char** arguments;
    int nArgs;
    int fileNameIndex;
} Arguments;

bool isDirectory(const char* pathname); //Checks if file in pathname is a directory

