#include "kf_file.h"

/* seek the drive to the next sector */
int func_00102728(int fd) {
    if ((unsigned int)fd >= 2 || D_00447600[fd].active == 0) {
        return -1;
    }
    func_00102790(fd, 0);
    return sceCdSeek(D_00447600[fd].cur);
}
