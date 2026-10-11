#include "common.h"

extern float D_00445194;
extern float D_00445198;

typedef struct {
    char pad0[0x18];
    void (*f18)(void);
    char pad1C[0x78 - 0x1C];
    char f78;
} Sub;

typedef struct {
    char pad0[4];
    float f4;
} Ctl;

typedef struct {
    char *p0;
    char pad4[0xC];
    Ctl *c10;
    char pad14[0x5C - 0x14];
    float *f5C;
    char pad60[0xC8 - 0x60];
    s16 sC8;
    char padCA[0xD8 - 0xCA];
    u64 b0 : 11;
    u64 b11 : 3;
    u64 b14 : 50;
    Sub sub;
} Actor;

s16 func_00190F90(float, void *, int);
float func_00194F80(void *, void *);
void func_001958B8(void *, void (*)(void), int);
void func_00216778(void *, void *);

void func_0013ED58(Actor *arg0, int arg1) {
    float vec[4];
    float limit;
    Sub *s;
    float *v;

    limit = 50.0f;
    v = (float *)((char *)arg0 + 0x110);
    s = &arg0->sub;
    arg0->b11 = 1;
    vec[0] = D_00445194;
    vec[1] = arg0->c10->f4;
    vec[2] = D_00445198;
    vec[3] = 0;
    func_00216778(v, vec);
    arg0->f5C = v;
    arg0->sC8 = func_00190F90(25.0f, arg0, 0);
    if (func_00194F80(arg0->p0 + 0xC0, vec) < limit) {
        s->f78 = 1;
    }
    func_001958B8(arg0, s->f18, arg1);
}
