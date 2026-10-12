#include "common.h"

extern int D_00413F80[];

void func_001AEC48(char *);
void func_001AED58(char *);
int func_001AF0B0(char *);
void func_001AF298(char *, char *);
void func_001AF5B0(char *, char *);
void func_001AFC88(float, float, char *, float *);
void KF_Vector_Subtract(float *, int *, char *);
void sceVu0AddVector(char *, char *, char *);

void func_001AB4D8(char *arg0) {
    float v[4];
    char *s1;
    char *t;
    char *p;
    u16 n;

    func_001AEC48(arg0);
    s1 = arg0 + 0x10;
    func_001AED58(arg0);
    func_001AF0B0(arg0);
    KF_Vector_Subtract(v, D_00413F80, *(char **)(arg0 + 0x50) + 0xC0);
    t = *(char **)(arg0 + 0x54);
    if (*(u16 *)(arg0 + 0x4C) < *(u16 *)(t + 0xEC)) {
        v[1] = v[1] - 220.0f;
        func_001AFC88(*(float *)(t + 0xD8), *(float *)(t + 0xDC), s1, v);
        func_001AF5B0(*(char **)(arg0 + 0x50) + 0xD0, s1);
        t = *(char **)(arg0 + 0x54);
    }
    n = *(u16 *)(t + 0x108);
    if (*(u16 *)(arg0 + 0x4C) < n || n == 0) {
        p = *(char **)(arg0 + 0x50) + 0xC0;
        sceVu0AddVector(p, p, s1);
    }
    func_001AF298(arg0, s1 + 0x10);
    *(u16 *)(arg0 + 0x4C) += 1;
}
