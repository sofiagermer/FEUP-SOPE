#include "utils.h"


bool isDirectory(const char* pathname) {

    struct stat sb;
    if (stat(pathname, &sb) == 0 && S_ISDIR(sb.st_mode))
        return true;
    return false;
    
}
