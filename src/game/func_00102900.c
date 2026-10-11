#include "kf_file.h"

/* size */
int func_00102900(int fd) {
    if ((unsigned int)fd >= 2) {
        return -1;
    }
    return D_00447600[fd].size;
}
