#pragma once

#include <stdbool.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {C, V, NONE} Verbose;

typedef struct {
    Verbose vflag;
    bool recursive;
} Options;

void diagnosticPrint(const char* filePath, mode_t oldMode, mode_t newMode, Options options); //Verbose messages
void processOption(const char optFlag, Options* options); //Processes an option
int get_bit(int bits, int pos);
void fromOctalToString(mode_t mode,char* permissions); //Converts mode to a string to be printed
void fourDigitOctal(mode_t mode,char *str);
bool checkChanges(const mode_t oldMode, const mode_t newMode); //Checks if there were changes

