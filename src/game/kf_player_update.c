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
#include "kf_vu0.h"

static s32 swing_magic_frames;                    /* 0x447114 frames the swing-cast magic still runs */
static s32 swing_magic_id;                        /* 0x447118 magic id cast from the swing */
static PlayerShot *item_shot;                     /* 0x44711C shot for projectile and bow handlers */
static s32 bow_request_ring_index;                /* 0x447120 next slot in D_0057A350 for bow shots */
static f32 movement_top_speed;                    /* 0x447124 top walk speed this frame (for footsteps) */

#define SHOT_REQUEST ((Unk1B1470Request *)item_shot->unk04)

extern struct PlayerWeaponModel *D_00446978;

#define WEAPON_MODEL ((ModelInstance *)D_00446978)

extern PlayerState player_state;
extern PlayerState player_previous;      /* the player as of the frame before */
extern PlayerScreenTint player_screen_tint;
extern PlayerItemDef weapon_defs[];
extern GameStatus game_status;
extern MapSlot map_slots[];
extern FxTableEntry fx_table[];
extern SoundLoadQueue sound_load_queue;
extern u16 pad_state[];
extern u8 item_counts[];
extern s8 game_options[];
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

/* The player's movement tables, tint colours, weapon tip and held item handlers (this file's scratch). */
/* 0x413DE0 the top speed of each row of the movement tables (see movement_row) */
f32 player_top_speed[6] = { 12.0f, 12.0f, 8.0f, 6.0f, 6.0f, 0.0f };
f32 player_speed_gain[6] = { 1.2f, 0.8f, 1.2f, 1.2f, 1.2f, 0.0f };                                            /* 0x413DF8 speed gained per frame */
f32 player_speed_loss[6] = { 1.2f, 0.6f, 1.2f, 1.2f, 1.2f, 0.0f };                                            /* 0x413E10 speed lost per frame */
f32 player_top_turn_speed[6] = { 0.029088821f, 0.029088821f, 0.019392548f, 0.014544411f, 0.014544411f, 0.0f };    /* 0x413E28 top turning speed */
f32 player_turn_gain[12] = { 0.0029088822f, 0.0029088822f, 0.0019392548f, 0.0014544411f, 0.0014544411f, 0.0f,
                       0.0029088822f, 0.0029088822f, 0.0019392548f, 0.0014544411f, 0.0014544411f, 0.0f }; /* 0x413E40 turning speed gained per frame, twice */
f32 player_top_look_speed[6] = { 0.02617994f, 0.02617994f, 0.02617994f, 0.01308997f, 0.02617994f, 0.0f };          /* 0x413E70 top speed of looking up and down */
f32 player_look_gain[6] = { 0.0026179939f, 0.0026179939f, 0.0026179939f, 0.0013089969f, 0.0026179939f, 0.0f }; /* 0x413E88 looking speed gained per frame */
f32 player_turn_loss[6] = { 0.0026179939f, 0.0026179939f, 0.0026179939f, 0.0013089969f, 0.0026179939f, 0.0f }; /* 0x413EA0 turning and looking speed lost per frame */
f32 player_level_step[6] = { 0.03374303f, 0.03374303f, 0.03374303f, 0.016871516f, 0.03374303f, 0.0f };        /* 0x413EB8 how fast the view levels out */
f32 player_gravity[6] = { 6.6666665f, 6.6666665f, 0.3333333f, 0.3333333f, 0.03f, 0.0f };                   /* 0x413ED0 falling speed gained per frame */
/* 0x413EE8 the screen tint colours, red, green and blue */
s32 screen_tint_colors[6][3] = {
    { 0x80, 0, 0 }, { 0, 0x80, 0 }, { 0, 0, 0x80 },
};
f32 weapon_tip_position[4] = { 0.0f, 0.0f, 0.0f, 1.0f };                                                        /* 0x413F30 the weapon tip */
f32 weapon_tip_angles[4] = { 0.0f, 0.0f, 0.0f, 1.0f };                                                        /* 0x413F40 the tip's angles */
/* 0x413F50 the held item's action handler, Player_UpdateActions picks it with PlayerItemDef.unk63 */
void (*item_action_handlers[4])(u16, u16) = { Player_SwingWeapon, Player_SwingWeapon, Player_InvokeTargetHandler, 0 };
/* 0x413F60 the held item's step handler, picked with PlayerState.unk258; three of them ignore the buttons */
void (*item_step_handlers[8])(u16, u16) = {
    (void (*)(u16, u16))Player_FireProjectile, (void (*)(u16, u16))Player_DrawBow,
    (void (*)(u16, u16))Player_PlayEquipSound, (void (*)(u16, u16))Player_FinishWeaponMotion,
    (void (*)(u16, u16))Player_HolsterWeapon, 0, 0, 0,
};

void *memset(void *, int, u32);
float sinf(float);
int rand(void);
int Sound_PlayFootstep(int);
int Sound_PlayCommon(int);
int func_001B4F30(u16, void *, void *, void *, void *);
void Player_Move(u16 held, u16 pressed); // match: retail passes both pad words
void func_001B9078(void);
void func_001B92C0(float);
void func_001B9478(void);
void func_001B94C0(void);
void func_001B9588(void);
void func_001B9610(void);
void func_001B96C8(void);
void func_001BA310(void);
void func_001BAE88(void);
void func_001BB260(int);
void func_001BB2E8(int);
int func_001BB308(int);
void func_001BB5E8(void);
void func_001BB728(u16 *, u32);
void func_001BB908(void);
void func_001BB978(void);
int func_001BF6E8(void);
void func_001B4B10(int, int);
void func_001B4DF8(int idx, int amount);

extern u8 spell_levels[6][5];
extern u64 player_status_bits[];            /* player_previous.unk1C8 */
extern GameDefinition D_00416D78[70];
extern LevelRecord weapon_level_records[70];
extern LevelDef item_name_defs[0x17C];

#define STRIDE_LENGTH 0x1400

#define GAUGE_FULL 5000
/* a timer that is running, stepped; TRUE on the frame it runs out */
#define TIMER_SIGNED_ENDS(timer) ((timer) != 0 && --(timer) <= 0)
#define TIMER_ENDS(timer) ((timer) != 0 && --(timer) == 0)
#define RUN_DOWN_PAIR(index, timers, second, first) \
    if (TIMER_SIGNED_ENDS(player_state.timers[index])) { \
        player_state.first[index] = 0; \
        player_state.second[index] = 0; \
    }
#define RUN_DOWN_SIGNED(index, timers, values) \
    if (TIMER_SIGNED_ENDS(player_state.timers[index])) { \
        player_state.values[index] = 0; \
    }
#define RUN_DOWN(index, timers, values) \
    if (TIMER_ENDS(player_state.timers[index])) { \
        player_state.values[index] = 0; \
    }
#define WEAR_OFF(index) \
    if (wear < (u16)player_state.timed_flags[index]) { \
        player_state.timed_flags[index] -= wear; \
    } else { \
        player_state.timed_flags[index] = 0; \
    }
/* one of the nine values: the item's row for its level plus what it gained, scaled by LevelRecord.level in percent */
#define SET_SCALED(n)     player_state.unk60.values[n] = (item_def->unk00[record->unk14].values[n] + record->unk00[n]) * record->level / 100

/* Sets the player up for a new game. */
void func_001B5810(void) {
    int i;
    int j;

    memset(&player_state, 0, sizeof(PlayerState));
    memset(&player_previous, 0, sizeof(PlayerState));
    memset(&player_screen_tint, 0, sizeof(PlayerScreenTint));
    player_state.magic_base = player_state.power_base = 20;
    player_state.gold = 50000;
    player_state.level = 1;
    player_state.hp_base = 50;
    player_state.mp_base = 15;
    player_state.experience = 0;
    for (i = 0; i < 6; i++) {
        for (j = 0; j < 5; j++) {
            spell_levels[i][j] = 0;
        }
    }
    player_state.equipment[EQUIP_HELM] = ITEM_WORN_INDEX(ITEM_HELM_NONE);
    player_state.equipment[EQUIP_ARMOR] = ITEM_WORN_INDEX(ITEM_ARMOR_NONE);
    player_state.equipment[EQUIP_GAUNTLET] = ITEM_WORN_INDEX(ITEM_GAUNTLET_NONE);
    player_state.equipment[EQUIP_BOOT] = ITEM_WORN_INDEX(ITEM_BOOT_NONE);
    player_state.equipment[EQUIP_RING] = ITEM_WORN_INDEX(ITEM_RING_NONE);
    player_state.equipment[EQUIP_BRACELET] = ITEM_WORN_INDEX(ITEM_BRACELET_NONE);
    player_state.equipment[EQUIP_AMULET] = ITEM_WORN_INDEX(ITEM_AMULET_NONE);
    player_state.position.x = player_state.position.y = player_state.position.z = player_state.position.w = 0.0f;
    player_state.hp_now = player_state.hp_base;
    player_state.mp_now = player_state.mp_base;
    player_state.rotation.y = player_state.rotation.z = player_state.rotation.w = 0.0f;
    player_state.timed_flags[0] = 0;
    player_state.timed_flags[1] = 0;
    player_state.timed_flags[2] = 0;
    player_state.timed_flags[3] = 0;
    player_state.timed_flags[4] = 0;
    player_state.timed_flags[5] = 0;
    player_state.weapon = 0;
    player_state.equipment[EQUIP_SHIELD] = ITEM_WORN_INDEX(ITEM_SHIELD_NONE);
    player_state.shortcut_type = 0;
    player_state.shortcut = 0;
    player_state.unkBC = 0;
    player_state.unkC0 = 0;
    player_state.unk1A4 = 0;
    player_state.unk1A6 = 0;
    player_state.unk264 = 0;
    player_state.rotation.x = 0.0f;
    player_state.unk250 = 0xFF;
    player_state.map_index = 0xFFFF;
    player_state.unk1C4 = 0.0f;
    player_state.unk1A8 = 0;
    player_state.unk1C0 = 0;
    func_001BA310();
    func_001B96C8();
    player_previous = player_state;
}

void func_001B5A10(void) {
    player_state.unkBC = 0;
    player_state.unkC0 = 0;
    player_state.unk1A4 = 0;
    player_state.unk1A6 = 0;
    player_state.unk264 = 0;
    ((int (*)(void))func_001BB5E8)(); // match: called as if it returned something, which keeps the call from being a jump
}

/* Makes sure the player's map record index is a valid one. */
static inline void check_map_index(void) {
    PlayerState *player = &player_state;
    u32 map_index = player->map_index; // match: a wide local, narrowed where it is tested

    if (map_index >= map_slots[D_00446770].header->record_count) {
        player->map_index = 0xFFFF;
        map_index = 0xFFFF;
        player->unk250 = 0xFF;
    }
    if ((u16)map_index == 0xFFFF) { // match: see map_index
        func_001BB908();
        if (player->map_index == 0xFFFF) {
            player->map_index = 0;
            player->unk250 = ((MapRecordInfo *)map_slots[D_00446770].records)->unk38;
        }
    }
}

