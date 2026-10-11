#include "common.h"

s16 func_0011E340(char *, int);

void func_001227D0(char *arg0) {
    if (!(*(int *)(arg0 + 0x10) & 0x20000)) {
        func_0011E340(arg0, 4);
        arg0[0x11] = 1;
    } else {
        func_0011E340(arg0, 5);
        arg0[0x11] = 3;
    }
}
