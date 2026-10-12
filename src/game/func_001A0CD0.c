#include "common.h"
#include "kf_sound.h"

extern int (*D_0043EE30[])(int, int);

/* Calls handler handler_index of D_0043EE30 with the low byte of handler_arg and value. */
void func_001A0CD0(int handler_arg, int handler_index, int value) {
    D_0043EE30[handler_index & 0xFF](handler_arg & 0xFF, value);
}