/* The player's frame: reads the pad, runs movement, magic and the item in hand, footsteps and timers. */
void Player_Update(void) {
    Vector4 zero;                   // match: cleared and never read
    f32 unused[16];                 // match: retail's frame has room for this
    PlayerItemDef *item_def = &weapon_defs[player_state.weapon];
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
    if (game_status.unk00 == 0 || (game_status.unk30 & 1)) {
        func_001B9478();
    }
    held = pad_state[0];
    pressed = pad_state[1];
    player_previous = player_state;

    PLAYER_STATUS(&player_state)->bit35 = FALSE;
    player_state.unk1E4 = 6;
    player_state.unk1EC = 0;
    player_state.unk1E0 = 0;
    if (game_status.unk00 != 0 && !(game_status.unk30 & 1)) {
        PLAYER_STATUS(&player_state)->bit38 = FALSE;
        PLAYER_STATUS(&player_state)->bit39 = FALSE;
        player_state.unk1DC = D_00446954[0];
        PLAYER_STATUS(&player_state)->bit34 = TRUE;
    } else {
        PLAYER_STATUS(&player_state)->bit34 = FALSE;
    }
    if (D_0044695C != 0) {
        D_0044695C--;
    }

    /* what the pad may do this frame */
    if (game_status.unk30 & 1) {
        pressed = 0;
        held = 0;
    }
    if (game_status.unk00 == 0) {
        pressed = 0;
        held = 0;
    }
    if (player_state.unk1A8 != 0) {
        pressed = 0;
        held = 0;
    }
    if (game_status.unk07 == 0) {
        held &= ~(D_0044692C | D_0044692E | D_00446934 | D_00446936);
        pressed &= ~(D_0044692C | D_0044692E | D_00446934 | D_00446936);
    }
    if ((u16)player_state.timed_flags[1] != 0) {
        mask = ~(D_0044693C | (D_0044692C | D_0044692E | D_00446934 | D_00446936 | D_00446930 | D_00446932 |
                               D_00446938 | D_0044693A) | D_0044693E);
        held &= mask;
        pressed &= mask;
    }
    if (game_status.bit6 || game_status.unk44 > 0) {
        held &= ~(D_0044693E | D_00446940 | D_00446942 | D_00446944);
        pressed &= ~(D_0044693E | D_00446940 | D_00446942 | D_00446944);
    }

    /* the magic button: an event magic is handed to the game, any other is cast when it is charged and paid for */
    if (!((D_0044693C | D_0044693E | D_00446940 | D_00446942) & pressed)) {
        if ((D_00446944 & pressed) && player_state.shortcut != 0 && player_state.unk1A8 == 0 &&
            !(game_status.bit2 || game_status.bit3 || game_status.bit4)) {
            switch (player_state.shortcut_type) {
            case 0:
                break;
            case 1:
                if (item_counts[player_state.shortcut] != 0) {
                    game_status.unk20 = player_state.shortcut;
                    game_status.bit6 = TRUE;
                }
                break;
            case 2:
                if (player_state.unkC0 >= 5000 && player_state.mp_now >= fx_table[player_state.shortcut].unkFC) {
                    Player_CastMagic(player_state.shortcut);
                }
                break;
            }
        }
    }
    game_status.bit1 = FALSE;
    if (D_00446940 & pressed) {
        game_status.bit1 = TRUE;
        if (game_status.bit6 || player_state.unk1A8 != 0) {
            game_status.bit1 = FALSE;
        }
    }

    /* running: the run button with forward or back, or with one of the two steps to the side */
    if ((D_00446940 & held) &&
        (((D_0044692C | D_0044692E) & held) || ((D_00446936 & held) && !(D_00446934 & held)) ||
         (!(D_00446936 & held) && (D_00446934 & held)))) {
        PLAYER_STATUS(&player_state)->bit35 = TRUE;
        if (game_status.unk06 != 0) {
            PLAYER_STATUS(&player_state)->bit35 = FALSE;
        } else if (player_state.weight > player_state.unkA8) {
            PLAYER_STATUS(&player_state)->bit35 = FALSE;
        } else if (item_def->unk63 == 2 && player_state.unk258 != 0) {
            PLAYER_STATUS(&player_state)->bit35 = FALSE;
        } else if ((u16)player_state.timed_flags[4] != 0) {
            PLAYER_STATUS(&player_state)->bit35 = FALSE;
        } else if (player_state.unk108 != 0) {
            PLAYER_STATUS(&player_state)->bit35 = FALSE;
        } else if (player_state.unk200 != 0) {
            PLAYER_STATUS(&player_state)->bit35 = FALSE;
        // match: unk3E is tested again with the rest of the set
        } else if (player_state.equipment[EQUIP_SHIELD] == ITEM_WORN_INDEX(ITEM_ANCIENT_SHIELD) &&
                   !PLAYER_WEARS_ANCIENT_SET(&player_state)) {
            PLAYER_STATUS(&player_state)->bit35 = FALSE;
        }
    } else {
        PLAYER_STATUS(&player_state)->bit35 = FALSE;
    }
    if ((D_00446942 & pressed) && !(D_00446940 & pressed)) {
        game_status.bit2 = TRUE;
    }

    /* what was added to unk2A0 comes off unkB8, and the screen tint builds up while it does */
    if (player_state.unk2A0 != 0) {
        if (player_state.unk2A0 < player_state.mp_now) {
            player_state.mp_now -= player_state.unk2A0;
        } else {
            player_state.mp_now = 0;
        }
        if (player_state.unk29C < 8) {
            player_state.unk29C++;
        }
        player_state.unk2A0 = 0;
    } else if (player_state.unk29C > 0) {
        player_state.unk29C--;
    }

    if (game_status.unk00 != 0 && !(game_status.unk30 & 1)) {
        func_001BA310();
        func_001B96C8();
    }
    if (game_status.unk00 != 0 && !(game_status.unk30 & 1)) {
        Player_Move(held, pressed);
        func_001B9078();
        Ladder_DispatchPad(held, pressed);
        func_001B94C0();
        func_001B9588();
        func_001B9610();
    } else if (player_state.map_index == 0xFFFF || player_state.position.x != player_previous.position.x ||
               player_state.position.y != player_previous.position.y || player_state.position.z != player_previous.position.z) {
        func_001BB908();
    }
    check_map_index();

    /* the stride: unk1C0 runs from 0 to STRIDE_LENGTH, faster when running and slower in water */
    if (game_status.unk00 != 0 && !(game_status.unk30 & 1)) {
        stride_speed = 1.0f;
        if ((s32)(player_state.unk1C8 >> 35) & 1) {
            stride_speed = 1.6f;
        }
        if ((s32)(player_state.unk1C8 >> 39) & 1) {
            stride_speed *= 0.5f;
        } else if ((s32)(player_state.unk1C8 >> 38) & 1) {
            stride_speed *= 0.6666667f;
        }
        stride_step = stride_speed * 256.0f;
        if (player_state.unk1B0 != 0.0f || player_state.unk1B4 != 0.0f) {
            player_state.unk1C0 += stride_step;
            if (player_state.unk1C0 >= STRIDE_LENGTH) {
                player_state.unk1C0 %= STRIDE_LENGTH;
                PLAYER_STATUS(&player_state)->step_sounded = FALSE;
            }
        } else if (player_state.unk1C0 != 0) {
            player_state.unk1C0 += stride_step;
            if (player_state.unk1C0 >= STRIDE_LENGTH) {
                player_state.unk1C0 = 0;
                PLAYER_STATUS(&player_state)->step_sounded = FALSE;
            }
        }
    }
    /* head bob, when the option is on */
    if (game_options[5] != 0) {
        player_state.unk1C4 = sinf((f32)player_state.unk1C0 * 6.2831855f / 5120.0f) * 2.75f;
    } else {
        player_state.unk1C4 = 0.0f;
    }

    /* one footstep sound per stride, three quarters of the way through it, by what the player walks on */
    if (game_status.unk00 != 0 && !(game_status.unk30 & 1) && player_state.unk1A8 == 0 &&
        PLAYER_STATUS(&player_state)->step_sounded == FALSE &&
        !(ABS(player_state.unk1B0) < movement_top_speed * 0.5f && ABS(player_state.unk1B4) < movement_top_speed * 0.5f) &&
        !((s32)(player_state.unk1C8 >> 34) & 1) && player_state.unk1C0 >= 0xF00) {
        sound_free = sound_load_queue.unk10 == 0 && sound_load_queue.unk14 == 0;
        D_00446958 = -1;
        if ((s32)(player_state.unk1C8 >> 39) & 1) {
            PLAYER_STATUS(&player_state)->step_sounded = TRUE;
            D_00446958 = Sound_PlayFootstep(0x25);
        } else if ((s32)(player_state.unk1C8 >> 38) & 1) {
            PLAYER_STATUS(&player_state)->step_sounded = TRUE;
            D_00446958 = Sound_PlayFootstep(0x24);
        } else if (player_state.unk1E0 != 0 && player_state.unk1E4 == 4) {
            PLAYER_STATUS(&player_state)->step_sounded = TRUE;
            if (sound_free) {
                D_00446958 = Sound_PlayFootstep(0x27);
            }
        } else {
            PLAYER_STATUS(&player_state)->step_sounded = TRUE;
            switch (player_state.unk210) {
            case 1:
                Sound_PlayFootstep(0x21);
                break;
            case 2:
                if (sound_free) {
                    D_00446958 = Sound_PlayFootstep(0x22);
                }
                break;
            case 3:
                if (sound_free) {
                    D_00446958 = Sound_PlayFootstep(0x23);
                }
                break;
            case 5:
                Sound_PlayFootstep(0x24);
                break;
            case 0:
            case 6:
                Sound_PlayFootstep(0x20);
                break;
            default:
                PLAYER_STATUS(&player_state)->step_sounded = FALSE;
                break;
            }
        }
    }
    /* the warning sound while unkB4 is at a tenth of unk58 or less */
    if (player_state.unk1A8 == 0 && player_state.hp_now > 0 && player_state.hp_now * 100 / player_state.hp_max < 11 &&
        D_00446ED4 % 27 == 0) {
        Sound_PlayCommon(0x29);
    }

    Player_UpdateActions(held, pressed);
    Player_UpdateScreenTints();
    if (player_state.unk2DC > 0) {
        player_state.unk2DC--;
    }
    if (player_state.unk2E0 > 0) {
        player_state.unk2E0--;
    }
    if (player_state.unk2E4 > 0) {
        player_state.unk2E4--;
    } else {
        player_state.unk2E8 = 0.0f;
    }
    if (player_state.unk2EC > 0) {
        player_state.unk2EC--;
    }

    /* item 0x32 worn: effect 0x7C starts at the player by chance, at most once in 30 frames, unless under water */
    if (player_state.equipment[EQUIP_HELM] == ITEM_WORN_INDEX(ITEM_SACRIFICIAL_MASK) && game_status.unk00 != 0 && !(game_status.unk30 & 1)) {
        if (D_00446950 > 0) {
            D_00446950--;
        } else if (!(((s32)(player_state.unk1C8 >> 38) & 1) &&
                     player_state.unk1DC < player_state.position.y - fx_table[0x7C].unk108) &&
                   (s32)((f32)rand() * 451.0f * 0.0000000004656613f) == 0) {
            shot = Shot_Alloc();
            shot->unk00 = 1;
            shot->unk20 = 0;
            shot->unk22 = 0;
            func_001B4F30(0x7C, shot, &player_state, &player_state.rotation, &map_slots[D_00446770]);
            D_00446950 = 30;
        }
    }
    if (map_slots[D_00446770].tag == 0x23 && D_00349F56[0] == 0 && ((s32)(player_state.unk1C8 >> 38) & 1) &&
        game_status.unk00 != 0 && !(game_status.unk30 & 1)) {
        func_001BB2E8(1);
    }
    func_001BB978();
    game_status.unk07 = 1;
}

