#include "common.h"

int func_00192C98(void *, int, int, u32, int);
int func_001958B8(void *, void *, int);
int func_00199EB8(void *, void *, float);
extern float D_00445640;
void func_00168F48(void);

typedef struct {
    char pad0[0x1C];
    int f1C[1];
    char pad20[0x48 - 0x20];
    float f48;
    char pad4C[0x7B - 0x4C];
    char c7B;
} Sub;

typedef struct {
    char pad0[0x82];
    s16 s82;
    char pad84[0x88 - 0x84];
    int i88;
} Inner;

typedef struct {
    char pad0[0xFC];
    Inner *p;
} Mid;

typedef struct {
    char pad0[0xC4];
    u8 c4;
} Ctl;


typedef struct {
    u64 z0 : 9;
    u64 b9 : 1;
    u64 z10 : 1;
    u64 f11 : 3;
    u64 z14 : 3;
    u64 b17 : 1;
    u64 z18 : 10;
    u64 b28 : 1;
    u64 z29 : 35;
} Flags;

typedef struct {
    Mid *m;
    char pad4[8];
    Ctl *c;
    char pad10[0x68 - 0x10];
    float f68;
    char pad6C[0x88 - 0x6C];
    float f88;
    char pad8C[0xA0 - 0x8C];
    s16 sA0;
    char padA2[0xC6 - 0xA2];
    u16 sC6;
    s16 sC8;
    char cCA;
    char padCB[0xD8 - 0xCB];
    union { Flags b; u64 l; } flags;
    Sub sub;
} Act;

void func_00168DB8(Act *a) {
    Sub *s = &a->sub;
    a->sC6 |= 0x3000;
    switch (a->cCA) {
    case 0:
        func_00192C98(a, 8, 0, a->c->c4 >> 1, 1);
        a->flags.b.f11 = 1;
        a->flags.b.b9 = 0;
        a->flags.b.b28 = 1;
        a->flags.b.b17 = 0;
        s->f48 = 0;
        a->cCA++;
    case 1:
        if (a->m->p->i88 == 0) {
            a->cCA++;
        }
        func_00199EB8(a, s->f1C, a->f68);
        return;
    case 2:
        if ((int)(a->flags.l >> 34) & 1) {
            a->sC6 = 0;
            a->sC8 = 0;
            a->sA0 = 0;
            s->c7B = 1;
            func_001958B8(a, func_00168F48, 1);
        }
        if (a->m->p->s82 >= 0x29) {
            float t;
            a->f88 = -s->f48;
            t = s->f48 + D_00445640;
            s->f48 = t;
            if (t > 12.0f) s->f48 = 12.0f;
        }
        break;
    }
}
