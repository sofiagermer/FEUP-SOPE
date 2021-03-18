#include "modes.h"



bool checkOctalMode(const char* octalMode) {
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
    if ((octal = atoi(mode)) != 0) {
        if (!checkOctalMode(mode)) {
            fprintf(stderr, "Invalid format for octal mode:%s\n", mode);
            exit(1);
        }
        modeNum = convert(octal);
    }
    else 
        modeNum = findMode(mode, filePath, oldMode);
    return modeNum;
}

mode_t findMode(const char* mode, const char* filePath, const mode_t oldMode) {

    mode_t newMode = 0;
    int length = strlen(mode);
    char target = mode[0];
    char operation = mode[1];

    switch (target) {
        case 'u': {
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
            exit(1);
        }
    }

    switch (operation) {
        case '+': {
            return oldMode | newMode;
        }
        case '-': {
            return oldMode & ~newMode;
        }
        case '=': {
            return newMode;
        }
        default: {
            fprintf(stderr, "Invalid type of mode operation:%c\n", operation);
            exit(1);
        }
    } 
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
    struct stat *mode=(struct stat*)malloc(sizeof(struct stat));
    if (stat(path, mode) == -1) {
        return -1;
    }

    int m = mode->st_mode;
    m &= 0x00fff;
    free(mode);
    return m;
}
