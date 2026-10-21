#include "player/magic_ladder.h"
#include "player/player.h"
#include "effect/effect.h"
#include "effect/shot.h"
#include "entity/entity.h"
#include "core/file.h"
#include "core/game.h"
#include "enemy/enemy.h"
#include <math.h>
#include "math/angle.h"
#include "math/vu0.h"
#include "sound/sound_game.h"

#define LADDER_TURN_SPEED_RAD 0.029088821f /* 5/3 degrees per frame, in radians */

/* magic numbers (Player_CastMagic) the switches below name; a spell's levels are consecutive */
#define MAGIC_EARTH_HEAL_LEVEL_1            0x10    /* counted only while hurt */
#define MAGIC_EARTH_HEAL_LEVEL_2            0x11
#define MAGIC_EARTH_HEAL_LEVEL_3            0x12
#define MAGIC_PURITY_LEVEL_1                0x2E    /* counted only while poisoned; also slows the poison ticks */
#define MAGIC_PURITY_LEVEL_2                0x2F
#define MAGIC_PURITY_LEVEL_3                0x30
#define MAGIC_DIVINE_LIGHT_LEVEL_1          0x3D    /* the map glow and light_spell_timer */
#define MAGIC_DIVINE_LIGHT_LEVEL_2          0x3E
#define MAGIC_DIVINE_LIGHT_LEVEL_3          0x3F
#define MAGIC_SHADOW_SKIN_LEVEL_1           0x4C    /* weaker enemies within intimidate_range attack from half as far */
#define MAGIC_SHADOW_SKIN_LEVEL_2           0x4D
#define MAGIC_SHADOW_SKIN_LEVEL_3           0x4E
#define MAGIC_RECKLESS_STRENGTH_LEVEL_1     0x52    /* raises rage_power_bonus to 10, 20 or 30 */
#define MAGIC_RECKLESS_STRENGTH_LEVEL_2     0x53
#define MAGIC_RECKLESS_STRENGTH_LEVEL_3     0x54

/* 0x4145C8 the ladder's state handlers, one per LadderState (Ladder_DispatchPad picks one) */
void (*ladder_state_handlers[6])(u16, u16) = {
    Ladder_WalkToFoot, Ladder_WaitOnRung, Ladder_ClimbRungUp, Ladder_ClimbRungDown,
};

static Vector4 ladder_foot_position;              /* 0x57B350 foot the player walks to before climbing */
static Vector4 ladder_walk_step;                  /* 0x57B360 unit step toward the foot each frame */
static Vector4 ladder_top_point;                  /* 0x57B370 ladder end from model get_point(1) */
static Vector4 ladder_bottom_point;               /* 0x57B380 ladder end from model get_point(0) */
static Vector4 ladder_span;                       /* 0x57B390 vector from bottom to top */
static PlayerShot channeled_magic_shots[16];      /* 0x57B3A0 ring of shots for held magics 7-9 */
static s32 ladder_state_index;                    /* 0x447128 index into ladder_state_handlers */
static ObjectReference *ladder_object;            /* 0x44712C map object being climbed */
static f32 ladder_facing_y;                       /* 0x447130 Y rotation to face the ladder */
static s32 ladder_walk_frames_remaining;          /* 0x447134 frames left walking to the foot */
static f32 ladder_rung_spacing;                   /* 0x447138 distance between rungs along the span */
static s32 ladder_rung_count;                     /* 0x44713C number of rungs on the ladder */
static s32 ladder_rung_anim_base;                 /* 0x447140 rung index at the start of a climb segment */
static s32 ladder_current_rung;                   /* 0x447144 rung the player is on or moving toward */
static s32 ladder_frames_per_rung;                /* 0x447148 frames to move one rung */
static s32 ladder_rung_anim_frame;                /* 0x44714C frame within the current rung move */
static s32 channeled_magic_active;                /* 0x447150 nonzero while held magic 7-9 is armed */
static s32 channeled_magic_unused;                /* 0x447154 never used by this code */
static s32 channeled_magic_warmup_frames;         /* 0x447158 countdown before MP drain for held magic */
static s32 channeled_shot_ring_index;             /* 0x44715C next slot in channeled_magic_shots */
static PlayerShot *channeled_shot_slot;           /* 0x447160 shot reused while magic 7-9 is held */