void GsDraw_FillScreen(int, int, int, int, int);

/* Draws the colours that lie over the screen (PlayerScreenTint). */
void Player_DrawScreenTints(void) {
    if (player_screen_tint.unk08 != 0 && player_screen_tint.kind == 0) {
        GsDraw_FillScreen(0, 0xFF, 0, player_screen_tint.unk08, 0x48);
    }
    if (player_screen_tint.unk0C != 0 && player_screen_tint.kind == 1) {
        GsDraw_FillScreen(0x80, 0x80, 0, player_screen_tint.unk0C, 0x44);
    }
    if (player_screen_tint.unk14 != 0 && player_screen_tint.kind == 3) {
        GsDraw_FillScreen(0xA0, 0, 0xA0, player_screen_tint.unk14, 0x48);
    }
    if (player_screen_tint.unk18 != 0 && player_screen_tint.kind == 4) {
        GsDraw_FillScreen(0xFF, 0xFF, 0xFF, player_screen_tint.unk18, 0x48);
    }
    if (player_screen_tint.unk28 != 0 && player_screen_tint.kind == 5) {
        GsDraw_FillScreen(player_screen_tint.red, player_screen_tint.green, player_screen_tint.blue, player_screen_tint.unk28, 0x48);
    }
    if (player_screen_tint.unk10 > 0) {
        GsDraw_FillScreen(0, 0, 0, player_screen_tint.unk10, 0x44);
    }
    if (player_screen_tint.unk04 != 0) {
        if (player_screen_tint.unk14 != 0 && player_screen_tint.kind == 3) {
            GsDraw_FillScreen(0x60, 0, 0, player_screen_tint.unk04, 0x48);
        } else {
            GsDraw_FillScreen(0xFF, 0, 0, player_screen_tint.unk04, 0x48);
        }
    }
    if (player_screen_tint.unk30 != 0) {
        GsDraw_FillScreen(0, 0, 0, player_screen_tint.unk30, 0x44);
    }
}

extern MapDrawState map_draw_state;
extern const s8 D_0043F260[30];

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

    player_screen_tint.kind = 0;
    if (newest < (s32)player_state.unk2C4 && (u16)player_state.timed_flags[0] != 0) {
        player_screen_tint.kind = 0;
        newest = player_state.unk2C4;
    }
    if (newest < (s32)player_state.unk2C8 && (u16)player_state.timed_flags[1] != 0) {
        player_screen_tint.kind = 1;
        newest = player_state.unk2C8;
    }
    if (newest < (s32)player_state.unk2CC && (u16)player_state.timed_flags[2] != 0) {
        darkened = TRUE;
    }
    if (newest < (s32)player_state.unk2D0 && (u16)player_state.timed_flags[3] != 0) {
        player_screen_tint.kind = 3;
        newest = player_state.unk2D0;
    }
    if (newest < (s32)player_state.unk2D4 && (u16)player_state.timed_flags[4] != 0) {
        player_screen_tint.kind = 4;
        newest = player_state.unk2D4;
    }
    if (newest < (s32)player_state.unk2D8 && (u16)player_state.timed_flags[5] != 0) {
        player_screen_tint.kind = 5;
    }

    player_screen_tint.unk04 = player_screen_tint.unk00 * 8;
    if (player_screen_tint.unk00 > 0) {
        if (player_state.aim_speed > 0.0f) {
            player_screen_tint.unk00 = 0;
        } else {
            player_screen_tint.unk00--;
        }
    }

    if ((u16)player_state.timed_flags[0] != 0) {
        player_screen_tint.unk08++;
        if (player_screen_tint.unk08 > 0x20) {
            player_screen_tint.unk08 = 0x20;
        }
    } else {
        player_screen_tint.unk08--;
        if (player_screen_tint.unk08 < 0) {
            player_screen_tint.unk08 = 0;
        }
    }

    if ((u16)player_state.timed_flags[1] != 0) {
        player_screen_tint.unk0C = D_0043F260[29 - (u16)player_state.timed_flags[1] % 30];
    } else {
        player_screen_tint.unk0C = 0;
    }

    tick = D_00446ED4;
    if (!(D_00446ED4 & 1)) {
        if ((u16)player_state.timed_flags[3] != 0) {
            player_screen_tint.unk14++;
            if (player_screen_tint.unk14 > 0x20) {
                player_screen_tint.unk14 = 0x20;
            }
        } else {
            player_screen_tint.unk14--;
            if (player_screen_tint.unk14 < 0) {
                player_screen_tint.unk14 = 0;
            }
        }
    }

    if ((u16)player_state.timed_flags[4] != 0) {
        player_screen_tint.unk18++;
        if (player_screen_tint.unk18 > 0x20) {
            player_screen_tint.unk18 = 0x20;
        }
    } else {
        player_screen_tint.unk18--;
        if (player_screen_tint.unk18 < 0) {
            player_screen_tint.unk18 = 0;
        }
    }

    cycling = player_state.timed_flags[5];
    colour = (tick >> 5) % 3;
    next_colour = (colour + 1) % 3;
    blend = tick & 0x1F;
    tick = 0; // match: a dead store; without it the frame count is not copied to a second register
    player_screen_tint.red = screen_tint_colors[colour][0] * (0x20 - blend) / 32 + screen_tint_colors[next_colour][0] * blend / 32;
    player_screen_tint.green = screen_tint_colors[colour][1] * (0x20 - blend) / 32 + screen_tint_colors[next_colour][1] * blend / 32;
    player_screen_tint.blue = screen_tint_colors[colour][2] * (0x20 - blend) / 32 + screen_tint_colors[next_colour][2] * blend / 32;
    if (cycling != 0) {
        if (!(D_00446ED4 & 1)) {
            player_screen_tint.unk28++;
            if (player_screen_tint.unk28 > 0x18) {
                player_screen_tint.unk28 = 0x18;
            }
        }
    } else if (!(D_00446ED4 & 1)) {
        player_screen_tint.unk28--;
        if (player_screen_tint.unk28 < 0) {
            player_screen_tint.unk28 = 0;
        }
    }

    if (player_state.unk2E0 != 0 || player_state.unk10C != 0) {
        map_draw_state.unk2A90 += 0.0078125f;
        if (map_draw_state.unk2A90 > 0.25f) {
            map_draw_state.unk2A90 = 0.25f;
        }
        player_screen_tint.unk34++;
        if (player_screen_tint.unk34 > 0xA0) {
            player_screen_tint.unk34 = 0xA0;
        }
    } else {
        map_draw_state.unk2A90 -= 0.0078125f;
        if (map_draw_state.unk2A90 < 0.0f) {
            map_draw_state.unk2A90 = 0.0f;
        }
        player_screen_tint.unk34--;
        if (player_screen_tint.unk34 < 0x80) {
            player_screen_tint.unk34 = 0x80;
        }
    }
    map_draw_state.unk2A98 = map_draw_state.unk2A94 = map_draw_state.unk2A90;

    if (player_state.unk2E4 != 0) {
        darkened = TRUE;
    }
    if (player_state.weapon == ITEM_WEAPON_INDEX(ITEM_ANCIENT_SWORD) &&
        !PLAYER_WEARS_ANCIENT_SET(&player_state)) {
        darkened = TRUE;
    } else if (player_state.equipment[EQUIP_HELM] == ITEM_WORN_INDEX(ITEM_SHADOW_MASK)) {
        darkened = TRUE;
    }
    if (darkened) {
        player_screen_tint.unk10 += 2;
        if (player_screen_tint.unk10 > 0x50) {
            player_screen_tint.unk10 = 0x50;
        }
    } else {
        player_screen_tint.unk10 -= 2;
        if (player_screen_tint.unk10 < 0) {
            player_screen_tint.unk10 = 0;
        }
    }

    if (player_state.unk1A8 == 0) {
        player_screen_tint.unk30 = 0;
    } else {
        player_screen_tint.unk30 = player_state.unk1A8 * 0x80 / 60;
    }
}

/* The player's actions for this frame: runs the handler of the item in hand, then the magic button. */
void Player_UpdateActions(u16 held, u16 pressed) {
    PlayerItemDef *def;

    player_state.unk259 = 0;
    def = &weapon_defs[player_state.weapon];
    player_state.unk25B = 0;
    if (player_state.unk258 != 0) {
        if (game_status.unk00 == 0 || (game_status.unk30 & 1) || player_state.unk1A8 != 0 ||
            (u16)player_state.timed_flags[1] != 0) {
            player_state.unk263 = 1;
            Effect_FreeSpawnersByParam(0x5D);
        }
    }
    item_action_handlers[def->unk63](held, pressed);
    if (player_state.unk258 == 0 || (player_state.unk25A == 0 && player_state.unk25B == 0)) {
        if (D_0044693E & pressed) {
            Player_CastMagic(player_state.equipped_magic);
        }
        if (D_0044693E & held) {
            Player_HoldMagic(player_state.equipped_magic);
        }
    }
    if (player_state.unk258 != 0) {
        player_state.unk254 = 0;
    } else {
        player_state.unk254++;
    }
}

extern LevelRecord weapon_level_records[];

void ModelMotion_Select(ModelInstance *, int);
void ModelMotion_Step(void *);
void Sound_PlaySw(int mode);
s32 func_001B1470(Unk1B1470Request *, Unk1B1470Extra *);

/* Handler of a weapon that is swung: the button starts the swing; while its motion runs the
   swing hits what it reaches, plays its sound and casts the weapon's two magics when they were
   asked for in time. */
