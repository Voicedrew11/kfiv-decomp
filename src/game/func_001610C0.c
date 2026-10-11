#include "common.h"

int func_00192C98(void *, int, int, u32, int);
extern int D_00414184[];

typedef struct {
    char pad0[0x10];
    u16 h10;
    char pad1[0xC4 - 0x12];
    u8 c4;
} Ctl;

typedef struct {
    char pad0[0xC];
    Ctl *c;
    char pad1[0xC8 - 0x10];
    u16 hc8;
} Actor;

void func_001610C0(Actor *arg0) {
    if (arg0 == (Actor *)D_00414184[0]) {
        if (arg0->c->h10 & 0x800) {
            arg0->hc8 = 0;
        }
    }
    if (arg0->hc8 & 4) {
        func_00192C98(arg0, 6, 0, arg0->c->c4, 0);
        return;
    }
    if (arg0->hc8 & 8) {
        func_00192C98(arg0, 7, 0, arg0->c->c4, 0);
        return;
    }
    if (arg0->hc8 & 1) {
        func_00192C98(arg0, 3, 0, arg0->c->c4, 0);
        return;
    }
    if (arg0->hc8 & 2) {
        func_00192C98(arg0, 5, 0, arg0->c->c4, 0);
        return;
    }
    func_00192C98(arg0, 0, 0, arg0->c->c4, 0);
}