/* Casts magic `magic_id`: takes its cost, then launches its shot or applies what it does to the player. */
int Player_CastMagic(int magic_id) {
    Vector4 angles;
    FxTableEntry *entry;
    FxTableStats *stats;
    PlayerShot *shot;

    if (magic_id == 0) {
        return FALSE;
    }
    channeled_magic_active = 0;
    entry = &fx_table[magic_id];
    stats = &entry->stats;
    if (entry->item_menu_magic == 0) {
        if (PLAYER_IS_UNDERWATER(&player_state)) {
            return FALSE;
        }
        if (PLAYER_IS_IN_WATER(&player_state)) {
            if (player_state.position.y - entry->effect_depth_below > player_state.water_surface_y) {
                return FALSE;
            }
        }
    }
    if (player_state.equipment[EQUIP_ARMOR] == ITEM_WORN_INDEX(ITEM_ICE_ARMOR) && magic_id > 0 && (magic_id - 1) / 15 == 0) {
        return FALSE;
    }
    if (player_state.magic_gauge < 5000) {
        return FALSE;
    }
    if (player_state.mp_now < entry->mp_cost) {
        return FALSE;
    }
    player_state.mp_now -= entry->mp_cost;
    player_state.magic_gauge_delay = entry->gauge_refill_delay;
    player_state.charging_magic = magic_id;
    player_state.magic_gauge = 0;
    if ((u32)(magic_id - 7) < 3) {
        channeled_magic_warmup_frames = 30;
        channeled_magic_active = 1;
        channeled_shot_slot = &channeled_magic_shots[channeled_shot_ring_index];
        memset(channeled_shot_slot, 0, sizeof(PlayerShot));
        channeled_shot_ring_index = (channeled_shot_ring_index + 1) % 16;
        return FALSE;
    }
    if (entry->item_menu_magic == 0) {
        shot = Shot_Alloc();
        shot->is_magic_shot = 1;
        shot->player_power = player_state.power;
        shot->player_magic = player_state.magic;
        shot->magic_id = magic_id;
        shot->worn_item_stats = player_state.worn_item_stats;
        Vector_Copy(&angles, &player_state.rotation);
        angles.x += player_state.view_tilt_pitch;
        if (angles.x > DEG_TO_RAD(58)) {
            angles.x = DEG_TO_RAD(58);
        } else if (angles.x < -DEG_TO_RAD(58)) {
            angles.x = -DEG_TO_RAD(58);
        }
        Attack_LaunchMagicEffect(magic_id, shot, &player_state, &player_state.rotation, &player_state); // match: the player's rotation is passed; the clamped angles above are never used
        player_state.cast_launched_shot = 1;
    } else {
        if (stats->clears_timed_flag[0] != 0) {
            player_state.timed_flags[0] = 0;
        }
        if (stats->clears_timed_flag[1] != 0) {
            player_state.timed_flags[1] = 0;
        }
        if (stats->clears_timed_flag[2] != 0) {
            player_state.timed_flags[2] = 0;
        }
        if (stats->clears_timed_flag[3] != 0) {
            player_state.timed_flags[3] = 0;
        }
        if (stats->clears_timed_flag[4] != 0) {
            player_state.timed_flags[4] = 0;
        }
        if (stats->clears_timed_flag[5] != 0) {
            player_state.timed_flags[5] = 0;
        }
        if (stats->heal_total >= 999) {
            player_state.hp_now = player_state.hp_max;
        } else if (stats->heal_total != 0) {
            player_state.hp_now += stats->heal_at_once;
            if (player_state.hp_now > player_state.hp_max) {
                player_state.hp_now = player_state.hp_max;
            }
            if (player_state.hp_now < 0) {
                player_state.hp_now = 0;
            }
            player_state.hp_step_remaining = stats->heal_total - stats->heal_at_once;
            if (player_state.hp_step_remaining == 0) {
                player_state.hp_step_period = 0;
            } else {
                if (stats->heal_step_period > 0) {
                    player_state.hp_step_period = stats->heal_step_period;
                } else {
                    player_state.hp_step_period = 1;
                }
            }
        }
        if (stats->duration != 0) {
            if (player_state.physical_strength_bonus[0] <= 0 && player_state.physical_strength_bonus[1] <= 0 && player_state.physical_strength_bonus[2] <= 0
                && player_state.magic_strength_bonus[0] <= 0 && player_state.magic_strength_bonus[1] <= 0 && player_state.magic_strength_bonus[2] <= 0
                && player_state.magic_strength_bonus[3] <= 0 && player_state.magic_strength_bonus[4] <= 0 && player_state.magic_strength_bonus[5] <= 0) {
                timed_boost_order = 0;
            }
            if (stats->strength_bonus[0] != 0) {
                player_state.physical_strength_bonus[0] = stats->strength_bonus[0];
                player_state.physical_strength_bonus_timer[0] = stats->duration;
                player_state.physical_bonus_stamp[0] = ++timed_boost_order;
            }
            if (stats->strength_bonus[1] != 0) {
                player_state.physical_strength_bonus[1] = stats->strength_bonus[1];
                player_state.physical_strength_bonus_timer[1] = stats->duration;
                player_state.physical_bonus_stamp[1] = ++timed_boost_order;
            }
            if (stats->strength_bonus[2] != 0) {
                player_state.physical_strength_bonus[2] = stats->strength_bonus[2];
                player_state.physical_strength_bonus_timer[2] = stats->duration;
                player_state.physical_bonus_stamp[2] = ++timed_boost_order;
            }
            if (stats->strength_bonus[3] != 0) {
                player_state.magic_strength_bonus[0] = stats->strength_bonus[3];
                player_state.magic_strength_bonus_timer[0] = stats->duration;
                player_state.magic_bonus_stamp[0] = ++timed_boost_order;
            }
            if (stats->strength_bonus[4] != 0) {
                player_state.magic_strength_bonus[1] = stats->strength_bonus[4];
                player_state.magic_strength_bonus_timer[1] = stats->duration;
                player_state.magic_bonus_stamp[1] = ++timed_boost_order;
            }
            if (stats->strength_bonus[5] != 0) {
                player_state.magic_strength_bonus[2] = stats->strength_bonus[5];
                player_state.magic_strength_bonus_timer[2] = stats->duration;
                player_state.magic_bonus_stamp[2] = ++timed_boost_order;
            }
            if (stats->strength_bonus[6] != 0) {
                player_state.magic_strength_bonus[3] = stats->strength_bonus[6];
                player_state.magic_strength_bonus_timer[3] = stats->duration;
                player_state.magic_bonus_stamp[3] = ++timed_boost_order;
            }
            if (stats->strength_bonus[7] != 0) {
                player_state.magic_strength_bonus[4] = stats->strength_bonus[7];
                player_state.magic_strength_bonus_timer[4] = stats->duration;
                player_state.magic_bonus_stamp[4] = ++timed_boost_order;
            }
            if (stats->strength_bonus[8] != 0) {
                player_state.magic_strength_bonus[5] = stats->strength_bonus[8];
                player_state.magic_strength_bonus_timer[5] = stats->duration;
                player_state.magic_bonus_stamp[5] = ++timed_boost_order;
            }
            if (stats->ailment_attack_bonus[0] != 0) {
                player_state.ailment_attack_bonus[0] = stats->ailment_attack_bonus[0];
                player_state.ailment_attack_bonus_timer[0] = stats->duration;
            }
            if (stats->ailment_attack_bonus[1] != 0) {
                player_state.ailment_attack_bonus[1] = stats->ailment_attack_bonus[1];
                player_state.ailment_attack_bonus_timer[1] = stats->duration;
            }
            if (stats->ailment_attack_bonus[2] != 0) {
                player_state.ailment_attack_bonus[2] = stats->ailment_attack_bonus[2];
                player_state.ailment_attack_bonus_timer[2] = stats->duration;
            }
            if (stats->ailment_attack_bonus[3] != 0) {
                player_state.ailment_attack_bonus[3] = stats->ailment_attack_bonus[3];
                player_state.ailment_attack_bonus_timer[3] = stats->duration;
            }
            if (stats->ailment_attack_bonus[4] != 0) {
                player_state.ailment_attack_bonus[4] = stats->ailment_attack_bonus[4];
                player_state.ailment_attack_bonus_timer[4] = stats->duration;
            }
            if (stats->ailment_attack_bonus[5] != 0) {
                player_state.ailment_attack_bonus[5] = stats->ailment_attack_bonus[5];
                player_state.ailment_attack_bonus_timer[5] = stats->duration;
            }
            if (stats->defence_bonus[0] != 0) {
                player_state.physical_defence_bonus[0] = stats->defence_bonus[0];
                player_state.physical_defence_bonus_timer[0] = stats->duration;
            }
            if (stats->defence_bonus[1] != 0) {
                player_state.physical_defence_bonus[1] = stats->defence_bonus[1];
                player_state.physical_defence_bonus_timer[1] = stats->duration;
            }
            if (stats->defence_bonus[2] != 0) {
                player_state.physical_defence_bonus[2] = stats->defence_bonus[2];
                player_state.physical_defence_bonus_timer[2] = stats->duration;
            }
            if (stats->defence_bonus[3] != 0) {
                player_state.magic_defence_bonus[0] = stats->defence_bonus[3];
                player_state.magic_defence_bonus_timer[0] = stats->duration;
            }
            if (stats->defence_bonus[4] != 0) {
                player_state.magic_defence_bonus[1] = stats->defence_bonus[4];
                player_state.magic_defence_bonus_timer[1] = stats->duration;
            }
            if (stats->defence_bonus[5] != 0) {
                player_state.magic_defence_bonus[2] = stats->defence_bonus[5];
                player_state.magic_defence_bonus_timer[2] = stats->duration;
            }
            if (stats->defence_bonus[6] != 0) {
                player_state.magic_defence_bonus[3] = stats->defence_bonus[6];
                player_state.magic_defence_bonus_timer[3] = stats->duration;
            }
            if (stats->defence_bonus[7] != 0) {
                player_state.magic_defence_bonus[4] = stats->defence_bonus[7];
                player_state.magic_defence_bonus_timer[4] = stats->duration;
            }
            if (stats->defence_bonus[8] != 0) {
                player_state.magic_defence_bonus[5] = stats->defence_bonus[8];
                player_state.magic_defence_bonus_timer[5] = stats->duration;
            }
            if (stats->ailment_defence_bonus[0] != 0) {
                player_state.ailment_defence_bonus[0] = stats->ailment_defence_bonus[0];
                player_state.ailment_defence_bonus_timer[0] = stats->duration;
            }
            if (stats->ailment_defence_bonus[1] != 0) {
                player_state.ailment_defence_bonus[1] = stats->ailment_defence_bonus[1];
                player_state.ailment_defence_bonus_timer[1] = stats->duration;
            }
            if (stats->ailment_defence_bonus[2] != 0) {
                player_state.ailment_defence_bonus[2] = stats->ailment_defence_bonus[2];
                player_state.ailment_defence_bonus_timer[2] = stats->duration;
            }
            if (stats->ailment_defence_bonus[3] != 0) {
                player_state.ailment_defence_bonus[3] = stats->ailment_defence_bonus[3];
                player_state.ailment_defence_bonus_timer[3] = stats->duration;
            }
            if (stats->ailment_defence_bonus[4] != 0) {
                player_state.ailment_defence_bonus[4] = stats->ailment_defence_bonus[4];
                player_state.ailment_defence_bonus_timer[4] = stats->duration;
            }
            if (stats->ailment_defence_bonus[5] != 0) {
                player_state.ailment_defence_bonus[5] = stats->ailment_defence_bonus[5];
                player_state.ailment_defence_bonus_timer[5] = stats->duration;
            }
        }
        switch (magic_id) {
        case MAGIC_PURITY_LEVEL_1:
            player_state.ailment0_slow_timer = stats->duration;
            break;
        case MAGIC_DIVINE_LIGHT_LEVEL_1:
        case MAGIC_DIVINE_LIGHT_LEVEL_2:
        case MAGIC_DIVINE_LIGHT_LEVEL_3:
            player_state.light_spell_timer = stats->duration;
            break;
        case MAGIC_SHADOW_SKIN_LEVEL_1:
        case MAGIC_SHADOW_SKIN_LEVEL_2:
        case MAGIC_SHADOW_SKIN_LEVEL_3:
            player_state.intimidate_timer = stats->duration;
            if (magic_id >= MAGIC_SHADOW_SKIN_LEVEL_2) {
                player_state.intimidate_range = 1000.0f;
            } else {
                player_state.intimidate_range = 600.0f;
            }
            break;
        case MAGIC_RECKLESS_STRENGTH_LEVEL_1:
            player_state.rage_timer = stats->duration;
            player_state.rage_power_bonus = 10;
            break;
        case MAGIC_RECKLESS_STRENGTH_LEVEL_2:
            player_state.rage_timer = stats->duration;
            player_state.rage_power_bonus = 20;
            break;
        case MAGIC_RECKLESS_STRENGTH_LEVEL_3:
            player_state.rage_timer = stats->duration;
            player_state.rage_power_bonus = 30;
            break;
        }
        Effect_FreeByParam(magic_id);
        Effect_Spawn(entry->effect_id, &origin_point, &origin_point, (Entity *)&player_state, magic_id, NULL);
        switch (magic_id) {
        case MAGIC_EARTH_HEAL_LEVEL_1:
        case MAGIC_EARTH_HEAL_LEVEL_2:
        case MAGIC_EARTH_HEAL_LEVEL_3:
        case 0x38:
        case 0x39:
        case 0x3A:
            if (player_previous.hp_now < player_previous.hp_max) {
                Player_CountMagicUse(magic_id);
            }
            break;
        case MAGIC_PURITY_LEVEL_1:
        case MAGIC_PURITY_LEVEL_2:
        case MAGIC_PURITY_LEVEL_3:
            if ((u16)player_previous.timed_flags[0] != 0) { // match: read unsigned here
                Player_CountMagicUse(magic_id);
            }
            break;
        default:
            Player_CountMagicUse(magic_id);
            break;
        }
    }
    return TRUE;
}

