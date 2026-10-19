#include "common.h"
#include <libdev_internal.h>


// libdev: Restarts a stopped VIF0.
int sceDevVif0Continue(void) {
    if (sceDevVif0GetStat() != 1) {
        return 0;
    }
    LIBDEV_REG32(VIF0_FBRST) = 8;
    return 1;
}
