#include "common.h"

void func_001958B8(char *, void (*)(void), int);

typedef struct P {
    char pad0[0xC0];
    float c0;
    float c4;
    float c8;
    char pad1[0x8];
    float d4;
} P;

typedef struct A {
    P *p0;
    char pad[0xD2];
    char d6;
    char pad2;
    u64 b0 : 11;
    u64 b11 : 3;
    u64 b14 : 4;
    u64 b18 : 1;
    u64 b19 : 1;
    u64 b20 : 1;
    u64 b21 : 1;
    u64 b22 : 1;
    u64 b23 : 1;
    u64 b24 : 2;
    u64 b26 : 1;
    u64 b27 : 1;
    u64 b28 : 1;
    u64 b29 : 35;
    char pad3[0x18];
    void (*f8)(void);
} A;

void func_0013B4C8(A *arg0, int arg1) {
    P *p;

    p = arg0->p0;
    arg0->b27 = 1;
    arg0->b18 = 1;
    arg0->b20 = 1;
    arg0->b23 = 0;
    arg0->b26 = 1;
    arg0->b21 = 1;
    arg0->b28 = 0;
    arg0->d6 = 0;
    p->c0 = -16200.0f;
    p->c4 = 3030.0f;
    p->c8 = 4900.0f;
    p->d4 = 1.5707964f;
    arg0->b11 = 1;
    func_001958B8((char *)arg0, arg0->f8, arg1);
}