/* Keeps a held magic (7 to 9) going: after the first 30 frames it costs one of mp_now every third frame, and
   every call launches its shot again from the slot Player_CastMagic set aside. Out of mp_now, it stops. */
void Player_HoldMagic(int magic_id) {
    Vector4 angles;
    FxTableEntry *entry;

    if (magic_id == 0) {
        return;
    }
    entry = &fx_table[magic_id];
    if ((u32)(magic_id - 7) < 3) {
        if (channeled_magic_active == 0) {
            return;
        }
        if (entry->item_menu_magic == 0) {
            if (PLAYER_IS_UNDERWATER(&player_state)) {
                channeled_magic_active = 0;
                return;
            }
            if (PLAYER_IS_IN_WATER(&player_state)) {
                if (player_state.position.y - entry->effect_depth_below > player_state.water_surface_y) {
                    // match: the water check keeps its block order only with this goto
                    goto stop;
                }
            }
        }
        if (player_state.equipment[EQUIP_ARMOR] == ITEM_WORN_INDEX(ITEM_ICE_ARMOR) && magic_id > 0 && (magic_id - 1) / 15 == 0) {
            return;
        }
        player_state.magic_gauge_delay = entry->gauge_refill_delay;
        player_state.magic_gauge = 0;
        if (channeled_magic_warmup_frames > 0) {
            channeled_magic_warmup_frames--;
        } else {
            if (player_state.mp_now <= 0) {
                channeled_magic_active = 0;
                return;
            }
            if (frame_counter % 3 == 0) {
                player_state.mp_now--;
            }
        }
        if (channeled_shot_slot != NULL) {
            channeled_shot = channeled_shot_slot;
            channeled_shot_slot->is_magic_shot = 1;
            channeled_shot->player_power = player_state.power;
            channeled_shot->player_magic = player_state.magic;
            channeled_shot->magic_id = magic_id;
            channeled_shot->worn_item_stats = player_state.worn_item_stats;
            Vector_Copy(&angles, &player_state.rotation);
            angles.x += player_state.view_tilt_pitch;
            if (angles.x > DEG_TO_RAD(58)) {
                angles.x = DEG_TO_RAD(58);
            } else if (angles.x < -DEG_TO_RAD(58)) {
                angles.x = -DEG_TO_RAD(58);
            }
            // match: the clamped angles are not what is passed
            Attack_LaunchMagicEffect(magic_id, channeled_shot, &player_state, &player_state.rotation, &player_state);
            player_state.cast_launched_shot = 1;
        }
    }
    return;
stop:
    channeled_magic_active = 0;
}

