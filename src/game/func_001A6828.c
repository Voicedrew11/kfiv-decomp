#include "common.h"

typedef struct {
    char pad0[0xD0];
    float vD0[4];
} T50;

typedef struct {
    char pad0[0xD8];
    float fD8;
    float fDC;
    float fE0;
    char padE4[0xE];
    u16 hF2;
} T54;

void func_001AF640(void *, int);
void func_001AF928(void *, int);
void func_001AF538(float *, float *, float);
s32 func_001A2838(u16, void *);

typedef struct {
    float w0;
    float f4;
    char pad8[8];
    char c10;
} Sub;

typedef struct {
    char pad0[0x10];
    Sub sub;
    char pad24[0x40 - 0x24];
    s64 l40;
    char pad48[8];
    T50 *p50;
    T54 *p54;
} Obj;

void func_001A6828(Obj *a, int arg1) {
    Sub *s = &a->sub;
    char buf[0x10]; /* unused, but retail reserves the 0x10 bytes */

    func_001AF640(a, arg1);
    func_001AF928(a, arg1);
    a->p50->vD0[0] = 0.0f;
    func_001AF538((float *)s, a->p50->vD0, a->p54->fD8);
    s->f4 = a->p54->fDC;
    if (a->p54->hF2 != 0) {
        func_001A2838(a->p54->hF2, a);
    }
    s->c10 = 0;
    a->l40 |= (s64)0x8000 << 0x2B;
}
