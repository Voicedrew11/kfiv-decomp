#include "common.h"

void func_001AF538(float *, float *, float);
void func_001AF640(void *, int);
void func_001AF928(char *, int);

void func_001AC400(char *arg0, int arg1) {
    float *temp_s2;

    temp_s2 = (float *)(arg0 + 0x10);
    func_001AF928(arg0, arg1);
    func_001AF640(arg0, arg1);
    func_001AF538(temp_s2, (float *)(*(char **)(arg0 + 0x50) + 0xD0), *(float *)(*(char **)(arg0 + 0x54) + 0xD8));
    *(s64 *)(arg0 + 0x40) |= (s64)0x8000 << 0x2B;
}
