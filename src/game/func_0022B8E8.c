#include "common.h"
#include <libdev_internal.h>

// libdev: sceDevVif1Reset (name inferred). Resets VIF1 and masks its DMAtag and VIFcode errors.
void func_0022B8E8(void) {
    LIBDEV_REG32(VIF1_FBRST) = 1;
    LIBDEV_REG32(VIF1_ERR) = 6;
}
