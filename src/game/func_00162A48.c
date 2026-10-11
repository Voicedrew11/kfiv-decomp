#include "common.h"

extern float func_001957B8(float, float);
extern int rand(void);

int func_00162A48(char *self, float arg1, float arg2) {
    float f20;
    float f1;

    if (*(s16 *)(self + 0x14C) <= 0) return 0;
    f20 = *(float *)(self + 0x64) - (*(float *)(self + 0x38) + 100.0f);
    f1 = func_001957B8(*(float *)(*(char **)self + 0xD4), *(float *)(self + 0x68));
    if (*(u16 *)(self + 0xC6) != 0) return 0;
    if (arg1 < f20) return 0;
    if (arg2 < (0.0f <= f1 ? f1 : -f1)) return 0;
    return (rand() % 100) < (int)(*(u8 *)(*(char **)(self + 0xC) + 0xCA) / 5U);
}
