#include "common.h"
#include <libdev_internal.h>


// libdev: Restarts a stopped VIF1.
int sceDevVif1Continue(void) {
    if (sceDevVif1GetStat() != 1) {
        return 0;
    }
    LIBDEV_REG32(VIF1_FBRST) = 8;
    return 1;
}
