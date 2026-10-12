#include "kf_gs.h"
#include "kf_packet.h"
#include "kf_screen.h"

extern ScreenFade D_0038D680;

void func_00109A50(void);
void func_00109A80(int, s64);
void func_00109AD0(void);
void func_0010A100(void *);

/* Map change drawing step: darkens the screen in ten strips, more each frame, until the timer runs out. */
void func_00118A98(void) {
    ScreenQuad quad;
    int remaining = 9;
    int total;

    func_00109A50();
    func_00109A80(GS_REG_TEST_1, 0x30000);
    func_00109A80(GS_REG_ZBUF_1, 0x10A000118L);
    func_00109A80(GS_REG_ALPHA_1, 0x88);
    func_00109AD0();
    total = 30;
    quad.tag = 0x26802ABC0L;
    quad.unk10 = 0x156;
    quad.x0 = 0x6BF8;
    quad.y0 = 0x71F8;
    quad.x1 = 0x7000;
    quad.y1 = 0x8E00;
    quad.unk08 = 0;
    quad.z1 = quad.z0 = 0xFFFFFFFF;
    quad.u1 = 0x400;
    quad.v1 = 0x1C00;
    quad.u0 = 0;
    quad.v0 = 0;
    quad.red = quad.green = quad.blue = (D_0038D680.timer << 7) / total;
    quad.alpha = 0x80;
    do {
        remaining--;
        func_0010A100(&quad);
        quad.x0 += 0x400;
        quad.x1 += 0x400;
        quad.u0 += 0x400;
        quad.u1 += 0x400;
    } while (remaining >= 0);
    func_00109A50();
    func_00109A80(GS_REG_ZBUF_1, 0xA000118);
    func_00109AD0();
    if (--D_0038D680.timer <= 0) {
        D_0038D680.draw_step++;
    }
}
