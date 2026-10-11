#include "kf_file.h"

/* load the KF4.DAT table of contents */
void func_00102178(void) {
    int fd;

    memset(D_00447600, 0, sizeof(D_00447600));
    D_00447000[1] = (int)D_00447680;
    D_00447000[2] = (int)(D_00447680 + 0x40);
    fd = func_00102238("KF4.DAT", 0, 0);
    func_001025C0(fd, D_00447680, 0x20000);
    ((KFDirHeader *)D_00447000[1])->lsn = D_00447600[fd].start;
    func_001024B0(fd);
}
