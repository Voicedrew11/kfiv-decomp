#include "common.h"

void func_001AF928(void *, int);
void func_001AF640(void *, int);
int func_001AFD08(void *, void *, void *);
void func_001AF538(float, void *, void *);

typedef struct {
    char pad[0x10];
    float f10;
    int w14;
    int w18;
    int w1C;
    int w20;
    char c24;
} Sub;

typedef struct {
    char pad0[0x10];
    Sub sub;
    char pad38[8];
    u64 b0 : 32;
    s16 h44;
    s8 c46;
    u64 b56 : 1;
    u64 b57 : 1;
    u64 b58 : 1;
    u64 b59 : 5;
    char pad48[8];
    char *p50;
    char *p54;
    char pad58[0x14];
    int w6C;
} Obj;

void func_001A4630(Obj *a, int arg1) {
    Sub *s = &a->sub;
    char *q;
    func_001AF928(a, arg1);
    func_001AF640(a, arg1);
    q = a->p50;
    *(float *)(q + 0xC4) = *(float *)(q + 0xC4) - 220.0f;
    a->w6C = func_001AFD08(a, q + 0xC0, q + 0xD0);
    func_001AF538(*(float *)(a->p54 + 0xD8), s, a->p50 + 0xD0);
    s->w14 = 0;
    s->f10 = *(float *)(a->p54 + 0xDC);
    s->w18 = 0;
    s->w1C = 0;
    s->w20 = 0;
    s->c24 = 0;
    a->b58 = 1;
}
