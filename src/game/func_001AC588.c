#include "common.h"

void func_001AF928(char *, int);
void func_001AF640(void *, int);
void func_001C1430(float, void *, void *);
void func_00216778(void *, void *);
int func_001A2A18(void *);

void func_001AC588(char *arg0, int arg1) {
    float vec[4];
    char *temp_s2;
    char *p54;
    char *p50;

    temp_s2 = arg0 + 0x10;
    func_001AF928(arg0, arg1);
    func_001AF640(arg0, arg1);
    p54 = *(char **)(arg0 + 0x54);
    p50 = *(char **)(arg0 + 0x50);
    vec[0] = 0;
    vec[1] = -*(u16 *)(p54 + 0xEC);
    vec[2] = 0;
    vec[3] = 0;
    *(float *)(p50 + 0xC4) -= *(u16 *)(p54 + 0xEE);
    func_001C1430(1.0f, p50 + 0xC0, vec);
    if (*(int *)0x70000010 != 0) {
        func_00216778(*(char **)(arg0 + 0x50) + 0xC0, (void *)0x70000000);
    } else {
        func_001A2A18(arg0);
    }
    *(float *)(temp_s2 + 0x0) = 0;
    *(float *)(temp_s2 + 0x4) = *(float *)(*(char **)(arg0 + 0x54) + 0xD8);
    *(float *)(temp_s2 + 0x8) = 0;
    *(float *)(temp_s2 + 0xC) = 0;
}
