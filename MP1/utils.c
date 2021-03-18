#include "utils.h"


bool isDirectory(const char* pathname) {
    bool ret=false;
    struct stat *sb=(struct stat*)malloc(sizeof(struct stat));
    if (stat(pathname, sb) == 0 && S_ISDIR(sb->st_mode))
        ret=true;
    return ret;
    
}
