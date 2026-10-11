#include "common.h"

int func_00192C98(void *, int, int, u32, int);

typedef struct {
    char pad0[0x58];
    float f58;
    float f5C;
    char pad60[0xC4 - 0x60];
    u8 c4;
} Ctl;

typedef struct {
    char pad0[0x1C];
    float f1C;
} Sub;

typedef struct {
    char pad0[0xC];
    Ctl *c;
    char pad10[0x94 - 0x10];
    float f94;
    char pad98[0xC8 - 0x98];
    u16 sC8;
    char padCA[0xE0 - 0xCA];
    Sub sub;
} Act;

void func_0016AFD8(Act *a) {
    Sub *s = &a->sub;
    u16 flags = a->sC8;
    float v;
    float lim;

    if (flags & 0x100) {
        s->f1C = s->f1C - a->c->f58;
        lim = -a->c->f5C;
        if (s->f1C < lim) {
            s->f1C = lim;
        }
    } else if (flags & 0x200) {
        s->f1C = s->f1C + a->c->f58;
        lim = a->c->f5C;
        if (lim < s->f1C) {
            s->f1C = lim;
        }
    } else {
        s->f1C = 0.0f;
    }
    v = s->f1C;
    if (flags & 0x400) {
        v *= 2.0f;
    }
    a->f94 = a->f94 + v;
    func_00192C98(a, 0, 0, a->c->c4, 0);
}
