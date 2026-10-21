#include "player/player_update.h"
#include "common.h"
#include "effect/effect.h"
#include "entity/entity.h"
#include "core/game.h"
#include "player/held_weapon.h"
#include "player/magic_ladder.h"
#include "map/map.h"
#include "model/model.h"
#include "player/player.h"
#include "map_object/map_object.h"
#include "effect/shot.h"
#include "sound/sound.h"
#include "math/vu0.h"
#include "core/file.h"
#include "gfx/gs_draw.h"
#include <libvu0.h>
#include <math.h>
#include "math/angle.h"
#include "model/motion.h"
#include "sound/sound_game.h"
#include <stdlib.h>
#include "save/save_state.h"
#include "enemy/enemy.h"
#include "input/pad.h"

static s32 swing_magic_frames;                    /* 0x447114 frames the swing-cast magic still runs */
static s32 swing_magic_id;                        /* 0x447118 magic id cast from the swing */
static PlayerShot *item_shot;                     /* 0x44711C shot for projectile and bow handlers */
static s32 bow_request_ring_index;                /* 0x447120 next slot in bow_shot_requests for bow shots */
static f32 movement_top_speed;                    /* 0x447124 top walk speed this frame (for footsteps) */

#define SHOT_REQUEST ((Unk1B1470Request *)item_shot->attack_request)

#define WEAPON_MODEL ((ModelInstance *)held_item_model_ptr)
#define WEAPON_AIM_MODEL ((struct PlayerWeaponModel *)held_item_model_ptr)
/* PlayerState.shortcut_type: what the shortcut button does (ini equip_shortcut_type) */
#define FALL_DAMAGE_MIN_SPEED 66.666664f
#define SCREEN_TINT_STEP_PER_FRAME (1.0f / 128.0f)

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
/* 0x413F50 the held item's action handler, Player_UpdateActions picks it with PlayerItemDef.action_handler_index */
void (*item_action_handlers[4])(u16, u16) = { Player_SwingWeapon, Player_SwingWeapon, Player_InvokeTargetHandler, 0 };
/* 0x413F60 the held item's step handler, picked with PlayerState.item_action_step; three of them ignore the buttons */
void (*item_step_handlers[8])(u16, u16) = {
    Player_FireProjectile, Player_DrawBow,
    Player_PlayEquipSound, Player_FinishWeaponMotion,
    Player_HolsterWeapon, 0, 0, 0,
};

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
#define SET_SCALED(n)     player_state.attack_stats.values[n] = (item_def->attack_by_level[record->level_index].values[n] + record->gained_values[n]) * record->level / 100

/* Sets the player up for a new game. */
void Player_ResetForNewGame(void) {
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
    player_state.power_gauge = 0;
    player_state.magic_gauge = 0;
    player_state.hp_bar_level = 0;
    player_state.mp_bar_level = 0;
    player_state.charging_magic = 0;
    player_state.rotation.x = 0.0f;
    player_state.record_flag_block = 0xFF;
    player_state.map_index = 0xFFFF;
    player_state.head_bob_offset = 0.0f;
    player_state.death_timer = 0;
    player_state.stride_phase = 0;
    Player_RecomputeStats();
    Player_UpdateStatus();
    player_previous = player_state;
}

/* Empties both gauges and the drawn HP and MP bars, then picks the magic the magic gauge charges for. */
void Player_ResetGauges(void) {
    player_state.power_gauge = 0;
    player_state.magic_gauge = 0;
    player_state.hp_bar_level = 0;
    player_state.mp_bar_level = 0;
    player_state.charging_magic = 0;
    ((int (*)(void))Player_PickChargingMagic)(); // match: called as if it returned something, which keeps the call from being a jump
}