/* Puts the player on the object (a ladder, it seems): notes its two end points, the walk to the end the player
   starts from (12 units a frame) and the number of steps. */
void Ladder_Start(ObjectReference *object, int from_far_end) {
    Vector4 span;
    int step_count;
    ReferencedObject *def;

    if (PLAYER_IS_ON_LADDER(&player_state)) {
        return;
    }
    if (PLAYER_IS_AIRBORNE(&player_state)) {
        return;
    }
    Vector_Copy(&ladder_top_point, object->model->get_point(object->model, 1));
    Vector_Copy(&ladder_bottom_point, object->model->get_point(object->model, 0));
    Vector_Subtract(&ladder_span, &ladder_top_point, &ladder_bottom_point);
    PLAYER_STATUS_BITS(&player_state)->on_ladder = TRUE;
    PLAYER_STATUS_BITS(&player_state)->airborne = FALSE;
    PLAYER_STATUS_BITS(&player_state)->step_up_pending = FALSE;
    player_state.fall_speed = 0.0f;
    player_state.strafe_speed = 0.0f;
    player_state.walk_speed = 0.0f;
    player_state.look_speed = 0.0f;
    player_state.turn_speed = 0.0f;
    ladder_state_index = 0;
    ladder_facing_y = object->model->rotation.y;
    ladder_object = object;
    if (from_far_end) {
        Vector_Copy(&ladder_foot_position, &ladder_top_point);
    } else {
        Vector_Copy(&ladder_foot_position, &ladder_bottom_point);
    }
    Vector_Subtract(&ladder_walk_step, &ladder_foot_position, &player_state.position);
    ladder_walk_frames_remaining = sqrtf(ladder_walk_step.x * ladder_walk_step.x + ladder_walk_step.y * ladder_walk_step.y + ladder_walk_step.z * ladder_walk_step.z) / 12.0f;
    Vector_Scale(&ladder_walk_step, &ladder_walk_step, 1.0f / (f32)ladder_walk_frames_remaining);
    def = object->definition;
    step_count = def->step_count;
    ladder_rung_count = step_count;
    if (from_far_end) {
        ladder_current_rung = step_count;
        ladder_rung_anim_base = step_count;
    } else {
        ladder_current_rung = 0;
        ladder_rung_anim_base = 0;
    }
    ladder_frames_per_rung = def->frames_per_step;
    ladder_rung_anim_frame = 0;
    Vector_Subtract(&span, &ladder_bottom_point, &ladder_top_point);
    ladder_rung_spacing = sqrtf(span.x * span.x + span.y * span.y + span.z * span.z) / (f32)ladder_rung_count;
}

