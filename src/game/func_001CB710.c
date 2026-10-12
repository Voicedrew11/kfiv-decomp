#include "common.h"
#include "kf_gs.h"
#include "kf_screen.h"

void func_00109A50(void);
void func_00109A80(int, s64);
void func_00109AD0(void);
void func_0010AEB8(void *sprite);

void func_001CB710(OverlaySprite *sprite) {
    func_00109A50();
    func_00109A80(GS_REG_ALPHA_1, ((s64)sprite->unk146 << 32) | 0x64);
    func_00109A80(GS_REG_TEST_1, 0x51001);
    func_00109A80(0x48, 0x30001);
    func_00109AD0();
    func_0010AEB8(sprite);
}
