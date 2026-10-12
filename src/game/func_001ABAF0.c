#include "common.h"

extern int D_004468E8;

void func_001AEC48(char *);
void func_001AED58(char *);
int func_001AF0B0(char *);
void func_001AF298(char *, char *);
void func_00216778(float *, void *);
void sceVu0AddVector(char *, char *, char *);

void func_001ABAF0(char *arg0) {
    float v[4];
    char *s1;
    char *p;
    u16 n;
    float f;

    D_004468E8 = 0;
    func_001AEC48(arg0);
    s1 = arg0 + 0x10;
    func_001AED58(arg0);
    func_001AF0B0(arg0);
    n = *(u16 *)(*(char **)(arg0 + 0x54) + 0x108);
    if (*(u16 *)(arg0 + 0x4C) < n || n == 0) {
        func_00216778(v, s1);
        if (*(float *)(*(char **)(arg0 + 0x54) + 0x11C) != 0.0f) {
            f = *(float *)(s1 + 0x14) + *(float *)(*(char **)(arg0 + 0x54) + 0x11C);
            *(float *)(s1 + 0x14) = f;
            v[0] = v[0] * f;
            v[2] = v[2] * f;
        }
        p = *(char **)(arg0 + 0x50) + 0xC0;
        sceVu0AddVector(p, p, (char *)v);
    }
    if (*(u16 *)(*(char **)(arg0 + 0x54) + 0xF0) < *(u16 *)(arg0 + 0x4C)) {
        func_001AF298(arg0, s1 + 0x18);
    }
    *(u16 *)(arg0 + 0x4C) += 1;
}
