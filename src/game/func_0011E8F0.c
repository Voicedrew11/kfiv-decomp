#include "kf_gs.h"
#include "kf_screen.h"

void func_00109A50(void);
void func_00109A80(int, s64);
void func_00109AD0(void);
void func_0010AEB8(void *sprite);

/* Sets the GS Z-buffer, alpha and test registers for sprite, then hands it on. */
void func_0011E8F0(OverlaySprite *sprite) {
    func_00109A50();
    if (sprite->alpha < 128.0f) {
        func_00109A80(GS_REG_ZBUF_1, 0x10A000118L);
        func_00109A80(GS_REG_ZBUF_2, 0x10A000118L);
    } else {
        func_00109A80(GS_REG_ZBUF_1, 0xA000118L);
        func_00109A80(GS_REG_ZBUF_2, 0xA000118L);
    }
    func_00109A80(GS_REG_ALPHA_1, 0x44);
    func_00109A80(GS_REG_ALPHA_2, 9);
    func_00109A80(GS_REG_TEST_1, 0x5000D);
    func_00109A80(GS_REG_TEST_2, 0x5000D);
    func_00109AD0();
    func_0010AEB8(sprite);
}
