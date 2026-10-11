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
    char pad0c[0x24];
    int f30;
} T;
extern S D_005CF410[];
extern T D_00428B80[];

int func_001F9FD0(int arg0, int arg1, int arg2) {
    S *s = D_005CF410;
    T *p = D_00428B80;

    if (s->f10 != 0) {
        return -1;
    }
    p->f30 = p->f8 = arg2;
    D_005CF410[0].f0 = arg0;
    s->f1A = arg1;
    s->f12 = 4;
    s->f10 = 4;
    s->f16 = 0;
    s->f14 = 0;
    s->f17 = 0;
    s->f15 = 0;
    return 0;
}
