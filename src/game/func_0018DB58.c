#include "common.h"

extern float D_00413F80[];

float atan2f(float, float);
void func_001A0CD0(float, int, int, char, float, float);

void func_0018DB58(void *arg0, int arg1, int arg2) {
    float a;
    float d;

    a = atan2f(*(float *)(*(char **)arg0 + 0xC0) - D_00413F80[0],
               *(float *)(*(char **)arg0 + 0xC8) - D_00413F80[2]);
    a = a - D_00413F80[5];
    d = a - (float)(int)(a / 6.2831855f) * 6.2831855f;
    if (d > 3.1415927f) {
        d = d - 6.2831855f;
    } else if (d < -3.1415927f) {
        d = d + 6.2831855f;
    }
    func_001A0CD0((float)*(u16 *)(*(char **)((char *)arg0 + 0xC) + 0x42E),
                  arg1 & 0xFF, arg2 & 0xFF, *(char *)((char *)arg0 + 0xD2),
                  *(float *)((char *)arg0 + 0x60), d);
}
