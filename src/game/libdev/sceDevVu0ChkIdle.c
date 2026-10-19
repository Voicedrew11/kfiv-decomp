#include "common.h"
#include <libdev_internal.h>

// libdev: TRUE while VU0 or VU1 is running.
int sceDevVu0ChkIdle(void) {
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
