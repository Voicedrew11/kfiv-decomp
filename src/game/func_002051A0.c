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
    char pad18[0x41 - 0x18];
    s8 f41;
    char pad42[0x60 - 0x42];
} S;

extern S D_005DD6A0;

S *func_002051A0(S *arg0, int arg1) {
    float r = (float)arg1 / (float)arg0->f41;
    int t;

    D_005DD6A0 = *arg0;
    t = (int)(r * 224.0f) << 4;
    D_005DD6A0.a.Y += t;
    D_005DD6A0.b.Y += t;
    if (D_005DD6A0.a.Y > 0xFFF0) {
        D_005DD6A0.a.Y = 0xFFF0;
    }
    if (D_005DD6A0.b.Y > 0xFFF0) {
        D_005DD6A0.b.Y = 0xFFF0;
    }
    return &D_005DD6A0;
}
