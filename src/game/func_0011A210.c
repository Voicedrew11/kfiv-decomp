#include "common.h"

typedef struct {
    char pad0[0xD8];
    u64 bits;
} Actor;

extern u8 D_0034B6D0[];
extern u8 D_00349690[];

void *func_001267E8(int, int);

void func_0011A210(int arg0) {
    Actor *t;

    t = func_001267E8(arg0, 1);
    if (D_0034B6D0[0x340] == 0) {
        if (D_0034B6D0[0x345] != 0 && D_0034B6D0[0x346] == 0) {
            D_0034B6D0[0x346] = 1;
        }
        if (D_0034B6D0[0x346] == 1) {
            if (D_00349690[0x1905] == 0) {
                D_00349690[0x1905] = 1;
                D_0034B6D0[0x345] = 0x11;
            }
            return;
        }
    }
    if (t != 0) {
        t->bits &= ~2ULL;
        t->bits &= ~0x80ULL;
    }
}
