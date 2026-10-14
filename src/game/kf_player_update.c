#include "kf_player_update.h"
#include "common.h"
#include "kf_effect.h"
#include "kf_entity.h"
#include "kf_game.h"
#include "kf_held_weapon.h"
#include "kf_magic_ladder.h"
#include "kf_map.h"
#include "kf_model.h"
#include "kf_player.h"
#include "kf_shot.h"
#include "kf_sound.h"


static s32 swing_magic_frames;                    /* 0x447114 frames the swing-cast magic still runs */
static s32 swing_magic_id;                        /* 0x447118 magic id cast from the swing */
static PlayerShot *item_shot;                     /* 0x44711C shot for projectile and bow handlers */
static s32 bow_request_ring_index;                /* 0x447120 next slot in D_0057A350 for bow shots */
static f32 movement_top_speed;                    /* 0x447124 top walk speed this frame (for footsteps) */

#define SHOT_REQUEST ((Unk1B1470Request *)item_shot->unk04)

extern struct PlayerWeaponModel *D_00446978;

#define WEAPON_MODEL ((ModelInstance *)D_00446978)

extern PlayerState D_00413F80;
extern PlayerState D_00414280;      /* the player as of the frame before */
extern PlayerItemDef weapon_defs[];
extern GameStatus D_0038CC50;
extern MapSlot D_0038CD20[];
extern FxTableEntry D_00402DA8[];
extern SoundLoadQueue D_0038A130;
extern u16 D_00413DA8[];
extern u8 item_counts[];
extern s8 D_00349680[];
extern u8 D_00349F56[];
extern int D_00446770;
extern u32 D_00446ED4;
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
extern u16 D_00446942; // match: u16 here; kf_game.c declares these pad masks as s16
extern u16 D_00446944; // match: u16 here; kf_game.c declares this pad mask as s16

