#include "common.h"

s32 func_0021DFD8(char *arg0, s32 arg1) {
    s32 temp_2;

    temp_2 = (*(int *)((char *)arg0 + 0xA8));
    (*(int *)((char *)arg0 + 0xA8)) = arg1;
    return temp_2;
}
