#include "common.h"

extern u8 D_00413F80[];
extern int D_0038CC50[];
int Sound_PlayCommon(int);

void func_001B9610(void)
{
    u8 *s = D_00413F80;

    if (s[0x1A8] == 0) {
        if (*(s16 *)(s + 0x1A4) > 0) {
            return;
        }
        if (*(int *)(s + 0xB4) > 0) {
            return;
        }
        s[0x1A8] = 1;
        Sound_PlayCommon(0x28);
    }
    if (s[0x1A8] < 0x3C) {
        *(float *)(s + 0x10) += (1.012291f - *(float *)(s + 0x10)) / (0x3D - s[0x1A8]);
        s[0x1A8]++;
    } else {
        D_0038CC50[9] |= 0x200;
    }
}
