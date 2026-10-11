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

extern S D_005DD580;

S *func_00204FB8(S *arg0, int arg1) {
    float scale = (float) arg1 / (float) arg0->f41;
    int adj;

    D_005DD580 = *arg0;
    adj = (int) (scale * 320.0f) * 16;
    D_005DD580.a.X -= adj;
    D_005DD580.b.X -= adj;
    return &D_005DD580;
}