void Player_SwingWeapon(u16 held, u16 pressed) {
    Unk1B1470Request request;
    LevelRecord *record = &weapon_level_records[player_state.weapon];
    PlayerItemDef *def = &weapon_defs[player_state.weapon];
    s32 level = record->unk14;
    PlayerShot *shot;

    if (player_state.unk258 == 0) {
        if (def->unk42 != 0 && def->unk42 != 0xFFFF && player_state.power >= def->unk8A && player_state.magic >= def->unk8C) {
            if (player_state.unk25C != 0) {
                player_state.unk25C--;
            }
            if (D_00446940 & pressed) {
                player_state.unk25C = 15;
            }
        } else {
            player_state.unk25C = 0;
        }
        player_state.unk25D = level >= 2 && def->unk52 != 0 && def->unk52 != 0xFFFF;
        if (D_0044693C & pressed) {
            shot = &player_state.unk268;
            memset(shot, 0, sizeof(PlayerShot));
            shot->unk00 = 0;
            shot->unk20 = player_state.power;
            shot->unk22 = player_state.magic;
            shot->unk02 = player_state.unkBC;
            shot->unk24 = player_state.weapon;
            ModelMotion_Select(WEAPON_MODEL, 0);
            player_state.unk260_0 = 0;
            player_state.unk260_1 = 0;
            player_state.unk260_2 = 0;
            player_state.unk258 = 1;
            player_state.unkC4 = def->unk7E;
            swing_magic_frames = 0;
            player_state.unkBC = 0;
            player_state.unk25B = 0;
            player_state.unk25A = 0;
            player_state.unk261 = 0;
            player_state.unk262 = 0;
            player_state.unk25E = 0;
            player_state.unk25F = 0;
            player_state.unk263 = 0;
            WeaponTrail_PickKind();
        }
        return;
    }
    if (player_state.unk261 == 0) {
        if (player_state.unk263 != 0) {
            swing_magic_frames = 0;
        }
        if (swing_magic_frames <= 0) {
            ModelMotion_Step(WEAPON_MODEL);
        } else {
            swing_magic_frames--;
            if (((s32)(player_state.unk1C8 >> 39) & 1) ||
                (((s32)(player_state.unk1C8 >> 38) & 1) &&
                 player_state.position.y - fx_table[swing_magic_id].unk108 > player_state.unk1DC)) {
                Effect_FreeSpawnersByParam(swing_magic_id);
                swing_magic_frames = 0;
            }
        }
        if (MODEL_CHANNEL(WEAPON_MODEL)->param_shifted <= 0) {
            player_state.unk258 = 0;
            return;
        }
        if (player_state.unk263 != 0) {
            return;
        }
        if (MODEL_CHANNEL(WEAPON_MODEL)->param >= def->unk78 && MODEL_CHANNEL(WEAPON_MODEL)->param <= def->unk7A && swing_magic_frames == 0) {
            memset(&request, 0, sizeof(request));
            Vector_Copy(request.position, &player_state);
            request.owner = (Entity *)&player_state;
            request.unk4C_0 = 3;
            request.position[1] -= 220.0f;
            request.unk30 = def->unk74 + 100.0f;
            request.unk34 = 230.0f;
            request.unk38 = player_state.rotation.y;
            request.unk3C = def->unk70 * 0.5f * 0.017453292f;
            request.unk40 = player_state.rotation.x;
            request.unk44 = def->unk6C * 0.5f * 0.017453292f;
            request.unk4D = 0x3E;
            *(PlayerShotStats *)&request.unk50 = player_state.unk60;
            *(PlayerItemRates *)&request.unk62 = player_state.unk72;
            request.unk6E = def->unk7F;
            request.unk6F = def->unk86;
            request.unk78 = player_state.position.y - 230.0f - 50.0f;
            request.unk7C = player_state.position.y + 50.0f;
            func_001B1470(&request, (Unk1B1470Extra *)&player_state.unk268);
        }
        if (!player_state.unk260_0 && def->unk80 != 0xFFFF && MODEL_CHANNEL(WEAPON_MODEL)->param >= def->unk82) {
            ((int (*)(int))Sound_PlaySw)(def->unk80); // match: called as if it returned a value
            player_state.unk260_0 = 1;
        }
        if (D_0044693E & pressed) {
            if (player_state.unk25C != 0 && player_state.unk25F == 0 && def->unk42 != 0xFFFF &&
                MODEL_CHANNEL(WEAPON_MODEL)->param >= def->unk4A && MODEL_CHANNEL(WEAPON_MODEL)->param <= def->unk4C) {
                player_state.unk25D = 0;
                player_state.unk25B = 1;
                player_state.unk25F = 1;
            }
            if (player_state.unk25D != 0 && player_state.unk25A == 0 && def->unk52 != 0xFFFF &&
                MODEL_CHANNEL(WEAPON_MODEL)->param >= def->unk5A && MODEL_CHANNEL(WEAPON_MODEL)->param <= def->unk5C) {
                player_state.unk25A = 1;
                player_state.unk25E = 1;
            }
            player_state.unk25C = 0;
            player_state.unk25D = 0;
        }
        if (player_state.unk25F == 1 && MODEL_CHANNEL(WEAPON_MODEL)->param >= def->unk44) {
            player_state.unk25F = 2;
            if (Player_CastMagic(def->unk42)) {
                swing_magic_id = def->unk42;
                swing_magic_frames = def->unk4E;
            }
        }
        if (player_state.unk25E == 1 && MODEL_CHANNEL(WEAPON_MODEL)->param >= def->unk54) {
            player_state.unk25E = 2;
            if (Player_CastMagic(def->unk52)) {
                swing_magic_id = def->unk52;
                swing_magic_frames = def->unk5E;
            }
        }
        if (player_state.unk25F != 0 && !player_state.unk260_1 && def->unk46 != 0xFFFF &&
            MODEL_CHANNEL(WEAPON_MODEL)->param >= def->unk48) {
            player_state.unk260_1 = 1;
        }
        if (player_state.unk25E != 0 && !player_state.unk260_2 && def->unk56 != 0xFFFF &&
            MODEL_CHANNEL(WEAPON_MODEL)->param >= def->unk58) {
            player_state.unk260_1 = 1; // match: bit 1 again, though bit 2 was tested
        }
    } else {
        ModelMotion_Step(WEAPON_MODEL);
        if (MODEL_CHANNEL(WEAPON_MODEL)->value == 0) {
            player_state.unk258 = 0;
        }
    }
}

extern u8 D_004141D8[];

void Player_InvokeTargetHandler(u16 target_id, u16 command) {
    Player_UpdateWeaponAim();
    item_step_handlers[D_004141D8[0]](target_id, command);
}

/* Handler of an item that shoots: on the button, starts the motion and launches the item's shot. */
void Player_FireProjectile(u16 held, u16 pressed) {
    PlayerItemDef *def = &weapon_defs[player_state.weapon];

    if ((D_0044693C & pressed) && !((s32)(player_state.unk1C8 >> 39) & 1) && item_counts[def->unk62] != 0) {
        ModelMotion_Select(WEAPON_MODEL, 0);
        MODEL_CHANNEL(WEAPON_MODEL)->param = 0;
        MODEL_CHANNEL(WEAPON_MODEL)->param_shifted = 0;
        ModelMotion_Step(WEAPON_MODEL);
        ((int (*)(ModelInstance *))WEAPON_MODEL->unk1B4)(WEAPON_MODEL); // match: called as if it returned a value
        player_state.unk260_0 = 0;
        player_state.unk298 = 0;
        player_state.unk258++;
        player_state.unk263 = 0;
        item_shot = Shot_Alloc();
        item_shot->unk00 = 0;
        item_shot->unk26 = def->unk52;
        item_shot->unk20 = player_state.power;
        item_shot->unk22 = player_state.magic;
        item_shot->unk02 = player_state.unkBC;
        func_001B4F30(def->unk52, item_shot, weapon_tip_position, weapon_tip_angles, &player_state);
    }
}

extern GameDefinition D_00416D78[];
static u8 D_0057A348[8] __attribute__((section(".bss")));  /* 0x57A348 the eight bytes before the requests */
static Unk1B1470Request D_0057A350[32];                     /* 0x57A350 the bow's requests, one per shot */

int ModelMotion_BlendInto(ModelInstance *model, u8 index, u16 speed);
void Sound_StopVoice(s8 slot);

/* Handler of a drawn weapon while the button is held: the draw motion runs to its last frame and the
   strength builds up; letting go looses the shot, anything else lowers the weapon. */
void Player_DrawBow(u16 held, u16 pressed) {
    GameDefinition *item = &D_00416D78[player_state.weapon];
    PlayerItemDef *def = &weapon_defs[player_state.weapon];
    s32 rate;
    s32 frame;
    s32 speed;

    if (MODEL_CHANNEL(WEAPON_MODEL)->param != 0 && MODEL_CHANNEL(WEAPON_MODEL)->param < def->unk7A) {
        ModelMotion_Step(WEAPON_MODEL);
    }
    if (player_state.unk298 == 0) {
        player_state.unk298 = MODEL_CHANNEL(WEAPON_MODEL)->param >= def->unk78;
    }
    if (player_state.unk263 == 0 && !((s32)(player_state.unk1C8 >> 39) & 1) && Player_CanFireAtAim()) {
        if (!player_state.unk260_0 && def->unk80 != 0xFFFF && MODEL_CHANNEL(WEAPON_MODEL)->param >= def->unk82) {
            ((int (*)(int))Sound_PlaySw)(0); // match: called as if it returned a value
            player_state.unk260_0 = 1;
        }
        if (D_0044693C & held) {
            rate = player_state.power;
            if (rate < 20) {
                rate = 20;
            }
            player_state.unkBC += rate * 42 / (item->unk33 + 1);
            if (player_state.unkBC > 5000) {
                player_state.unkBC = 5000;
            }
            return;
        }
        if (player_state.unk298 != 0) {
            if (player_state.unk260_0) {
                Sound_StopVoice(2);
            }
            item_shot->unk02 = player_state.unkBC;
            item_shot->unk24 = player_state.weapon;
            item_shot->unk04 = &D_0057A350[bow_request_ring_index];
            bow_request_ring_index = (bow_request_ring_index + 1) % 32;
            memset(item_shot->unk04, 0, sizeof(Unk1B1470Request));
            *(PlayerShotStats *)&SHOT_REQUEST->unk50 = player_state.unk60;
            *(PlayerItemRates *)&SHOT_REQUEST->unk62 = player_state.unk72;
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
            ModelMotion_BlendInto(WEAPON_MODEL, 1, speed);
            MODEL_CHANNEL(WEAPON_MODEL)->unk8F = 0;
            player_state.unk258++;
            item_counts[def->unk62]--;
            return;
        }
    }
    ModelMotion_BlendInto(WEAPON_MODEL, 0, 0xF);
    MODEL_CHANNEL(WEAPON_MODEL)->unk8F = 0;
    player_state.unk298 = 0;
    player_state.unkBC = 0;
    player_state.unk258 += 3;
}

void Player_PlayEquipSound(void) {
    Sound_PlaySw(1);
    player_state.unkBC = 0;
    player_state.unk258++;
}

void Player_FinishWeaponMotion(void) {
    if (MODEL_CHANNEL(WEAPON_MODEL)->value != 0) {
        ModelMotion_Step(WEAPON_MODEL);
        if (MODEL_CHANNEL(WEAPON_MODEL)->value == 0) {
            ModelMotion_Step(WEAPON_MODEL);
        }
    } else if (MODEL_CHANNEL(WEAPON_MODEL)->param_shifted > 0) {
        ModelMotion_Step(WEAPON_MODEL);
    } else {
        player_state.unk258 = 0;
        player_state.unk298 = 0;
    }
}

void Player_HolsterWeapon(void) {
    ModelMotion_Step(WEAPON_MODEL);
    if (MODEL_CHANNEL(WEAPON_MODEL)->value == 0) {
        Effect_FreePlayerWeaponKinds();
        D_004141D8[0] = 0;
    }
}

extern float D_00420D10[];

int Player_CanFireAtAim(void) {
    float vec[4];
    int result;

    Vector_Scale(vec, D_00420D10, 100.0f);
    Vector_Add(vec, vec, &player_state);
    vec[1] -= 186.0f;
    result = 0;
    if (vec[1] <= player_state.unk1DC) {
        result = 1;
    }
    return result;
}

float atan2f(float, float);
float sqrtf(float);