/* Clears ladder approach state when the player is knocked off during a climb. */
void Ladder_CancelApproach(void) {
    ladder_state_index = 0;
    player_state.status_bits &= ~(1L << 36);
}

/* Dispatches pad input to the ladder state handler when the player is on a ladder. */
int Ladder_DispatchPad(u16 held, u16 pressed) {
    PlayerState *player = &player_state;

    if (PLAYER_IS_ON_LADDER(player)) {
        ladder_state_handlers[ladder_state_index](held, pressed);
    }
    return PLAYER_IS_ON_LADDER(player);
}

/* Walks the player toward the ladder foot and turns to face it. */
void Ladder_WalkToFoot(void) {
    if (ladder_walk_frames_remaining > 0) {
        ladder_walk_frames_remaining--;
        if (ladder_walk_frames_remaining > 0) {
            Vector_Add(&player_state.position, &player_state.position, &ladder_walk_step);
        } else {
            Vector_Copy(&player_state.position, &ladder_foot_position);
        }
    }
    if (Angle_Approach(&player_state.rotation.y, ladder_facing_y, LADDER_TURN_SPEED_RAD) && ladder_walk_frames_remaining <= 0) {
        ladder_state_index++;
    }
}

/* Waits on a rung; up/down input starts climbing or leaves the ladder. */
void Ladder_WaitOnRung(u16 buttons) {
    if (!(ladder_object->flags & MAP_OBJECT_IN_USE)) {
        player_state.status_bits &= ~(1L << 36);
    }
    if (buttons & (step_right_button | step_left_button)) {
        player_state.status_bits &= ~(1L << 36);
    }
    if ((buttons & (turn_right_button | turn_left_button)) && !(buttons & (forward_button | back_button))) {
        player_state.status_bits &= ~(1L << 36);
    }
    if (forward_button & buttons) {
        ladder_current_rung++;
        if (ladder_current_rung <= ladder_rung_count) {
            ladder_rung_anim_frame = 0;
            ladder_state_index++;
            return;
        }
    } else {
        if (!(back_button & buttons)) {
            return;
        }
        ladder_current_rung--;
        if (ladder_current_rung >= 0) {
            ladder_rung_anim_frame = ladder_frames_per_rung;
            ladder_state_index += 2;
            return;
        }
    }
    player_state.status_bits &= ~(1L << 36);
}

