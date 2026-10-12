#include "kf_gs.h"

s32 func_00207320(int, s64);

// match: called as a void function, so the result register is free at once
#define SET_TEST(value) ((void (*)(int, s64))func_00207320)(GS_REG_TEST_1, value)

/* Sets the alpha test and the blend mode of ALPHA_1 from mode and writes them; the value is never seeded. */
int func_001E5210(u8 mode) {
    GsAlphaReg reg;

    if (mode == 0) {
        SET_TEST( 0x30000);
        reg.f.a_source = 0;
        reg.f.b_source = 1;
        reg.f.c_source = 0;
        reg.f.d_source = 1;
        reg.f.fix = 0;
    } else if (mode == 1) {
        SET_TEST( 0x30000);
        reg.f.a_source = 0;
        reg.f.b_source = 2;
        reg.f.c_source = 0;
        reg.f.d_source = 1;
        reg.f.fix = 0;
    } else if (mode == 2) {
        SET_TEST( 0x30000);
        reg.f.a_source = 2;
        reg.f.b_source = 0;
        reg.f.c_source = 0;
        reg.f.d_source = 1;
        reg.f.fix = 0;
    } else if (mode == 3) {
        SET_TEST( 3);
        reg.f.a_source = 0;
        reg.f.b_source = 2;
        reg.f.c_source = 0;
        reg.f.d_source = 0;
        reg.f.fix = 0;
    }
    return func_00207320(GS_REG_ALPHA_1, reg.raw);
}
