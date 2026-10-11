#include "common.h"

int FlushCache(int);
int sceSifFreeIopHeap(int);
extern char *D_004471D4;

int func_001CD6E0(u32 arg0) {
    int *e;
    u32 v;
    int t;
    int i;

    if (arg0 >= 0x10U) {
        return -1;
    }
    e = (int *)(D_004471D4 + arg0 * 32);
    if (*(int *)(D_004471D4 + 0x4C0) == e[3] + e[5]) {
        *(int *)(D_004471D4 + 0x4C0) = e[3];
    }
    e[3] = 0;
    if (e[2] != 0) {
        if (sceSifFreeIopHeap(e[2]) != 0) {
            return -1;
        }
        e[2] = 0;
    }
    if (e[0] != 0) {
        if (sceSifFreeIopHeap(e[0]) != 0) {
            return -1;
        }
        e[0] = 0;
    }
    if (e[1] != 0) {
        if (sceSifFreeIopHeap(e[1]) != 0) {
            return -1;
        }
        e[1] = 0;
    }
    *(int *)(D_004471D4 + 0x4C0) = 0x5010;
    i = 15;
    e = (int *)D_004471D4;
    do {
        v = *(u32 *)(D_004471D4 + 0x4C0);
        t = e[3];
        if ((u32)t >= v) {
            *(int *)(D_004471D4 + 0x4C0) = t + e[5];
        }
        e += 8;
        i--;
    } while (i >= 0);
    FlushCache(0);
    return 0;
}
