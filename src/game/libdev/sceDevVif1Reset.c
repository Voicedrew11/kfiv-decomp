#include "common.h"
#include <libdev_internal.h>

// libdev: Resets VIF1 and masks its DMAtag and VIFcode errors.
void sceDevVif1Reset(void) {
    LIBDEV_REG32(VIF1_FBRST) = 1;
    LIBDEV_REG32(VIF1_ERR) = 6;
}
