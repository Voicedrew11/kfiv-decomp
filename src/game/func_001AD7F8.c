#include "common.h"

void func_001AF928(char *, int);
void func_001AF640(void *, int);
void func_00216778(void *, void *);
int rand(void);

void func_001AD7F8(char *arg0, int arg1) {
    char *temp_s0;
    char *temp_v0;
    int hi;

    temp_s0 = arg0 + 0x10;
    func_001AF928(arg0, arg1);
    func_001AF640(arg0, arg1);
    func_00216778(temp_s0, *(char **)(arg0 + 0x50) + 0xC0);
    temp_v0 = *(char **)(arg0 + 0x54);
    *(int *)(temp_s0 + 0x14) = 0;
    *(float *)(temp_s0 + 0x10) = *(float *)(*(char **)(arg0 + 0x50) + 0xD4) + *(float *)(temp_v0 + 0xE0);
    *(float *)(temp_s0 + 0x18) = (float)*(u16 *)(temp_v0 + 0xEC) / ((float)*(u16 *)(temp_v0 + 0x102) * 0.5f);
    *(float *)(temp_s0 + 0x1C) = (float)*(u16 *)(temp_v0 + 0xEE) / (float)*(u16 *)(temp_v0 + 0x102);
    hi = rand() % 10;
    *(char *)(temp_s0 + 0x21) = 0;
    *(char *)(temp_s0 + 0x20) = (char)hi;
}
