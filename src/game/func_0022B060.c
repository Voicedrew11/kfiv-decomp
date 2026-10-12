#include "common.h"
#include <libdev_internal.h>

// libdev: sceDevVu0ChkIdle in libdev_internal.h (name inferred); TRUE while VU0 or VU1 is running.
int func_0022B060(void) {
    u32 status;

    LIBDEV_CFC2(status, vi29);
    if ((status & 0xF) == 1) {
        return TRUE;
    }
    if ((status & 0xF00) == 0x100) {
        return TRUE;
    }
    return FALSE;
}
