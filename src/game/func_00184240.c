#include "common.h"

float func_001957B8(float, float);
void func_00195898(char *, int);
void func_001958B8(char *, void (*)(void), int);
void func_00184550(void);

typedef struct A {
    struct A *p0;
    char pad[0xD4];
    u64 b0 : 9;
    u64 b9 : 1;
    u64 b10 : 54;
} A;

void func_00184240(char *arg0, int arg1) {
    char *s;
    float a;
    float g;

    s = arg0 + 0xE0;
    a = func_001957B8(*(float *)(*(char **)arg0 + 0xD4), *(float *)(*(char **)(arg0 + 0x10) + 0x14));
    g = (float)*(s16 *)(*(char **)(arg0 + 0xC) + 0x438) * 0.017453292f;
    if (0.0f <= a ? g < a : g < -a) {
        func_001958B8(arg0, func_00184550, arg1);
    } else {
        *(s16 *)(s + 0x68) = 0;
        ((A *)arg0)->b9 = 0;
        func_00195898(arg0, 0);
        func_001958B8(arg0, *(void (**)(void))(s + 0x18), arg1);
    }
}
