#include "kf_gs.h"
#include "kf_packet.h"

void func_00109A50(void);
void func_00109A80(int, s64);
void func_00109AD0(void);
void func_0010A100(void *);

/* Fills the 640x448 screen with grey, z writes off and with register 8 set to 5 meanwhile. */
void func_00116998(void) {
    ScreenQuad quad;

    func_00109A50();
    func_00109A80(GS_REG_TEST_1, 0x30000);
    func_00109A80(GS_REG_ZBUF_1, 0x10A000118L);
    func_00109A80(8, 5);
    func_00109AD0();
    quad.tag = 0x624A16BC0L;
    quad.unk08 = 0x60;
    quad.unk10 = 0x116;
    quad.red = 0x80;
    quad.green = 0x80;
    quad.blue = 0x80;
    quad.alpha = 0x80;
    quad.x0 = 0x6C00;
    quad.y0 = 0x7200;
    quad.x1 = 0x9400;
    quad.y1 = 0x8E00;
    quad.u1 = 0x1400;
    quad.v1 = 0xE00;
    quad.z0 = 0;
    quad.u0 = 0;
    quad.v0 = 0;
    quad.z1 = 0;
    func_0010A100(&quad);
    func_00109A50();
    func_00109A80(GS_REG_ZBUF_1, 0xA000118);
    func_00109A80(8, 0);
    func_00109AD0();
}
