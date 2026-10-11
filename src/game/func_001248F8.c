#include "kf_gs.h"
#include "kf_screen.h"

void func_00109A50(void);
void func_00109A80(int, s64);
void func_00109AD0(void);
void func_0010AEB8(void *sprite);

/* Sets the GS Z-buffer, alpha and test registers, then draws the sprite. */
void func_001248F8(OverlaySprite *sprite) {
    func_00109A50();
    func_00109A80(GS_REG_ZBUF_1, 0x10A000118L);
    func_00109A80(GS_REG_TEST_1, 0x50000);
    func_00109A80(GS_REG_ALPHA_1, 0x48);
    func_00109AD0();
    func_0010AEB8(sprite);
}