void *memset(void *, int, u32);
float sinf(float);
int rand(void);
int func_0019D020(int);
int func_0019D0F0(int);
int func_001B4F30(u16, void *, void *, void *, void *);
void Player_Move(u16 held, u16 pressed); // match: retail passes both pad words
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
void Player_Update(void) {
    Vector4 zero;                   // match: cleared and never read
    f32 unused[16];                 // match: retail's frame has room for this
    PlayerItemDef *item_def = &weapon_defs[D_00413F80.weapon];
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
        if ((D_00446944 & pressed) && D_00413F80.shortcut != 0 && D_00413F80.unk1A8 == 0 &&
            !(D_0038CC50.bit2 || D_0038CC50.bit3 || D_0038CC50.bit4)) {
            switch (D_00413F80.shortcut_type) {
            case 0:
                break;
            case 1:
                if (item_counts[D_00413F80.shortcut] != 0) {
                    D_0038CC50.unk20 = D_00413F80.shortcut;
                    D_0038CC50.bit6 = TRUE;
                }
                break;
            case 2:
                if (D_00413F80.unkC0 >= 5000 && D_00413F80.mp_now >= D_00402DA8[D_00413F80.shortcut].unkFC) {
                    Player_CastMagic(D_00413F80.shortcut);
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
        } else if (D_00413F80.weight > D_00413F80.unkA8) {
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
        } else if (D_00413F80.equipment[EQUIP_SHIELD] == ITEM_WORN_INDEX(ITEM_ANCIENT_SHIELD) &&
                   !PLAYER_WEARS_ANCIENT_SET(&D_00413F80)) {
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
        if (D_00413F80.unk2A0 < D_00413F80.mp_now) {
            D_00413F80.mp_now -= D_00413F80.unk2A0;
        } else {
            D_00413F80.mp_now = 0;
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
        Player_Move(held, pressed);
        func_001B9078();
        Ladder_DispatchPad(held, pressed);
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
        !(ABS(D_00413F80.unk1B0) < movement_top_speed * 0.5f && ABS(D_00413F80.unk1B4) < movement_top_speed * 0.5f) &&
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
    if (D_00413F80.unk1A8 == 0 && D_00413F80.hp_now > 0 && D_00413F80.hp_now * 100 / D_00413F80.hp_max < 11 &&
        D_00446ED4 % 27 == 0) {
        func_0019D0F0(0x29);
    }

    Player_UpdateActions(held, pressed);
    Player_UpdateScreenTints();
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
    if (D_00413F80.equipment[EQUIP_HELM] == ITEM_WORN_INDEX(ITEM_SACRIFICIAL_MASK) && D_0038CC50.unk00 != 0 && !(D_0038CC50.unk30 & 1)) {
        if (D_00446950 > 0) {
            D_00446950--;
        } else if (!(((s32)(D_00413F80.unk1C8 >> 38) & 1) &&
                     D_00413F80.unk1DC < D_00413F80.position.y - D_00402DA8[0x7C].unk108) &&
                   (s32)((f32)rand() * 451.0f * 0.0000000004656613f) == 0) {
            shot = Shot_Alloc();
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

extern PlayerScreenTint D_00414580;

void func_0010A4E8(int, int, int, int, int);

/* Draws the colours that lie over the screen (PlayerScreenTint). */
void Player_DrawScreenTints(void) {
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

extern PlayerState D_00413F80;
extern PlayerScreenTint D_00414580;
extern MapDrawState D_0038A1B0;
extern s32 D_00413EE8[][3];
extern const s8 D_0043F260[30];
extern u32 D_00446ED4;

/* Works out the colours laid over the screen (PlayerScreenTint) for the frame: which timed
   flag shows, how strong each tint is, and the map's glow. */
void Player_UpdateScreenTints(void) {
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
    if (D_00413F80.weapon == ITEM_WEAPON_INDEX(ITEM_ANCIENT_SWORD) &&
        !PLAYER_WEARS_ANCIENT_SET(&D_00413F80)) {
        darkened = TRUE;
    } else if (D_00413F80.equipment[EQUIP_HELM] == ITEM_WORN_INDEX(ITEM_SHADOW_MASK)) {
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

extern PlayerState D_00413F80;
extern PlayerItemDef weapon_defs[];
extern GameStatus D_0038CC50;
extern void (*D_00413F50[])(u16, u16);
extern u16 D_0044693E;

/* The player's actions for this frame: runs the handler of the item in hand, then the magic button. */
void Player_UpdateActions(u16 held, u16 pressed) {
    PlayerItemDef *def;

    D_00413F80.unk259 = 0;
    def = &weapon_defs[D_00413F80.weapon];
    D_00413F80.unk25B = 0;
    if (D_00413F80.unk258 != 0) {
        if (D_0038CC50.unk00 == 0 || (D_0038CC50.unk30 & 1) || D_00413F80.unk1A8 != 0 ||
            (u16)D_00413F80.timed_flags[1] != 0) {
            D_00413F80.unk263 = 1;
            Effect_FreeSpawnersByParam(0x5D);
        }
    }
    D_00413F50[def->unk63](held, pressed);
    if (D_00413F80.unk258 == 0 || (D_00413F80.unk25A == 0 && D_00413F80.unk25B == 0)) {
        if (D_0044693E & pressed) {
            Player_CastMagic(D_00413F80.equipped_magic);
        }
        if (D_0044693E & held) {
            Player_HoldMagic(D_00413F80.equipped_magic);
        }
    }
    if (D_00413F80.unk258 != 0) {
        D_00413F80.unk254 = 0;
    } else {
        D_00413F80.unk254++;
    }
}

extern PlayerState D_00413F80;
extern PlayerItemDef weapon_defs[];
extern LevelRecord D_003679A0[];
extern FxTableEntry D_00402DA8[];
extern u16 D_0044693C;
extern u16 D_0044693E;
extern u16 D_00446940;

void *memset(void *, int, u32);
void func_00108A08(ModelInstance *, int);
void func_00108AC8(void *);
void func_0019CE00(int mode);
s32 func_001B1470(Unk1B1470Request *, Unk1B1470Extra *);
void Vector_Copy(void *, void *);

/* Handler of a weapon that is swung: the button starts the swing; while its motion runs the
   swing hits what it reaches, plays its sound and casts the weapon's two magics when they were
   asked for in time. */
void Player_SwingWeapon(u16 held, u16 pressed) {
    Unk1B1470Request request;
    LevelRecord *record = &D_003679A0[D_00413F80.weapon];
    PlayerItemDef *def = &weapon_defs[D_00413F80.weapon];
    s32 level = record->unk14;
    PlayerShot *shot;

    if (D_00413F80.unk258 == 0) {
        if (def->unk42 != 0 && def->unk42 != 0xFFFF && D_00413F80.power >= def->unk8A && D_00413F80.magic >= def->unk8C) {
            if (D_00413F80.unk25C != 0) {
                D_00413F80.unk25C--;
            }
            if (D_00446940 & pressed) {
                D_00413F80.unk25C = 15;
            }
        } else {
            D_00413F80.unk25C = 0;
        }
        D_00413F80.unk25D = level >= 2 && def->unk52 != 0 && def->unk52 != 0xFFFF;
        if (D_0044693C & pressed) {
            shot = &D_00413F80.unk268;
            memset(shot, 0, sizeof(PlayerShot));
            shot->unk00 = 0;
            shot->unk20 = D_00413F80.power;
            shot->unk22 = D_00413F80.magic;
            shot->unk02 = D_00413F80.unkBC;
            shot->unk24 = D_00413F80.weapon;
            func_00108A08(WEAPON_MODEL, 0);
            D_00413F80.unk260_0 = 0;
            D_00413F80.unk260_1 = 0;
            D_00413F80.unk260_2 = 0;
            D_00413F80.unk258 = 1;
            D_00413F80.unkC4 = def->unk7E;
            swing_magic_frames = 0;
            D_00413F80.unkBC = 0;
            D_00413F80.unk25B = 0;
            D_00413F80.unk25A = 0;
            D_00413F80.unk261 = 0;
            D_00413F80.unk262 = 0;
            D_00413F80.unk25E = 0;
            D_00413F80.unk25F = 0;
            D_00413F80.unk263 = 0;
            WeaponTrail_PickKind();
        }
        return;
    }
    if (D_00413F80.unk261 == 0) {
        if (D_00413F80.unk263 != 0) {
            swing_magic_frames = 0;
        }
        if (swing_magic_frames <= 0) {
            func_00108AC8(WEAPON_MODEL);
        } else {
            swing_magic_frames--;
            if (((s32)(D_00413F80.unk1C8 >> 39) & 1) ||
                (((s32)(D_00413F80.unk1C8 >> 38) & 1) &&
                 D_00413F80.position.y - D_00402DA8[swing_magic_id].unk108 > D_00413F80.unk1DC)) {
                Effect_FreeSpawnersByParam(swing_magic_id);
                swing_magic_frames = 0;
            }
        }
        if (MODEL_CHANNEL(WEAPON_MODEL)->param_shifted <= 0) {
            D_00413F80.unk258 = 0;
            return;
        }
        if (D_00413F80.unk263 != 0) {
            return;
        }
        if (MODEL_CHANNEL(WEAPON_MODEL)->param >= def->unk78 && MODEL_CHANNEL(WEAPON_MODEL)->param <= def->unk7A && swing_magic_frames == 0) {
            memset(&request, 0, sizeof(request));
            Vector_Copy(request.position, &D_00413F80);
            request.owner = (Entity *)&D_00413F80;
            request.unk4C_0 = 3;
            request.position[1] -= 220.0f;
            request.unk30 = def->unk74 + 100.0f;
            request.unk34 = 230.0f;
            request.unk38 = D_00413F80.rotation.y;
            request.unk3C = def->unk70 * 0.5f * 0.017453292f;
            request.unk40 = D_00413F80.rotation.x;
            request.unk44 = def->unk6C * 0.5f * 0.017453292f;
            request.unk4D = 0x3E;
            *(PlayerShotStats *)&request.unk50 = D_00413F80.unk60;
            *(PlayerItemRates *)&request.unk62 = D_00413F80.unk72;
            request.unk6E = def->unk7F;
            request.unk6F = def->unk86;
            request.unk78 = D_00413F80.position.y - 230.0f - 50.0f;
            request.unk7C = D_00413F80.position.y + 50.0f;
            func_001B1470(&request, (Unk1B1470Extra *)&D_00413F80.unk268);
        }
        if (!D_00413F80.unk260_0 && def->unk80 != 0xFFFF && MODEL_CHANNEL(WEAPON_MODEL)->param >= def->unk82) {
            ((int (*)(int))func_0019CE00)(def->unk80); // match: called as if it returned a value
            D_00413F80.unk260_0 = 1;
        }
        if (D_0044693E & pressed) {
            if (D_00413F80.unk25C != 0 && D_00413F80.unk25F == 0 && def->unk42 != 0xFFFF &&
                MODEL_CHANNEL(WEAPON_MODEL)->param >= def->unk4A && MODEL_CHANNEL(WEAPON_MODEL)->param <= def->unk4C) {
                D_00413F80.unk25D = 0;
                D_00413F80.unk25B = 1;
                D_00413F80.unk25F = 1;
            }
            if (D_00413F80.unk25D != 0 && D_00413F80.unk25A == 0 && def->unk52 != 0xFFFF &&
                MODEL_CHANNEL(WEAPON_MODEL)->param >= def->unk5A && MODEL_CHANNEL(WEAPON_MODEL)->param <= def->unk5C) {
                D_00413F80.unk25A = 1;
                D_00413F80.unk25E = 1;
            }
            D_00413F80.unk25C = 0;
            D_00413F80.unk25D = 0;
        }
        if (D_00413F80.unk25F == 1 && MODEL_CHANNEL(WEAPON_MODEL)->param >= def->unk44) {
            D_00413F80.unk25F = 2;
            if (Player_CastMagic(def->unk42)) {
                swing_magic_id = def->unk42;
                swing_magic_frames = def->unk4E;
            }
        }
        if (D_00413F80.unk25E == 1 && MODEL_CHANNEL(WEAPON_MODEL)->param >= def->unk54) {
            D_00413F80.unk25E = 2;
            if (Player_CastMagic(def->unk52)) {
                swing_magic_id = def->unk52;
                swing_magic_frames = def->unk5E;
            }
        }
        if (D_00413F80.unk25F != 0 && !D_00413F80.unk260_1 && def->unk46 != 0xFFFF &&
            MODEL_CHANNEL(WEAPON_MODEL)->param >= def->unk48) {
            D_00413F80.unk260_1 = 1;
        }
        if (D_00413F80.unk25E != 0 && !D_00413F80.unk260_2 && def->unk56 != 0xFFFF &&
            MODEL_CHANNEL(WEAPON_MODEL)->param >= def->unk58) {
            D_00413F80.unk260_1 = 1; // match: bit 1 again, though bit 2 was tested
        }
    } else {
        func_00108AC8(WEAPON_MODEL);
        if (MODEL_CHANNEL(WEAPON_MODEL)->value == 0) {
            D_00413F80.unk258 = 0;
        }
    }
}

typedef void (*StateHandler)(u16, u16);

extern u8 D_004141D8[];
extern StateHandler D_00413F60[];

void Player_InvokeTargetHandler(u16 target_id, u16 command) {
    Player_UpdateWeaponAim();
    D_00413F60[D_004141D8[0]](target_id, command);
}

extern PlayerState D_00413F80;
extern PlayerItemDef weapon_defs[];
extern u8 item_counts[];
extern int D_00413F30[];
extern float D_00413F40[];
extern u16 D_0044693C;

void func_00108A08(ModelInstance *, int);
void func_00108AC8(void *);
int func_001B4F30(u16, void *, void *, void *, void *);

/* Handler of an item that shoots: on the button, starts the motion and launches the item's shot. */
void Player_FireProjectile(u16 held, u16 pressed) {
    PlayerItemDef *def = &weapon_defs[D_00413F80.weapon];

    if ((D_0044693C & pressed) && !((s32)(D_00413F80.unk1C8 >> 39) & 1) && item_counts[def->unk62] != 0) {
        func_00108A08(WEAPON_MODEL, 0);
        MODEL_CHANNEL(WEAPON_MODEL)->param = 0;
        MODEL_CHANNEL(WEAPON_MODEL)->param_shifted = 0;
        func_00108AC8(WEAPON_MODEL);
        ((int (*)(ModelInstance *))WEAPON_MODEL->unk1B4)(WEAPON_MODEL); // match: called as if it returned a value
        D_00413F80.unk260_0 = 0;
        D_00413F80.unk298 = 0;
        D_00413F80.unk258++;
        D_00413F80.unk263 = 0;
        item_shot = Shot_Alloc();
        item_shot->unk00 = 0;
        item_shot->unk26 = def->unk52;
        item_shot->unk20 = D_00413F80.power;
        item_shot->unk22 = D_00413F80.magic;
        item_shot->unk02 = D_00413F80.unkBC;
        func_001B4F30(def->unk52, item_shot, D_00413F30, D_00413F40, &D_00413F80);
    }
}

extern PlayerState D_00413F80;
extern PlayerItemDef weapon_defs[];
extern GameDefinition D_00416D78[];
extern u8 item_counts[];
extern Unk1B1470Request D_0057A350[32];
extern u16 D_0044693C;

void *memset(void *, s32, u32);
void func_00108AC8(void *);
int func_00109098(ModelInstance *model, u8 index, u16 speed);
void func_0019CDA0(s8 slot);
void func_0019CE00(int mode);


/* Handler of a drawn weapon while the button is held: the draw motion runs to its last frame and the
   strength builds up; letting go looses the shot, anything else lowers the weapon. */
void Player_DrawBow(u16 held, u16 pressed) {
    GameDefinition *item = &D_00416D78[D_00413F80.weapon];
    PlayerItemDef *def = &weapon_defs[D_00413F80.weapon];
    s32 rate;
    s32 frame;
    s32 speed;

    if (MODEL_CHANNEL(WEAPON_MODEL)->param != 0 && MODEL_CHANNEL(WEAPON_MODEL)->param < def->unk7A) {
        func_00108AC8(WEAPON_MODEL);
    }
    if (D_00413F80.unk298 == 0) {
        D_00413F80.unk298 = MODEL_CHANNEL(WEAPON_MODEL)->param >= def->unk78;
    }
    if (D_00413F80.unk263 == 0 && !((s32)(D_00413F80.unk1C8 >> 39) & 1) && Player_CanFireAtAim()) {
        if (!D_00413F80.unk260_0 && def->unk80 != 0xFFFF && MODEL_CHANNEL(WEAPON_MODEL)->param >= def->unk82) {
            ((int (*)(int))func_0019CE00)(0); // match: called as if it returned a value
            D_00413F80.unk260_0 = 1;
        }
        if (D_0044693C & held) {
            rate = D_00413F80.power;
            if (rate < 20) {
                rate = 20;
            }
            D_00413F80.unkBC += rate * 42 / (item->unk33 + 1);
            if (D_00413F80.unkBC > 5000) {
                D_00413F80.unkBC = 5000;
            }
            return;
        }
        if (D_00413F80.unk298 != 0) {
            if (D_00413F80.unk260_0) {
                func_0019CDA0(2);
            }
            item_shot->unk02 = D_00413F80.unkBC;
            item_shot->unk24 = D_00413F80.weapon;
            item_shot->unk04 = &D_0057A350[bow_request_ring_index];
            bow_request_ring_index = (bow_request_ring_index + 1) % 32;
            memset(item_shot->unk04, 0, sizeof(Unk1B1470Request));
            *(PlayerShotStats *)&SHOT_REQUEST->unk50 = D_00413F80.unk60;
            *(PlayerItemRates *)&SHOT_REQUEST->unk62 = D_00413F80.unk72;
            SHOT_REQUEST->unk6E = def->unk7F;
            SHOT_REQUEST->unk6F = def->unk86;
            if (MODEL_CHANNEL(WEAPON_MODEL)->param != 0) {
                frame = MODEL_CHANNEL(WEAPON_MODEL)->param;
            } else {
                frame = def->unk7A;
            }
            speed = (frame - def->unk78) * 3 / (def->unk7A - def->unk78);
            if (speed <= 0) {
                speed = 1;
            }
            func_00109098(WEAPON_MODEL, 1, speed);
            MODEL_CHANNEL(WEAPON_MODEL)->unk8F = 0;
            D_00413F80.unk258++;
            item_counts[def->unk62]--;
            return;
        }
    }
    func_00109098(WEAPON_MODEL, 0, 0xF);
    MODEL_CHANNEL(WEAPON_MODEL)->unk8F = 0;
    D_00413F80.unk298 = 0;
    D_00413F80.unkBC = 0;
    D_00413F80.unk258 += 3;
}

extern PlayerState D_00413F80;

void func_0019CE00(int mode);

void Player_PlayEquipSound(void) {
    func_0019CE00(1);
    D_00413F80.unkBC = 0;
    D_00413F80.unk258++;
}

extern PlayerState D_00413F80;

void func_00108AC8(void *);

void Player_FinishWeaponMotion(void) {
    if (MODEL_CHANNEL(WEAPON_MODEL)->value != 0) {
        func_00108AC8(WEAPON_MODEL);
        if (MODEL_CHANNEL(WEAPON_MODEL)->value == 0) {
            func_00108AC8(WEAPON_MODEL);
        }
    } else if (MODEL_CHANNEL(WEAPON_MODEL)->param_shifted > 0) {
        func_00108AC8(WEAPON_MODEL);
    } else {
        D_00413F80.unk258 = 0;
        D_00413F80.unk298 = 0;
    }
}

extern u8 D_004141D8[];

void func_00108AC8(void *);

void Player_HolsterWeapon(void) {
    func_00108AC8(WEAPON_MODEL);
    if (MODEL_CHANNEL(WEAPON_MODEL)->value == 0) {
        Effect_FreePlayerWeaponKinds();
        D_004141D8[0] = 0;
    }
}

extern PlayerState D_00413F80;
extern float D_00420D10[];

void Vector_Scale(float *, float *, float);
void Vector_Add(void *dst, void *a, void *b);

int Player_CanFireAtAim(void) {
    float vec[4];
    int result;

    Vector_Scale(vec, D_00420D10, 100.0f);
    Vector_Add(vec, vec, &D_00413F80);
    vec[1] -= 186.0f;
    result = 0;
    if (vec[1] <= D_00413F80.unk1DC) {
        result = 1;
    }
    return result;
}

float atan2f(float, float);
float sqrtf(float);
void Vector_Copy(void *, void *);
void Vector_Subtract(void *, void *, void *);
extern float D_00413F40[];
extern int D_00413F30[];

void Player_UpdateWeaponAim(void) {
    float vec[4];
    void *weapon_base;
    void *weapon_tip;

    D_00446978->update_aim(D_00446978);
    weapon_base = D_00446978->get_point(D_00446978, 0);
    weapon_tip = D_00446978->get_point(D_00446978, 1);
    Vector_Subtract(vec, weapon_tip, weapon_base);
    Vector_Copy(D_00413F30, weapon_tip);
    D_00413F40[0] = atan2f(-vec[1], sqrtf(vec[0] * vec[0] + vec[2] * vec[2]));
    D_00413F40[1] = atan2f(vec[0], vec[2]);
}

extern PlayerState D_00413F80;
extern PlayerState D_00414280;      /* the player as of the frame before */
extern GameStatus D_0038CC50;
extern MapSlot D_0038CD20[];
extern int D_00446770;
extern f32 D_00431AF0[16];

/* the movement tables, one row per way of moving (see movement_row) */
extern f32 D_00413DE0[6];           /* top speed */
extern f32 D_00413DF8[6];           /* speed gained per frame */
extern f32 D_00413E10[6];           /* speed lost per frame */
extern f32 D_00413E28[6];           /* top turning speed */
extern f32 D_00413E40[6];           /* turning speed gained per frame */
extern f32 D_00413E70[6];           /* top speed of looking up and down */
extern f32 D_00413E88[6];           /* looking speed gained per frame */
extern f32 D_00413EA0[6];           /* turning and looking speed lost per frame */
extern f32 D_00413EB8[6];           /* how fast the view levels out */
extern f32 D_00413ED0[6];           /* falling speed gained per frame */

/* the pad bits of each control */
extern u16 D_0044692C;              /* forward */
extern u16 D_0044692E;              /* back */
extern u16 D_00446930;              /* turn right */
extern u16 D_00446932;              /* turn left */
extern u16 D_00446934;              /* step right */
extern u16 D_00446936;              /* step left */
extern u16 D_00446938;              /* look down */
extern u16 D_0044693A;              /* look up */

void *memset(void *, int, u32);
float sqrtf(float);
float sinf(float);
float cosf(float);
float atan2f(float, float);
double sin(double);
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0RotMatrixZ(float *, float *, float);
void Vector_Subtract(void *, void *, void *);
void Vector_Scale(float *, float *, float);
float func_0019B838(float);
int func_0019B908(float *, float, float);
int func_0019D0F0(int);
int func_0019D158(int);
void func_001B92C0(float);
void func_001B9610(void);
int func_001BDDC0(void);
void func_001BE760(void);
void func_001BEA50(void);
void func_001BECA8(int);
void func_001BF350(int);
void Vector_Copy(void *, void *);
void Vector_ApplyMatrix(void *, void *, void *);
void Vector_Add(void *, void *, void *);
void Vector_Normalize(void *, void *);

#define PLAYER_FLAG_32 0x100000000L /* the view is levelling out */
#define PLAYER_FLAG_37 0x2000000000L /* the view dips after a landing */

#define STEP_LENGTH 25.0f

/* Moves the player for one frame: the pad bits in `buttons` turn, look and walk; then gravity,
   collision in steps of 25 units, landing, and the push of the floor. */
void Player_Move(u16 buttons, u16 unused_pressed) {
    Vector4 delta;
    Vector4 turn;
    Vector4 step;
    f32 matrix[16];
    f32 unused[16];
    MapObject *platform;
    MapRecord *record;
    Vector4 *velocity;
    s32 row;
    s32 steps;
    f32 top_speed;
    f32 speed_gain;
    f32 speed_loss;
    f32 top_turn;
    f32 turn_gain;
    f32 top_look;
    f32 look_gain;
    f32 turn_loss;
    f32 level_step;
    f32 gravity;
    f32 ratio;
    f32 angle;
    f32 distance;
    f32 speed;
    f32 backward;

    memset(&delta, 0, sizeof(delta));
    memset(&turn, 0, sizeof(turn));
    memset(&step, 0, sizeof(step));

    platform = (MapObject *)D_00414280.unk208;
    if (platform != NULL && (platform->unk10 & 0x10000) && (MAP_OBJECT_DEF_FLAGS(platform->unk04) & MAP_OBJECT_DEF_CARRIES)) {
        Vector_Subtract(&turn, &platform->model->rotation, &platform->unk30);
        turn.x = func_0019B838(turn.x);
        turn.y = func_0019B838(turn.y);
        turn.z = func_0019B838(turn.z);
        sceVu0RotMatrixZ(matrix, D_00431AF0, turn.z);
        sceVu0RotMatrixX(matrix, matrix, turn.x);
        sceVu0RotMatrixY(matrix, matrix, turn.y);
        Vector_Subtract(&delta, &D_00413F80, &platform->unk20);
        Vector_ApplyMatrix(&delta, matrix, &delta);
        Vector_Add(&D_00413F80, &platform->model->position, &delta);
        D_00413F80.rotation.y += platform->model->rotation.y - platform->unk34;
    }
    Vector_Add(&D_00413F80, &D_00413F80, &D_00413F80.unk240);

    if (D_00414280.unk1E0 != 0 && D_00414280.unk1E4 == 4) {
        row = 4;
    } else if ((s32)(D_00414280.unk1C8 >> 39) & 1) {
        row = 3;
    } else if ((s32)(D_00414280.unk1C8 >> 38) & 1) {
        row = 2;
    } else {
        row = D_00414280.unk210 == 3;
    }
    top_speed = D_00413DE0[row];
    speed_gain = D_00413DF8[row];
    speed_loss = D_00413E10[row];
    top_turn = D_00413E28[row];
    turn_gain = D_00413E40[row];
    top_look = D_00413E70[row];
    look_gain = D_00413E88[row];
    turn_loss = D_00413EA0[row];
    level_step = D_00413EB8[row];
    gravity = D_00413ED0[row];

    if (D_00413F80.weight > D_00413F80.unkA8) {
        ratio = (f32)(D_00413F80.weight - D_00413F80.unkA8) / (f32)D_00413F80.unkA8;
        if (ratio < 1.0f) {
            top_speed -= top_speed * ratio;
            top_turn -= top_turn * ratio;
            top_look -= top_look * ratio;
        } else {
            top_look = top_turn = top_speed = 0.0f;
        }
    }
    if ((u16)D_00413F80.timed_flags[4] != 0 || D_00413F80.unk108 != 0 || D_00413F80.unk200 > 0) {
        top_speed *= 0.5f;
        top_turn *= 0.5f;
        top_look *= 0.5f;
        level_step *= 0.5f;
    }
    if ((s32)(D_00413F80.unk1C8 >> 35) & 1) {
        top_speed *= 1.6f;
    }
    if ((s32)(D_00413F80.unk1C8 >> 36) & 1) {
        buttons &= D_00446938 | D_0044693A;
    }

    if (D_00413F80.aim_speed > 0.0f) {
        Vector_Scale((f32 *)&delta, (f32 *)&D_00413F80.aim, D_00413F80.aim_speed);
        Vector_Add(&D_00413F80, &D_00413F80, &delta);
        D_00413F80.aim_speed -= ((f32)D_00413F80.weight + 600.0f) / 600.0f * 20.0f;
        if (D_00413F80.aim_speed < 0.0) {
            D_00413F80.aim_speed = 0.0f;
        }
    }

    if (D_00413F80.unk234 != 0) {
        D_00413F80.unk234--;
        D_00413F80.unk238 += 0.034906585f;
        D_00413F80.unk23C += 0.0017453292f;
        if (D_00413F80.unk238 > 1.012291f) {
            D_00413F80.unk234 = 0;
        }
    } else {
        if (D_00413F80.unk238 > 0.0f) {
            D_00413F80.unk238 -= 0.008726646f;
            if (D_00413F80.unk238 < 0.0f) {
                D_00413F80.unk238 = 0.0f;
            }
        }
        if (D_00413F80.unk23C > 0.0f) {
            D_00413F80.unk23C -= 0.0008726646f;
            if (D_00413F80.unk23C < 0.0f) {
                D_00413F80.unk23C = 0.0f;
            }
        }
    }

    /* looking up and down */
    if (!((s32)(D_00413F80.unk1C8 >> 32) & 1)) {
        if (D_00446938 & buttons) {
            if (D_0044693A & buttons) {
                D_00413F80.unk1B8 = 0.0f;
                D_00413F80.unk1C8 |= PLAYER_FLAG_32;
            } else {
                D_00413F80.unk1B8 += look_gain;
                if (D_00413F80.unk1B8 > top_look) {
                    D_00413F80.unk1B8 = top_look;
                }
            }
        } else if (D_0044693A & buttons) {
            D_00413F80.unk1B8 -= look_gain;
            if (D_00413F80.unk1B8 < -top_look) {
                D_00413F80.unk1B8 = -top_look;
            }
        } else {
            if (D_00413F80.unk1B8 > 0.0f) {
                D_00413F80.unk1B8 -= turn_loss;
                if (D_00413F80.unk1B8 < 0.0f) {
                    D_00413F80.unk1B8 = 0.0f;
                }
            }
            if (D_00413F80.unk1B8 < 0.0f) {
                D_00413F80.unk1B8 += turn_loss;
                if (D_00413F80.unk1B8 > 0.0f) {
                    D_00413F80.unk1B8 = 0.0f;
                }
            }
        }
    } else if ((D_00446938 & buttons) && (D_0044693A & buttons)) {
        func_0019B908(&D_00413F80.rotation.x, 0.0f, level_step);
    } else {
        D_00413F80.unk1C8 &= ~PLAYER_FLAG_32;
    }

    /* turning */
    if ((D_00446932 & buttons) && !(D_00446930 & buttons)) {
        D_00413F80.unk1BC -= turn_gain;
        if (D_00413F80.unk1BC < -top_turn) {
            D_00413F80.unk1BC = -top_turn;
        }
    } else if (!(D_00446932 & buttons) && (D_00446930 & buttons)) {
        D_00413F80.unk1BC += turn_gain;
        if (D_00413F80.unk1BC > top_turn) {
            D_00413F80.unk1BC = top_turn;
        }
    } else {
        if (D_00413F80.unk1BC > 0.0f) {
            D_00413F80.unk1BC -= turn_loss;
            if (D_00413F80.unk1B8 < 0.0f) { // match: retail tests the looking speed here, not the turning speed
                D_00413F80.unk1BC = 0.0f;
            }
        }
        if (D_00413F80.unk1BC < 0.0f) {
            D_00413F80.unk1BC += turn_loss;
            if (D_00413F80.unk1BC > 0.0f) {
                D_00413F80.unk1BC = 0.0f;
            }
        }
    }

    /* walking */
    movement_top_speed = top_speed;
    if ((D_0044692C & buttons) && !(D_0044692E & buttons)) {
        if (D_00413F80.unk1B4 < top_speed) {
            D_00413F80.unk1B4 += speed_gain;
            if (D_00413F80.unk1B4 > top_speed) {
                D_00413F80.unk1B4 = top_speed;
            }
        } else if (D_00413F80.unk1B4 > top_speed) {
            D_00413F80.unk1B4 -= speed_loss;
            if (D_00413F80.unk1B4 < top_speed) {
                D_00413F80.unk1B4 = top_speed;
            }
        }
    } else if (!(D_0044692C & buttons) && (D_0044692E & buttons)) {
        backward = -top_speed;
        if (D_00413F80.unk1B4 > backward) {
            D_00413F80.unk1B4 -= speed_gain;
            if (D_00413F80.unk1B4 < backward) {
                D_00413F80.unk1B4 = backward;
            }
        } else if (D_00413F80.unk1B4 < backward) {
            D_00413F80.unk1B4 += speed_loss;
            if (D_00413F80.unk1B4 > backward) {
                D_00413F80.unk1B4 = backward;
            }
        }
    } else {
        if (D_00413F80.unk1B4 > 0.0f) {
            D_00413F80.unk1B4 -= speed_loss;
            if (D_00413F80.unk1B4 < 0.0f) {
                D_00413F80.unk1B4 = 0.0f;
            }
        }
        if (D_00413F80.unk1B4 < 0.0f) {
            D_00413F80.unk1B4 += speed_loss;
            if (D_00413F80.unk1B4 > 0.0f) {
                D_00413F80.unk1B4 = 0.0f;
            }
        }
    }

    /* stepping sideways */
    if (!(D_00446936 & buttons) && (D_00446934 & buttons)) {
        if (D_00413F80.unk1B0 < top_speed) {
            D_00413F80.unk1B0 += speed_gain;
            if (D_00413F80.unk1B0 > top_speed) {
                D_00413F80.unk1B0 = top_speed;
            }
        } else if (D_00413F80.unk1B0 > top_speed) {
            D_00413F80.unk1B0 -= speed_loss;
            if (D_00413F80.unk1B0 < top_speed) {
                D_00413F80.unk1B0 = top_speed;
            }
        }
    } else if ((D_00446936 & buttons) && !(D_00446934 & buttons)) {
        backward = -top_speed;
        if (D_00413F80.unk1B0 > backward) {
            D_00413F80.unk1B0 -= speed_gain;
            if (D_00413F80.unk1B0 < backward) {
                D_00413F80.unk1B0 = backward;
            }
        } else if (D_00413F80.unk1B0 < backward) {
            D_00413F80.unk1B0 += speed_loss;
            if (D_00413F80.unk1B0 > backward) {
                D_00413F80.unk1B0 = backward;
            }
        }
    } else {
        if (D_00413F80.unk1B0 > 0.0f) {
            D_00413F80.unk1B0 -= speed_loss;
            if (D_00413F80.unk1B0 < 0.0f) {
                D_00413F80.unk1B0 = 0.0f;
            }
        }
        if (D_00413F80.unk1B0 < 0.0f) {
            D_00413F80.unk1B0 += speed_loss;
            if (D_00413F80.unk1B0 > 0.0f) {
                D_00413F80.unk1B0 = 0.0f;
            }
        }
    }

    /* falling */
    if (D_0038CC50.unk00 != 0 && !(D_0038CC50.unk30 & 1) && !((s32)(D_00413F80.unk1C8 >> 36) & 1)) {
        if ((s32)(D_00413F80.unk1C8 >> 33) & 1) {
            D_00413F80.unk1AC = 0.0f;
        } else if (D_00413F80.unk1A8 == 0) {
            D_00413F80.unk1AC += gravity;
        }
    } else {
        D_00413F80.unk1AC = 0.0f;
    }

    delta.y = D_00413F80.unk1BC;
    delta.x = D_00413F80.unk1B8;
    if ((u16)D_00413F80.timed_flags[4] != 0) {
        delta.x *= 0.5f;
        delta.y *= 0.5f;
    }
    D_00413F80.rotation.y = func_0019B838(D_00413F80.rotation.y + delta.y);
    D_00413F80.rotation.x += delta.x;
    if (D_00413F80.rotation.x > 1.012291f) {
        D_00413F80.rotation.x = 1.012291f;
    }
    if (D_00413F80.rotation.x < -1.012291f) {
        D_00413F80.rotation.x = -1.012291f;
    }

    delta.z = D_00413F80.unk1B4;
    delta.x = D_00413F80.unk1B0;
    delta.y = delta.w = 0.0f;
    angle = atan2f(ABS(D_00413F80.unk1B4), ABS(D_00413F80.unk1B0));
    delta.x *= cosf(angle);
    delta.z *= sinf(angle);
    sceVu0RotMatrixY(matrix, D_00431AF0, D_00413F80.rotation.y);
    Vector_ApplyMatrix(&delta, matrix, &delta);
    D_00413F80.position.x += delta.x;
    D_00413F80.position.z += delta.z;

    if (D_0038CC50.unk00 != 0 && !(D_0038CC50.unk30 & 1)) {
        if (D_00414280.unk208 == 0) {
            Vector_Subtract(&delta, &D_00413F80, &D_00414280);
            distance = sqrtf(delta.x * delta.x + delta.z * delta.z);
            if (distance > STEP_LENGTH) {
                steps = (s32)(distance / STEP_LENGTH);
                delta.y = 0.0f;
                Vector_Normalize(&delta, &delta);
                Vector_Scale((f32 *)&step, (f32 *)&delta, STEP_LENGTH);
                Vector_Scale((f32 *)&delta, (f32 *)&delta, distance - (f32)steps * STEP_LENGTH);
                Vector_Copy(&D_00413F80, &D_00414280);
                do {
                    Vector_Add(&D_00413F80, &D_00413F80, &step);
                    func_001BECA8(0);
                    func_001BF350(0);
                    func_001BE760();
                } while (--steps != 0);
                Vector_Add(&D_00413F80, &D_00413F80, &delta);
            }
        }
        func_001BECA8(1);
        if (!((s32)(D_00413F80.unk1C8 >> 36) & 1)) {
            func_001BDDC0();
            func_001BF350(1);
            if (D_00413F80.unk210 == 6 && D_00413F80.unk208 != 0 && ((MapObject *)D_00413F80.unk208)->unk04->kind == 12) {
                func_001BEA50();
            }
            if (!((s32)(D_00413F80.unk1C8 >> 34) & 1)) {
                if (((s32)(D_00414280.unk1C8 >> 34) & 1) && D_00413F80.unk1A8 == 0) {
                    if (D_00413F80.position.y - D_00413F80.unk1D8 > 110.0f) {
                        if (!((s32)(D_00413F80.unk1C8 >> 38) & 1)) {
                            func_0019D0F0(0x22);
                        } else if (!((s32)(D_00414280.unk1C8 >> 38) & 1)) {
                            func_0019D158(0x38);
                        }
                    }
                    if (D_00414280.unk1AC >= 66.666664f) {
                        func_001B92C0(D_00414280.unk1AC);
                        if (!((s32)(D_00413F80.unk1C8 >> 38) & 1) && D_00413F80.unk1E4 != 4) {
                            D_00413F80.unk1C8 |= PLAYER_FLAG_37;
                            D_00413F80.unk1F4 = D_00414280.unk1AC * 0.5f;
                            if (D_00413F80.unk1F4 > 150.0f) {
                                D_00413F80.unk1F4 = 150.0f;
                            }
                            D_00413F80.unk1F0 = 0.0f;
                            D_00413F80.unk1FC = 0;
                            D_00413F80.unk1F8 = (s32)(D_00413F80.unk1F4 * 40.0f / 150.0f);
                        }
                    }
                }
            }
            if (((s32)(D_00413F80.unk1C8 >> 34) & 1) && !((s32)(D_00414280.unk1C8 >> 34) & 1)) {
                D_00413F80.unk1D8 = D_00414280.position.y;
            }
        }
        if (D_00413F80.map_index == 0xFFFF) {
            D_00413F80.map_index = 0;
            D_00413F80.unk250 = 0;
        }
    }

    if ((s32)(D_00413F80.unk1C8 >> 37) & 1) {
        D_00413F80.unk1F0 = (f32)sin((f32)D_00413F80.unk1FC * 3.1415927f / (f32)D_00413F80.unk1F8) * D_00413F80.unk1F4;
        if (D_00413F80.unk1FC++ >= D_00413F80.unk1F8) {
            D_00413F80.unk1C8 &= ~PLAYER_FLAG_37;
            D_00413F80.unk1F0 = 0.0f;
            D_00413F80.unk1F4 = 0.0f;
            D_00413F80.unk1FC = 0;
            D_00413F80.unk1F8 = 0;
        }
    }

    /* the push of the floor */
    if (D_0038CC50.unk00 != 0 && !(D_0038CC50.unk30 & 1)) {
        if (D_00413F80.map_index != 0xFFFF && D_00413F80.map_index < D_0038CD20[D_00446770].header->record_count) {
            record = &((MapRecord *)D_0038CD20[D_00446770].records)[D_00413F80.map_index];
        } else {
            record = NULL;
        }
        speed = sqrtf(D_00413F80.unk240.x * D_00413F80.unk240.x + D_00413F80.unk240.y * D_00413F80.unk240.y
                      + D_00413F80.unk240.z * D_00413F80.unk240.z);
        if (record != NULL && record->unk2E != 0 && record->unk2C != 0) {
            velocity = &D_00413F80.unk240;
            if (speed < (f32)record->unk2C) {
                delta.x = sinf(record->unk1C) * (f32)record->unk2E;
                delta.y = 0.0f;
                delta.z = cosf(record->unk1C) * (f32)record->unk2E;
                delta.w = 0.0f;
                Vector_Add(velocity, velocity, &delta);
                speed = sqrtf(D_00413F80.unk240.x * D_00413F80.unk240.x + D_00413F80.unk240.y * D_00413F80.unk240.y
                              + D_00413F80.unk240.z * D_00413F80.unk240.z);
                if (speed > (f32)record->unk2C) {
                    Vector_Normalize(velocity, velocity);
                    Vector_Scale((f32 *)velocity, (f32 *)velocity, (f32)record->unk2C);
                }
            } else {
                Vector_Normalize(&delta, velocity);
                Vector_Scale((f32 *)&delta, (f32 *)&delta, -speed_loss);
                Vector_Add(velocity, velocity, &delta);
                speed = sqrtf(D_00413F80.unk240.x * D_00413F80.unk240.x + D_00413F80.unk240.y * D_00413F80.unk240.y
                              + D_00413F80.unk240.z * D_00413F80.unk240.z);
                if (speed > (f32)record->unk2C) {
                    Vector_Normalize(velocity, velocity);
                    Vector_Scale((f32 *)velocity, (f32 *)velocity, (f32)record->unk2C);
                }
            }
        } else if (speed > speed_loss) {
            Vector_Normalize(&delta, &D_00413F80.unk240);
            Vector_Scale((f32 *)&delta, (f32 *)&delta, -speed_loss);
            Vector_Add(&D_00413F80.unk240, &D_00413F80.unk240, &delta);
        } else {
            D_00413F80.unk240.x = D_00413F80.unk240.y = D_00413F80.unk240.z = 0.0f;
        }
    }

    if (D_00413F80.unk1A8 == 0 && D_00413F80.unk1AC > 1000.0f) {
        D_00413F80.hp_now = 0;
        D_00413F80.unk1A4 = 0;
        func_001B9610();
    }
}