#include "common.h"

extern u8 D_00369830[];
extern void *D_003BE2B0[];

int func_0011D600(void *arg0, int arg1) {
    int ret;

    if (D_00369830[arg1] == 0) {
        ret = 0;
    } else {
        ret = ((int (*)(void *, int))D_003BE2B0[*(u8 *)*(char **)((char *)arg0 + 4)])(arg0, arg1) != 0;
    }
    return ret;
}
