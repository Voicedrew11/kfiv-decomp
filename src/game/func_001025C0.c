#include "kf_file.h"

/* read */
int func_001025C0(int fd, void *buf, int size) {
    sceCdRMode mode;
    KFFile *f;
    int sectors;

    if ((unsigned int)fd >= 2 || D_00447600[fd].active == 0) {
        return -1;
    }
    if (size < 0) {
        size = 0x1000000;
    }
    func_00102790(fd, 0);
    f = &D_00447600[fd];
    if (f->size != 0) {
        if (f->remain == 0) {
            return 0;
        }
        if (f->remain < size) {
            size = f->remain;
        }
    }
    sectors = (size + 0x7FF) / 0x800;
    memset(&mode, 0, sizeof(mode));
    mode.trycount = 0;
    mode.spindlctrl = 1;
    mode.datapattern = 0;
    mode.pad = 0;
    f->pending = 1;
    f->pend_lsn = f->cur;
    f->pend_sectors = sectors;
    f->pend_buf = buf;
    f->pend_mode = mode;
    while (sceCdRead(f->cur, sectors, buf, &mode) == 0) {
    }
    f->cur += sectors;
    if (f->nowait == 0) {
        func_00102790(fd, 0);
    }
    if (f->size != 0) {
        f->remain -= size;
    }
    return size;
}
