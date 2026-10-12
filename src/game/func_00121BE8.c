#include "common.h"

s16 func_0011E340(char *, int);
int func_0011E3A0(void *);

void func_00121BE8(char *arg0) {
    int n = *(int *)(arg0 + 0x40);
    char buf[0x10]; /* unused, but retail reserves the 0x10 bytes */

    if (n != 0) {
        if (*(s16 *)(*(char **)(arg0 + 8) + 0x44) > 0) {
            *(int *)(arg0 + 0x40) = n - 1;
        }
    } else {
        if (*(s16 *)(*(char **)(*(char **)arg0 + 0xFC) + 0x82) != 0) {
            func_0011E3A0(arg0);
        } else {
            func_0011E340(arg0, 5);
            arg0[0x11] = 3;
            *(int *)(arg0 + 0x40) = *(s16 *)(*(char **)(arg0 + 8) + 0x46);
        }
    }
}
