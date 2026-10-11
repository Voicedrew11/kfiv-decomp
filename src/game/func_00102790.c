#include "kf_file.h"

/* wait for the read in flight; reissue it while the drive reports an error */
int func_00102790(int fd, int mode) {
    sceCdRMode rmode;
    unsigned int err;
    int ret;

    if ((unsigned int)fd >= 2 || D_00447600[fd].active == 0) {
        return -1;
    }
    ret = sceCdSync(mode);
    if (ret == 0) {
    err = sceCdGetError();
    if (D_00447600[fd].pending != 0) {
        while (err >= 2) {
            rmode = D_00447600[fd].pend_mode;
            while (sceCdRead(D_00447600[fd].pend_lsn, D_00447600[fd].pend_sectors,
                             D_00447600[fd].pend_buf, &rmode) == 0) {
            }
            ret = sceCdSync(0);
            err = sceCdGetError();
        }
        D_00447600[fd].pending = 0;
        D_00447600[fd].pend_lsn = 0;
        D_00447600[fd].pend_sectors = 0;
        D_00447600[fd].pend_buf = 0;
        memset(&D_00447600[fd].pend_mode, 0, sizeof(sceCdRMode));
    }
    }
    return ret;
}
