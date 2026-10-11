#include "common.h"

typedef struct {
    int f0;
    char pad4[0xC];
    s16 f10;
    s16 f12;
    char f14, f15, f16, f17;
    char pad18[2];
    char f1A;
} S;

typedef struct {
    char pad0[8];
    int f8;
    char pad0c[0x10];
    int f1C;
    char pad20[0x10];
    int f30;
    char pad34[0x10];
    int f44;
} T;

extern char D_00428B80[];
extern char D_005CF410[];

int func_001FA4F0(int arg0, int arg1, int arg2, int arg3) {
    T *t = (T *)D_00428B80;
    S *s = (S *)D_005CF410;
    int *e = (int *)(D_00428B80 + arg0 * 0x28);

    t->f8 = arg2;
    t->f1C = arg3;
    t->f30 = arg2;
    t->f44 = arg3;
    if (s->f10 != 0) {
        return -1;
    }
    if (e[0x8 / 4] == 0) {
        return -1;
    }
    s->f0 = arg0;
    s->f12 = 5;
    s->f10 = 5;
    s->f16 = 0;
    s->f14 = 0;
    s->f17 = 0;
    s->f15 = 0;
    s->f1A = arg1;
    return 0;
}
