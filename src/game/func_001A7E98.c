#include "common.h"

void func_001AF928(void);
void func_001AF640(void *, int);
void func_001AF538(float, void *, void *);
int rand(void);
extern float D_004460A4;
extern float D_004460A8;

void func_001A7E98(char *arg0, int arg1) {
    char *temp_s2;
    float temp_f0;
    float temp_f1;

    temp_s2 = arg0 + 0x10;
    func_001AF928();
    func_001AF640(arg0, arg1);
    func_001AF538(*(float *)(*(char **)(arg0 + 0x54) + 0xD8), temp_s2, *(char **)(arg0 + 0x50) + 0xD0);
    *(s64 *)(arg0 + 0x40) = *(s64 *)(arg0 + 0x40) | ((s64)0x8000 << 43);
    *(float *)(temp_s2 + 0x10) = (float)((((float)(rand() & 0xFFFF) * 0.000030517578f) - 1.0f) * 0.5f);
    temp_f0 = (float)(rand() & 0xFFFF);
    temp_f1 = (float)((((temp_f0 * 0.000030517578f) - 1.0f) * D_004460A4) + D_004460A8);
    *(float *)(temp_s2 + 0x18) = 1.0f;
    *(char *)(temp_s2 + 0x1D) = 0;
    *(float *)(temp_s2 + 0x14) = temp_f1;
}
