#include "kf_player.h"
#include "kf_entity.h"

extern PlayerState D_00413F80;
extern ObjectReference *D_0044712C;
extern u16 D_0044692C;
extern u16 D_0044692E;
extern u16 D_00446930;
extern u16 D_00446932;
extern u16 D_00446934;
extern u16 D_00446936;
extern int D_00447128;
extern int D_0044713C;
extern int D_00447144;
extern int D_00447148;
extern int D_0044714C;

void func_001BD808(u16 buttons) {
    if (!(D_0044712C->unk10 & 0x10000)) {
        D_00413F80.unk1C8 &= ~(1L << 36);
    }
    if (buttons & (D_00446934 | D_00446936)) {
        D_00413F80.unk1C8 &= ~(1L << 36);
    }
    if ((buttons & (D_00446930 | D_00446932)) && !(buttons & (D_0044692C | D_0044692E))) {
        D_00413F80.unk1C8 &= ~(1L << 36);
    }
    if (D_0044692C & buttons) {
        D_00447144++;
        if (D_00447144 <= D_0044713C) {
            D_0044714C = 0;
            D_00447128++;
            return;
        }
    } else {
        if (!(D_0044692E & buttons)) {
            return;
        }
        D_00447144--;
        if (D_00447144 >= 0) {
            D_0044714C = D_00447148;
            D_00447128 += 2;
            return;
        }
    }
    D_00413F80.unk1C8 &= ~(1L << 36);
}