void Player_UpdateWeaponAim(void) {
    float vec[4];
    void *weapon_base;
    void *weapon_tip;

    D_00446978->update_aim(D_00446978);
    weapon_base = D_00446978->get_point(D_00446978, 0);
    weapon_tip = D_00446978->get_point(D_00446978, 1);
    Vector_Subtract(vec, weapon_tip, weapon_base);
    Vector_Copy(weapon_tip_position, weapon_tip);
    weapon_tip_angles[0] = atan2f(-vec[1], sqrtf(vec[0] * vec[0] + vec[2] * vec[2]));
    weapon_tip_angles[1] = atan2f(vec[0], vec[2]);
}

extern f32 D_00431AF0[16];

/* the pad bits of each control */

float cosf(float);
double sin(double);
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0RotMatrixZ(float *, float *, float);
float Angle_Wrap(float);
int Angle_Approach(float *, float, float);
int Sound_PlayMap(int);
int func_001BDDC0(void);
void func_001BE760(void);
void func_001BEA50(void);
void func_001BECA8(int);
void func_001BF350(int);

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

    platform = (MapObject *)player_previous.unk208;
    if (platform != NULL && (platform->unk10 & 0x10000) && (MAP_OBJECT_DEF_FLAGS(platform->unk04) & MAP_OBJECT_DEF_CARRIES)) {
        Vector_Subtract(&turn, &platform->model->rotation, &platform->unk30);
        turn.x = Angle_Wrap(turn.x);
        turn.y = Angle_Wrap(turn.y);
        turn.z = Angle_Wrap(turn.z);
        sceVu0RotMatrixZ(matrix, D_00431AF0, turn.z);
        sceVu0RotMatrixX(matrix, matrix, turn.x);
        sceVu0RotMatrixY(matrix, matrix, turn.y);
        Vector_Subtract(&delta, &player_state, &platform->unk20);
        Vector_ApplyMatrix(&delta, matrix, &delta);
        Vector_Add(&player_state, &platform->model->position, &delta);
        player_state.rotation.y += platform->model->rotation.y - platform->unk34;
    }
    Vector_Add(&player_state, &player_state, &player_state.unk240);

    if (player_previous.unk1E0 != 0 && player_previous.unk1E4 == 4) {
        row = 4;
    } else if ((s32)(player_previous.unk1C8 >> 39) & 1) {
        row = 3;
    } else if ((s32)(player_previous.unk1C8 >> 38) & 1) {
        row = 2;
    } else {
        row = player_previous.unk210 == 3;
    }
    top_speed = player_top_speed[row];
    speed_gain = player_speed_gain[row];
    speed_loss = player_speed_loss[row];
    top_turn = player_top_turn_speed[row];
    turn_gain = player_turn_gain[row];
    top_look = player_top_look_speed[row];
    look_gain = player_look_gain[row];
    turn_loss = player_turn_loss[row];
    level_step = player_level_step[row];
    gravity = player_gravity[row];

    if (player_state.weight > player_state.unkA8) {
        ratio = (f32)(player_state.weight - player_state.unkA8) / (f32)player_state.unkA8;
        if (ratio < 1.0f) {
            top_speed -= top_speed * ratio;
            top_turn -= top_turn * ratio;
            top_look -= top_look * ratio;
        } else {
            top_look = top_turn = top_speed = 0.0f;
        }
    }
    if ((u16)player_state.timed_flags[4] != 0 || player_state.unk108 != 0 || player_state.unk200 > 0) {
        top_speed *= 0.5f;
        top_turn *= 0.5f;
        top_look *= 0.5f;
        level_step *= 0.5f;
    }
    if ((s32)(player_state.unk1C8 >> 35) & 1) {
        top_speed *= 1.6f;
    }
    if ((s32)(player_state.unk1C8 >> 36) & 1) {
        buttons &= D_00446938 | D_0044693A;
    }

    if (player_state.aim_speed > 0.0f) {
        Vector_Scale((f32 *)&delta, (f32 *)&player_state.aim, player_state.aim_speed);
        Vector_Add(&player_state, &player_state, &delta);
        player_state.aim_speed -= ((f32)player_state.weight + 600.0f) / 600.0f * 20.0f;
        if (player_state.aim_speed < 0.0) {
            player_state.aim_speed = 0.0f;
        }
    }

    if (player_state.unk234 != 0) {
        player_state.unk234--;
        player_state.unk238 += 0.034906585f;
        player_state.unk23C += 0.0017453292f;
        if (player_state.unk238 > 1.012291f) {
            player_state.unk234 = 0;
        }
    } else {
        if (player_state.unk238 > 0.0f) {
            player_state.unk238 -= 0.008726646f;
            if (player_state.unk238 < 0.0f) {
                player_state.unk238 = 0.0f;
            }
        }
        if (player_state.unk23C > 0.0f) {
            player_state.unk23C -= 0.0008726646f;
            if (player_state.unk23C < 0.0f) {
                player_state.unk23C = 0.0f;
            }
        }
    }

    /* looking up and down */
    if (!((s32)(player_state.unk1C8 >> 32) & 1)) {
        if (D_00446938 & buttons) {
            if (D_0044693A & buttons) {
                player_state.unk1B8 = 0.0f;
                player_state.unk1C8 |= PLAYER_FLAG_32;
            } else {
                player_state.unk1B8 += look_gain;
                if (player_state.unk1B8 > top_look) {
                    player_state.unk1B8 = top_look;
                }
            }
        } else if (D_0044693A & buttons) {
            player_state.unk1B8 -= look_gain;
            if (player_state.unk1B8 < -top_look) {
                player_state.unk1B8 = -top_look;
            }
        } else {
            if (player_state.unk1B8 > 0.0f) {
                player_state.unk1B8 -= turn_loss;
                if (player_state.unk1B8 < 0.0f) {
                    player_state.unk1B8 = 0.0f;
                }
            }
            if (player_state.unk1B8 < 0.0f) {
                player_state.unk1B8 += turn_loss;
                if (player_state.unk1B8 > 0.0f) {
                    player_state.unk1B8 = 0.0f;
                }
            }
        }
    } else if ((D_00446938 & buttons) && (D_0044693A & buttons)) {
        Angle_Approach(&player_state.rotation.x, 0.0f, level_step);
    } else {
        player_state.unk1C8 &= ~PLAYER_FLAG_32;
    }

    /* turning */
    if ((D_00446932 & buttons) && !(D_00446930 & buttons)) {
        player_state.unk1BC -= turn_gain;
        if (player_state.unk1BC < -top_turn) {
            player_state.unk1BC = -top_turn;
        }
    } else if (!(D_00446932 & buttons) && (D_00446930 & buttons)) {
        player_state.unk1BC += turn_gain;
        if (player_state.unk1BC > top_turn) {
            player_state.unk1BC = top_turn;
        }
    } else {
        if (player_state.unk1BC > 0.0f) {
            player_state.unk1BC -= turn_loss;
            if (player_state.unk1B8 < 0.0f) { // match: retail tests the looking speed here, not the turning speed
                player_state.unk1BC = 0.0f;
            }
        }
        if (player_state.unk1BC < 0.0f) {
            player_state.unk1BC += turn_loss;
            if (player_state.unk1BC > 0.0f) {
                player_state.unk1BC = 0.0f;
            }
        }
    }

    /* walking */
    movement_top_speed = top_speed;
    if ((D_0044692C & buttons) && !(D_0044692E & buttons)) {
        if (player_state.unk1B4 < top_speed) {
            player_state.unk1B4 += speed_gain;
            if (player_state.unk1B4 > top_speed) {
                player_state.unk1B4 = top_speed;
            }
        } else if (player_state.unk1B4 > top_speed) {
            player_state.unk1B4 -= speed_loss;
            if (player_state.unk1B4 < top_speed) {
                player_state.unk1B4 = top_speed;
            }
        }
    } else if (!(D_0044692C & buttons) && (D_0044692E & buttons)) {
        backward = -top_speed;
        if (player_state.unk1B4 > backward) {
            player_state.unk1B4 -= speed_gain;
            if (player_state.unk1B4 < backward) {
                player_state.unk1B4 = backward;
            }
        } else if (player_state.unk1B4 < backward) {
            player_state.unk1B4 += speed_loss;
            if (player_state.unk1B4 > backward) {
                player_state.unk1B4 = backward;
            }
        }
    } else {
        if (player_state.unk1B4 > 0.0f) {
            player_state.unk1B4 -= speed_loss;
            if (player_state.unk1B4 < 0.0f) {
                player_state.unk1B4 = 0.0f;
            }
        }
        if (player_state.unk1B4 < 0.0f) {
            player_state.unk1B4 += speed_loss;
            if (player_state.unk1B4 > 0.0f) {
                player_state.unk1B4 = 0.0f;
            }
        }
    }

    /* stepping sideways */
    if (!(D_00446936 & buttons) && (D_00446934 & buttons)) {
        if (player_state.unk1B0 < top_speed) {
            player_state.unk1B0 += speed_gain;
            if (player_state.unk1B0 > top_speed) {
                player_state.unk1B0 = top_speed;
            }
        } else if (player_state.unk1B0 > top_speed) {
            player_state.unk1B0 -= speed_loss;
            if (player_state.unk1B0 < top_speed) {
                player_state.unk1B0 = top_speed;
            }
        }
    } else if ((D_00446936 & buttons) && !(D_00446934 & buttons)) {
        backward = -top_speed;
        if (player_state.unk1B0 > backward) {
            player_state.unk1B0 -= speed_gain;
            if (player_state.unk1B0 < backward) {
                player_state.unk1B0 = backward;
            }
        } else if (player_state.unk1B0 < backward) {
            player_state.unk1B0 += speed_loss;
            if (player_state.unk1B0 > backward) {
                player_state.unk1B0 = backward;
            }
        }
    } else {
        if (player_state.unk1B0 > 0.0f) {
            player_state.unk1B0 -= speed_loss;
            if (player_state.unk1B0 < 0.0f) {
                player_state.unk1B0 = 0.0f;
            }
        }
        if (player_state.unk1B0 < 0.0f) {
            player_state.unk1B0 += speed_loss;
            if (player_state.unk1B0 > 0.0f) {
                player_state.unk1B0 = 0.0f;
            }
        }
    }

    /* falling */
    if (game_status.unk00 != 0 && !(game_status.unk30 & 1) && !((s32)(player_state.unk1C8 >> 36) & 1)) {
        if ((s32)(player_state.unk1C8 >> 33) & 1) {
            player_state.unk1AC = 0.0f;
        } else if (player_state.unk1A8 == 0) {
            player_state.unk1AC += gravity;
        }
    } else {
        player_state.unk1AC = 0.0f;
    }

    delta.y = player_state.unk1BC;
    delta.x = player_state.unk1B8;
    if ((u16)player_state.timed_flags[4] != 0) {
        delta.x *= 0.5f;
        delta.y *= 0.5f;
    }
    player_state.rotation.y = Angle_Wrap(player_state.rotation.y + delta.y);
    player_state.rotation.x += delta.x;
    if (player_state.rotation.x > 1.012291f) {
        player_state.rotation.x = 1.012291f;
    }
    if (player_state.rotation.x < -1.012291f) {
        player_state.rotation.x = -1.012291f;
    }

    delta.z = player_state.unk1B4;
    delta.x = player_state.unk1B0;
    delta.y = delta.w = 0.0f;
    angle = atan2f(ABS(player_state.unk1B4), ABS(player_state.unk1B0));
    delta.x *= cosf(angle);
    delta.z *= sinf(angle);
    sceVu0RotMatrixY(matrix, D_00431AF0, player_state.rotation.y);
    Vector_ApplyMatrix(&delta, matrix, &delta);
    player_state.position.x += delta.x;
    player_state.position.z += delta.z;

    if (game_status.unk00 != 0 && !(game_status.unk30 & 1)) {
        if (player_previous.unk208 == 0) {
            Vector_Subtract(&delta, &player_state, &player_previous);
            distance = sqrtf(delta.x * delta.x + delta.z * delta.z);
            if (distance > STEP_LENGTH) {
                steps = (s32)(distance / STEP_LENGTH);
                delta.y = 0.0f;
                Vector_Normalize(&delta, &delta);
                Vector_Scale((f32 *)&step, (f32 *)&delta, STEP_LENGTH);
                Vector_Scale((f32 *)&delta, (f32 *)&delta, distance - (f32)steps * STEP_LENGTH);
                Vector_Copy(&player_state, &player_previous);
                do {
                    Vector_Add(&player_state, &player_state, &step);
                    func_001BECA8(0);
                    func_001BF350(0);
                    func_001BE760();
                } while (--steps != 0);
                Vector_Add(&player_state, &player_state, &delta);
            }
        }
        func_001BECA8(1);
        if (!((s32)(player_state.unk1C8 >> 36) & 1)) {
            func_001BDDC0();
            func_001BF350(1);
            if (player_state.unk210 == 6 && player_state.unk208 != 0 && ((MapObject *)player_state.unk208)->unk04->kind == 12) {
                func_001BEA50();
            }
            if (!((s32)(player_state.unk1C8 >> 34) & 1)) {
                if (((s32)(player_previous.unk1C8 >> 34) & 1) && player_state.unk1A8 == 0) {
                    if (player_state.position.y - player_state.unk1D8 > 110.0f) {
                        if (!((s32)(player_state.unk1C8 >> 38) & 1)) {
                            Sound_PlayCommon(0x22);
                        } else if (!((s32)(player_previous.unk1C8 >> 38) & 1)) {
                            Sound_PlayMap(0x38);
                        }
                    }
                    if (player_previous.unk1AC >= 66.666664f) {
                        func_001B92C0(player_previous.unk1AC);
                        if (!((s32)(player_state.unk1C8 >> 38) & 1) && player_state.unk1E4 != 4) {
                            player_state.unk1C8 |= PLAYER_FLAG_37;
                            player_state.unk1F4 = player_previous.unk1AC * 0.5f;
                            if (player_state.unk1F4 > 150.0f) {
                                player_state.unk1F4 = 150.0f;
                            }
                            player_state.unk1F0 = 0.0f;
                            player_state.unk1FC = 0;
                            player_state.unk1F8 = (s32)(player_state.unk1F4 * 40.0f / 150.0f);
                        }
                    }
                }
            }
            if (((s32)(player_state.unk1C8 >> 34) & 1) && !((s32)(player_previous.unk1C8 >> 34) & 1)) {
                player_state.unk1D8 = player_previous.position.y;
            }
        }
        if (player_state.map_index == 0xFFFF) {
            player_state.map_index = 0;
            player_state.unk250 = 0;
        }
    }

    if ((s32)(player_state.unk1C8 >> 37) & 1) {
        player_state.unk1F0 = (f32)sin((f32)player_state.unk1FC * 3.1415927f / (f32)player_state.unk1F8) * player_state.unk1F4;
        if (player_state.unk1FC++ >= player_state.unk1F8) {
            player_state.unk1C8 &= ~PLAYER_FLAG_37;
            player_state.unk1F0 = 0.0f;
            player_state.unk1F4 = 0.0f;
            player_state.unk1FC = 0;
            player_state.unk1F8 = 0;
        }
    }

    /* the push of the floor */
    if (game_status.unk00 != 0 && !(game_status.unk30 & 1)) {
        if (player_state.map_index != 0xFFFF && player_state.map_index < map_slots[D_00446770].header->record_count) {
            record = &((MapRecord *)map_slots[D_00446770].records)[player_state.map_index];
        } else {
            record = NULL;
        }
        speed = sqrtf(player_state.unk240.x * player_state.unk240.x + player_state.unk240.y * player_state.unk240.y
                      + player_state.unk240.z * player_state.unk240.z);
        if (record != NULL && record->unk2E != 0 && record->unk2C != 0) {
            velocity = &player_state.unk240;
            if (speed < (f32)record->unk2C) {
                delta.x = sinf(record->unk1C) * (f32)record->unk2E;
                delta.y = 0.0f;
                delta.z = cosf(record->unk1C) * (f32)record->unk2E;
                delta.w = 0.0f;
                Vector_Add(velocity, velocity, &delta);
                speed = sqrtf(player_state.unk240.x * player_state.unk240.x + player_state.unk240.y * player_state.unk240.y
                              + player_state.unk240.z * player_state.unk240.z);
                if (speed > (f32)record->unk2C) {
                    Vector_Normalize(velocity, velocity);
                    Vector_Scale((f32 *)velocity, (f32 *)velocity, (f32)record->unk2C);
                }
            } else {
                Vector_Normalize(&delta, velocity);
                Vector_Scale((f32 *)&delta, (f32 *)&delta, -speed_loss);
                Vector_Add(velocity, velocity, &delta);
                speed = sqrtf(player_state.unk240.x * player_state.unk240.x + player_state.unk240.y * player_state.unk240.y
                              + player_state.unk240.z * player_state.unk240.z);
                if (speed > (f32)record->unk2C) {
                    Vector_Normalize(velocity, velocity);
                    Vector_Scale((f32 *)velocity, (f32 *)velocity, (f32)record->unk2C);
                }
            }
        } else if (speed > speed_loss) {
            Vector_Normalize(&delta, &player_state.unk240);
            Vector_Scale((f32 *)&delta, (f32 *)&delta, -speed_loss);
            Vector_Add(&player_state.unk240, &player_state.unk240, &delta);
        } else {
            player_state.unk240.x = player_state.unk240.y = player_state.unk240.z = 0.0f;
        }
    }

    if (player_state.unk1A8 == 0 && player_state.unk1AC > 1000.0f) {
        player_state.hp_now = 0;
        player_state.unk1A4 = 0;
        func_001B9610();
    }
}

