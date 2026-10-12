#include "common.h"
#include <libdev_internal.h>

int func_0022B6B8(void);

// libdev: sceDevVif0Continue (name inferred). Restarts a stopped VIF0.
int func_0022B648(void) {
    if (func_0022B6B8() != 1) {
        return 0;
    }
    LIBDEV_REG32(VIF0_FBRST) = 8;
    return 1;
}
