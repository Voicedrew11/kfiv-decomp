#include "kf_player.h"
#include "kf_game.h"
#include "kf_map.h"
#include "kf_effect.h"
#include "kf_sound.h"

extern PlayerState D_00413F80;
extern PlayerState D_00414280;      /* the player as of the frame before */
extern PlayerItemDef D_0041B9B8[];
extern GameStatus D_0038CC50;
extern MapSlot D_0038CD20[];
extern FxTableEntry D_00402DA8[];
extern SoundLoadQueue D_0038A130;
extern u16 D_00413DA8[];
extern u8 D_00369830[];
extern s8 D_00349680[];
extern u8 D_00349F56[];
extern int D_00446770;
extern u32 D_00446ED4;
extern f32 D_00447124;
extern int D_00446950;              /* frames until the item 0x32 may launch its effect again */
extern f32 D_00446954[];
extern s8 D_00446958;               /* what the last footstep sound returned */
extern int D_0044695C;

/* the pad bits of each control */
extern u16 D_0044692C;              /* forward */
extern u16 D_0044692E;              /* back */
extern u16 D_00446930;              /* turn right */
extern u16 D_00446932;              /* turn left */
extern u16 D_00446934;              /* step right */
extern u16 D_00446936;              /* step left */
extern u16 D_00446938;              /* look down */
extern u16 D_0044693A;              /* look up */
extern u16 D_0044693C;
extern u16 D_0044693E;
extern u16 D_00446940;              /* run */
extern u16 D_00446942;
extern u16 D_00446944;              /* magic */

void *memset(void *, int, u32);
float sinf(float);
int rand(void);
int func_0019D020(int);
int func_0019D0F0(int);
void *func_001B1390(void);
int func_001B4F30(u16, void *, void *, void *, void *);
void func_001B69D0(void);
void func_001B6FA0(u16, u16);
void func_001B7EE0(u16, u16); // match: retail passes both pad words
void func_001B9078(void);
void func_001B9478(void);
void func_001B94C0(void);
void func_001B9588(void);
void func_001B9610(void);
void func_001B96C8(void);
void func_001BA310(void);
void func_001BB2E8(int);
void func_001BB908(void);
void func_001BB978(void);
int func_001BC9E0(int);
int func_001BD710(u16, u16);

#define STRIDE_LENGTH 0x1400

/* Makes sure the player's map record index is a valid one. */
static inline void check_map_index(void) {
    PlayerState *player = &D_00413F80;
    u32 map_index = player->map_index; // match: a wide local, narrowed where it is tested

    if (map_index >= D_0038CD20[D_00446770].header->record_count) {
        player->map_index = 0xFFFF;
        map_index = 0xFFFF;
        player->unk250 = 0xFF;
    }
    if ((u16)map_index == 0xFFFF) { // match: see map_index
        func_001BB908();
        if (player->map_index == 0xFFFF) {
            player->map_index = 0;
            player->unk250 = ((MapRecordInfo *)D_0038CD20[D_00446770].records)->unk38;
        }
    }
}

