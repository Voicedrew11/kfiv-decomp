#include "kf_gs.h"

s32 func_00207320(int, s64);

/* Sets the blend mode of ALPHA_1 from mode and writes it; the value is never seeded. */
s32 func_001E53A8(u8 mode, char flag) {
    GsAlphaReg reg;

    if (flag == 0) {
        func_00207320(GS_REG_TEST_1, 0x30000);
    } else {
        func_00207320(GS_REG_TEST_1, 0x70000);
    }
    if (mode == 0) {
        reg.f.a_source = 0;
        reg.f.b_source = 1;
        reg.f.c_source = 0;
        reg.f.d_source = 1;
        reg.f.fix = 0;
    } else if (mode == 1) {
        reg.f.a_source = 0;
        reg.f.b_source = 2;
        reg.f.c_source = 0;
        reg.f.d_source = 1;
        reg.f.fix = 0;
    } else if (mode == 2) {
        reg.f.a_source = 2;
        reg.f.b_source = 0;
        reg.f.c_source = 0;
        reg.f.d_source = 1;
        reg.f.fix = 0;
    }
    return func_00207320(GS_REG_ALPHA_1, reg.raw);
}