/* What the ground under the player does to him: death, poison (by chance or at once), damage, or experience. */
void func_001B9078(void) {
    if (player_state.unk1EC != 0) {
        player_state.hp_now = 0;
        player_state.unk1A4 = 0;
        if (player_state.unk1A8 == 0) {
            func_001B9610();
        }
    }
    if (player_state.unk1E0 == 0) {
        return;
    }
    switch (player_state.unk1E4) {
    case 1:
        if ((u16)player_state.timed_flags[0] == 0 && player_state.flag_timers[0] == 0) {
            if (player_state.unk1E8 % 60 == 0) {
                // match: the chance (at least 5 in 100) is worked out after the roll, inside the comparison
                if ((int)(rand() * 100.0f * 4.656613e-10f) < (20 - player_state.unkDC[0] > 4 ? 20 - player_state.unkDC[0] : 5)) {
                    player_state.timed_flags[0] = 1800;
                    player_state.unk2C4 = D_00446ED4;
                }
            }
        }
        break;
    case 2:
        if ((u16)player_state.timed_flags[0] == 0 && player_state.flag_timers[0] == 0) {
            if (player_state.unk1E8 % 60 == 0) {
                // match: the chance (at least 5 in 100) is worked out after the roll, inside the comparison
                if ((int)(rand() * 100.0f * 4.656613e-10f) < (50 - player_state.unkDC[0] > 4 ? 50 - player_state.unkDC[0] : 5)) {
                    player_state.timed_flags[0] = 1800;
                    player_state.unk2C4 = D_00446ED4;
                }
            }
        }
        break;
    case 3:
        if ((u16)player_state.timed_flags[0] == 0) {
            player_state.timed_flags[0] = 1800;
            player_state.unk2C4 = D_00446ED4;
        }
        break;
    case 4:
        func_001BB2E8(10);
        break;
    case 5:
        func_001BB2E8(1);
        func_001B4B10(player_state.weapon, 1);
        func_001B4DF8(player_state.equipment[EQUIP_SHIELD], 1);
        func_001B4DF8(player_state.equipment[EQUIP_HELM], 1);
        func_001B4DF8(player_state.equipment[EQUIP_ARMOR], 1);
        func_001B4DF8(player_state.equipment[EQUIP_GAUNTLET], 1);
        func_001B4DF8(player_state.equipment[EQUIP_BOOT], 1);
        break;
    }
}

void func_001B92C0(float speed) {
    int amount;

    if (speed < 66.666664f) {
        return;
    }
    amount = speed * speed / (1800.0f / (speed / 6.6666665f - 9.0f));
    if (player_state.unk204 != 0) {
        amount = amount * 60 / 100;
    } else if ((player_state.unk1C8 >> 38 & 1) == 1) {
        if (!(player_status_bits[0] >> 38 & 1)) {
            amount = amount * 80 / 100;
        }
    }
    if (amount > 0) {
        func_001BB260(amount);
        if (player_state.equipment[EQUIP_BOOT] != ITEM_WORN_INDEX(ITEM_ABSORPTION_BOOTS)) {
            player_state.unk200 = 60;
        }
    }
}

/* Moves both player records by offset (the other map slot became the current one): their positions and the
   heights kept with them. */
void func_001B93B8(f32 *offset) {
    Vector_Add(&player_state.position, &player_state.position, offset);
    Vector_Add(&player_previous.position, &player_previous.position, offset);
    player_state.unk1D8 += offset[1];
    player_state.unk1DC += offset[1];
    player_state.unk214 += offset[1];
    player_previous.unk1D8 += offset[1];
    player_previous.unk1DC += offset[1];
    player_previous.unk214 += offset[1];
}

void func_001B9478(void) {
    player_state.unk1C4 = 0.0f;
    player_state.unk1B0 = 0.0f;
    player_state.unk1B4 = 0.0f;
    player_state.unk1B8 = 0.0f;
    player_state.unk1BC = 0.0f;
    player_state.unk1C0 = 0;
    player_state.unk1C8 &= ~0x100000000L;
}

void func_001B94C0(void) {
    PlayerState *player;
    u64 was_set;
    int start_full;
    u64 is_set;
    int full;
    int filled;

    player = &player_state;
    was_set = (player_status_bits[0] >> 39) & 1;
    is_set = (player->unk1C8 >> 39) & 1;
    if (is_set != was_set) {
        if (is_set != 0) {
            start_full = (player->power * 9) + 0x78;
            player->unkCC = player->unkD0 = start_full;
        } else {
            player->unkD0 = player->unkCC = 0;
        }
    } else if (is_set != 0) {
        filled = player->unkCC;
        full = (player->power * 9) + 0x78;
        player->unkD0 = full;
        if (full < filled) {
            player->unkCC = full;
            filled = full;
        }
        if (filled > 0) {
            if ((player->equipment[EQUIP_AMULET] != 0x9A) || ((D_00446ED4 & 1) != 0)) {
                player->unkCC = filled - 1;
            }
        } else {
            func_001BB308(1);
        }
    }
}

