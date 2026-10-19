#include "common.h"
#include <libdev_internal.h>

// libdev: Resets VIF0 and masks its DMAtag and VIFcode errors.
void sceDevVif0Reset(void) {
    LIBDEV_REG32(VIF0_FBRST) = 1;
    LIBDEV_REG32(VIF0_ERR) = 6;
}
