#include "common.h"

typedef struct {
    char pad0[0xEC];
    u16 uEC;
    char pad1[0x14];
    u16 u102;
} Ctl;

typedef struct {
    int cnt;
    u16 u4;
} Sub;

typedef struct {
    char pad0[0x10];
    Sub sub;
    char pad1[0x30];
    void *p48;
    u16 u4C;
    u16 u4E;
    char *p50;
    Ctl *p54;
    char pad2[4];
    int i5C;
} Actor;

void func_001AEC48(void *);
void func_001AED58(void *);
int func_001AF0B0(void *);
void func_001A1E28(u16, void *, void *, int, u16, void *);

void func_001ADFC0(Actor *arg0) {
    Sub *s;
    Ctl *c;
    u16 t;
    int n;

    func_001AEC48(arg0);
    s = &arg0->sub;
    func_001AED58(arg0);
    func_001AF0B0(arg0);
    n = s->cnt - 1;
    s->cnt = n;
    if (n <= 0) {
        c = arg0->p54;
        t = s->u4;
        arg0->u4C = arg0->u4C + 1;
        s->cnt = t;
        if (arg0->u4C >= c->u102) {
            func_001A1E28(c->uEC, arg0->p50 + 0xC0, arg0->p50 + 0xD0, arg0->i5C, arg0->u4E, arg0->p48);
            arg0->u4C = 0;
        }
    }
}
