#include "common.h"
#include <libdev_internal.h>


// libdev: Mode 0 force-breaks VIF1, mode 1 stops it after the current VIFcode.
int sceDevVif1Pause(int mode) {
    int status = sceDevVif1GetStat();

    if (status == 0) {
        return 0;
    }
    if (status == 1) {
        return 0;
    }
    if (mode == 0) {
        LIBDEV_REG32(VIF1_FBRST) = 2;
    } else if (mode == 1) {
        LIBDEV_REG32(VIF1_FBRST) = 4;
    }
    return 1;
}
