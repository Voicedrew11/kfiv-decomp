#include "kf_gs.h"
#include "kf_packet.h"

extern int D_00446F78;

void func_00109A50(void);
void func_00109A80(int, s64);
void func_00109AD0(void);
void func_00109E18(void);
void func_00109F80(s64, s64, s64);
void func_0010A100(void *);

/* Copy the displayed frame into a width x height texture at `fbp`. */
void func_0010AC80(int fbp, int psm, int width, int height) {
    ScreenSprite sprite;
    int frame_base;

    frame_base = 0x1180;
    if (D_00446F78 != 0) {
        frame_base = 0;
    }
    func_00109F80((fbp >> 5) | ((u64)(width / 64) << 16) | ((u64)psm << 24),
                  ((u64)(width - 1) << 16) | ((u64)(height - 1) << 48),
                  ((0x800 - (width >> 1)) << 4) | ((u64)((0x800 - (height >> 1)) << 4) << 32));
    func_00109A50();
    func_00109A80(GS_REG_ALPHA_1, 0x80000000A8L);
    func_00109A80(GS_REG_TEST_1, 0x51001);
    func_00109A80(8, 5);
    func_00109AD0();
    func_00109E18();
    sprite.tex0 = frame_base | 0x28028000L | (0xC800L << 19);
    sprite.tex1 = 0x60;
    sprite.prim = 0x156;
    sprite.rgba[3] = sprite.rgba[2] = sprite.rgba[1] = sprite.rgba[0] = 0x80;
    sprite.vertex[0].x = (0x800 - width / 2) << 4;
    sprite.vertex[0].y = (0x800 - height / 2) << 4;
    sprite.vertex[1].x = ((width / 2) << 4) + 0x7FF8;
    sprite.vertex[1].y = ((height / 2) << 4) + 0x7FF8;
    sprite.vertex[1].z = sprite.vertex[0].z = 0xFFFFFFFF;
    sprite.vertex[1].s = 0x2800;
    sprite.vertex[1].t = 0x1C00;
    sprite.vertex[0].s = 0;
    sprite.vertex[0].t = 0;
    func_0010A100(&sprite);
    func_00109A50();
    func_00109A80(8, 0);
    func_00109AD0();
    func_00109E18();
}
