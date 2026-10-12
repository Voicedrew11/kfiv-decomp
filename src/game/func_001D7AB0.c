#include "common.h"

extern void *D_00446B20;

/* Runs the current screen handler and reports that it ran. */
s32 func_001D7AB0(void) {
    ((void (*)(void))D_00446B20)();
    return TRUE;
}
