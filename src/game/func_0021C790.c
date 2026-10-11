#include "common.h"

typedef struct {
    int a00;
    char pad04[0x28];
    u32 a2C;
    int a30;
    int a34;
    int a38;
    int a3C;
    int a40;
} Obj;

void func_0021C790(Obj *arg0, int arg1) {
    u32 temp_a0;
    u32 temp_v0;
    u32 temp_v0_2;

    if (arg0->a00 == 0) {
        temp_a0 = arg0->a2C;
        temp_v0 = 0x28 - temp_a0;
        temp_v0_2 = (temp_v0 >= arg1) ? arg1 : temp_v0;
        temp_a0 += temp_v0_2;
        arg0->a2C = temp_a0;
        if (temp_a0 >= 0x28U) {
            arg0->a00 = 1;
        }
        arg1 -= temp_v0_2;
    }
    temp_v0 = (arg0->a34 + arg1) % arg0->a3C;
    arg0->a38 = arg0->a38 + arg1;
    arg0->a40 = arg0->a40 + arg1;
    arg0->a34 = temp_v0;
}
