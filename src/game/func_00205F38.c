#include "common.h"

typedef struct {
    u64 X : 16;
    u64 Y : 16;
    u64 Z : 32;
} Xyz;

typedef struct {
    Xyz a;
    s64 pad8;
    Xyz b;
    float f18;
    char pad1C[0x3F - 0x1C];
    s8 f3F;
    char pad40[0x60 - 0x40];
} S;

extern S D_005DDA00;
extern u8 D_00349681[];
extern u8 D_00349682[];
extern u8 D_00349683[];

S *func_00205F38(S *arg0) {
    u8 v;

    D_005DDA00 = *arg0;
    switch (arg0->f3F) {
    case 0:
        v = D_00349681[0];
        break;
    case 1:
        v = D_00349682[0];
        break;
    case 2:
        v = D_00349683[0];
        break;
    default:
        v = 0x7F;
        break;
    }
    D_005DDA00.b.X = D_005DDA00.a.X + v * 16;
    D_005DDA00.f18 = (float)(v * 2) / 255.0f;
    return &D_005DDA00;
}
