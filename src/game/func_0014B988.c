#include "common.h"

int rand(void);
void func_0018B778(float, void *, void *, void *);
s16 func_00190F90(float, void *, int);
void func_001958B8(char *, void (*)(void), int);

typedef struct {
    char pad0[0x18];
    void (*f18)(void);
} Sub;

typedef struct {
    char pad0[0x4C];
    u16 s4C;
} Ctl;

typedef struct {
    u64 z0 : 11;
    u64 f11 : 3;
    u64 z14 : 50;
} Flags;

typedef struct {
    char *p0;
    char pad4[0xC];
    Ctl *c;
    char pad14[0x5C - 0x14];
    void *p5C;
    char pad60[0xA0 - 0x60];
    u16 sA0;
    char padA2[0xC8 - 0xA2];
    s16 sC8;
    char padCA[0xD8 - 0xCA];
    union { Flags b; u64 l; } flags;
    Sub sub;
} Actor;

void func_0014B988(Actor *arg0, int arg1) {
    Sub *s = &arg0->sub;

    arg0->flags.b.f11 = 0;
    if (arg0->sA0 == 0) {
        if (rand() % 100 >= 0x15) {
            void *p = (char *)arg0 + 0x110;
            func_0018B778((float)arg0->c->s4C, p, arg0->c, arg0->p0 + 0xC0);
            arg0->p5C = p;
            arg0->sA0 = rand() % 90 + 0xF0;
        } else {
            arg0->p5C = 0;
            arg0->sA0 = rand() % 60 + 0x5A;
        }
    }
    arg0->sC8 = func_00190F90(50.0f, arg0, 0);
    func_001958B8((char *)arg0, s->f18, arg1);
}
