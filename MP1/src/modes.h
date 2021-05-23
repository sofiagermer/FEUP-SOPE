#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <unistd.h>

#include "utils.h"

bool checkOctalMode(const char* octalMode); //Checks if octal mode's structure is valid
mode_t findMode(const char* mode, const char* filePath, const mode_t oldMode); //Returns the mode to be applied (in case of MODE option)
mode_t getModeNum(const char* mode, const char* filePath, const mode_t oldMode); //Decides between octal and MODE and returns numerical (mode_t) code for mode
mode_t convert(int octal); //To convert octal do decimal
mode_t getFilePermissions(const char *path); //Returns the current permissions of the file being treated