/* The player's frame: reads the pad, runs movement, magic and the item in hand, footsteps and timers. */
void func_001B5A48(void) {
    Vector4 zero;                   // match: cleared and never read
    f32 unused[16];                 // match: retail's frame has room for this
    PlayerItemDef *item_def = &D_0041B9B8[D_00413F80.unk3C];
    u16 held;
    u16 pressed;
    u16 mask;
    f32 stride_speed;
    s32 stride_step;
    s32 sound_free;
    PlayerShot *shot;

    memset(&zero, 0, sizeof(Vector4));
    check_map_index();
    /* not playing (unk00 clear, or bit 0 of unk30 set): the walk state is cleared */
    if (D_0038CC50.unk00 == 0 || (D_0038CC50.unk30 & 1)) {
        func_001B9478();
    }
    held = D_00413DA8[0];
    pressed = D_00413DA8[1];
    D_00414280 = D_00413F80;

    PLAYER_STATUS(&D_00413F80)->bit35 = FALSE;
    D_00413F80.unk1E4 = 6;
    D_00413F80.unk1EC = 0;
    D_00413F80.unk1E0 = 0;
    if (D_0038CC50.unk00 != 0 && !(D_0038CC50.unk30 & 1)) {
        PLAYER_STATUS(&D_00413F80)->bit38 = FALSE;
        PLAYER_STATUS(&D_00413F80)->bit39 = FALSE;
        D_00413F80.unk1DC = D_00446954[0];
        PLAYER_STATUS(&D_00413F80)->bit34 = TRUE;
    } else {
        PLAYER_STATUS(&D_00413F80)->bit34 = FALSE;
    }
    if (D_0044695C != 0) {
        D_0044695C--;
    }

    /* what the pad may do this frame */
    if (D_0038CC50.unk30 & 1) {
        pressed = 0;
        held = 0;
    }
    if (D_0038CC50.unk00 == 0) {
        pressed = 0;
        held = 0;
    }
    if (D_00413F80.unk1A8 != 0) {
        pressed = 0;
        held = 0;
    }
    if (D_0038CC50.unk07 == 0) {
        held &= ~(D_0044692C | D_0044692E | D_00446934 | D_00446936);
        pressed &= ~(D_0044692C | D_0044692E | D_00446934 | D_00446936);
    }
    if ((u16)D_00413F80.timed_flags[1] != 0) {
        mask = ~(D_0044693C | (D_0044692C | D_0044692E | D_00446934 | D_00446936 | D_00446930 | D_00446932 |
                               D_00446938 | D_0044693A) | D_0044693E);
        held &= mask;
        pressed &= mask;
    }
    if (D_0038CC50.bit6 || D_0038CC50.unk44 > 0) {
        held &= ~(D_0044693E | D_00446940 | D_00446942 | D_00446944);
        pressed &= ~(D_0044693E | D_00446940 | D_00446942 | D_00446944);
    }

    /* the magic button: an event magic is handed to the game, any other is cast when it is charged and paid for */
    if (!((D_0044693C | D_0044693E | D_00446940 | D_00446942) & pressed)) {
        if ((D_00446944 & pressed) && D_00413F80.unk46 != 0 && D_00413F80.unk1A8 == 0 &&
            !(D_0038CC50.bit2 || D_0038CC50.bit3 || D_0038CC50.bit4)) {
            switch (D_00413F80.unk48) {
            case 0:
                break;
            case 1:
                if (D_00369830[D_00413F80.unk46] != 0) {
                    D_0038CC50.unk20 = D_00413F80.unk46;
                    D_0038CC50.bit6 = TRUE;
                }
                break;
            case 2:
                if (D_00413F80.unkC0 >= 5000 && D_00413F80.unkB8 >= D_00402DA8[D_00413F80.unk46].unkFC) {
                    func_001BC9E0(D_00413F80.unk46);
                }
                break;
            }
        }
    }
    D_0038CC50.bit1 = FALSE;
    if (D_00446940 & pressed) {
        D_0038CC50.bit1 = TRUE;
        if (D_0038CC50.bit6 || D_00413F80.unk1A8 != 0) {
            D_0038CC50.bit1 = FALSE;
        }
    }

    /* running: the run button with forward or back, or with one of the two steps to the side */
    if ((D_00446940 & held) &&
        (((D_0044692C | D_0044692E) & held) || ((D_00446936 & held) && !(D_00446934 & held)) ||
         (!(D_00446936 & held) && (D_00446934 & held)))) {
        PLAYER_STATUS(&D_00413F80)->bit35 = TRUE;
        if (D_0038CC50.unk06 != 0) {
            PLAYER_STATUS(&D_00413F80)->bit35 = FALSE;
        } else if (D_00413F80.unkA4 > D_00413F80.unkA8) {
            PLAYER_STATUS(&D_00413F80)->bit35 = FALSE;
        } else if (item_def->unk63 == 2 && D_00413F80.unk258 != 0) {
            PLAYER_STATUS(&D_00413F80)->bit35 = FALSE;
        } else if ((u16)D_00413F80.timed_flags[4] != 0) {
            PLAYER_STATUS(&D_00413F80)->bit35 = FALSE;
        } else if (D_00413F80.unk108 != 0) {
            PLAYER_STATUS(&D_00413F80)->bit35 = FALSE;
        } else if (D_00413F80.unk200 != 0) {
            PLAYER_STATUS(&D_00413F80)->bit35 = FALSE;
        // match: unk3E is tested again with the rest of the set
        } else if (D_00413F80.equipment[0] == 0x19 &&
                   !(D_00413F80.unk3C == 0x2B && D_00413F80.equipment[0] == 0x19 && D_00413F80.equipment[1] == 0x35 &&
                     D_00413F80.equipment[2] == 0x4D && D_00413F80.equipment[3] == 0x66 && D_00413F80.equipment[4] == 0x80)) {
            PLAYER_STATUS(&D_00413F80)->bit35 = FALSE;
        }
    } else {
        PLAYER_STATUS(&D_00413F80)->bit35 = FALSE;
    }
    if ((D_00446942 & pressed) && !(D_00446940 & pressed)) {
        D_0038CC50.bit2 = TRUE;
    }

    /* what was added to unk2A0 comes off unkB8, and the screen tint builds up while it does */
    if (D_00413F80.unk2A0 != 0) {
        if (D_00413F80.unk2A0 < D_00413F80.unkB8) {
            D_00413F80.unkB8 -= D_00413F80.unk2A0;
        } else {
            D_00413F80.unkB8 = 0;
        }
        if (D_00413F80.unk29C < 8) {
            D_00413F80.unk29C++;
        }
        D_00413F80.unk2A0 = 0;
    } else if (D_00413F80.unk29C > 0) {
        D_00413F80.unk29C--;
    }

    if (D_0038CC50.unk00 != 0 && !(D_0038CC50.unk30 & 1)) {
        func_001BA310();
        func_001B96C8();
    }
    if (D_0038CC50.unk00 != 0 && !(D_0038CC50.unk30 & 1)) {
        func_001B7EE0(held, pressed);
        func_001B9078();
        func_001BD710(held, pressed);
        func_001B94C0();
        func_001B9588();
        func_001B9610();
    } else if (D_00413F80.map_index == 0xFFFF || D_00413F80.position.x != D_00414280.position.x ||
               D_00413F80.position.y != D_00414280.position.y || D_00413F80.position.z != D_00414280.position.z) {
        func_001BB908();
    }
    check_map_index();

    /* the stride: unk1C0 runs from 0 to STRIDE_LENGTH, faster when running and slower in water */
    if (D_0038CC50.unk00 != 0 && !(D_0038CC50.unk30 & 1)) {
        stride_speed = 1.0f;
        if ((s32)(D_00413F80.unk1C8 >> 35) & 1) {
            stride_speed = 1.6f;
        }
        if ((s32)(D_00413F80.unk1C8 >> 39) & 1) {
            stride_speed *= 0.5f;
        } else if ((s32)(D_00413F80.unk1C8 >> 38) & 1) {
            stride_speed *= 0.6666667f;
        }
        stride_step = stride_speed * 256.0f;
        if (D_00413F80.unk1B0 != 0.0f || D_00413F80.unk1B4 != 0.0f) {
            D_00413F80.unk1C0 += stride_step;
            if (D_00413F80.unk1C0 >= STRIDE_LENGTH) {
                D_00413F80.unk1C0 %= STRIDE_LENGTH;
                PLAYER_STATUS(&D_00413F80)->step_sounded = FALSE;
            }
        } else if (D_00413F80.unk1C0 != 0) {
            D_00413F80.unk1C0 += stride_step;
            if (D_00413F80.unk1C0 >= STRIDE_LENGTH) {
                D_00413F80.unk1C0 = 0;
                PLAYER_STATUS(&D_00413F80)->step_sounded = FALSE;
            }
        }
    }
    /* head bob, when the option is on */
    if (D_00349680[5] != 0) {
        D_00413F80.unk1C4 = sinf((f32)D_00413F80.unk1C0 * 6.2831855f / 5120.0f) * 2.75f;
    } else {
        D_00413F80.unk1C4 = 0.0f;
    }

    /* one footstep sound per stride, three quarters of the way through it, by what the player walks on */
    if (D_0038CC50.unk00 != 0 && !(D_0038CC50.unk30 & 1) && D_00413F80.unk1A8 == 0 &&
        PLAYER_STATUS(&D_00413F80)->step_sounded == FALSE &&
        !(ABS(D_00413F80.unk1B0) < D_00447124 * 0.5f && ABS(D_00413F80.unk1B4) < D_00447124 * 0.5f) &&
        !((s32)(D_00413F80.unk1C8 >> 34) & 1) && D_00413F80.unk1C0 >= 0xF00) {
        sound_free = D_0038A130.unk10 == 0 && D_0038A130.unk14 == 0;
        D_00446958 = -1;
        if ((s32)(D_00413F80.unk1C8 >> 39) & 1) {
            PLAYER_STATUS(&D_00413F80)->step_sounded = TRUE;
            D_00446958 = func_0019D020(0x25);
        } else if ((s32)(D_00413F80.unk1C8 >> 38) & 1) {
            PLAYER_STATUS(&D_00413F80)->step_sounded = TRUE;
            D_00446958 = func_0019D020(0x24);
        } else if (D_00413F80.unk1E0 != 0 && D_00413F80.unk1E4 == 4) {
            PLAYER_STATUS(&D_00413F80)->step_sounded = TRUE;
            if (sound_free) {
                D_00446958 = func_0019D020(0x27);
            }
        } else {
            PLAYER_STATUS(&D_00413F80)->step_sounded = TRUE;
            switch (D_00413F80.unk210) {
            case 1:
                func_0019D020(0x21);
                break;
            case 2:
                if (sound_free) {
                    D_00446958 = func_0019D020(0x22);
                }
                break;
            case 3:
                if (sound_free) {
                    D_00446958 = func_0019D020(0x23);
                }
                break;
            case 5:
                func_0019D020(0x24);
                break;
            case 0:
            case 6:
                func_0019D020(0x20);
                break;
            default:
                PLAYER_STATUS(&D_00413F80)->step_sounded = FALSE;
                break;
            }
        }
    }
    /* the warning sound while unkB4 is at a tenth of unk58 or less */
    if (D_00413F80.unk1A8 == 0 && D_00413F80.unkB4 > 0 && D_00413F80.unkB4 * 100 / D_00413F80.unk58 < 11 &&
        D_00446ED4 % 27 == 0) {
        func_0019D0F0(0x29);
    }

    func_001B6FA0(held, pressed);
    func_001B69D0();
    if (D_00413F80.unk2DC > 0) {
        D_00413F80.unk2DC--;
    }
    if (D_00413F80.unk2E0 > 0) {
        D_00413F80.unk2E0--;
    }
    if (D_00413F80.unk2E4 > 0) {
        D_00413F80.unk2E4--;
    } else {
        D_00413F80.unk2E8 = 0.0f;
    }
    if (D_00413F80.unk2EC > 0) {
        D_00413F80.unk2EC--;
    }

    /* item 0x32 worn: effect 0x7C starts at the player by chance, at most once in 30 frames, unless under water */
    if (D_00413F80.equipment[1] == 0x32 && D_0038CC50.unk00 != 0 && !(D_0038CC50.unk30 & 1)) {
        if (D_00446950 > 0) {
            D_00446950--;
        } else if (!(((s32)(D_00413F80.unk1C8 >> 38) & 1) &&
                     D_00413F80.unk1DC < D_00413F80.position.y - D_00402DA8[0x7C].unk108) &&
                   (s32)((f32)rand() * 451.0f * 0.0000000004656613f) == 0) {
            shot = func_001B1390();
            shot->unk00 = 1;
            shot->unk20 = 0;
            shot->unk22 = 0;
            func_001B4F30(0x7C, shot, &D_00413F80, &D_00413F80.rotation, &D_0038CD20[D_00446770]);
            D_00446950 = 30;
        }
    }
    if (D_0038CD20[D_00446770].tag == 0x23 && D_00349F56[0] == 0 && ((s32)(D_00413F80.unk1C8 >> 38) & 1) &&
        D_0038CC50.unk00 != 0 && !(D_0038CC50.unk30 & 1)) {
        func_001BB2E8(1);
    }
    func_001BB978();
    D_0038CC50.unk07 = 1;
}
