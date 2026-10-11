#include "common.h"

typedef struct { char pad0[0x10]; int w0; float f4; int w8; char c0xC; } Sub;
typedef struct Ctl { char pad0[0xD8]; float f_d8; char pad1[0xEC - 0xDC]; u16 h_ec; } Ctl;
typedef struct { char pad0[0x10]; Sub s; char pad1[0x50-0x10-sizeof(Sub)]; char *p50; Ctl *p54; } Actor;
void func_001AF538(float, void *, void *);
void func_001AF640(void *, int);
int func_001AF928();
void func_001AAEA0(Actor *arg0, int arg1) {
    Sub *s = (Sub *)((char *)arg0 + 0x10);
    func_001AF928();
    func_001AF640(arg0, arg1);
    func_001AF538(arg0->p54->f_d8, s, arg0->p50 + 0xD0);
    s->f4 = 1.0f;
    s->w8 = arg0->p54->h_ec;
    s->w0 = 0;
    s->c0xC = 0;
}
