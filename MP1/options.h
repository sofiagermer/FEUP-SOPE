#ifndef OPTIONS_H
#define OPTIONS_H

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

void diagnosticPrint(const char* filePath, const mode_t oldMode, const mode_t newMode, Options options); //Verbose messages
void processOption(const char optFlag, Options* options); //Processes an option
int get_bit(int bits, int pos);
char* fromOctalToString(mode_t mode); //Converts mode to a string to be printed
char* fourDigitOctal(mode_t mode);
bool checkChanges(const mode_t oldMode, const mode_t newMode); //Checks if there were changes

#endif //OPTIONS_H