#include "common.h"
#include <libdev_internal.h>

// libdev: sceDevVif0Reset (name inferred). Resets VIF0 and masks its DMAtag and VIFcode errors.
void func_0022B5B8(void) {
    LIBDEV_REG32(VIF0_FBRST) = 1;
    LIBDEV_REG32(VIF0_ERR) = 6;
}
