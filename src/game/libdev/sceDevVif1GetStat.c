#include "common.h"
#include <libdev_internal.h>

// libdev: 0 idle, 1 stopped, 2 waiting for VU1, 3 stalled on an error, 4 busy.
int sceDevVif1GetStat(void) {
    u32 status = LIBDEV_REG32(VIF1_STAT);

    if (!(status & VIF_STAT_VPS_MASK)) {
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
