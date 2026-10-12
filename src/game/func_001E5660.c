#include "kf_player.h"

extern PlayerState D_00413F80;

/* Tells whether the item numbered value is the one the player has in the given equipment slot. */
int func_001E5660(u32 slot, s32 kind, s32 value) {
    switch (slot) {
        case 0:
            if (value == D_00413F80.unk3C + 150) {
                return TRUE;
            }
            break;
        case 1:
            if (value == D_00413F80.unk3D) {
                return TRUE;
            }
            break;
        case 2:
            if (value == D_00413F80.unk3E + 220) {
                return TRUE;
            }
            break;
        case 3:
            if (value == D_00413F80.unk3F + 220) {
                return TRUE;
            }
            break;
        case 4:
            if (value == D_00413F80.unk40 + 220) {
                return TRUE;
            }
            break;
        case 5:
            if (value == D_00413F80.unk41 + 220) {
                return TRUE;
            }
            break;
        case 6:
            if (value == D_00413F80.unk42 + 220) {
                return TRUE;
            }
            break;
        case 7:
            if (value == D_00413F80.unk43 + 220) {
                return TRUE;
            }
            break;
        case 8:
            if (value == D_00413F80.unk44 + 220) {
                return TRUE;
            }
            break;
        case 9:
            if (value == D_00413F80.unk45 + 220) {
                return TRUE;
            }
            break;
        case 10:
            if ((kind == 11 && D_00413F80.unk48 < 2) || (kind == 1 && D_00413F80.unk48 == 2)) {
                if (value == D_00413F80.unk46) {
                    return TRUE;
                }
            }
            break;
    }
    return FALSE;
}
