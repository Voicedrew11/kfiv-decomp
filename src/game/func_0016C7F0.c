#include "common.h"

int func_00192C98(void *, int, int, u32, int);

typedef struct {
    char pad0[0xC4];
    u8 c4;
} Ctl;

typedef struct {
    char pad0[0xC];
    Ctl *c;
    char pad10[0xC8 - 0x10];
    u16 sC8;
} Act;

void func_0016C7F0(Act *a) {
    u16 flags = a->sC8;

    if (flags & 4) {
        func_00192C98(a, 6, 0, a->c->c4, 0);
        return;
    }
    if (flags & 8) {
        func_00192C98(a, 7, 0, a->c->c4, 0);
        return;
    }
    if (flags & 1) {
        func_00192C98(a, 3, 0, a->c->c4, 0);
        return;
    }
    if (flags & 2) {
        func_00192C98(a, 5, 0, a->c->c4, 0);
        return;
    }
    func_00192C98(a, 0, 0, a->c->c4, 0);
}
