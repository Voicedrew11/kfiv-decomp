#include "kf_gs.h"
#include "kf_player.h"

extern PlayerState D_00413F80;

void func_00109A50(void);
void func_00109A80(int, s64);
void func_00109AD0(void);
void func_0010A4E8(int, int, int, int, int);

/* Fades the screen toward white (D_00413F80.unk2A8 > 0) or black (< 0) by that amount. */
void func_001BBA58(void) {
    if (D_00413F80.unk2A8 != 0.0f) {
        func_00109A50();
        func_00109A80(GS_REG_TEST_1, 0x30000);
        func_00109A80(GS_REG_ZBUF_1, 0x10A000118L);
        func_00109AD0();
        if (D_00413F80.unk2A8 > 0.0f) {
            func_0010A4E8(0xFF, 0xFF, 0xFF, (int)D_00413F80.unk2A8, 0x48);
        } else if (D_00413F80.unk2A8 < 0.0f) {
            func_0010A4E8(0, 0, 0, (int)-D_00413F80.unk2A8, 0x44);
        }
        func_00109A50();
        func_00109A80(GS_REG_ZBUF_1, 0xA000118L);
        func_00109AD0();
    }
}
