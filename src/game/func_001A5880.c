#include "common.h"

typedef struct {
    char pad0[0xC0];
    float vC0;
    float vC4;
    char padC8[0x8];
    char vD0[0x10];
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
void func_001AF538(float, void *, void *);
void func_001C1430(f32, void *, void *);

typedef struct {
    char pad[0x10];
    float w10;
    float f14;
    float w18;
    float w1C;
    float f20;
    float w24;
    float f28;
    char c2C;
} Sub;

typedef struct {
    char pad0[0x10];
    Sub sub;
    char pad40[0x10];
    T50 *p50;
    T54 *p54;
} Obj;

void func_001A5880(Obj *a, int arg1) {
    Sub *s = &a->sub;
    T50 *q;
    float f, g, h;

    func_001AF640(a, arg1);
    func_001AF928(a, arg1);
    if (a->p54->hF2 == 0) {
        func_001AF538(a->p54->fD8, s, a->p50->vD0);
        g = a->p54->fDC;
        h = g * 0.5f;
        s->c2C = 0;
        s->f20 = g;
        s->w24 = 0;
        s->w10 = 0;
        s->f14 = a->p54->fDC;
        s->w18 = 0;
        s->w1C = 0;
        q = a->p50;
        f = q->vC4;
        q->vC4 -= h;
        func_001C1430(1.0f, &q->vC0, (char *)a + 0x20);
        a->p50->vC4 = f;
        if (*(int *)0x70000010 == 0) {
            s->c2C = 1;
        } else {
            a->p50->vC4 = *(float *)0x70000004;
        }
        s->f28 = a->p54->fE0;
    }
}