/* Counts six countdown timers down to zero, clearing each matching flag on the way. */
void func_001B9588(void) {
    PlayerState *player = &player_state;

    if (player->flag_timers[0] != 0) {
        player->timed_flags[0] = 0;
        player->flag_timers[0]--;
    }
    if (player->flag_timers[1] != 0) {
        player->timed_flags[1] = 0;
        player->flag_timers[1]--;
    }
    if (player->flag_timers[2] != 0) {
        player->timed_flags[2] = 0;
        player->flag_timers[2]--;
    }
    if (player->flag_timers[3] != 0) {
        player->timed_flags[3] = 0;
        player->flag_timers[3]--;
    }
    if (player->flag_timers[4] != 0) {
        player->timed_flags[4] = 0;
        player->flag_timers[4]--;
    }
    if (player->flag_timers[5] != 0) {
        player->timed_flags[5] = 0;
        player->flag_timers[5]--;
    }
}

void func_001B9610(void)
{
    PlayerState *player = &player_state;

    if (player->unk1A8 == 0) {
        if (player->unk1A4 > 0) {
            return;
        }
        if (player->hp_now > 0) {
            return;
        }
        player->unk1A8 = 1;
        Sound_PlayCommon(0x28);
    }
    if (player->unk1A8 < 0x3C) {
        player->rotation.x += (1.012291f - player->rotation.x) / (0x3D - player->unk1A8);
        player->unk1A8++;
    } else {
        game_status.bit9 = 1;
    }
}

/* TRUE once every `period` frames */
static inline s32 is_frame_multiple(u32 period) {
    return D_00446ED4 % period == 0;
}

/* The per-frame bookkeeping of the player: timers, the two gauges, and the drawn bars. */
void func_001B96C8(void) {
    GameDefinition *definition = &D_00416D78[player_state.weapon];
    PlayerItemDef *item_def = &weapon_defs[player_state.weapon];
    FxTableEntry *fx_entry = &fx_table[player_state.unk264];
    s32 wear;
    s32 draining;
    s32 bar_target;
    s32 bar_step;

    if (player_state.unk1A8 == 0 && player_state.unk2B4 != 0 && D_00446ED4 % player_state.unk2B4 == 0) {
        if (player_state.unk2B0 > 0) {
            if (player_state.hp_now > 0) {
                player_state.hp_now++;
            }
            player_state.unk2B0--;
        } else {
            player_state.unk2B0++;
            player_state.hp_now--;
        }
        if (player_state.unk2B0 == 0) {
            player_state.unk2B4 = 0;
        }
    }

    RUN_DOWN_PAIR(0, unk158, unk188, unk11C);
    RUN_DOWN_PAIR(1, unk158, unk188, unk11C);
    RUN_DOWN_PAIR(2, unk158, unk188, unk11C);
    RUN_DOWN_PAIR(0, unk15E, unk18E, unk122);
    RUN_DOWN_PAIR(1, unk15E, unk18E, unk122);
    RUN_DOWN_PAIR(2, unk15E, unk18E, unk122);
    RUN_DOWN_PAIR(3, unk15E, unk18E, unk122);
    RUN_DOWN_PAIR(4, unk15E, unk18E, unk122);
    RUN_DOWN_PAIR(5, unk15E, unk18E, unk122);
    RUN_DOWN(0, unk14C, unk110);
    RUN_DOWN(1, unk14C, unk110);
    RUN_DOWN(2, unk14C, unk110);
    RUN_DOWN(3, unk14C, unk110);
    RUN_DOWN(4, unk14C, unk110);
    RUN_DOWN(5, unk14C, unk110);
    RUN_DOWN_SIGNED(0, unk176, unk13A);
    RUN_DOWN_SIGNED(1, unk176, unk13A);
    RUN_DOWN_SIGNED(2, unk176, unk13A);
    RUN_DOWN_SIGNED(0, unk17C, unk140);
    RUN_DOWN_SIGNED(1, unk17C, unk140);
    RUN_DOWN_SIGNED(2, unk17C, unk140);
    RUN_DOWN_SIGNED(3, unk17C, unk140);
    RUN_DOWN_SIGNED(4, unk17C, unk140);
    RUN_DOWN_SIGNED(5, unk17C, unk140);
    RUN_DOWN(0, unk16A, unk12E);
    RUN_DOWN(1, unk16A, unk12E);
    RUN_DOWN(2, unk16A, unk12E);
    RUN_DOWN(3, unk16A, unk12E);
    RUN_DOWN(4, unk16A, unk12E);
    RUN_DOWN(5, unk16A, unk12E);

    draining = player_state.unkEC != 0;
    if (player_state.unkF0 != 0 && D_00446ED4 % player_state.unkF0 == 0 && player_state.unk1A8 == 0 && player_state.hp_now > 0) {
        player_state.hp_now++;
    }
    if (player_state.unkF4 != 0 && D_00446ED4 % player_state.unkF4 == 0 && player_state.unk1A8 == 0) {
        player_state.hp_now--;
    }
    if (player_state.unkF8 != 0 && D_00446ED4 % player_state.unkF8 == 0 && player_state.unk1A8 == 0) {
        player_state.mp_now++;
    }
    if (player_state.unkFC != 0 && D_00446ED4 % player_state.unkFC == 0 && player_state.unk1A8 == 0) {
        player_state.mp_now--;
    }
    if (player_state.power_seed_timer > 0) {
        player_state.power_seed_timer--;
    }
    if (player_state.widda_needle_timer > 0) {
        player_state.widda_needle_timer--;
    }

    wear = 1;
    if (is_frame_multiple(3)) {
        if (player_state.equipment[EQUIP_ARMOR] == ITEM_WORN_INDEX(ITEM_DEMON_ARMOR)) {
            wear = 0;
        }
        if (player_state.equipment[EQUIP_AMULET] == ITEM_WORN_INDEX(ITEM_PURITY_AMULET)) {
            wear++;
        }
    }
    if ((u16)player_state.timed_flags[0] != 0) {
        if (player_state.unk2DC <= 0 ? is_frame_multiple(30) : is_frame_multiple(90)) {
            func_001BB2E8(3);
        }
    }
    if ((u16)player_state.timed_flags[3] != 0) {
        draining = TRUE;
    }
    if (wear != 0) {
        WEAR_OFF(0);
        WEAR_OFF(3);
        WEAR_OFF(1);
        WEAR_OFF(2);
        WEAR_OFF(4);
        WEAR_OFF(5);
    }
    if (draining) {
        if (is_frame_multiple(30)) {
            func_001BB2E8(1);
            player_state.mp_now--;
        }
    }

    if (player_state.hp_now > player_state.hp_max) {
        player_state.hp_now = player_state.hp_max;
    } else if (player_state.hp_now < 0) {
        player_state.hp_now = 0;
    }
    if (player_state.mp_now > player_state.mp_max) {
        player_state.mp_now = player_state.mp_max;
    } else if (player_state.mp_now < 0) {
        player_state.mp_now = 0;
    }

    if (player_state.unk1A8 == 0 && player_state.unk258 == 0) {
        if ((s32)(player_state.unk1C8 >> 35) & 1) {
            player_state.unkBC -= 83;
            if (player_state.unkBC < 0) {
                player_state.unkBC = 0;
            }
        } else if (item_def->unk63 < 2) {
            if (player_state.unkC4 <= 0) {
                if (player_state.unkBC < GAUGE_FULL) {
                    s32 rate = player_state.power;

                    if (rate < 20) {
                        rate = 20;
                    }
                    player_state.unkBC += rate * 42 / (definition->unk33 + 1);
                    if (player_state.unkBC > GAUGE_FULL) {
                        player_state.unkBC = GAUGE_FULL;
                    }
                }
            }
            if (player_state.unkC4 > 0) {
                player_state.unkC4--;
            }
        } else {
            player_state.unkBC = 0;
        }
    }
    if (player_state.unk1A8 == 0 && player_state.unk264 != 0) {
        if ((s32)(player_state.unk1C8 >> 35) & 1) {
            player_state.unkC0 -= 83;
            if (player_state.unkC0 < 0) {
                player_state.unkC0 = 0;
            }
        } else {
            if (player_state.unkC8 <= 0) {
                if (player_state.unkC0 < GAUGE_FULL) {
                    s32 rate = player_state.magic;

                    if (rate < 20) {
                        rate = 20;
                    }
                    player_state.unkC0 += rate * 42 / (fx_entry->unkFE + 1);
                    if (player_state.unkC0 > GAUGE_FULL) {
                        player_state.unkC0 = GAUGE_FULL;
                    }
                }
            }
            if (player_state.unkC8 > 0) {
                player_state.unkC8--;
            }
        }
    }
    if (player_state.equipped_magic == 0 && item_def->unk42 == 0xFFFF
        && (item_def->unk52 == 0xFFFF || (weapon_level_records + player_state.weapon)->unk14 + 1 < 3) && func_001BF6E8() == 0) {
        player_state.unkC0 = 0;
        player_state.unk264 = 0;
    }
    if ((u16)player_state.timed_flags[5] != 0) {
        player_state.unkC0 = 0;
    }
    if (!((s32)(player_state.unk1C8 >> 37) & 1) && player_state.unk200 > 0) {
        player_state.unk200--;
    }

    if (player_state.unk1A8 == 0 && player_state.hp_now > 0) {
        bar_target = player_state.hp_now * 49 / player_state.hp_max;
        if (bar_target < player_state.unk1A4) {
            player_state.unk1A4 -= 2;
            if (player_state.unk1A4 < bar_target) {
                player_state.unk1A4 = bar_target;
            }
        } else if (player_state.unk1A4 < bar_target) {
            player_state.unk1A4++;
            if (bar_target < player_state.unk1A4) {
                player_state.unk1A4 = bar_target;
            }
        }
    } else {
        player_state.unk1A4 = 0;
    }

    if (player_state.mp_max < player_state.unk1A6) {
        player_state.unk1A6 = player_state.mp_max;
    }
    bar_step = player_state.mp_max / 64;
    if (bar_step <= 0) {
        bar_step = 1;
    }
    if (player_state.mp_now < player_state.unk1A6) {
        player_state.unk1A6 -= bar_step;
        if (player_state.unk1A6 < player_state.mp_now) {
            player_state.unk1A6 = player_state.mp_now;
        }
        if (player_state.unk1A6 < 0) {
            player_state.unk1A6 = 0;
        }
    } else if (player_state.unk1A6 < player_state.mp_now) {
        player_state.unk1A6 += bar_step;
        if (player_state.mp_now < player_state.unk1A6) {
            player_state.unk1A6 = player_state.mp_now;
        }
    }

    if (player_state.weapon == 0x24) {
        if (is_frame_multiple(600)) {
            if ((s32)(rand() * 100.0f * 4.656613e-10f) < 70) {
                player_state.timed_flags[0] = 1800;
                player_state.unk2C4 = D_00446ED4;
            }
        }
    }
}

