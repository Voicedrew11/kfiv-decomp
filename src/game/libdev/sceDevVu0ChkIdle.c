#include "common.h"
#include <libdev_internal.h>

// libdev: TRUE while VU0 or VU1 is running.
int sceDevVu0ChkIdle(void) {
    u32 status;

    LIBDEV_CFC2(status, vi29);
    if ((status & VU_STAT_VU0_MASK) == VU_STAT_VU0_RUNNING) {
        return TRUE;
    }
    if ((status & VU_STAT_VU1_MASK) == VU_STAT_VU1_RUNNING) {
        return TRUE;
    }
    return FALSE;
}