/* Makes sure the player's map record index is a valid one. */
static inline void check_map_index(void) {
    PlayerState *player = &player_state;
    u32 map_index = player->map_index; // match: a wide local, narrowed where it is tested

    if (map_index >= map_slots[current_map_slot].header->record_count) {
        player->map_index = 0xFFFF;
        map_index = 0xFFFF;
        player->record_flag_block = 0xFF;
    }
    if ((u16)map_index == 0xFFFF) { // match: see map_index
        Player_FindMapRecord();
        if (player->map_index == 0xFFFF) {
            player->map_index = 0;
            player->record_flag_block = ((MapRecordInfo *)map_slots[current_map_slot].records)->flag_block_bit;
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
    if (game_status.player_control == 0 || (game_status.update_hold_flags & 1)) {
        Player_StopWalking();
    }
    held = pad_state.held;
    pressed = pad_state.pressed;
    player_previous = player_state;

    PLAYER_STATUS(&player_state)->running = FALSE;
    player_state.ground_effect_kind = GROUND_EFFECT_NONE;
    player_state.ground_kills = 0;
    player_state.ground_effect_active = 0;
    if (game_status.player_control != 0 && !(game_status.update_hold_flags & 1)) {
        PLAYER_STATUS(&player_state)->in_water = FALSE;
        PLAYER_STATUS(&player_state)->bit39 = FALSE;
        player_state.water_surface_y = player_no_water_surface_y[0];
        PLAYER_STATUS(&player_state)->bit34 = TRUE;
    } else {
        PLAYER_STATUS(&player_state)->bit34 = FALSE;
    }
    if (recent_damage_frames != 0) {
        recent_damage_frames--;
    }

    /* what the pad may do this frame */
    if (game_status.update_hold_flags & 1) {
        pressed = 0;
        held = 0;
    }
    if (game_status.player_control == 0) {
        pressed = 0;
        held = 0;
    }
    if (player_state.death_timer != 0) {
        pressed = 0;
        held = 0;
    }
    if (game_status.movement_input_enabled == 0) {
        held &= ~(forward_button | back_button | step_right_button | step_left_button);
        pressed &= ~(forward_button | back_button | step_right_button | step_left_button);
    }
    if ((u16)player_state.timed_flags[1] != 0) {
        mask = ~(attack_button | (forward_button | back_button | step_right_button | step_left_button | turn_right_button | turn_left_button |
                               look_down_button | look_up_button) | magic_button);
        held &= mask;
        pressed &= mask;
    }
    if (game_status.bit6 || game_status.item_use_cooldown > 0) {
        held &= ~(magic_button | action_button | menu_button | shortcut_button);
        pressed &= ~(magic_button | action_button | menu_button | shortcut_button);
    }

    /* the magic button: an event magic is handed to the game, any other is cast when it is charged and paid for */
    if (!((attack_button | magic_button | action_button | menu_button) & pressed)) {
        if ((shortcut_button & pressed) && player_state.shortcut != 0 && player_state.death_timer == 0 &&
            !(game_status.bit2 || game_status.bit3 || game_status.bit4)) {
            switch (player_state.shortcut_type) {
            case SHORTCUT_TYPE_NONE:
                break;
            case SHORTCUT_TYPE_ITEM:
                if (item_counts.bytes[player_state.shortcut] != 0) {
                    game_status.item_to_use = player_state.shortcut;
                    game_status.bit6 = TRUE;
                }
                break;
            case SHORTCUT_TYPE_MAGIC:
                if (player_state.magic_gauge >= 5000 && player_state.mp_now >= fx_table[player_state.shortcut].mp_cost) {
                    Player_CastMagic(player_state.shortcut);
                }
                break;
            }
        }
    }
    game_status.using_faced_object = FALSE;
    if (action_button & pressed) {
        game_status.using_faced_object = TRUE;
        if (game_status.bit6 || player_state.death_timer != 0) {
            game_status.using_faced_object = FALSE;
        }
    }

    /* running: the run button with forward or back, or with one of the two steps to the side */
    if ((action_button & held) &&
        (((forward_button | back_button) & held) || ((step_left_button & held) && !(step_right_button & held)) ||
         (!(step_left_button & held) && (step_right_button & held)))) {
        PLAYER_STATUS(&player_state)->running = TRUE;
        if (game_status.player_run_disabled != 0) {
            PLAYER_STATUS(&player_state)->running = FALSE;
        } else if (player_state.weight > player_state.max_weight) {
            PLAYER_STATUS(&player_state)->running = FALSE;
        } else if (item_def->action_handler_index == 2 && player_state.item_action_step != PLAYER_ITEM_ACTION_IDLE) {
            PLAYER_STATUS(&player_state)->running = FALSE;
        } else if ((u16)player_state.timed_flags[4] != 0) {
            PLAYER_STATUS(&player_state)->running = FALSE;
        } else if (player_state.movement_halved != 0) {
            PLAYER_STATUS(&player_state)->running = FALSE;
        } else if (player_state.landing_slow_timer != 0) {
            PLAYER_STATUS(&player_state)->running = FALSE;
        // match: unk3E is tested again with the rest of the set
        } else if (player_state.equipment[EQUIP_SHIELD] == ITEM_WORN_INDEX(ITEM_ANCIENT_SHIELD) &&
                   !PLAYER_WEARS_ANCIENT_SET(&player_state)) {
            PLAYER_STATUS(&player_state)->running = FALSE;
        }
    } else {
        PLAYER_STATUS(&player_state)->running = FALSE;
    }
    if ((menu_button & pressed) && !(action_button & pressed)) {
        game_status.bit2 = TRUE;
    }

    /* what was added to unk2A0 comes off unkB8, and the screen tint builds up while it does */
    if (player_state.pending_mp_drain != 0) {
        if (player_state.pending_mp_drain < player_state.mp_now) {
            player_state.mp_now -= player_state.pending_mp_drain;
        } else {
            player_state.mp_now = 0;
        }
        if (player_state.mp_drain_tint < 8) {
            player_state.mp_drain_tint++;
        }
        player_state.pending_mp_drain = 0;
    } else if (player_state.mp_drain_tint > 0) {
        player_state.mp_drain_tint--;
    }

    if (game_status.player_control != 0 && !(game_status.update_hold_flags & 1)) {
        Player_RecomputeStats();
        Player_UpdateStatus();
    }
    if (game_status.player_control != 0 && !(game_status.update_hold_flags & 1)) {
        Player_Move(held, pressed);
        Player_ApplyGroundEffects();
        Ladder_DispatchPad(held, pressed);
        Player_UpdateBreath();
        Player_UpdateAilmentImmunity();
        Player_UpdateDying();
    } else if (player_state.map_index == 0xFFFF || player_state.position.x != player_previous.position.x ||
               player_state.position.y != player_previous.position.y || player_state.position.z != player_previous.position.z) {
        Player_FindMapRecord();
    }
    check_map_index();

    /* the stride: unk1C0 runs from 0 to STRIDE_LENGTH, faster when running and slower in water */
    if (game_status.player_control != 0 && !(game_status.update_hold_flags & 1)) {
        stride_speed = 1.0f;
        if ((s32)(player_state.status_bits >> 35) & 1) {
            stride_speed = 1.6f;
        }
        if ((s32)(player_state.status_bits >> 39) & 1) {
            stride_speed *= 0.5f;
        } else if ((s32)(player_state.status_bits >> 38) & 1) {
            stride_speed *= 2.0f / 3.0f;
        }
        stride_step = stride_speed * 256.0f;
        if (player_state.strafe_speed != 0.0f || player_state.walk_speed != 0.0f) {
            player_state.stride_phase += stride_step;
            if (player_state.stride_phase >= STRIDE_LENGTH) {
                player_state.stride_phase %= STRIDE_LENGTH;
                PLAYER_STATUS(&player_state)->step_sounded = FALSE;
            }
        } else if (player_state.stride_phase != 0) {
            player_state.stride_phase += stride_step;
            if (player_state.stride_phase >= STRIDE_LENGTH) {
                player_state.stride_phase = 0;
                PLAYER_STATUS(&player_state)->step_sounded = FALSE;
            }
        }
    }
    /* head bob, when the option is on */
    if ((s8)game_options[5] != 0) { // match: retail reads the head bob option through game_options, as a signed byte
        player_state.head_bob_offset = sinf((f32)player_state.stride_phase * TWO_PI / 5120.0f) * 2.75f;
    } else {
        player_state.head_bob_offset = 0.0f;
    }

    /* one footstep sound per stride, three quarters of the way through it, by what the player walks on */
    if (game_status.player_control != 0 && !(game_status.update_hold_flags & 1) && player_state.death_timer == 0 &&
        PLAYER_STATUS(&player_state)->step_sounded == FALSE &&
        !(ABS(player_state.strafe_speed) < movement_top_speed * 0.5f && ABS(player_state.walk_speed) < movement_top_speed * 0.5f) &&
        !((s32)(player_state.status_bits >> 34) & 1) && player_state.stride_phase >= 0xF00) {
        sound_free = sound_load_queue.pending_map_slot == 0 && sound_load_queue.next_map_slot == 0;
        footstep_voice = -1;
        if ((s32)(player_state.status_bits >> 39) & 1) {
            PLAYER_STATUS(&player_state)->step_sounded = TRUE;
            footstep_voice = Sound_PlayFootstep(0x25);
        } else if ((s32)(player_state.status_bits >> 38) & 1) {
            PLAYER_STATUS(&player_state)->step_sounded = TRUE;
            footstep_voice = Sound_PlayFootstep(0x24);
        } else if (player_state.ground_effect_active != 0 && player_state.ground_effect_kind == GROUND_EFFECT_DAMAGE) {
            PLAYER_STATUS(&player_state)->step_sounded = TRUE;
            if (sound_free) {
                footstep_voice = Sound_PlayFootstep(0x27);
            }
        } else {
            PLAYER_STATUS(&player_state)->step_sounded = TRUE;
            switch (player_state.floor_material) {
            case 1:
                Sound_PlayFootstep(0x21);
                break;
            case 2:
                if (sound_free) {
                    footstep_voice = Sound_PlayFootstep(0x22);
                }
                break;
            case 3:
                if (sound_free) {
                    footstep_voice = Sound_PlayFootstep(0x23);
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
    if (player_state.death_timer == 0 && player_state.hp_now > 0 && player_state.hp_now * 100 / player_state.hp_max < 11 &&
        frame_counter % 27 == 0) {
        Sound_PlayCommon(0x29);
    }

    Player_UpdateActions(held, pressed);
    Player_UpdateScreenTints();
    if (player_state.ailment0_slow_timer > 0) {
        player_state.ailment0_slow_timer--;
    }
    if (player_state.light_spell_timer > 0) {
        player_state.light_spell_timer--;
    }
    if (player_state.intimidate_timer > 0) {
        player_state.intimidate_timer--;
    } else {
        player_state.intimidate_range = 0.0f;
    }
    if (player_state.rage_timer > 0) {
        player_state.rage_timer--;
    }

    /* item 0x32 worn: effect 0x7C starts at the player by chance, at most once in 30 frames, unless under water */
    if (player_state.equipment[EQUIP_HELM] == ITEM_WORN_INDEX(ITEM_SACRIFICIAL_MASK) && game_status.player_control != 0 && !(game_status.update_hold_flags & 1)) {
        if (sacrificial_mask_cooldown > 0) {
            sacrificial_mask_cooldown--;
        } else if (!(((s32)(player_state.status_bits >> 38) & 1) &&
                     player_state.water_surface_y < player_state.position.y - fx_table[0x7C].effect_depth_below) &&
                   (s32)((f32)rand() * 451.0f * RAND_SCALE) == 0) {
            shot = Shot_Alloc();
            shot->is_magic_shot = 1;
            shot->player_power = 0;
            shot->player_magic = 0;
            Attack_LaunchMagicEffect(0x7C, shot, &player_state, &player_state.rotation, &map_slots[current_map_slot]);
            sacrificial_mask_cooldown = 30;
        }
    }
    if (map_slots[current_map_slot].tag == 0x23 && guardians_room_water_purified[0] == 0 && ((s32)(player_state.status_bits >> 38) & 1) &&
        game_status.player_control != 0 && !(game_status.update_hold_flags & 1)) {
        Player_TakeDamageNoRumble(1);
    }
    Player_UpdateScreenFade();
    game_status.movement_input_enabled = 1;
}

/* player_screen_tint.kind: which timed tint is shown, by its colour */
#define SCREEN_TINT_GREEN       0
#define SCREEN_TINT_DARK_YELLOW 1
#define SCREEN_TINT_PURPLE      3
#define SCREEN_TINT_WHITE       4
#define SCREEN_TINT_CYCLING     5

/* Draws the colours that lie over the screen (PlayerScreenTint). */
void Player_DrawScreenTints(void) {
    if (player_screen_tint.green_tint_alpha != 0 && player_screen_tint.kind == SCREEN_TINT_GREEN) {
        GsDraw_FillScreen(0, 0xFF, 0, player_screen_tint.green_tint_alpha, 0x48);
    }
    if (player_screen_tint.dark_yellow_tint_alpha != 0 && player_screen_tint.kind == SCREEN_TINT_DARK_YELLOW) {
        GsDraw_FillScreen(0x80, 0x80, 0, player_screen_tint.dark_yellow_tint_alpha, 0x44);
    }
    if (player_screen_tint.purple_tint_alpha != 0 && player_screen_tint.kind == SCREEN_TINT_PURPLE) {
        GsDraw_FillScreen(0xA0, 0, 0xA0, player_screen_tint.purple_tint_alpha, 0x48);
    }
    if (player_screen_tint.white_tint_alpha != 0 && player_screen_tint.kind == SCREEN_TINT_WHITE) {
        GsDraw_FillScreen(0xFF, 0xFF, 0xFF, player_screen_tint.white_tint_alpha, 0x48);
    }
    if (player_screen_tint.cycling_tint_alpha != 0 && player_screen_tint.kind == SCREEN_TINT_CYCLING) {
        GsDraw_FillScreen(player_screen_tint.red, player_screen_tint.green, player_screen_tint.blue, player_screen_tint.cycling_tint_alpha, 0x48);
    }
    if (player_screen_tint.darkness_tint_alpha > 0) {
        GsDraw_FillScreen(0, 0, 0, player_screen_tint.darkness_tint_alpha, 0x44);
    }
    if (player_screen_tint.damage_flash_alpha != 0) {
        if (player_screen_tint.purple_tint_alpha != 0 && player_screen_tint.kind == SCREEN_TINT_PURPLE) {
            GsDraw_FillScreen(0x60, 0, 0, player_screen_tint.damage_flash_alpha, 0x48);
        } else {
            GsDraw_FillScreen(0xFF, 0, 0, player_screen_tint.damage_flash_alpha, 0x48);
        }
    }
    if (player_screen_tint.death_fade_tint_alpha != 0) {
        GsDraw_FillScreen(0, 0, 0, player_screen_tint.death_fade_tint_alpha, 0x44);
    }
}

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

    player_screen_tint.kind = SCREEN_TINT_GREEN;
    if (newest < (s32)player_state.ailment0_start_frame && (u16)player_state.timed_flags[0] != 0) {
        player_screen_tint.kind = SCREEN_TINT_GREEN;
        newest = player_state.ailment0_start_frame;
    }
    if (newest < (s32)player_state.ailment1_start_frame && (u16)player_state.timed_flags[1] != 0) {
        player_screen_tint.kind = SCREEN_TINT_DARK_YELLOW;
        newest = player_state.ailment1_start_frame;
    }
    if (newest < (s32)player_state.ailment2_start_frame && (u16)player_state.timed_flags[2] != 0) {
        darkened = TRUE;
    }
    if (newest < (s32)player_state.ailment3_start_frame && (u16)player_state.timed_flags[3] != 0) {
        player_screen_tint.kind = SCREEN_TINT_PURPLE;
        newest = player_state.ailment3_start_frame;
    }
    if (newest < (s32)player_state.ailment4_start_frame && (u16)player_state.timed_flags[4] != 0) {
        player_screen_tint.kind = SCREEN_TINT_WHITE;
        newest = player_state.ailment4_start_frame;
    }
    if (newest < (s32)player_state.ailment5_start_frame && (u16)player_state.timed_flags[5] != 0) {
        player_screen_tint.kind = SCREEN_TINT_CYCLING;
    }

    player_screen_tint.damage_flash_alpha = player_screen_tint.damage_flash_frames * 8;
    if (player_screen_tint.damage_flash_frames > 0) {
        if (player_state.aim_speed > 0.0f) {
            player_screen_tint.damage_flash_frames = 0;
        } else {
            player_screen_tint.damage_flash_frames--;
        }
    }

    if ((u16)player_state.timed_flags[0] != 0) {
        player_screen_tint.green_tint_alpha++;
        if (player_screen_tint.green_tint_alpha > 0x20) {
            player_screen_tint.green_tint_alpha = 0x20;
        }
    } else {
        player_screen_tint.green_tint_alpha--;
        if (player_screen_tint.green_tint_alpha < 0) {
            player_screen_tint.green_tint_alpha = 0;
        }
    }

    if ((u16)player_state.timed_flags[1] != 0) {
        player_screen_tint.dark_yellow_tint_alpha = yellow_tint_pulse[29 - (u16)player_state.timed_flags[1] % 30];
    } else {
        player_screen_tint.dark_yellow_tint_alpha = 0;
    }

    tick = frame_counter;
    if (!(frame_counter & 1)) {
        if ((u16)player_state.timed_flags[3] != 0) {
            player_screen_tint.purple_tint_alpha++;
            if (player_screen_tint.purple_tint_alpha > 0x20) {
                player_screen_tint.purple_tint_alpha = 0x20;
            }
        } else {
            player_screen_tint.purple_tint_alpha--;
            if (player_screen_tint.purple_tint_alpha < 0) {
                player_screen_tint.purple_tint_alpha = 0;
            }
        }
    }

    if ((u16)player_state.timed_flags[4] != 0) {
        player_screen_tint.white_tint_alpha++;
        if (player_screen_tint.white_tint_alpha > 0x20) {
            player_screen_tint.white_tint_alpha = 0x20;
        }
    } else {
        player_screen_tint.white_tint_alpha--;
        if (player_screen_tint.white_tint_alpha < 0) {
            player_screen_tint.white_tint_alpha = 0;
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
        if (!(frame_counter & 1)) {
            player_screen_tint.cycling_tint_alpha++;
            if (player_screen_tint.cycling_tint_alpha > 0x18) {
                player_screen_tint.cycling_tint_alpha = 0x18;
            }
        }
    } else if (!(frame_counter & 1)) {
        player_screen_tint.cycling_tint_alpha--;
        if (player_screen_tint.cycling_tint_alpha < 0) {
            player_screen_tint.cycling_tint_alpha = 0;
        }
    }

    if (player_state.light_spell_timer != 0 || player_state.ring_of_sight_active != 0) {
        map_draw_state.screen_tint_amount += SCREEN_TINT_STEP_PER_FRAME;
        if (map_draw_state.screen_tint_amount > 0.25f) {
            map_draw_state.screen_tint_amount = 0.25f;
        }
        player_screen_tint.magic_glow_strength++;
        if (player_screen_tint.magic_glow_strength > 0xA0) {
            player_screen_tint.magic_glow_strength = 0xA0;
        }
    } else {
        map_draw_state.screen_tint_amount -= SCREEN_TINT_STEP_PER_FRAME;
        if (map_draw_state.screen_tint_amount < 0.0f) {
            map_draw_state.screen_tint_amount = 0.0f;
        }
        player_screen_tint.magic_glow_strength--;
        if (player_screen_tint.magic_glow_strength < 0x80) {
            player_screen_tint.magic_glow_strength = 0x80;
        }
    }
    map_draw_state.screen_tint_amount_copy2 = map_draw_state.screen_tint_amount_copy = map_draw_state.screen_tint_amount;

    if (player_state.intimidate_timer != 0) {
        darkened = TRUE;
    }
    if (player_state.weapon == ITEM_WEAPON_INDEX(ITEM_ANCIENT_SWORD) &&
        !PLAYER_WEARS_ANCIENT_SET(&player_state)) {
        darkened = TRUE;
    } else if (player_state.equipment[EQUIP_HELM] == ITEM_WORN_INDEX(ITEM_SHADOW_MASK)) {
        darkened = TRUE;
    }
    if (darkened) {
        player_screen_tint.darkness_tint_alpha += 2;
        if (player_screen_tint.darkness_tint_alpha > 0x50) {
            player_screen_tint.darkness_tint_alpha = 0x50;
        }
    } else {
        player_screen_tint.darkness_tint_alpha -= 2;
        if (player_screen_tint.darkness_tint_alpha < 0) {
            player_screen_tint.darkness_tint_alpha = 0;
        }
    }

    if (player_state.death_timer == 0) {
        player_screen_tint.death_fade_tint_alpha = 0;
    } else {
        player_screen_tint.death_fade_tint_alpha = player_state.death_timer * 0x80 / 60;
    }
}

/* The player's actions for this frame: runs the handler of the item in hand, then the magic button. */
void Player_UpdateActions(u16 held, u16 pressed) {
    PlayerItemDef *def;

    player_state.cast_launched_shot = 0;
    def = &weapon_defs[player_state.weapon];
    player_state.magic1_asked = 0;
    if (player_state.item_action_step != PLAYER_ITEM_ACTION_IDLE) {
        if (game_status.player_control == 0 || (game_status.update_hold_flags & 1) || player_state.death_timer != 0 ||
            (u16)player_state.timed_flags[1] != 0) {
            player_state.swing_ended = 1;
            Effect_FreeSpawnersByParam(0x5D);
        }
    }
    item_action_handlers[def->action_handler_index](held, pressed);
    if (player_state.item_action_step == PLAYER_ITEM_ACTION_IDLE || (player_state.magic2_asked == 0 && player_state.magic1_asked == 0)) {
        if (magic_button & pressed) {
            Player_CastMagic(player_state.equipped_magic);
        }
        if (magic_button & held) {
            Player_HoldMagic(player_state.equipped_magic);
        }
    }
    if (player_state.item_action_step != PLAYER_ITEM_ACTION_IDLE) {
        player_state.frames_since_item_action = 0;
    } else {
        player_state.frames_since_item_action++;
    }
}

/* Handler of a weapon that is swung: the button starts the swing; while its motion runs the
   swing hits what it reaches, plays its sound and casts the weapon's two magics when they were
   asked for in time. */
void Player_SwingWeapon(u16 held, u16 pressed) {
    Unk1B1470Request request;
    LevelRecord *record = &weapon_level_records[player_state.weapon];
    PlayerItemDef *def = &weapon_defs[player_state.weapon];
    s32 level = record->level_index;
    PlayerShot *shot;

    if (player_state.item_action_step == PLAYER_ITEM_ACTION_IDLE) {
        if (def->first_magic_id != 0 && def->first_magic_id != 0xFFFF && player_state.power >= def->first_magic_min_power && player_state.magic >= def->first_magic_min_magic) {
            if (player_state.magic1_window != 0) {
                player_state.magic1_window--;
            }
            if (action_button & pressed) {
                player_state.magic1_window = 15;
            }
        } else {
            player_state.magic1_window = 0;
        }
        player_state.magic2_armed = level >= 2 && def->second_magic_id != 0 && def->second_magic_id != 0xFFFF;
        if (attack_button & pressed) {
            shot = &player_state.swing_shot;
            memset(shot, 0, sizeof(PlayerShot));
            shot->is_magic_shot = 0;
            shot->player_power = player_state.power;
            shot->player_magic = player_state.magic;
            shot->strength_gauge = player_state.power_gauge;
            shot->weapon_index = player_state.weapon;
            ModelMotion_Select(WEAPON_MODEL, 0);
            player_state.draw_sound_played = 0;
            player_state.magic1_motion_reached = FALSE;
            player_state.magic2_motion_reached = FALSE;
            player_state.item_action_step = PLAYER_ITEM_ACTION_SWING;
            player_state.power_gauge_delay = def->swing_gauge_delay_frames;
            swing_magic_frames = 0;
            player_state.power_gauge = 0;
            player_state.magic1_asked = 0;
            player_state.magic2_asked = 0;
            player_state.weapon_bounced = 0;
            player_state.struck_solid_object = 0;
            player_state.magic2_state = PLAYER_MAGIC_STATE_NONE;
            player_state.magic1_state = PLAYER_MAGIC_STATE_NONE;
            player_state.swing_ended = 0;
            WeaponTrail_PickKind();
        }
        return;
    }
    if (player_state.weapon_bounced == 0) {
        if (player_state.swing_ended != 0) {
            swing_magic_frames = 0;
        }
        if (swing_magic_frames <= 0) {
            ModelMotion_Step(WEAPON_MODEL);
        } else {
            swing_magic_frames--;
            if (((s32)(player_state.status_bits >> 39) & 1) ||
                (((s32)(player_state.status_bits >> 38) & 1) &&
                 player_state.position.y - fx_table[swing_magic_id].effect_depth_below > player_state.water_surface_y)) {
                Effect_FreeSpawnersByParam(swing_magic_id);
                swing_magic_frames = 0;
            }
        }
        if (MODEL_CHANNEL(WEAPON_MODEL)->frame_fixed <= 0) {
            player_state.item_action_step = PLAYER_ITEM_ACTION_IDLE;
            return;
        }
        if (player_state.swing_ended != 0) {
            return;
        }
        if (MODEL_CHANNEL(WEAPON_MODEL)->frame >= def->draw_ready_frame && MODEL_CHANNEL(WEAPON_MODEL)->frame <= def->draw_last_frame && swing_magic_frames == 0) {
            memset(&request, 0, sizeof(request));
            Vector_Copy(request.position, &player_state);
            request.owner = (Entity *)&player_state;
            request.shape = 3;
            request.position[1] -= 220.0f;
            request.radius = def->swing_reach + 100.0f;
            request.height = 230.0f;
            request.facing = player_state.rotation.y;
            request.half_angle = def->swing_yaw_arc_degrees * 0.5f * DEG_TO_RAD(1);
            request.pitch = player_state.rotation.x;
            request.half_pitch = def->swing_pitch_arc_degrees * 0.5f * DEG_TO_RAD(1);
            request.target_mask = 0x3E;
            *(PlayerShotStats *)&request.strengths = player_state.attack_stats;
            *(PlayerItemRates *)&request.ailment_chances = player_state.ailment_attack_rates;
            request.map_object_damage_multiplier = def->map_object_damage_multiplier;
            request.double_hit_chance_percent = def->double_hit_chance_percent;
            request.lowest_y = player_state.position.y - 230.0f - 50.0f;
            request.highest_y = player_state.position.y + 50.0f;
            Attack_Run(&request, (Unk1B1470Extra *)&player_state.swing_shot);
        }
        if (!player_state.draw_sound_played && def->draw_sound_id != 0xFFFF && MODEL_CHANNEL(WEAPON_MODEL)->frame >= def->draw_sound_frame) {
            ((int (*)(int))Sound_PlaySw)(def->draw_sound_id); // match: called as if it returned a value
            player_state.draw_sound_played = 1;
        }
        if (magic_button & pressed) {
            if (player_state.magic1_window != 0 && player_state.magic1_state == PLAYER_MAGIC_STATE_NONE && def->first_magic_id != 0xFFFF &&
                MODEL_CHANNEL(WEAPON_MODEL)->frame >= def->first_magic_window_start_frame && MODEL_CHANNEL(WEAPON_MODEL)->frame <= def->first_magic_window_end_frame) {
                player_state.magic2_armed = 0;
                player_state.magic1_asked = 1;
                player_state.magic1_state = PLAYER_MAGIC_STATE_ASKED;
            }
            if (player_state.magic2_armed != 0 && player_state.magic2_asked == 0 && def->second_magic_id != 0xFFFF &&
                MODEL_CHANNEL(WEAPON_MODEL)->frame >= def->second_magic_window_start_frame && MODEL_CHANNEL(WEAPON_MODEL)->frame <= def->second_magic_window_end_frame) {
                player_state.magic2_asked = 1;
                player_state.magic2_state = PLAYER_MAGIC_STATE_ASKED;
            }
            player_state.magic1_window = 0;
            player_state.magic2_armed = 0;
        }
        if (player_state.magic1_state == PLAYER_MAGIC_STATE_ASKED && MODEL_CHANNEL(WEAPON_MODEL)->frame >= def->first_magic_cast_frame) {
            player_state.magic1_state = PLAYER_MAGIC_STATE_CAST;
            if (Player_CastMagic(def->first_magic_id)) {
                swing_magic_id = def->first_magic_id;
                swing_magic_frames = def->first_magic_effect_frames;
            }
        }
        if (player_state.magic2_state == PLAYER_MAGIC_STATE_ASKED && MODEL_CHANNEL(WEAPON_MODEL)->frame >= def->second_magic_cast_frame) {
            player_state.magic2_state = PLAYER_MAGIC_STATE_CAST;
            if (Player_CastMagic(def->second_magic_id)) {
                swing_magic_id = def->second_magic_id;
                swing_magic_frames = def->second_magic_effect_frames;
            }
        }
        if (player_state.magic1_state != PLAYER_MAGIC_STATE_NONE && !player_state.magic1_motion_reached && def->first_magic_motion_id != PLAYER_MAGIC_MOTION_NONE &&
            MODEL_CHANNEL(WEAPON_MODEL)->frame >= def->first_magic_latch_frame) {
            player_state.magic1_motion_reached = TRUE;
        }
        if (player_state.magic2_state != PLAYER_MAGIC_STATE_NONE && !player_state.magic2_motion_reached && def->second_magic_motion_id != PLAYER_MAGIC_MOTION_NONE &&
            MODEL_CHANNEL(WEAPON_MODEL)->frame >= def->second_magic_latch_frame) {
            player_state.magic1_motion_reached = TRUE; // match: bit 1 again, though bit 2 was tested
        }
    } else {
        ModelMotion_Step(WEAPON_MODEL);
        if (MODEL_CHANNEL(WEAPON_MODEL)->blend_left == 0) {
            player_state.item_action_step = PLAYER_ITEM_ACTION_IDLE;
        }
    }
}

void Player_InvokeTargetHandler(u16 target_id, u16 command) {
    Player_UpdateWeaponAim();
    item_step_handlers[player_weapon_step[0]](target_id, command);
}

/* Handler of an item that shoots: on the button, starts the motion and launches the item's shot. */
void Player_FireProjectile(u16 held, u16 pressed) {
    PlayerItemDef *def = &weapon_defs[player_state.weapon];

    if ((attack_button & pressed) && !((s32)(player_state.status_bits >> 39) & 1) && item_counts.bytes[def->ammo_item_index] != 0) {
        ModelMotion_Select(WEAPON_MODEL, 0);
        MODEL_CHANNEL(WEAPON_MODEL)->frame = 0;
        MODEL_CHANNEL(WEAPON_MODEL)->frame_fixed = 0;
        ModelMotion_Step(WEAPON_MODEL);
        ((int (*)(ModelInstance *))WEAPON_MODEL->update_bone_points)(WEAPON_MODEL); // match: called as if it returned a value
        player_state.draw_sound_played = 0;
        player_state.draw_motion_complete = FALSE;
        player_state.item_action_step++;
        player_state.swing_ended = 0;
        item_shot = Shot_Alloc();
        item_shot->is_magic_shot = 0;
        item_shot->magic_id = def->second_magic_id;
        item_shot->player_power = player_state.power;
        item_shot->player_magic = player_state.magic;
        item_shot->strength_gauge = player_state.power_gauge;
        Attack_LaunchMagicEffect(def->second_magic_id, item_shot, weapon_tip_position, weapon_tip_angles, &player_state);
    }
}

static u8 bow_shot_requests_lead_pad[8] __attribute__((section(".bss")));  /* 0x57A348 the eight bytes before the requests */
static Unk1B1470Request bow_shot_requests[32];                     /* 0x57A350 the bow's requests, one per shot */

/* Handler of a drawn weapon while the button is held: the draw motion runs to its last frame and the
   strength builds up; letting go looses the shot, anything else lowers the weapon. */
void Player_DrawBow(u16 held, u16 pressed) {
    GameDefinition *item = &weapon_name_defs[player_state.weapon];
    PlayerItemDef *def = &weapon_defs[player_state.weapon];
    s32 rate;
    s32 frame;
    s32 speed;

    if (MODEL_CHANNEL(WEAPON_MODEL)->frame != 0 && MODEL_CHANNEL(WEAPON_MODEL)->frame < def->draw_last_frame) {
        ModelMotion_Step(WEAPON_MODEL);
    }
    if (player_state.draw_motion_complete == FALSE) {
        player_state.draw_motion_complete = MODEL_CHANNEL(WEAPON_MODEL)->frame >= def->draw_ready_frame;
    }
    if (player_state.swing_ended == 0 && !((s32)(player_state.status_bits >> 39) & 1) && Player_CanFireAtAim()) {
        if (!player_state.draw_sound_played && def->draw_sound_id != 0xFFFF && MODEL_CHANNEL(WEAPON_MODEL)->frame >= def->draw_sound_frame) {
            ((int (*)(int))Sound_PlaySw)(0); // match: called as if it returned a value
            player_state.draw_sound_played = 1;
        }
        if (attack_button & held) {
            rate = player_state.power;
            if (rate < 20) {
                rate = 20;
            }
            player_state.power_gauge += rate * 42 / (item->weight_tenths_of_kilogram + 1);
            if (player_state.power_gauge > 5000) {
                player_state.power_gauge = 5000;
            }
            return;
        }
        if (player_state.draw_motion_complete != FALSE) {
            if (player_state.draw_sound_played) {
                Sound_StopVoice(2);
            }
            item_shot->strength_gauge = player_state.power_gauge;
            item_shot->weapon_index = player_state.weapon;
            item_shot->attack_request = &bow_shot_requests[bow_request_ring_index];
            bow_request_ring_index = (bow_request_ring_index + 1) % 32;
            memset(item_shot->attack_request, 0, sizeof(Unk1B1470Request));
            *(PlayerShotStats *)&SHOT_REQUEST->strengths = player_state.attack_stats;
            *(PlayerItemRates *)&SHOT_REQUEST->ailment_chances = player_state.ailment_attack_rates;
            SHOT_REQUEST->map_object_damage_multiplier = def->map_object_damage_multiplier;
            SHOT_REQUEST->double_hit_chance_percent = def->double_hit_chance_percent;
            if (MODEL_CHANNEL(WEAPON_MODEL)->frame != 0) {
                frame = MODEL_CHANNEL(WEAPON_MODEL)->frame;
            } else {
                frame = def->draw_last_frame;
            }
            speed = (frame - def->draw_ready_frame) * 3 / (def->draw_last_frame - def->draw_ready_frame);
            if (speed <= 0) {
                speed = 1;
            }
            ModelMotion_BlendInto(WEAPON_MODEL, 1, speed);
            MODEL_CHANNEL(WEAPON_MODEL)->blend_speed = 0;
            player_state.item_action_step++;
            item_counts.bytes[def->ammo_item_index]--;
            return;
        }
    }
    ModelMotion_BlendInto(WEAPON_MODEL, 0, 0xF);
    MODEL_CHANNEL(WEAPON_MODEL)->blend_speed = 0;
    player_state.draw_motion_complete = FALSE;
    player_state.power_gauge = 0;
    player_state.item_action_step += 3;
}

void Player_PlayEquipSound(u16 held, u16 pressed) {
    Sound_PlaySw(1);
    player_state.power_gauge = 0;
    player_state.item_action_step++;
}

void Player_FinishWeaponMotion(u16 held, u16 pressed) {
    if (MODEL_CHANNEL(WEAPON_MODEL)->blend_left != 0) {
        ModelMotion_Step(WEAPON_MODEL);
        if (MODEL_CHANNEL(WEAPON_MODEL)->blend_left == 0) {
            ModelMotion_Step(WEAPON_MODEL);
        }
    } else if (MODEL_CHANNEL(WEAPON_MODEL)->frame_fixed > 0) {
        ModelMotion_Step(WEAPON_MODEL);
    } else {
        player_state.item_action_step = PLAYER_ITEM_ACTION_IDLE;
        player_state.draw_motion_complete = FALSE;
    }
}

void Player_HolsterWeapon(u16 held, u16 pressed) {
    ModelMotion_Step(WEAPON_MODEL);
    if (MODEL_CHANNEL(WEAPON_MODEL)->blend_left == 0) {
        Effect_FreePlayerWeaponKinds();
        player_weapon_step[0] = 0;
    }
}

int Player_CanFireAtAim(void) {
    float vec[4];
    int result;

    Vector_Scale(vec, view_ray_direction, 100.0f);
    Vector_Add(vec, vec, &player_state);
    vec[1] -= 186.0f;
    result = 0;
    if (vec[1] <= player_state.water_surface_y) {
        result = 1;
    }
    return result;
}

void Player_UpdateWeaponAim(void) {
    float vec[4];
    void *weapon_base;
    void *weapon_tip;

    WEAPON_AIM_MODEL->update_aim(held_item_model_ptr);
    weapon_base = WEAPON_AIM_MODEL->get_point(held_item_model_ptr, 0);
    weapon_tip = WEAPON_AIM_MODEL->get_point(held_item_model_ptr, 1);
    Vector_Subtract(vec, weapon_tip, weapon_base);
    Vector_Copy(weapon_tip_position, weapon_tip);
    weapon_tip_angles[0] = atan2f(-vec[1], sqrtf(vec[0] * vec[0] + vec[2] * vec[2]));
    weapon_tip_angles[1] = atan2f(vec[0], vec[2]);
}

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

    platform = player_previous.floor_map_object;
    if (platform != NULL && (platform->state_flags & MAP_OBJECT_IN_USE) && (MAP_OBJECT_DEF_FLAGS(platform->definition) & MAP_OBJECT_DEF_CARRIES)) {
        Vector_Subtract(&turn, &platform->model->rotation, &platform->last_rotation_x);
        turn.x = Angle_Wrap(turn.x);
        turn.y = Angle_Wrap(turn.y);
        turn.z = Angle_Wrap(turn.z);
        sceVu0RotMatrixZ(matrix, identity_matrix, turn.z);
        sceVu0RotMatrixX(matrix, matrix, turn.x);
        sceVu0RotMatrixY(matrix, matrix, turn.y);
        Vector_Subtract(&delta, &player_state, &platform->last_position_x);
        Vector_ApplyMatrix(&delta, matrix, &delta);
        Vector_Add(&player_state, &platform->model->position, &delta);
        player_state.rotation.y += platform->model->rotation.y - platform->last_rotation_y;
    }
    Vector_Add(&player_state, &player_state, &player_state.floor_push);

    if (player_previous.ground_effect_active != 0 && player_previous.ground_effect_kind == GROUND_EFFECT_DAMAGE) {
        row = 4;
    } else if ((s32)(player_previous.status_bits >> 39) & 1) {
        row = 3;
    } else if ((s32)(player_previous.status_bits >> 38) & 1) {
        row = 2;
    } else {
        row = player_previous.floor_material == 3;
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

    if (player_state.weight > player_state.max_weight) {
        ratio = (f32)(player_state.weight - player_state.max_weight) / (f32)player_state.max_weight;
        if (ratio < 1.0f) {
            top_speed -= top_speed * ratio;
            top_turn -= top_turn * ratio;
            top_look -= top_look * ratio;
        } else {
            top_look = top_turn = top_speed = 0.0f;
        }
    }
    if ((u16)player_state.timed_flags[4] != 0 || player_state.movement_halved != 0 || player_state.landing_slow_timer > 0) {
        top_speed *= 0.5f;
        top_turn *= 0.5f;
        top_look *= 0.5f;
        level_step *= 0.5f;
    }
    if ((s32)(player_state.status_bits >> 35) & 1) {
        top_speed *= 1.6f;
    }
    if ((s32)(player_state.status_bits >> 36) & 1) {
        buttons &= look_down_button | look_up_button;
    }

    if (player_state.aim_speed > 0.0f) {
        Vector_Scale((f32 *)&delta, (f32 *)&player_state.aim, player_state.aim_speed);
        Vector_Add(&player_state, &player_state, &delta);
        player_state.aim_speed -= ((f32)player_state.weight + 600.0f) / 600.0f * 20.0f;
        if (player_state.aim_speed < 0.0) {
            player_state.aim_speed = 0.0f;
        }
    }

    if (player_state.view_tilt_frames != 0) {
        player_state.view_tilt_frames--;
        player_state.view_tilt_pitch += DEG_TO_RAD(2);
        player_state.view_tilt_roll += DEG_TO_RAD(0.1f);
        if (player_state.view_tilt_pitch > DEG_TO_RAD(58)) {
            player_state.view_tilt_frames = 0;
        }
    } else {
        if (player_state.view_tilt_pitch > 0.0f) {
            player_state.view_tilt_pitch -= DEG_TO_RAD(0.5f);
            if (player_state.view_tilt_pitch < 0.0f) {
                player_state.view_tilt_pitch = 0.0f;
            }
        }
        if (player_state.view_tilt_roll > 0.0f) {
            player_state.view_tilt_roll -= DEG_TO_RAD(0.05f);
            if (player_state.view_tilt_roll < 0.0f) {
                player_state.view_tilt_roll = 0.0f;
            }
        }
    }

    /* looking up and down */
    if (!((s32)(player_state.status_bits >> 32) & 1)) {
        if (look_down_button & buttons) {
            if (look_up_button & buttons) {
                player_state.look_speed = 0.0f;
                player_state.status_bits |= PLAYER_FLAG_32;
            } else {
                player_state.look_speed += look_gain;
                if (player_state.look_speed > top_look) {
                    player_state.look_speed = top_look;
                }
            }
        } else if (look_up_button & buttons) {
            player_state.look_speed -= look_gain;
            if (player_state.look_speed < -top_look) {
                player_state.look_speed = -top_look;
            }
        } else {
            if (player_state.look_speed > 0.0f) {
                player_state.look_speed -= turn_loss;
                if (player_state.look_speed < 0.0f) {
                    player_state.look_speed = 0.0f;
                }
            }
            if (player_state.look_speed < 0.0f) {
                player_state.look_speed += turn_loss;
                if (player_state.look_speed > 0.0f) {
                    player_state.look_speed = 0.0f;
                }
            }
        }
    } else if ((look_down_button & buttons) && (look_up_button & buttons)) {
        Angle_Approach(&player_state.rotation.x, 0.0f, level_step);
    } else {
        player_state.status_bits &= ~PLAYER_FLAG_32;
    }

    /* turning */
    if ((turn_left_button & buttons) && !(turn_right_button & buttons)) {
        player_state.turn_speed -= turn_gain;
        if (player_state.turn_speed < -top_turn) {
            player_state.turn_speed = -top_turn;
        }
    } else if (!(turn_left_button & buttons) && (turn_right_button & buttons)) {
        player_state.turn_speed += turn_gain;
        if (player_state.turn_speed > top_turn) {
            player_state.turn_speed = top_turn;
        }
    } else {
        if (player_state.turn_speed > 0.0f) {
            player_state.turn_speed -= turn_loss;
            if (player_state.look_speed < 0.0f) { // match: retail tests the looking speed here, not the turning speed
                player_state.turn_speed = 0.0f;
            }
        }
        if (player_state.turn_speed < 0.0f) {
            player_state.turn_speed += turn_loss;
            if (player_state.turn_speed > 0.0f) {
                player_state.turn_speed = 0.0f;
            }
        }
    }

    /* walking */
    movement_top_speed = top_speed;
    if ((forward_button & buttons) && !(back_button & buttons)) {
        if (player_state.walk_speed < top_speed) {
            player_state.walk_speed += speed_gain;
            if (player_state.walk_speed > top_speed) {
                player_state.walk_speed = top_speed;
            }
        } else if (player_state.walk_speed > top_speed) {
            player_state.walk_speed -= speed_loss;
            if (player_state.walk_speed < top_speed) {
                player_state.walk_speed = top_speed;
            }
        }
    } else if (!(forward_button & buttons) && (back_button & buttons)) {
        backward = -top_speed;
        if (player_state.walk_speed > backward) {
            player_state.walk_speed -= speed_gain;
            if (player_state.walk_speed < backward) {
                player_state.walk_speed = backward;
            }
        } else if (player_state.walk_speed < backward) {
            player_state.walk_speed += speed_loss;
            if (player_state.walk_speed > backward) {
                player_state.walk_speed = backward;
            }
        }
    } else {
        if (player_state.walk_speed > 0.0f) {
            player_state.walk_speed -= speed_loss;
            if (player_state.walk_speed < 0.0f) {
                player_state.walk_speed = 0.0f;
            }
        }
        if (player_state.walk_speed < 0.0f) {
            player_state.walk_speed += speed_loss;
            if (player_state.walk_speed > 0.0f) {
                player_state.walk_speed = 0.0f;
            }
        }
    }

    /* stepping sideways */
    if (!(step_left_button & buttons) && (step_right_button & buttons)) {
        if (player_state.strafe_speed < top_speed) {
            player_state.strafe_speed += speed_gain;
            if (player_state.strafe_speed > top_speed) {
                player_state.strafe_speed = top_speed;
            }
        } else if (player_state.strafe_speed > top_speed) {
            player_state.strafe_speed -= speed_loss;
            if (player_state.strafe_speed < top_speed) {
                player_state.strafe_speed = top_speed;
            }
        }
    } else if ((step_left_button & buttons) && !(step_right_button & buttons)) {
        backward = -top_speed;
        if (player_state.strafe_speed > backward) {
            player_state.strafe_speed -= speed_gain;
            if (player_state.strafe_speed < backward) {
                player_state.strafe_speed = backward;
            }
        } else if (player_state.strafe_speed < backward) {
            player_state.strafe_speed += speed_loss;
            if (player_state.strafe_speed > backward) {
                player_state.strafe_speed = backward;
            }
        }
    } else {
        if (player_state.strafe_speed > 0.0f) {
            player_state.strafe_speed -= speed_loss;
            if (player_state.strafe_speed < 0.0f) {
                player_state.strafe_speed = 0.0f;
            }
        }
        if (player_state.strafe_speed < 0.0f) {
            player_state.strafe_speed += speed_loss;
            if (player_state.strafe_speed > 0.0f) {
                player_state.strafe_speed = 0.0f;
            }
        }
    }

    /* falling */
    if (game_status.player_control != 0 && !(game_status.update_hold_flags & 1) && !((s32)(player_state.status_bits >> 36) & 1)) {
        if ((s32)(player_state.status_bits >> 33) & 1) {
            player_state.fall_speed = 0.0f;
        } else if (player_state.death_timer == 0) {
            player_state.fall_speed += gravity;
        }
    } else {
        player_state.fall_speed = 0.0f;
    }

    delta.y = player_state.turn_speed;
    delta.x = player_state.look_speed;
    if ((u16)player_state.timed_flags[4] != 0) {
        delta.x *= 0.5f;
        delta.y *= 0.5f;
    }
    player_state.rotation.y = Angle_Wrap(player_state.rotation.y + delta.y);
    player_state.rotation.x += delta.x;
    if (player_state.rotation.x > DEG_TO_RAD(58)) {
        player_state.rotation.x = DEG_TO_RAD(58);
    }
    if (player_state.rotation.x < -DEG_TO_RAD(58)) {
        player_state.rotation.x = -DEG_TO_RAD(58);
    }

    delta.z = player_state.walk_speed;
    delta.x = player_state.strafe_speed;
    delta.y = delta.w = 0.0f;
    angle = atan2f(ABS(player_state.walk_speed), ABS(player_state.strafe_speed));
    delta.x *= cosf(angle);
    delta.z *= sinf(angle);
    sceVu0RotMatrixY(matrix, identity_matrix, player_state.rotation.y);
    Vector_ApplyMatrix(&delta, matrix, &delta);
    player_state.position.x += delta.x;
    player_state.position.z += delta.z;

    if (game_status.player_control != 0 && !(game_status.update_hold_flags & 1)) {
        if (player_previous.floor_map_object == 0) {
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
                    Player_CollideWithEntities(0);
                    Player_CollideWithMapObjects(0);
                    Player_PushOutOfWalls();
                } while (--steps != 0);
                Vector_Add(&player_state, &player_state, &delta);
            }
        }
        Player_CollideWithEntities(1);
        if (!((s32)(player_state.status_bits >> 36) & 1)) {
            Player_CollideWithMap();
            Player_CollideWithMapObjects(1);
            if (player_state.floor_material == 6 && player_state.floor_map_object != 0 && player_state.floor_map_object->definition->kind == MAP_OBJECT_KIND_SYMBOL_OF_PASSAGE) {
                Player_FindFloorMaterial();
            }
            if (!((s32)(player_state.status_bits >> 34) & 1)) {
                if (((s32)(player_previous.status_bits >> 34) & 1) && player_state.death_timer == 0) {
                    if (player_state.position.y - player_state.airborne_start_y > 110.0f) {
                        if (!((s32)(player_state.status_bits >> 38) & 1)) {
                            Sound_PlayCommon(0x22);
                        } else if (!((s32)(player_previous.status_bits >> 38) & 1)) {
                            Sound_PlayMap(0x38);
                        }
                    }
                    if (player_previous.fall_speed >= FALL_DAMAGE_MIN_SPEED) {
                        Player_TakeFallDamage(player_previous.fall_speed);
                        if (!((s32)(player_state.status_bits >> 38) & 1) && player_state.ground_effect_kind != GROUND_EFFECT_DAMAGE) {
                            player_state.status_bits |= PLAYER_FLAG_37;
                            player_state.landing_dip_depth = player_previous.fall_speed * 0.5f;
                            if (player_state.landing_dip_depth > 150.0f) {
                                player_state.landing_dip_depth = 150.0f;
                            }
                            player_state.landing_dip_offset = 0.0f;
                            player_state.landing_dip_frame = 0;
                            player_state.landing_dip_frames = (s32)(player_state.landing_dip_depth * 40.0f / 150.0f);
                        }
                    }
                }
            }
            if (((s32)(player_state.status_bits >> 34) & 1) && !((s32)(player_previous.status_bits >> 34) & 1)) {
                player_state.airborne_start_y = player_previous.position.y;
            }
        }
        if (player_state.map_index == 0xFFFF) {
            player_state.map_index = 0;
            player_state.record_flag_block = 0;
        }
    }

    if ((s32)(player_state.status_bits >> 37) & 1) {
        player_state.landing_dip_offset = (f32)sin((f32)player_state.landing_dip_frame * PI / (f32)player_state.landing_dip_frames) * player_state.landing_dip_depth;
        if (player_state.landing_dip_frame++ >= player_state.landing_dip_frames) {
            player_state.status_bits &= ~PLAYER_FLAG_37;
            player_state.landing_dip_offset = 0.0f;
            player_state.landing_dip_depth = 0.0f;
            player_state.landing_dip_frame = 0;
            player_state.landing_dip_frames = 0;
        }
    }

    /* the push of the floor */
    if (game_status.player_control != 0 && !(game_status.update_hold_flags & 1)) {
        if (player_state.map_index != 0xFFFF && player_state.map_index < map_slots[current_map_slot].header->record_count) {
            record = &((MapRecord *)map_slots[current_map_slot].records)[player_state.map_index];
        } else {
            record = NULL;
        }
        speed = sqrtf(player_state.floor_push.x * player_state.floor_push.x + player_state.floor_push.y * player_state.floor_push.y
                      + player_state.floor_push.z * player_state.floor_push.z);
        if (record != NULL && record->push_acceleration != 0 && record->push_max_speed != 0) {
            velocity = &player_state.floor_push;
            if (speed < (f32)record->push_max_speed) {
                delta.x = sinf(record->push_heading) * (f32)record->push_acceleration;
                delta.y = 0.0f;
                delta.z = cosf(record->push_heading) * (f32)record->push_acceleration;
                delta.w = 0.0f;
                Vector_Add(velocity, velocity, &delta);
                speed = sqrtf(player_state.floor_push.x * player_state.floor_push.x + player_state.floor_push.y * player_state.floor_push.y
                              + player_state.floor_push.z * player_state.floor_push.z);
                if (speed > (f32)record->push_max_speed) {
                    Vector_Normalize(velocity, velocity);
                    Vector_Scale((f32 *)velocity, (f32 *)velocity, (f32)record->push_max_speed);
                }
            } else {
                Vector_Normalize(&delta, velocity);
                Vector_Scale((f32 *)&delta, (f32 *)&delta, -speed_loss);
                Vector_Add(velocity, velocity, &delta);
                speed = sqrtf(player_state.floor_push.x * player_state.floor_push.x + player_state.floor_push.y * player_state.floor_push.y
                              + player_state.floor_push.z * player_state.floor_push.z);
                if (speed > (f32)record->push_max_speed) {
                    Vector_Normalize(velocity, velocity);
                    Vector_Scale((f32 *)velocity, (f32 *)velocity, (f32)record->push_max_speed);
                }
            }
        } else if (speed > speed_loss) {
            Vector_Normalize(&delta, &player_state.floor_push);
            Vector_Scale((f32 *)&delta, (f32 *)&delta, -speed_loss);
            Vector_Add(&player_state.floor_push, &player_state.floor_push, &delta);
        } else {
            player_state.floor_push.x = player_state.floor_push.y = player_state.floor_push.z = 0.0f;
        }
    }

    if (player_state.death_timer == 0 && player_state.fall_speed > 1000.0f) {
        player_state.hp_now = 0;
        player_state.hp_bar_level = 0;
        Player_UpdateDying();
    }
}

/* What the ground under the player does to him: death, poison (by chance or at once), damage, or experience. */
void Player_ApplyGroundEffects(void) {
    if (player_state.ground_kills != 0) {
        player_state.hp_now = 0;
        player_state.hp_bar_level = 0;
        if (player_state.death_timer == 0) {
            Player_UpdateDying();
        }
    }
    if (player_state.ground_effect_active == 0) {
        return;
    }
    switch (player_state.ground_effect_kind) {
    case GROUND_EFFECT_POISON_LOW_CHANCE:
        if ((u16)player_state.timed_flags[0] == 0 && player_state.flag_timers[0] == 0) {
            if (player_state.ground_effect_frames % 60 == 0) {
                // match: the chance (at least 5 in 100) is worked out after the roll, inside the comparison
                if ((int)(rand() * 100.0f * RAND_SCALE) < (20 - player_state.ailment_defence[0] > 4 ? 20 - player_state.ailment_defence[0] : 5)) {
                    player_state.timed_flags[0] = 1800;
                    player_state.ailment0_start_frame = frame_counter;
                }
            }
        }
        break;
    case GROUND_EFFECT_POISON_HIGH_CHANCE:
        if ((u16)player_state.timed_flags[0] == 0 && player_state.flag_timers[0] == 0) {
            if (player_state.ground_effect_frames % 60 == 0) {
                // match: the chance (at least 5 in 100) is worked out after the roll, inside the comparison
                if ((int)(rand() * 100.0f * RAND_SCALE) < (50 - player_state.ailment_defence[0] > 4 ? 50 - player_state.ailment_defence[0] : 5)) {
                    player_state.timed_flags[0] = 1800;
                    player_state.ailment0_start_frame = frame_counter;
                }
            }
        }
        break;
    case GROUND_EFFECT_POISON_CERTAIN:
        if ((u16)player_state.timed_flags[0] == 0) {
            player_state.timed_flags[0] = 1800;
            player_state.ailment0_start_frame = frame_counter;
        }
        break;
    case GROUND_EFFECT_DAMAGE:
        Player_TakeDamageNoRumble(10);
        break;
    case GROUND_EFFECT_WEAR_DAMAGE:
        Player_TakeDamageNoRumble(1);
        Attack_WearWeapon(player_state.weapon, 1);
        Attack_WearWornItem(player_state.equipment[EQUIP_SHIELD], 1);
        Attack_WearWornItem(player_state.equipment[EQUIP_HELM], 1);
        Attack_WearWornItem(player_state.equipment[EQUIP_ARMOR], 1);
        Attack_WearWornItem(player_state.equipment[EQUIP_GAUNTLET], 1);
        Attack_WearWornItem(player_state.equipment[EQUIP_BOOT], 1);
        break;
    }
}

/* A landing at the fall speed given hurts from 66.67 up (less with unk204 set, or into water); a hurt that is
   not softened by the Absorption Boots slows the player for 60 frames (unk200). */
void Player_TakeFallDamage(float speed) {
    int amount;

    if (speed < FALL_DAMAGE_MIN_SPEED) {
        return;
    }
    amount = speed * speed / (1800.0f / (speed / 6.6666665f - 9.0f));
    if (player_state.stepped_on_entity != 0) {
        amount = amount * 60 / 100;
    } else if ((player_state.status_bits >> 38 & 1) == 1) {
        if (!(player_status_bits[0] >> 38 & 1)) {
            amount = amount * 80 / 100;
        }
    }
    if (amount > 0) {
        Player_TakeDamage(amount);
        if (player_state.equipment[EQUIP_BOOT] != ITEM_WORN_INDEX(ITEM_ABSORPTION_BOOTS)) {
            player_state.landing_slow_timer = 60;
        }
    }
}

/* Moves both player records by offset (the other map slot became the current one): their positions and the
   heights kept with them. */
void Player_MoveByOffset(f32 *offset) {
    Vector_Add(&player_state.position, &player_state.position, offset);
    Vector_Add(&player_previous.position, &player_previous.position, offset);
    player_state.airborne_start_y += offset[1];
    player_state.water_surface_y += offset[1];
    player_state.collision_base_y += offset[1];
    player_previous.airborne_start_y += offset[1];
    player_previous.water_surface_y += offset[1];
    player_previous.collision_base_y += offset[1];
}

/* Stops the player's walking, turning and looking speeds, the stride and the head bob. */
void Player_StopWalking(void) {
    player_state.head_bob_offset = 0.0f;
    player_state.strafe_speed = 0.0f;
    player_state.walk_speed = 0.0f;
    player_state.look_speed = 0.0f;
    player_state.turn_speed = 0.0f;
    player_state.stride_phase = 0;
    player_state.status_bits &= ~0x100000000L;
}

/* The breath bar under water (status bit 39, which Player_CollideWithMap sets while the water surface, strip
   list 5, is 220 or more above the feet): full (power * 9 + 120) on going under, one less a frame (every other
   frame with the Water Talisman), and one HP a frame once it is empty. */
void Player_UpdateBreath(void) {
    PlayerState *player;
    u64 was_set;
    int start_full;
    u64 is_set;
    int full;
    int filled;

    player = &player_state;
    was_set = (player_status_bits[0] >> 39) & 1;
    is_set = (player->status_bits >> 39) & 1;
    if (is_set != was_set) {
        if (is_set != 0) {
            start_full = (player->power * 9) + 0x78;
            player->breath_now = player->breath_max = start_full;
        } else {
            player->breath_max = player->breath_now = 0;
        }
    } else if (is_set != 0) {
        filled = player->breath_now;
        full = (player->power * 9) + 0x78;
        player->breath_max = full;
        if (full < filled) {
            player->breath_now = full;
            filled = full;
        }
        if (filled > 0) {
            if ((player->equipment[EQUIP_AMULET] != 0x9A) || ((frame_counter & 1) != 0)) {
                player->breath_now = filled - 1;
            }
        } else {
            Player_LoseHp(1);
        }
    }
}

/* Counts the six flag_timers down; while one runs its ailment (timed_flags) is held at 0. */
void Player_UpdateAilmentImmunity(void) {
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

/* Dying: once HP and the drawn HP bar are empty, starts the 60 frames in which the view tips over (unk1A8 counts
   them, sound 0x28), then ends the main loop (GameStatus.bit9). */
void Player_UpdateDying(void)
{
    PlayerState *player = &player_state;

    if (player->death_timer == 0) {
        if (player->hp_bar_level > 0) {
            return;
        }
        if (player->hp_now > 0) {
            return;
        }
        player->death_timer = 1;
        Sound_PlayCommon(0x28);
    }
    if (player->death_timer < 0x3C) {
        player->rotation.x += (DEG_TO_RAD(58) - player->rotation.x) / (0x3D - player->death_timer);
        player->death_timer++;
    } else {
        game_status.bit9 = 1;
    }
}

/* TRUE once every `period` frames */
static inline s32 is_frame_multiple(u32 period) {
    return frame_counter % period == 0;
}

/* The per-frame bookkeeping of the player: timers, the two gauges, and the drawn bars. */
void Player_UpdateStatus(void) {
    GameDefinition *definition = &weapon_name_defs[player_state.weapon];
    PlayerItemDef *item_def = &weapon_defs[player_state.weapon];
    FxTableEntry *fx_entry = &fx_table[player_state.charging_magic];
    s32 wear;
    s32 draining;
    s32 bar_target;
    s32 bar_step;

    if (player_state.death_timer == 0 && player_state.hp_step_period != 0 && frame_counter % player_state.hp_step_period == 0) {
        if (player_state.hp_step_remaining > 0) {
            if (player_state.hp_now > 0) {
                player_state.hp_now++;
            }
            player_state.hp_step_remaining--;
        } else {
            player_state.hp_step_remaining++;
            player_state.hp_now--;
        }
        if (player_state.hp_step_remaining == 0) {
            player_state.hp_step_period = 0;
        }
    }

    RUN_DOWN_PAIR(0, physical_strength_bonus_timer, physical_bonus_stamp, physical_strength_bonus);
    RUN_DOWN_PAIR(1, physical_strength_bonus_timer, physical_bonus_stamp, physical_strength_bonus);
    RUN_DOWN_PAIR(2, physical_strength_bonus_timer, physical_bonus_stamp, physical_strength_bonus);
    RUN_DOWN_PAIR(0, magic_strength_bonus_timer, magic_bonus_stamp, magic_strength_bonus);
    RUN_DOWN_PAIR(1, magic_strength_bonus_timer, magic_bonus_stamp, magic_strength_bonus);
    RUN_DOWN_PAIR(2, magic_strength_bonus_timer, magic_bonus_stamp, magic_strength_bonus);
    RUN_DOWN_PAIR(3, magic_strength_bonus_timer, magic_bonus_stamp, magic_strength_bonus);
    RUN_DOWN_PAIR(4, magic_strength_bonus_timer, magic_bonus_stamp, magic_strength_bonus);
    RUN_DOWN_PAIR(5, magic_strength_bonus_timer, magic_bonus_stamp, magic_strength_bonus);
    RUN_DOWN(0, ailment_attack_bonus_timer, ailment_attack_bonus);
    RUN_DOWN(1, ailment_attack_bonus_timer, ailment_attack_bonus);
    RUN_DOWN(2, ailment_attack_bonus_timer, ailment_attack_bonus);
    RUN_DOWN(3, ailment_attack_bonus_timer, ailment_attack_bonus);
    RUN_DOWN(4, ailment_attack_bonus_timer, ailment_attack_bonus);
    RUN_DOWN(5, ailment_attack_bonus_timer, ailment_attack_bonus);
    RUN_DOWN_SIGNED(0, physical_defence_bonus_timer, physical_defence_bonus);
    RUN_DOWN_SIGNED(1, physical_defence_bonus_timer, physical_defence_bonus);
    RUN_DOWN_SIGNED(2, physical_defence_bonus_timer, physical_defence_bonus);
    RUN_DOWN_SIGNED(0, magic_defence_bonus_timer, magic_defence_bonus);
    RUN_DOWN_SIGNED(1, magic_defence_bonus_timer, magic_defence_bonus);
    RUN_DOWN_SIGNED(2, magic_defence_bonus_timer, magic_defence_bonus);
    RUN_DOWN_SIGNED(3, magic_defence_bonus_timer, magic_defence_bonus);
    RUN_DOWN_SIGNED(4, magic_defence_bonus_timer, magic_defence_bonus);
    RUN_DOWN_SIGNED(5, magic_defence_bonus_timer, magic_defence_bonus);
    RUN_DOWN(0, ailment_defence_bonus_timer, ailment_defence_bonus);
    RUN_DOWN(1, ailment_defence_bonus_timer, ailment_defence_bonus);
    RUN_DOWN(2, ailment_defence_bonus_timer, ailment_defence_bonus);
    RUN_DOWN(3, ailment_defence_bonus_timer, ailment_defence_bonus);
    RUN_DOWN(4, ailment_defence_bonus_timer, ailment_defence_bonus);
    RUN_DOWN(5, ailment_defence_bonus_timer, ailment_defence_bonus);

    draining = player_state.worn_drain_active != 0;
    if (player_state.hp_regen_period != 0 && frame_counter % player_state.hp_regen_period == 0 && player_state.death_timer == 0 && player_state.hp_now > 0) {
        player_state.hp_now++;
    }
    if (player_state.hp_drain_period != 0 && frame_counter % player_state.hp_drain_period == 0 && player_state.death_timer == 0) {
        player_state.hp_now--;
    }
    if (player_state.mp_regen_period != 0 && frame_counter % player_state.mp_regen_period == 0 && player_state.death_timer == 0) {
        player_state.mp_now++;
    }
    if (player_state.mp_drain_period != 0 && frame_counter % player_state.mp_drain_period == 0 && player_state.death_timer == 0) {
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
        if (player_state.ailment0_slow_timer <= 0 ? is_frame_multiple(30) : is_frame_multiple(90)) {
            Player_TakeDamageNoRumble(3);
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
            Player_TakeDamageNoRumble(1);
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

    if (player_state.death_timer == 0 && player_state.item_action_step == PLAYER_ITEM_ACTION_IDLE) {
        if ((s32)(player_state.status_bits >> 35) & 1) {
            player_state.power_gauge -= 83;
            if (player_state.power_gauge < 0) {
                player_state.power_gauge = 0;
            }
        } else if (item_def->action_handler_index < 2) {
            if (player_state.power_gauge_delay <= 0) {
                if (player_state.power_gauge < GAUGE_FULL) {
                    s32 rate = player_state.power;

                    if (rate < 20) {
                        rate = 20;
                    }
                    player_state.power_gauge += rate * 42 / (definition->weight_tenths_of_kilogram + 1);
                    if (player_state.power_gauge > GAUGE_FULL) {
                        player_state.power_gauge = GAUGE_FULL;
                    }
                }
            }
            if (player_state.power_gauge_delay > 0) {
                player_state.power_gauge_delay--;
            }
        } else {
            player_state.power_gauge = 0;
        }
    }
    if (player_state.death_timer == 0 && player_state.charging_magic != 0) {
        if ((s32)(player_state.status_bits >> 35) & 1) {
            player_state.magic_gauge -= 83;
            if (player_state.magic_gauge < 0) {
                player_state.magic_gauge = 0;
            }
        } else {
            if (player_state.magic_gauge_delay <= 0) {
                if (player_state.magic_gauge < GAUGE_FULL) {
                    s32 rate = player_state.magic;

                    if (rate < 20) {
                        rate = 20;
                    }
                    player_state.magic_gauge += rate * 42 / (fx_entry->gauge_charge_divisor + 1);
                    if (player_state.magic_gauge > GAUGE_FULL) {
                        player_state.magic_gauge = GAUGE_FULL;
                    }
                }
            }
            if (player_state.magic_gauge_delay > 0) {
                player_state.magic_gauge_delay--;
            }
        }
    }
    if (player_state.equipped_magic == 0 && item_def->first_magic_id == 0xFFFF
        && (item_def->second_magic_id == 0xFFFF || (weapon_level_records + player_state.weapon)->level_index + 1 < 3) && Player_KnowsItemMenuMagic() == 0) {
        player_state.magic_gauge = 0;
        player_state.charging_magic = 0;
    }
    if ((u16)player_state.timed_flags[5] != 0) {
        player_state.magic_gauge = 0;
    }
    if (!((s32)(player_state.status_bits >> 37) & 1) && player_state.landing_slow_timer > 0) {
        player_state.landing_slow_timer--;
    }

    if (player_state.death_timer == 0 && player_state.hp_now > 0) {
        bar_target = player_state.hp_now * 49 / player_state.hp_max;
        if (bar_target < player_state.hp_bar_level) {
            player_state.hp_bar_level -= 2;
            if (player_state.hp_bar_level < bar_target) {
                player_state.hp_bar_level = bar_target;
            }
        } else if (player_state.hp_bar_level < bar_target) {
            player_state.hp_bar_level++;
            if (bar_target < player_state.hp_bar_level) {
                player_state.hp_bar_level = bar_target;
            }
        }
    } else {
        player_state.hp_bar_level = 0;
    }

    if (player_state.mp_max < player_state.mp_bar_level) {
        player_state.mp_bar_level = player_state.mp_max;
    }
    bar_step = player_state.mp_max / 64;
    if (bar_step <= 0) {
        bar_step = 1;
    }
    if (player_state.mp_now < player_state.mp_bar_level) {
        player_state.mp_bar_level -= bar_step;
        if (player_state.mp_bar_level < player_state.mp_now) {
            player_state.mp_bar_level = player_state.mp_now;
        }
        if (player_state.mp_bar_level < 0) {
            player_state.mp_bar_level = 0;
        }
    } else if (player_state.mp_bar_level < player_state.mp_now) {
        player_state.mp_bar_level += bar_step;
        if (player_state.mp_now < player_state.mp_bar_level) {
            player_state.mp_bar_level = player_state.mp_now;
        }
    }

    if (player_state.weapon == 0x24) {
        if (is_frame_multiple(600)) {
            if ((s32)(rand() * 100.0f * RAND_SCALE) < 70) {
                player_state.timed_flags[0] = 1800;
                player_state.ailment0_start_frame = frame_counter;
            }
        }
    }
}

/* Works the player's derived numbers out again from the base ones, what is worn and the magic in effect. */
void Player_RecomputeStats(void) {
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
    Player_SumWornBonuses();
    player_state.max_weight = player_state.power * 3 + 80;
    player_state.max_weight = (f32)player_state.max_weight * KILOGRAMS_TO_POUNDS;
    player_state.weight = item_name_defs[player_state.weapon + ITEM_WEAPON_FIRST].weight_tenths_of_pound;
    player_state.weight += item_name_defs[player_state.equipment[EQUIP_SHIELD] + ITEM_WORN_FIRST].weight_tenths_of_pound;
    player_state.weight += item_name_defs[player_state.equipment[EQUIP_HELM] + ITEM_WORN_FIRST].weight_tenths_of_pound;
    player_state.weight += item_name_defs[player_state.equipment[EQUIP_ARMOR] + ITEM_WORN_FIRST].weight_tenths_of_pound;
    player_state.weight += item_name_defs[player_state.equipment[EQUIP_GAUNTLET] + ITEM_WORN_FIRST].weight_tenths_of_pound;
    player_state.weight += item_name_defs[player_state.equipment[EQUIP_BOOT] + ITEM_WORN_FIRST].weight_tenths_of_pound;
    if (PLAYER_WEARS_ANCIENT_SET(&player_state)) {
        player_state.weight /= 3;
    }
    player_state.weight += item_name_defs[player_state.equipment[EQUIP_RING] + ITEM_WORN_FIRST].weight_tenths_of_pound;
    player_state.weight += item_name_defs[player_state.equipment[EQUIP_BRACELET] + ITEM_WORN_FIRST].weight_tenths_of_pound;
    player_state.weight += item_name_defs[player_state.equipment[EQUIP_AMULET] + ITEM_WORN_FIRST].weight_tenths_of_pound;
    SET_SCALED(0);
    SET_SCALED(1);
    SET_SCALED(2);
    SET_SCALED(3);
    SET_SCALED(4);
    SET_SCALED(5);
    SET_SCALED(6);
    SET_SCALED(7);
    SET_SCALED(8);
    player_state.attack_stats.values[0] += player_state.physical_strength_bonus[0];
    player_state.attack_stats.values[1] += player_state.physical_strength_bonus[1];
    player_state.attack_stats.values[2] += player_state.physical_strength_bonus[2];
    player_state.attack_stats.values[3] += player_state.magic_strength_bonus[0];
    player_state.attack_stats.values[4] += player_state.magic_strength_bonus[1];
    player_state.attack_stats.values[5] += player_state.magic_strength_bonus[2];
    player_state.attack_stats.values[6] += player_state.magic_strength_bonus[3];
    player_state.attack_stats.values[7] += player_state.magic_strength_bonus[4];
    player_state.attack_stats.values[8] += player_state.magic_strength_bonus[5];
    player_state.ailment_attack_rates = item_def->ailment_attack_rates;
    player_state.ailment_attack_rates.values[0] += player_state.ailment_attack_bonus[0];
    player_state.ailment_attack_rates.values[1] += player_state.ailment_attack_bonus[1];
    player_state.ailment_attack_rates.values[2] += player_state.ailment_attack_bonus[2];
    player_state.ailment_attack_rates.values[3] += player_state.ailment_attack_bonus[3];
    player_state.ailment_attack_rates.values[4] += player_state.ailment_attack_bonus[4];
    player_state.ailment_attack_rates.values[5] += player_state.ailment_attack_bonus[5];
    memset(&player_state.worn_item_stats, 0, sizeof(PlayerShotStats));
    Player_AddWornItemValues((u16 *)player_state.worn_item_stats.values, player_state.equipment[EQUIP_SHIELD]);
    Player_AddWornItemValues((u16 *)player_state.worn_item_stats.values, player_state.equipment[EQUIP_HELM]);
    Player_AddWornItemValues((u16 *)player_state.worn_item_stats.values, player_state.equipment[EQUIP_ARMOR]);
    Player_AddWornItemValues((u16 *)player_state.worn_item_stats.values, player_state.equipment[EQUIP_GAUNTLET]);
    Player_AddWornItemValues((u16 *)player_state.worn_item_stats.values, player_state.equipment[EQUIP_BOOT]);
    Player_AddWornItemValues((u16 *)player_state.worn_item_stats.values, player_state.equipment[EQUIP_RING]);
    Player_AddWornItemValues((u16 *)player_state.worn_item_stats.values, player_state.equipment[EQUIP_BRACELET]);
    Player_AddWornItemValues((u16 *)player_state.worn_item_stats.values, player_state.equipment[EQUIP_AMULET]);
    memset(&player_state.defence_stats, 0, sizeof(PlayerShotStats));
    Player_AddWornItemValues((u16 *)player_state.defence_stats.values, player_state.equipment[EQUIP_SHIELD]);
    Player_AddWornItemValues((u16 *)player_state.defence_stats.values, player_state.equipment[EQUIP_HELM]);
    Player_AddWornItemValues((u16 *)player_state.defence_stats.values, player_state.equipment[EQUIP_ARMOR]);
    Player_AddWornItemValues((u16 *)player_state.defence_stats.values, player_state.equipment[EQUIP_GAUNTLET]);
    Player_AddWornItemValues((u16 *)player_state.defence_stats.values, player_state.equipment[EQUIP_BOOT]);
    Player_AddWornItemValues((u16 *)player_state.defence_stats.values, player_state.equipment[EQUIP_RING]);
    Player_AddWornItemValues((u16 *)player_state.defence_stats.values, player_state.equipment[EQUIP_BRACELET]);
    Player_AddWornItemValues((u16 *)player_state.defence_stats.values, player_state.equipment[EQUIP_AMULET]);
    player_state.defence_stats.values[0] += player_state.physical_defence_bonus[0];
    player_state.defence_stats.values[1] += player_state.physical_defence_bonus[1];
    player_state.defence_stats.values[2] += player_state.physical_defence_bonus[2];
    player_state.defence_stats.values[3] += player_state.magic_defence_bonus[0];
    player_state.defence_stats.values[4] += player_state.magic_defence_bonus[1];
    player_state.defence_stats.values[5] += player_state.magic_defence_bonus[2];
    player_state.defence_stats.values[6] += player_state.magic_defence_bonus[3];
    player_state.defence_stats.values[7] += player_state.magic_defence_bonus[4];
    player_state.defence_stats.values[8] += player_state.magic_defence_bonus[5];
    player_state.ailment_defence[0] += player_state.ailment_defence_bonus[0];
    player_state.ailment_defence[1] += player_state.ailment_defence_bonus[1];
    player_state.ailment_defence[2] += player_state.ailment_defence_bonus[2];
    player_state.ailment_defence[3] += player_state.ailment_defence_bonus[3];
    player_state.ailment_defence[4] += player_state.ailment_defence_bonus[4];
    player_state.ailment_defence[5] += player_state.ailment_defence_bonus[5];
    if (player_state.worn_drain_active != 0 || (u16)player_state.timed_flags[3] != 0) { // match: read unsigned here
        player_state.power /= 2;
        player_state.magic /= 2;
    }
    if (player_state.rage_timer > 0) {
        player_state.power += player_state.rage_power_bonus;
    }
    if (player_state.power_seed_timer != 0) {
        player_state.power += player_state.power / 10;
    }
    if (player_state.widda_needle_timer != 0) {
        player_state.magic += player_state.magic / 10;
    }
    if (player_state.rage_timer > 0) {
        player_state.magic = 1;
    }
    if (player_state.equipment[EQUIP_GAUNTLET] == ITEM_WORN_INDEX(ITEM_SACRED_GLOVES)) {
        s32 item_weight = item_name_defs[player_state.weapon + ITEM_WEAPON_FIRST].weight_tenths_of_pound;

        player_state.weight -= item_weight / 4;
    }
    if (player_state.equipment[EQUIP_GAUNTLET] == ITEM_WORN_INDEX(ITEM_BONE_HANDS)) {
        player_state.weight += item_name_defs[player_state.weapon + ITEM_WEAPON_FIRST].weight_tenths_of_pound;
    }
    if (player_state.equipment[EQUIP_GAUNTLET] == ITEM_WORN_INDEX(ITEM_LORD_MYUS_GLOVES)) {
        s32 item_weight = item_name_defs[player_state.weapon + ITEM_WEAPON_FIRST].weight_tenths_of_pound;

        player_state.weight -= item_weight / 4;
    }
    if (player_state.equipment[EQUIP_GAUNTLET] == ITEM_WORN_INDEX(ITEM_SILVERY_GLOVES)) {
        player_state.attack_stats.values[0] += player_state.attack_stats.values[0] / 10;
        player_state.attack_stats.values[1] += player_state.attack_stats.values[1] / 10;
        player_state.attack_stats.values[2] += player_state.attack_stats.values[2] / 10;
    }
    if (player_state.equipment[EQUIP_GAUNTLET] == ITEM_WORN_INDEX(ITEM_VENOM_GLOVES)) {
        player_state.ailment_attack_rates.values[0] += 70;
    }
    if (player_state.equipment[EQUIP_RING] == ITEM_WORN_INDEX(ITEM_WIDDAS_RING)) {
        player_state.hp_max += player_state.hp_max / 5;
    }
    if (player_state.equipment[EQUIP_RING] == ITEM_WORN_INDEX(ITEM_RING_OF_FIRE)) {
        player_state.attack_stats.values[3] += 20;
    }
    if (player_state.equipment[EQUIP_RING] == ITEM_WORN_INDEX(ITEM_RING_OF_EARTH)) {
        player_state.attack_stats.values[4] += 20;
    }
    if (player_state.equipment[EQUIP_RING] == ITEM_WORN_INDEX(ITEM_RING_OF_WIND)) {
        player_state.attack_stats.values[5] += 20;
    }
    if (player_state.equipment[EQUIP_RING] == ITEM_WORN_INDEX(ITEM_RING_OF_WATER)) {
        player_state.attack_stats.values[6] += 20;
    }
    if (player_state.equipment[EQUIP_BRACELET] == ITEM_WORN_INDEX(ITEM_POWER_BRACELET)) {
        player_state.attack_stats.values[0] += player_state.attack_stats.values[0] / 10;
        player_state.attack_stats.values[1] += player_state.attack_stats.values[1] / 10;
        player_state.attack_stats.values[2] += player_state.attack_stats.values[2] / 10;
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
    if (player_state.ailment_attack_rates.values[0] > 100) {
        player_state.ailment_attack_rates.values[0] = 100;
    }
    if (player_state.ailment_attack_rates.values[1] > 100) {
        player_state.ailment_attack_rates.values[1] = 100;
    }
    if (player_state.ailment_attack_rates.values[2] > 100) {
        player_state.ailment_attack_rates.values[2] = 100;
    }
    if (player_state.ailment_attack_rates.values[3] > 100) {
        player_state.ailment_attack_rates.values[3] = 100;
    }
    if (player_state.ailment_attack_rates.values[4] > 100) {
        player_state.ailment_attack_rates.values[4] = 100;
    }
    if (player_state.ailment_attack_rates.values[5] > 100) {
        player_state.ailment_attack_rates.values[5] = 100;
    }
    if (player_state.ailment_defence[0] > 100) {
        player_state.ailment_defence[0] = 100;
    }
    if (player_state.ailment_defence[1] > 100) {
        player_state.ailment_defence[1] = 100;
    }
    if (player_state.ailment_defence[2] > 100) {
        player_state.ailment_defence[2] = 100;
    }
    if (player_state.ailment_defence[3] > 100) {
        player_state.ailment_defence[3] = 100;
    }
    if (player_state.ailment_defence[4] > 100) {
        player_state.ailment_defence[4] = 100;
    }
    if (player_state.ailment_defence[5] > 100) {
        player_state.ailment_defence[5] = 100;
    }
}
