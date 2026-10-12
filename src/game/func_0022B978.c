#include "common.h"
#include <libdev_internal.h>

int func_0022B9E8(void);

// libdev: sceDevVif1Continue (name inferred). Restarts a stopped VIF1.
int func_0022B978(void) {
    if (func_0022B9E8() != 1) {
        return 0;
    }
    LIBDEV_REG32(VIF1_FBRST) = 8;
    return 1;
}
