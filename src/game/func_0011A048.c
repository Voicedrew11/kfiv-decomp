#include "common.h"

extern u8 D_0034A017[];
extern char D_0038CD20[];
extern const float D_00444C98;
extern const float D_00444C9C;
extern const float D_00444CA0;
extern const float D_00444CA4;

typedef struct {
    char pad0[0x20];
    float f20, f24, f28, f2C;
    s64 l30;
    char pad38[8];
    float f40, f44, f48, f4C;
    s64 l50;
} T;

void func_0011A048(int arg0) {
    char *p = D_0038CD20 + arg0 * 0x60;
    if (D_0034A017[0] == 1) {
        T *q = *(T **)(p + 0xC);
        float zero = 0.0f, a = D_00444C98, b = D_00444C9C, c = D_00444CA0, d = D_00444CA4;
        q->f20 = q->f24 = zero;
        q->f28 = a;
        q->f2C = b;
        q->l30 = 0;
        q->f40 = q->f44 = zero;
        q->f48 = c;
        q->f4C = d;
        q->l50 = 0;
    }
}
