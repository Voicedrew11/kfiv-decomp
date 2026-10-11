#include "common.h"

typedef struct {
    char pad0[0xEC];
    u16 uEC;
    u16 uEE;
    u16 uF0;
    char pad1[0x12];
    float f104;
} Ctl;

typedef struct {
    char pad0[0x10];
    float f10;
    u16 u14;
    char t16;
} Sub;

typedef struct {
    char pad0[0x10];
    Sub sub;
    char pad1[0x20];
    void *p48;
    u16 count;
    u16 u4E;
    char *p50;
    Ctl *p54;
    char pad2[4];
    int i5C;
} Actor;

void func_001AEC48(void *);
void func_001AED58(void *);
int func_001AF0B0(void *);
int func_001A2A18(void *);
void func_001A1E28(u16, void *, void *, int, u16, void *);

void func_001ADDD0(Actor *arg0) {
    Sub *s;
    Ctl *c;
    char *p50;
    u16 var_a0;

    func_001AEC48(arg0);
    s = &arg0->sub;
    func_001AED58(arg0);
    func_001AF0B0(arg0);
    c = arg0->p54;
    if (arg0->count >= c->uF0) {
        p50 = arg0->p50;
        s->f10 = s->f10 + c->f104;
        *(float *)(p50 + 0xC4) = *(float *)(p50 + 0xC4) + s->f10;
        if (arg0->count >= s->u14) {
            if (s->t16 == 5) {
                var_a0 = c->uEC;
            } else {
                var_a0 = c->uEE;
            }
            func_001A1E28(var_a0, s, p50 + 0xD0, arg0->i5C, arg0->u4E, arg0->p48);
            func_001A2A18(arg0);
        }
    }
    arg0->count = arg0->count + 1;
}
