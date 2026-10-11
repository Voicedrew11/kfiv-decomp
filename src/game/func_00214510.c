#include "common.h"

typedef union {
    float f[4];
    s64 q[2];
} Vec4;

int memset(void *, int, int);
int func_0019D1C0(float, int, void *);

void func_00214510(int *arg0) {
    Vec4 b;
    Vec4 a;
    float temp_f2;

    memset(&a, 0, 0x10);
    temp_f2 = *(float *)((char *)arg0 + 0x30);
    a.f[0] = *(float *)((char *)arg0 + 0x20);
    a.f[1] = *(float *)((char *)arg0 + 0x28);
    a.f[2] = temp_f2;
    b.q[0] = a.q[0];
    b.q[1] = a.q[1];
    func_0019D1C0(temp_f2, arg0[4], &b);
    arg0[0] = 0;
}
