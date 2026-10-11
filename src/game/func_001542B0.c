#include "common.h"

extern float D_00413F80[];

float func_00195478(float *, char *);
float func_00194FB8(float);
void func_001A0CD0(float, int, int, char, float, float);

void func_001542B0(char *arg0) {
    u64 fl;
    float f;

    fl = *(u64 *)(arg0 + 0xD8) & ~0x20000000ULL;
    *(u64 *)(arg0 + 0xD8) = fl;
    if ((int)(fl >> 43) & 1) {
        if (*(u16 *)(arg0 + 0xA0) == 0) {
            *(u64 *)(arg0 + 0xD8) = fl | 0x20000000ULL;
            f = func_00195478(D_00413F80, *(char **)arg0 + 0xC0);
            f = func_00194FB8(f - D_00413F80[5]);
            func_001A0CD0((float)*(u16 *)(*(char **)(arg0 + 0xC) + 0x42E),
                          *(u8 *)(*(char **)(arg0 + 0xC) + 0x432),
                          *(u8 *)(*(char **)(arg0 + 0xC) + 0x430),
                          *(char *)(arg0 + 0xD2),
                          *(float *)(arg0 + 0x60), f);
            *(u16 *)(arg0 + 0xA0) = *(u16 *)(*(char **)(arg0 + 0xC) + 0x434);
        }
    }
    *(u64 *)(arg0 + 0xD8) = *(u64 *)(arg0 + 0xD8) & ~(1ULL << 43);
}
