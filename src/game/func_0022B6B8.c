#include "common.h"
#include <libdev_internal.h>

// libdev: sceDevVif0GetStat (name inferred). 0 idle, 1 stopped, 2 waiting for VU0, 3 stalled on an error, 4 busy.
int func_0022B6B8(void) {
    u32 status = LIBDEV_REG32(VIF0_STAT);

    if (!(status & 3)) {
        return 0;
    }
    if (status & 0x700) {
        return 1;
    }
    if (status & 0xC) {
        return 2;
    }
    if (status & 0x3000) {
        return 3;
    }
    return 4;
}
