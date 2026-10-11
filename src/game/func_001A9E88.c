#include "common.h"

void func_001AF928(char *, int);
void func_001AF640(void *, int);
void func_001AF538(float, void *, void *);
s16 func_001B0208(float, void *, void *, void *);

void func_001A9E88(char *arg0, int arg1) {
    char *temp_s2;
    char *temp_a0;
    u32 temp_v0;
    float temp_f0;

    temp_s2 = arg0 + 0x10;
    func_001AF928(arg0, arg1);
    func_001AF640(arg0, arg1);
    func_001AF538(*(float *)(*(char **)(arg0 + 0x54) + 0xD8), temp_s2, *(char **)(arg0 + 0x50) + 0xD0);
    temp_v0 = func_001B0208(*(float *)(*(char **)(arg0 + 0x54) + 0xDC), arg0, temp_s2, arg0 + 0x20);
    *(s16 *)(temp_s2 + 0x22) = temp_v0;
    temp_a0 = *(char **)(arg0 + 0x54);
    if (*(u16 *)(temp_a0 + 0xF0) < (u32)(temp_v0 & 0xFFFF)) {
        *(u16 *)(temp_s2 + 0x22) = 0xFFFF;
    }
    temp_f0 = *(float *)(temp_a0 + 0xE8);
    *(char *)(temp_s2 + 0x25) = 0;
    *(char *)(temp_s2 + 0x24) = (int)temp_f0;
    *(u16 *)(temp_s2 + 0x20) = *(u16 *)(*(char **)(arg0 + 0x54) + 0x118);
}
