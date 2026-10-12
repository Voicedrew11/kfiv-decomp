#include "kf_map.h"
#include "kf_player.h"

extern PlayerState D_00413F80;
extern PlayerScreenTint D_00414580;
extern MapDrawState D_0038A1B0;
extern s32 D_00413EE8[][3];
extern const s8 D_0043F260[30];
extern u32 D_00446ED4;

/* Works out the colours laid over the screen (PlayerScreenTint) for the frame: which timed
   flag shows, how strong each tint is, and the map's glow. */
void func_001B69D0(void) {
    s32 newest = -1;
    s32 darkened = FALSE;
    u32 tick;                       /* the frame count */
    s32 colour;
    s32 next_colour;
    s32 blend;
    u16 cycling;

    D_00414580.kind = 0;
    if (newest < (s32)D_00413F80.unk2C4 && (u16)D_00413F80.timed_flags[0] != 0) {
        D_00414580.kind = 0;
        newest = D_00413F80.unk2C4;
    }
    if (newest < (s32)D_00413F80.unk2C8 && (u16)D_00413F80.timed_flags[1] != 0) {
        D_00414580.kind = 1;
        newest = D_00413F80.unk2C8;
    }
    if (newest < (s32)D_00413F80.unk2CC && (u16)D_00413F80.timed_flags[2] != 0) {
        darkened = TRUE;
    }
    if (newest < (s32)D_00413F80.unk2D0 && (u16)D_00413F80.timed_flags[3] != 0) {
        D_00414580.kind = 3;
        newest = D_00413F80.unk2D0;
    }
    if (newest < (s32)D_00413F80.unk2D4 && (u16)D_00413F80.timed_flags[4] != 0) {
        D_00414580.kind = 4;
        newest = D_00413F80.unk2D4;
    }
    if (newest < (s32)D_00413F80.unk2D8 && (u16)D_00413F80.timed_flags[5] != 0) {
        D_00414580.kind = 5;
    }

    D_00414580.unk04 = D_00414580.unk00 * 8;
    if (D_00414580.unk00 > 0) {
        if (D_00413F80.aim_speed > 0.0f) {
            D_00414580.unk00 = 0;
        } else {
            D_00414580.unk00--;
        }
    }

    if ((u16)D_00413F80.timed_flags[0] != 0) {
        D_00414580.unk08++;
        if (D_00414580.unk08 > 0x20) {
            D_00414580.unk08 = 0x20;
        }
    } else {
        D_00414580.unk08--;
        if (D_00414580.unk08 < 0) {
            D_00414580.unk08 = 0;
        }
    }

    if ((u16)D_00413F80.timed_flags[1] != 0) {
        D_00414580.unk0C = D_0043F260[29 - (u16)D_00413F80.timed_flags[1] % 30];
    } else {
        D_00414580.unk0C = 0;
    }

    tick = D_00446ED4;
    if (!(D_00446ED4 & 1)) {
        if ((u16)D_00413F80.timed_flags[3] != 0) {
            D_00414580.unk14++;
            if (D_00414580.unk14 > 0x20) {
                D_00414580.unk14 = 0x20;
            }
        } else {
            D_00414580.unk14--;
            if (D_00414580.unk14 < 0) {
                D_00414580.unk14 = 0;
            }
        }
    }

    if ((u16)D_00413F80.timed_flags[4] != 0) {
        D_00414580.unk18++;
        if (D_00414580.unk18 > 0x20) {
            D_00414580.unk18 = 0x20;
        }
    } else {
        D_00414580.unk18--;
        if (D_00414580.unk18 < 0) {
            D_00414580.unk18 = 0;
        }
    }

    cycling = D_00413F80.timed_flags[5];
    colour = (tick >> 5) % 3;
    next_colour = (colour + 1) % 3;
    blend = tick & 0x1F;
    tick = 0; // match: a dead store; without it the frame count is not copied to a second register
    D_00414580.red = D_00413EE8[colour][0] * (0x20 - blend) / 32 + D_00413EE8[next_colour][0] * blend / 32;
    D_00414580.green = D_00413EE8[colour][1] * (0x20 - blend) / 32 + D_00413EE8[next_colour][1] * blend / 32;
    D_00414580.blue = D_00413EE8[colour][2] * (0x20 - blend) / 32 + D_00413EE8[next_colour][2] * blend / 32;
    if (cycling != 0) {
        if (!(D_00446ED4 & 1)) {
            D_00414580.unk28++;
            if (D_00414580.unk28 > 0x18) {
                D_00414580.unk28 = 0x18;
            }
        }
    } else if (!(D_00446ED4 & 1)) {
        D_00414580.unk28--;
        if (D_00414580.unk28 < 0) {
            D_00414580.unk28 = 0;
        }
    }

    if (D_00413F80.unk2E0 != 0 || D_00413F80.unk10C != 0) {
        D_0038A1B0.unk2A90 += 0.0078125f;
        if (D_0038A1B0.unk2A90 > 0.25f) {
            D_0038A1B0.unk2A90 = 0.25f;
        }
        D_00414580.unk34++;
        if (D_00414580.unk34 > 0xA0) {
            D_00414580.unk34 = 0xA0;
        }
    } else {
        D_0038A1B0.unk2A90 -= 0.0078125f;
        if (D_0038A1B0.unk2A90 < 0.0f) {
            D_0038A1B0.unk2A90 = 0.0f;
        }
        D_00414580.unk34--;
        if (D_00414580.unk34 < 0x80) {
            D_00414580.unk34 = 0x80;
        }
    }
    D_0038A1B0.unk2A98 = D_0038A1B0.unk2A94 = D_0038A1B0.unk2A90;

    if (D_00413F80.unk2E4 != 0) {
        darkened = TRUE;
    }
    if (D_00413F80.unk3C == 0x2B &&
        !(D_00413F80.unk3C == 0x2B && D_00413F80.equipment[0] == 0x19 && D_00413F80.equipment[1] == 0x35 && D_00413F80.equipment[2] == 0x4D &&
          D_00413F80.equipment[3] == 0x66 && D_00413F80.equipment[4] == 0x80)) {
        darkened = TRUE;
    } else if (D_00413F80.equipment[1] == 0x31) {
        darkened = TRUE;
    }
    if (darkened) {
        D_00414580.unk10 += 2;
        if (D_00414580.unk10 > 0x50) {
            D_00414580.unk10 = 0x50;
        }
    } else {
        D_00414580.unk10 -= 2;
        if (D_00414580.unk10 < 0) {
            D_00414580.unk10 = 0;
        }
    }

    if (D_00413F80.unk1A8 == 0) {
        D_00414580.unk30 = 0;
    } else {
        D_00414580.unk30 = D_00413F80.unk1A8 * 0x80 / 60;
    }
}
