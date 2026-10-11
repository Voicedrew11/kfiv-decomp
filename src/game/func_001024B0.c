#include "kf_file.h"

/* close */
int func_001024B0(int fd) {
    if (fd == -1) {
        puts("-1");
    }
    if ((unsigned int)fd >= 2 || D_00447600[fd].active == 0) {
        return -1;
    }
    func_00102790(fd, 0);
    D_00447600[fd].active = 0;
    D_00447600[fd].start = 0;
    D_00447600[fd].cur = 0;
    D_00447600[fd].size = 0;
    D_00447600[fd].remain = 0;
    return 1;
}