/* Works the player's derived numbers out again from the base ones, what is worn and the magic in effect. */
void func_001BA310(void) {
    PlayerItemDef *item_def = &weapon_defs[player_state.weapon];
    LevelRecord *record = &weapon_level_records[player_state.weapon];
    s32 unused_third;
    s32 unused_fifth;
    s32 unused_fifteenth;

    if (player_state.equipped_magic != 0) {
        // match: three divisions whose results are never used; only their divide checks are left in retail,
        // so what was divided is not known
        unused_third = player_state.hp_base / 3;
        unused_fifth = player_state.hp_base / 5;
        unused_fifteenth = player_state.hp_base / 15;
    }
    player_state.hp_max = player_state.hp_base;
    player_state.mp_max = player_state.mp_base;
    player_state.power = player_state.power_base;
    player_state.magic = player_state.magic_base;
    func_001BAE88();
    player_state.unkA8 = player_state.power * 3 + 80;
    player_state.unkA8 = (f32)player_state.unkA8 * 2.2046;
    player_state.weight = item_name_defs[player_state.weapon + ITEM_WEAPON_FIRST].unk3E;
    player_state.weight += item_name_defs[player_state.equipment[EQUIP_SHIELD] + ITEM_WORN_FIRST].unk3E;
    player_state.weight += item_name_defs[player_state.equipment[EQUIP_HELM] + ITEM_WORN_FIRST].unk3E;
    player_state.weight += item_name_defs[player_state.equipment[EQUIP_ARMOR] + ITEM_WORN_FIRST].unk3E;
    player_state.weight += item_name_defs[player_state.equipment[EQUIP_GAUNTLET] + ITEM_WORN_FIRST].unk3E;
    player_state.weight += item_name_defs[player_state.equipment[EQUIP_BOOT] + ITEM_WORN_FIRST].unk3E;
    if (PLAYER_WEARS_ANCIENT_SET(&player_state)) {
        player_state.weight /= 3;
    }
    player_state.weight += item_name_defs[player_state.equipment[EQUIP_RING] + ITEM_WORN_FIRST].unk3E;
    player_state.weight += item_name_defs[player_state.equipment[EQUIP_BRACELET] + ITEM_WORN_FIRST].unk3E;
    player_state.weight += item_name_defs[player_state.equipment[EQUIP_AMULET] + ITEM_WORN_FIRST].unk3E;
    SET_SCALED(0);
    SET_SCALED(1);
    SET_SCALED(2);
    SET_SCALED(3);
    SET_SCALED(4);
    SET_SCALED(5);
    SET_SCALED(6);
    SET_SCALED(7);
    SET_SCALED(8);
    player_state.unk60.values[0] += player_state.unk11C[0];
    player_state.unk60.values[1] += player_state.unk11C[1];
    player_state.unk60.values[2] += player_state.unk11C[2];
    player_state.unk60.values[3] += player_state.unk122[0];
    player_state.unk60.values[4] += player_state.unk122[1];
    player_state.unk60.values[5] += player_state.unk122[2];
    player_state.unk60.values[6] += player_state.unk122[3];
    player_state.unk60.values[7] += player_state.unk122[4];
    player_state.unk60.values[8] += player_state.unk122[5];
    player_state.unk72 = item_def->unk36;
    player_state.unk72.values[0] += player_state.unk110[0];
    player_state.unk72.values[1] += player_state.unk110[1];
    player_state.unk72.values[2] += player_state.unk110[2];
    player_state.unk72.values[3] += player_state.unk110[3];
    player_state.unk72.values[4] += player_state.unk110[4];
    player_state.unk72.values[5] += player_state.unk110[5];
    memset(&player_state.unk7E, 0, sizeof(PlayerShotStats));
    func_001BB728((u16 *)player_state.unk7E.values, player_state.equipment[EQUIP_SHIELD]);
    func_001BB728((u16 *)player_state.unk7E.values, player_state.equipment[EQUIP_HELM]);
    func_001BB728((u16 *)player_state.unk7E.values, player_state.equipment[EQUIP_ARMOR]);
    func_001BB728((u16 *)player_state.unk7E.values, player_state.equipment[EQUIP_GAUNTLET]);
    func_001BB728((u16 *)player_state.unk7E.values, player_state.equipment[EQUIP_BOOT]);
    func_001BB728((u16 *)player_state.unk7E.values, player_state.equipment[EQUIP_RING]);
    func_001BB728((u16 *)player_state.unk7E.values, player_state.equipment[EQUIP_BRACELET]);
    func_001BB728((u16 *)player_state.unk7E.values, player_state.equipment[EQUIP_AMULET]);
    memset(&player_state.unk90, 0, sizeof(PlayerShotStats));
    func_001BB728((u16 *)player_state.unk90.values, player_state.equipment[EQUIP_SHIELD]);
    func_001BB728((u16 *)player_state.unk90.values, player_state.equipment[EQUIP_HELM]);
    func_001BB728((u16 *)player_state.unk90.values, player_state.equipment[EQUIP_ARMOR]);
    func_001BB728((u16 *)player_state.unk90.values, player_state.equipment[EQUIP_GAUNTLET]);
    func_001BB728((u16 *)player_state.unk90.values, player_state.equipment[EQUIP_BOOT]);
    func_001BB728((u16 *)player_state.unk90.values, player_state.equipment[EQUIP_RING]);
    func_001BB728((u16 *)player_state.unk90.values, player_state.equipment[EQUIP_BRACELET]);
    func_001BB728((u16 *)player_state.unk90.values, player_state.equipment[EQUIP_AMULET]);
    player_state.unk90.values[0] += player_state.unk13A[0];
    player_state.unk90.values[1] += player_state.unk13A[1];
    player_state.unk90.values[2] += player_state.unk13A[2];
    player_state.unk90.values[3] += player_state.unk140[0];
    player_state.unk90.values[4] += player_state.unk140[1];
    player_state.unk90.values[5] += player_state.unk140[2];
    player_state.unk90.values[6] += player_state.unk140[3];
    player_state.unk90.values[7] += player_state.unk140[4];
    player_state.unk90.values[8] += player_state.unk140[5];
    player_state.unkDC[0] += player_state.unk12E[0];
    player_state.unkDC[1] += player_state.unk12E[1];
    player_state.unkDC[2] += player_state.unk12E[2];
    player_state.unkDC[3] += player_state.unk12E[3];
    player_state.unkDC[4] += player_state.unk12E[4];
    player_state.unkDC[5] += player_state.unk12E[5];
    if (player_state.unkEC != 0 || (u16)player_state.timed_flags[3] != 0) { // match: read unsigned here
        player_state.power /= 2;
        player_state.magic /= 2;
    }
    if (player_state.unk2EC > 0) {
        player_state.power += player_state.unk2F0;
    }
    if (player_state.power_seed_timer != 0) {
        player_state.power += player_state.power / 10;
    }
    if (player_state.widda_needle_timer != 0) {
        player_state.magic += player_state.magic / 10;
    }
    if (player_state.unk2EC > 0) {
        player_state.magic = 1;
    }
    if (player_state.equipment[EQUIP_GAUNTLET] == ITEM_WORN_INDEX(ITEM_SACRED_GLOVES)) {
        s32 item_weight = item_name_defs[player_state.weapon + ITEM_WEAPON_FIRST].unk3E;

        player_state.weight -= item_weight / 4;
    }
    if (player_state.equipment[EQUIP_GAUNTLET] == ITEM_WORN_INDEX(ITEM_BONE_HANDS)) {
        player_state.weight += item_name_defs[player_state.weapon + ITEM_WEAPON_FIRST].unk3E;
    }
    if (player_state.equipment[EQUIP_GAUNTLET] == ITEM_WORN_INDEX(ITEM_LORD_MYUS_GLOVES)) {
        s32 item_weight = item_name_defs[player_state.weapon + ITEM_WEAPON_FIRST].unk3E;

        player_state.weight -= item_weight / 4;
    }
    if (player_state.equipment[EQUIP_GAUNTLET] == ITEM_WORN_INDEX(ITEM_SILVERY_GLOVES)) {
        player_state.unk60.values[0] += player_state.unk60.values[0] / 10;
        player_state.unk60.values[1] += player_state.unk60.values[1] / 10;
        player_state.unk60.values[2] += player_state.unk60.values[2] / 10;
    }
    if (player_state.equipment[EQUIP_GAUNTLET] == ITEM_WORN_INDEX(ITEM_VENOM_GLOVES)) {
        player_state.unk72.values[0] += 70;
    }
    if (player_state.equipment[EQUIP_RING] == ITEM_WORN_INDEX(ITEM_WIDDAS_RING)) {
        player_state.hp_max += player_state.hp_max / 5;
    }
    if (player_state.equipment[EQUIP_RING] == ITEM_WORN_INDEX(ITEM_RING_OF_FIRE)) {
        player_state.unk60.values[3] += 20;
    }
    if (player_state.equipment[EQUIP_RING] == ITEM_WORN_INDEX(ITEM_RING_OF_EARTH)) {
        player_state.unk60.values[4] += 20;
    }
    if (player_state.equipment[EQUIP_RING] == ITEM_WORN_INDEX(ITEM_RING_OF_WIND)) {
        player_state.unk60.values[5] += 20;
    }
    if (player_state.equipment[EQUIP_RING] == ITEM_WORN_INDEX(ITEM_RING_OF_WATER)) {
        player_state.unk60.values[6] += 20;
    }
    if (player_state.equipment[EQUIP_BRACELET] == ITEM_WORN_INDEX(ITEM_POWER_BRACELET)) {
        player_state.unk60.values[0] += player_state.unk60.values[0] / 10;
        player_state.unk60.values[1] += player_state.unk60.values[1] / 10;
        player_state.unk60.values[2] += player_state.unk60.values[2] / 10;
    }
    if (player_state.equipment[EQUIP_BRACELET] == ITEM_WORN_INDEX(ITEM_FEATHER_BRACELET)) {
        player_state.weight = player_state.weight * 3 / 4;
    }
    if (player_state.equipment[EQUIP_HELM] == ITEM_WORN_INDEX(ITEM_ANGELIC_CROWN)) {
        player_state.mp_max += player_state.mp_max / 5;
    }
    if (player_state.equipment[EQUIP_BRACELET] == ITEM_WORN_INDEX(ITEM_CLARITY_BRACELET)) {
        player_state.mp_max *= 5;
    }
    if (player_state.hp_max >= 1000) {
        player_state.hp_max = 999;
    }
    if (player_state.mp_max >= 1000) {
        player_state.mp_max = 999;
    }
    if (player_state.unk72.values[0] > 100) {
        player_state.unk72.values[0] = 100;
    }
    if (player_state.unk72.values[1] > 100) {
        player_state.unk72.values[1] = 100;
    }
    if (player_state.unk72.values[2] > 100) {
        player_state.unk72.values[2] = 100;
    }
    if (player_state.unk72.values[3] > 100) {
        player_state.unk72.values[3] = 100;
    }
    if (player_state.unk72.values[4] > 100) {
        player_state.unk72.values[4] = 100;
    }
    if (player_state.unk72.values[5] > 100) {
        player_state.unk72.values[5] = 100;
    }
    if (player_state.unkDC[0] > 100) {
        player_state.unkDC[0] = 100;
    }
    if (player_state.unkDC[1] > 100) {
        player_state.unkDC[1] = 100;
    }
    if (player_state.unkDC[2] > 100) {
        player_state.unkDC[2] = 100;
    }
    if (player_state.unkDC[3] > 100) {
        player_state.unkDC[3] = 100;
    }
    if (player_state.unkDC[4] > 100) {
        player_state.unkDC[4] = 100;
    }
    if (player_state.unkDC[5] > 100) {
        player_state.unkDC[5] = 100;
    }
}
