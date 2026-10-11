#include "common.h"

typedef struct {
    float x;
    float y;
    float z;
    float w;
} Vec4 __attribute__((aligned(8)));

typedef struct {
    int a00;
    char pad04[4];
    char a08;
    char pad09[7];
    u8 a10;
    char pad11[7];
    u8 a18;
    char pad19[7];
    float a20;
    char pad24[4];
    float a28;
    char pad2C[4];
    float a30;
    char pad34[4];
    float a38;
    char pad3C[4];
    float a40;
    char pad44[4];
    float a48;
    char pad4C[4];
    float a50;
} Act;

void func_0019DAA8(char, u8, u8, Vec4 *, Vec4 *, float);

void func_002147A0(Act *arg0) {
    Vec4 a;

    a = (Vec4){arg0->a28, arg0->a30, arg0->a38, 1.0f};
    {
        Vec4 b;

        b = (Vec4){arg0->a40, arg0->a48, arg0->a50, 1.0f};
        func_0019DAA8(arg0->a08, arg0->a18, arg0->a10, &a, &b, arg0->a20);
    }
    arg0->a00 = 0;
}
