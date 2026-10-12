#include "common.h"
#include <libdev_internal.h>

// libdev: sceDevVif1PutErr (name inferred). Writes the three mask bits of VIF1_ERR and returns what it wrote.
int func_0022B9B8(int mask_interrupt, int mask_dmatag, int mask_vifcode) {
    int error_mask = mask_interrupt | (mask_dmatag << 1) | (mask_vifcode << 2);

    LIBDEV_REG32(VIF1_ERR) = error_mask;
    return error_mask;
}