/* Moves the player one rung up along a sine ease and plays a step sound. */
void Ladder_ClimbRungUp(u16 buttons) {
    Vector4 offset;
    f32 angle;
    f32 fraction;

    if (!(ladder_object->flags & MAP_OBJECT_IN_USE)) {
        player_state.status_bits &= ~(1L << 36);
    }
    if (buttons & (step_right_button | step_left_button)) {
        player_state.status_bits &= ~(1L << 36);
    }
    if ((buttons & (turn_right_button | turn_left_button)) && !(buttons & (forward_button | back_button))) {
        player_state.status_bits &= ~(1L << 36);
    }
    ladder_rung_anim_frame++;
    angle = DEG_TO_RAD(110);
    fraction = 1.0f;
    fraction /= (f32)ladder_rung_count;
    fraction = fraction * sinf((f32)ladder_rung_anim_frame * angle / (f32)ladder_frames_per_rung) / sinf(angle);
    Vector_Scale(&offset, &ladder_span,
                    fraction + (f32)(ladder_rung_anim_base * ladder_frames_per_rung) / (f32)(ladder_rung_count * ladder_frames_per_rung));
    Vector_Add(&player_state.position, &ladder_bottom_point, &offset);
    if (ladder_rung_anim_frame >= ladder_frames_per_rung) {
        ladder_rung_anim_base = ladder_current_rung;
        ladder_state_index--;
        Ladder_PlayStepSound();
    }
}

