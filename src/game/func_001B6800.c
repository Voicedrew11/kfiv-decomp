#include "kf_player.h"

extern PlayerScreenTint D_00414580;

void func_0010A4E8(int, int, int, int, int);

/* Draws the colours that lie over the screen (PlayerScreenTint). */
void func_001B6800(void) {
    if (D_00414580.unk08 != 0 && D_00414580.kind == 0) {
        func_0010A4E8(0, 0xFF, 0, D_00414580.unk08, 0x48);
    }
    if (D_00414580.unk0C != 0 && D_00414580.kind == 1) {
        func_0010A4E8(0x80, 0x80, 0, D_00414580.unk0C, 0x44);
    }
    if (D_00414580.unk14 != 0 && D_00414580.kind == 3) {
        func_0010A4E8(0xA0, 0, 0xA0, D_00414580.unk14, 0x48);
    }
    if (D_00414580.unk18 != 0 && D_00414580.kind == 4) {
        func_0010A4E8(0xFF, 0xFF, 0xFF, D_00414580.unk18, 0x48);
    }
    if (D_00414580.unk28 != 0 && D_00414580.kind == 5) {
        func_0010A4E8(D_00414580.red, D_00414580.green, D_00414580.blue, D_00414580.unk28, 0x48);
    }
    if (D_00414580.unk10 > 0) {
        func_0010A4E8(0, 0, 0, D_00414580.unk10, 0x44);
    }
    if (D_00414580.unk04 != 0) {
        if (D_00414580.unk14 != 0 && D_00414580.kind == 3) {
            func_0010A4E8(0x60, 0, 0, D_00414580.unk04, 0x48);
        } else {
            func_0010A4E8(0xFF, 0, 0, D_00414580.unk04, 0x48);
        }
    }
    if (D_00414580.unk30 != 0) {
        func_0010A4E8(0, 0, 0, D_00414580.unk30, 0x44);
    }
}
