#include "common.h"
#include <libdev_internal.h>

// libdev: 0 idle, 1 stopped, 2 waiting for VU0, 3 stalled on an error, 4 busy.
int sceDevVif0GetStat(void) {
    u32 status = LIBDEV_REG32(VIF0_STAT);

    if (!(status & 3)) {
        return 0;
    }
    if (status & VIF_STAT_STOPPED_MASK) {
        return 1;
    }
    if (status & VIF_STAT_WAITING_MASK) {
        return 2;
    }
    if (status & VIF_STAT_ERROR_MASK) {
        return 3;
    }
    return 4;
}
