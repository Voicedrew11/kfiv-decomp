#include "common.h"

typedef struct {
    char pad0[0xC];
    float f0C;
    float f10;
    float f14;
    char pad1[0x34];
    u16 u4C;
    char pad2[2];
    int p50;
    char pad3[0x10];
    int i64;
    int i68;
} Actor;

extern int D_00413F80[];
void func_001AEC48(Actor *);
void func_001AED58(Actor *);
int func_001AF0B0(Actor *);
float func_001B0178(int *, int);
void func_00216778(int, int);

void func_001AAAC0(Actor *arg0) {
    float *s;
    float var;
    float t;

    func_001AEC48(arg0);
    s = &arg0->f10;
    func_001AED58(arg0);
    func_001AF0B0(arg0);
    t = func_001B0178(D_00413F80, arg0->p50 + 0xC0);
    if (s[1] < t) {
        var = 1.0f;
    } else {
        if (t < arg0->f10) {
            var = 0.0f;
        } else {
            var = (t - arg0->f10) / (s[1] - arg0->f10);
        }
    }
    arg0->f0C = arg0->f0C * var;
    func_00216778(arg0->p50 + 0xC0, arg0->i64);
    func_00216778(arg0->p50 + 0xD0, arg0->i68);
    arg0->u4C = arg0->u4C + 1;
}
