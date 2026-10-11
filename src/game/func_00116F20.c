#include "common.h"

extern char D_00471F60[];
extern char D_00471FE0[];

void func_00116F20(int arg0) {
    int i;
    s16 *p;

    p = (s16 *)(arg0 * 64 + D_00471F60);
    i = 0xF;
    do {
        p[0] = 0;
        i--;
        p[1] = 0;
        p += 2;
    } while (i >= 0);
    p = (s16 *)(arg0 * 64 + D_00471FE0);
    i = 0xF;
    do {
        p[0] = 0;
        i--;
        p[1] = 0;
        p += 2;
    } while (i >= 0);
}
