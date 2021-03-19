#include "modes.h"



bool checkOctalMode(const char* octalMode) {

    //Checks if the first one is a 0, if the string has a valid size and if the values are valid (0-7)
    if (strlen(octalMode) > 4) return false;
    if (octalMode[0] != '0') return false;
    if (octalMode[1] == '0' || octalMode[1] == '1' || octalMode[1] == '2' || octalMode[1] == '3' || octalMode[1] == '4' || octalMode[1] == '5' || octalMode[1] == '6' || octalMode[1] == '7'){
        if (octalMode[2] == '0' || octalMode[2] == '1' || octalMode[2] == '2' || octalMode[2] == '3' || octalMode[2] == '4' || octalMode[2] == '5' || octalMode[2] == '6' || octalMode[2] == '7'){
            if (octalMode[3] == '0' || octalMode[3] == '1' || octalMode[3] == '2' || octalMode[3] == '3' || octalMode[3] == '4' || octalMode[3] == '5' || octalMode[3] == '6' || octalMode[3] == '7'){
                return true;;
            }
        }
    }
    return false;
}

mode_t getModeNum(const char* mode, const char* filePath, const mode_t oldMode) {
    mode_t modeNum;
    int octal;

    //In octal
    if ((octal = atoi(mode)) != 0 || strcmp(mode, "0000") == 0) {
        if (!checkOctalMode(mode)) {
            fprintf(stderr, "Invalid format for octal mode:%s\n", mode);
            endProgram(1);
        }
        modeNum = convert(octal);
    }
    //In mode
    else 
        modeNum = findMode(mode, filePath, oldMode);
    return modeNum;
}

mode_t findMode(const char* mode, const char* filePath, const mode_t oldMode) {

    mode_t newMode = 0;
    int length = strlen(mode);
    char target = mode[0];
    char operation = mode[1];
    mode_t mask = 0;

    //Depending on the target and the permission (rwx), it will use the write macro
    switch (target) {
        case 'u': {
            mask |= S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IWOTH | S_IXOTH; //For equals
            for (unsigned int i = 2; i < length; i++) {
                switch (mode[i]) {
                    case 'r': {
                        newMode |= S_IRUSR;
                        break;
                    }
                    case 'w': {
                        newMode |= S_IWUSR;
                        break;
                    }
                    case 'x': {
                        newMode |= S_IXUSR;
                        break;
                    }
                }
            }
            break;
        }
        case 'g': {
            mask |= S_IRUSR | S_IWUSR | S_IXUSR | S_IROTH | S_IWOTH | S_IXOTH;
            for (unsigned int i = 2; i < length; i++) {
                switch (mode[i]) {
                    case 'r': {
                        newMode |= S_IRGRP;
                        break;
                    }
                    case 'w': {
                        newMode |= S_IWGRP;
                        break;
                    }
                    case 'x': {
                        newMode |= S_IXGRP;
                        break;
                    }
                }
            }
            break;
        }
        case 'o': {
            mask |= S_IRGRP | S_IWGRP | S_IXGRP | S_IRUSR | S_IWUSR | S_IXUSR;
            for (unsigned int i = 2; i < length; i++) {
                switch (mode[i]) {
                    case 'r': {
                        newMode |= S_IROTH;
                        break;
                    }
                    case 'w': {
                        newMode |= S_IWOTH;
                        break;
                    }
                    case 'x': {
                        newMode |= S_IXOTH;
                        break;
                    }
                }
            }
            break;
        }
        case 'a': {
            mask = 0;
            for (unsigned int i = 2; i < length; i++) {
                switch (mode[i]) {
                    case 'r': {
                        newMode |= S_IRUSR | S_IRGRP | S_IROTH;
                        break;
                    }
                    case 'w': {
                        newMode |= S_IWUSR | S_IWGRP | S_IWOTH;
                        break;
                    }
                    case 'x': {
                        newMode |= S_IXUSR | S_IXGRP | S_IXOTH;
                        break;
                    }
                }
            }
            break;
        }
        default: {
            fprintf(stderr, "Invalid target identifier:%c\n", target);
            endProgram(1);
        }
    }

    //Depending on the operation, returns the updated mode correctly
    switch (operation) {
        case '+': {
            newMode = oldMode | newMode;
            break;
        }
        case '-': {
            newMode = oldMode & ~newMode;
            break;
        }
        case '=': {
            mask &= oldMode; 
            newMode |= mask;
            break;
        }
        default: {
            fprintf(stderr, "Invalid type of mode operation:%c\n", operation);
            endProgram(1);
            break;
        }
    } 
    return newMode;
}

mode_t convert(int octal) {
    int decimal = 0, i = 0;

    while(octal != 0) {
        decimal += (octal%10) * pow(8,i);
        ++i;
        octal/=10;
    }

    return decimal;
}

mode_t getFilePermissions(const char *path) {
    struct stat mode;
    int result = stat(path, &mode);
    if (errno != ENOENT) pInfo->noFilesFound++; //Found a file
    if (result == -1) {
        fprintf(stderr, "Error accessing file:%s\n", strerror(errno));
        return 999999;
    }

    mode_t m = mode.st_mode;
    m &= 0x00fff;
    return m;
}
