#include "common.h"
#include <libdev_internal.h>

// libdev: Writes the three mask bits of VIF0_ERR and returns what it wrote.
int sceDevVif0PutErr(int mask_interrupt, int mask_dmatag, int mask_vifcode) {
    int error_mask = mask_interrupt | (mask_dmatag << 1) | (mask_vifcode << 2);

    LIBDEV_REG32(VIF0_ERR) = error_mask;
    return error_mask;
}