/* Moves the player one rung down along a sine ease and plays a step sound. */
void Ladder_ClimbRungDown(u16 buttons) {
    Vector4 offset;
    f32 angle;
    f32 fraction;

    if (!(ladder_object->flags & MAP_OBJECT_IN_USE)) {
        player_state.status_bits &= ~(1L << 36);
    }
    if (buttons & (step_right_button | step_left_button)) {
        player_state.status_bits &= ~(1L << 36);
    }
    if ((buttons & (turn_right_button | turn_left_button)) && !(buttons & (forward_button | back_button))) {
        player_state.status_bits &= ~(1L << 36);
    }
    ladder_rung_anim_frame--;
    angle = DEG_TO_RAD(95);
    fraction = 1.0f;
    fraction /= (f32)ladder_rung_count;
    fraction = fraction * sinf((f32)ladder_rung_anim_frame * angle / (f32)ladder_frames_per_rung) / sinf(angle);
    Vector_Scale(&offset, &ladder_span,
                    fraction + (f32)(ladder_current_rung * ladder_frames_per_rung) / (f32)(ladder_rung_count * ladder_frames_per_rung));
    Vector_Add(&player_state.position, &ladder_bottom_point, &offset);
    if (ladder_rung_anim_frame <= 0) {
        ladder_rung_anim_base = ladder_current_rung;
        ladder_state_index -= 2;
        Ladder_PlayStepSound();
    }
}

/* Plays the sound for the kind of object ladder_object refers to. */
int Ladder_PlayStepSound(void) {
    u16 kind;

    if (ladder_object != NULL) {
        kind = ladder_object->definition->kind;
        switch (kind) {
        case 0x56:
            return Sound_PlayFootstep(0x22);
        case 0x167:
        case 0x1B1:
            return Sound_PlayFootstep(0x20);
        default:
            Sound_PlayFootstep(0x20);
            break;
        }
    }
}
