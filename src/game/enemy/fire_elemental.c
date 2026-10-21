/* FireElemental: the behaviour of entity type 48 (EntityParams.entity_type, the index into entity_type_handlers): its
   set-up function, which the table names, and the handlers it installs. Used by: Fire Elemental. The game
   names no monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm
   records by hit points and experience, with resistances and drops where two monsters share those. */
#include "entity/entity.h"
#include "common.h"
#include "enemy/enemy.h"
#include "entity/entity_ai_handlers.h"
#include "entity/entity_events.h"
#include "math/vu0.h"
#include "model/motion.h"
#include <stdlib.h>
#include "entity/entity_ai.h"
#include "entity/entity_ai_choice.h"
#include "entity/entity_math.h"
#include "player/player.h"

/* Entity.action_step values past the shared ones */
#define FIRE_ELEMENTAL_STAND_STEP_WATCH       2   /* stands its ground facing the player, dodging, guarding or attacking */
#define FIRE_ELEMENTAL_BACK_AWAY_STEP_BACK    2   /* backs away from the player until it is far from its spawn */

static void FireElemental_Think(Entity *entity, s32 run_now);
static void FireElemental_ThinkCombat(Entity *entity, s32 run_now);
static void FireElemental_StandAndAttack(Entity *entity);
static void FireElemental_PlayTurnAnim(Entity *entity);
static void FireElemental_Dodge(Entity *entity);
static s32 FireElemental_ShouldDodgeMagic(Entity *entity, f32 min_distance, f32 max_angle);
static s32 FireElemental_ShouldDodge(Entity *entity, f32 max_distance, f32 max_angle);
static void FireElemental_RangedAttack1(Entity *entity);
static void FireElemental_AlertSameKind(Entity *entity);
static void FireElemental_OnHit(Entity *entity);
static void FireElemental_BackAwayFromPlayer(Entity *entity);
static void FireElemental_Move(Entity *entity);
static s32 FireElemental_BackwardsCommand(Entity *entity, s32 unused, f32 reach);
static void FireElemental_ThinkReturn(Entity *entity, s32 run_now);
static void FireElemental_RangedAttack0(Entity *entity);

/* Set-up of a type 48 entity: clears its work area and installs the type's handlers. */
void FireElemental_Init(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    char *unk138 = entity->model->look_at_part;
    s32 i;

    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        ENTITY_WORK_BYTES(entity)[i] = 0;
    }
    EntityAi_SetUpdate(entity, entity_type_handlers[entity->params->entity_type][1]);
    ai_state->think = FireElemental_Think;
    ai_state->animate = EntityAi_Animate;
    ai_state->on_hit = FireElemental_OnHit;
    ai_state->move = FireElemental_Move;
    ai_state->dormant = EntityAi_Dormant;
    ai_state->death = EntityAi_Death;
    for (i = 3; i != -1; i--) {
        entity->choice_weights[i] = 0x80;
    }
    for (i = 5; i != -1; i--) {
        entity->attack_weights[i] = 0x80;
    }
    ENTITY_FLAG_BITS(entity)->no_floor_lift = 0;
    entity->target = NULL;
    if (ENTITY_SPAWN_FLAGS(entity) & (0x8000L << 43)) {
        EntityAi_StartAction(entity, ai_state->dormant, 1);
    } else {
        EntityAi_StartAction(entity, ai_state->move, 0);
    }
    if (unk138 != NULL) {
        Vector_Copy(ai_state->look_part_angles, unk138 + 0x50);
    }
    ai_state->pose_blend_weight = 0.5f;
    entity->model_alpha = (entity->params->flags & ENTITY_PARAMS_FLAG_HALF_MODEL_ALPHA) ? 0x40 : 0x80;
    entity->model->alpha = entity->model_alpha;
    Motion_ClearPartSlots(entity->model);
    ai_state->step_index = 0;
    Motion_StepModel(ENTITY_MODEL(entity), 0, 0);
}

/* The think handler: as EntityAi_Think, with FireElemental_ThinkReturn for flag bit 53 and its own
   state 2: FireElemental_StandAndAttack while ai_state.step_index is 0 (states 0 and others clear it),
   then FireElemental_ThinkCombat. ai_state.action_counter is put to 30 while player_state.cast_launched_shot is set and
   counts down. While ai_state.pause_timer is 1 (FireElemental_AlertSameKind of another entity sets it) it
   starts FireElemental_BackAwayFromPlayer instead of all that. */
static void FireElemental_Think(Entity *entity, s32 run_now) {
    EntityAiState *ai_state = &entity->ai_state;

    if (frame_counter % 10U == entity->spawn_index % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (player_state.cast_launched_shot != 0) {
        ai_state->action_counter = 30;
    }
    if (ai_state->action_counter > 0) {
        ai_state->action_counter--;
    }
    if (ai_state->pause_timer == 1) {
        if (entity->busy_flags == 0) {
            EntityAi_StartAction(entity, FireElemental_BackAwayFromPlayer, 1);
            return;
        }
    } else if (entity->busy_flags == 0) {
        EntityAi_CheckFarFromSpawn(entity);
        EntityAi_StepState(entity);
        if (ENTITY_FLAG_BITS(entity)->far_from_spawn) {
            FireElemental_ThinkReturn(entity, run_now);
        } else {
            switch (entity->state) {
            case ENTITY_STATE_UNAWARE:
                ai_state->step_index = 0;
                EntityAi_ThinkIdle(entity, run_now);
                break;
            case ENTITY_STATE_SEARCHING:
                EntityAi_ThinkPursue(entity, run_now);
                break;
            case ENTITY_STATE_NOTICED:
                if (ai_state->step_index == 0) {
                    EntityAi_StartAction(entity, FireElemental_StandAndAttack, run_now);
                } else {
                    FireElemental_ThinkCombat(entity, run_now);
                }
                break;
            default:
                ai_state->step_index = 0;
                EntityAi_ThinkIdle(entity, run_now);
                break;
            }
        }
    }
}

/* The think function of state 2: as EntityAi_ThinkCombat, but it first starts EntityAi_Dodge when
   FireElemental_ShouldDodgeMagic says so, ranged record 1 is FireElemental_RangedAttack1, and closing in gives the
   backwards command (bit 2, from FireElemental_BackwardsCommand) where the shared one gives the forwards one. */
static void FireElemental_ThinkCombat(Entity *entity, s32 run_now) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 direction[4];
    f32 angle;
    s32 side;
    s32 turn;

    if (entity->params->flags & ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE) {
        ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_IN_PLACE;
    } else {
        ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
    }
    if (FireElemental_ShouldDodgeMagic(entity, 200.0f, DEG_TO_RAD(60))) {
        EntityAi_StartAction(entity, EntityAi_Dodge, 1);
        return;
    }
    if (EntityAi_ShouldGuard(entity, 300.0f, DEG_TO_RAD(30))) {
        EntityAi_StartAction(entity, EntityAi_Guard, 1);
        return;
    }
    if (EntityAi_ShouldDodge(entity, 300.0f, DEG_TO_RAD(30))) {
        EntityAi_StartAction(entity, EntityAi_Dodge, 1);
        return;
    }
    side = EntityAi_PickCombatChoice(entity, 0);
    ENTITY_FLAG_BITS(entity)->look_at_target = 1;
    if (entity->action_timer == 0 || side == 0) {
        entity->prev_combat_choice = entity->combat_choice;
        entity->combat_choice = side;
        if (entity->combat_choice != entity->prev_combat_choice) {
            entity->choice_change_frame = frame_counter;
        }
        EntityAi_LowerChoiceWeight(entity, side);
        switch (entity->combat_choice) {
        case COMBAT_CHOICE_ATTACK:
            if (EntityAi_PickAttack(entity)) {
                if (ENTITY_FLAG_BITS(entity)->attack_index == 1) {
                    EntityAi_StartAction(entity, FireElemental_RangedAttack1, run_now);
                } else {
                    EntityAi_StartAction(entity, EntityAi_RangedAttack, run_now);
                }
            } else {
                EntityAi_StartAction(entity, EntityAi_MeleeAttack, run_now);
            }
            return;
        case COMBAT_CHOICE_CLOSE_IN:
            entity->target = &player_state;
            entity->action_timer = rand() % 60 + 120;
            ai_state->move_choice = MOVE_CHOICE_CLOSE_IN;
            break;
        case COMBAT_CHOICE_WAIT:
            entity->action_timer = rand() % 30 + 30;
            ai_state->move_choice = MOVE_CHOICE_NONE;
            break;
        default:
            ai_state->move_choice = MOVE_CHOICE_NONE;
            break;
        }
    }
    if (ai_state->move_choice == MOVE_CHOICE_CLOSE_IN) {
        EntityAi_ClearTurnInPlace(entity);
        if (ENTITY_FLAG_BITS(entity)->turn_in_place) {
            angle = EntityMath_AngleDifference(entity->model->rotation[1], entity->heading_to_player);
            if (ai_state->turn_in_place == 0) {
                ai_state->turn_in_place = ABS(angle) > DEG_TO_RAD(30);
            }
            if (ai_state->turn_in_place != 0) {
                entity->move_command = EntityAi_TurnCommand(entity);
                if (entity->move_command == 0) {
                    ai_state->turn_in_place = 0;
                }
            }
        } else {
            turn = EntityAi_TurnFromPush(entity, direction);
            if (turn > 0) {
                entity->move_command = (MOVE_COMMAND_BACK | MOVE_COMMAND_TURN_RIGHT);
            } else if (turn < 0) {
                entity->move_command = (MOVE_COMMAND_BACK | MOVE_COMMAND_TURN_LEFT);
            } else {
                entity->move_command = FireElemental_BackwardsCommand(entity, 1, entity->radius + 100.0f + 30.0f);
            }
        }
    }
    EntityAi_StartAction(entity, ai_state->move, run_now);
}

/* An action (busy bit 0x1000) that FireElemental_Think starts in state 2 while ai_state.step_index is 0:
   plays animation 17 turning to the player, then stays in its last step and decides each frame. It
   sets step_index and starts FireElemental_Dodge when FireElemental_ShouldDodgeMagic or FireElemental_ShouldDodge
   says so, or EntityAi_MeleeAttack with record 1 when the player is within EntityParams.near_distance plus
   100 plus the radius and 20 degrees ahead. Each time the timer runs out it starts
   FireElemental_RangedAttack0 (and sets step_index) when the pause in ai_state.sub_timer is over and the
   player is within 20 degrees ahead, starts FireElemental_PlayTurnAnim when the player is more than
   90 degrees off, and else turns towards the player in animation 4 or stands in animation 0 for 30
   to 59 frames. */
static void FireElemental_StandAndAttack(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 direction[4];
    s8 turn;

    entity->busy_flags |= ENTITY_BUSY_1000;
    switch (entity->action_step) {
    case ACTION_STEP_START:
        Motion_Start(entity, 0x11, 0, entity->params->motion_blend_frames, 0);
        ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
        ai_state->sub_timer = entity->params->far_distance / 2 + rand() % (entity->params->far_distance / 2);
        ai_state->turn_rate = 0.0f;
        ENTITY_FLAG_BITS(entity)->look_at_target = 0;
        entity->action_step++;
    case ACTION_STEP_BLEND_IN:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        EntityAi_TurnToHeading(entity, &ai_state->turn_rate, entity->heading_to_player);
        break;
    case FIRE_ELEMENTAL_STAND_STEP_WATCH:
        if (ai_state->sub_timer > 0) {
            ai_state->sub_timer--;
        }
        ENTITY_FLAG_BITS(entity)->look_at_target = 1;
        if (FireElemental_ShouldDodgeMagic(entity, 200.0f, DEG_TO_RAD(60)) || FireElemental_ShouldDodge(entity, 300.0f, DEG_TO_RAD(30))) {
            ai_state->step_index = 1;
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            EntityAi_StartAction(entity, FireElemental_Dodge, 1);
            break;
        }
        if (entity->distance_to_player < (f32)entity->params->near_distance + 100.0f + entity->radius &&
            ABS(EntityMath_WrapAngle(entity->heading_to_player - entity->model->rotation[1])) < DEG_TO_RAD(20)) {
            ai_state->step_index = 1;
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            ENTITY_FLAG_BITS(entity)->attack_index = 1;
            EntityAi_StartAction(entity, EntityAi_MeleeAttack, 1);
            break;
        }
        if (entity->action_timer == 0) {
            if (ai_state->sub_timer == 0 && ABS(EntityMath_WrapAngle(entity->heading_to_player - entity->model->rotation[1])) < DEG_TO_RAD(20)) {
                ai_state->step_index = 1;
                ai_state->sub_timer = entity->params->far_distance / 2 + rand() % (entity->params->far_distance / 2);
                entity->busy_flags = 0;
                entity->move_command = 0;
                entity->action_timer = 0;
                EntityAi_StartAction(entity, FireElemental_RangedAttack0, 1);
                break;
            }
            if (ABS(EntityMath_WrapAngle(entity->heading_to_player - entity->model->rotation[1])) > HALF_PI) {
                entity->busy_flags = 0;
                entity->move_command = 0;
                entity->action_timer = 0;
                ai_state->action_flags = EntityMath_WrapAngle(entity->heading_to_player - entity->model->rotation[1]) > 0.0f ? 1 : -1;
                EntityAi_StartAction(entity, FireElemental_PlayTurnAnim, 1);
                break;
            }
            if (ABS(EntityMath_WrapAngle(entity->heading_to_player - entity->model->rotation[1])) > DEG_TO_RAD(20)) {
                entity->target = &player_state;
                entity->action_timer = rand() % 30 + 30;
                ai_state->move_choice = MOVE_CHOICE_CLOSE_IN;
            } else {
                entity->action_timer = rand() % 30 + 30;
                ai_state->move_choice = MOVE_CHOICE_NONE;
            }
        }
        turn = EntityAi_TurnFromPush(entity, direction);
        switch (ai_state->move_choice) {
        case MOVE_CHOICE_CLOSE_IN:
            Motion_Start(entity, 4, 0, entity->params->motion_blend_frames, 0);
            if (turn > 0) {
                entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_RUN | MOVE_COMMAND_TURN_RIGHT);
            } else if (turn < 0) {
                entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_RUN | MOVE_COMMAND_TURN_LEFT);
            } else {
                entity->move_command = EntityAi_MoveCommand(entity, 1, entity->radius + 100.0f + 30.0f);
            }
            if (entity->move_command & MOVE_COMMAND_TURN_LEFT) {
                ai_state->turn_rate -= entity->params->turn_step_per_frame;
                if (ai_state->turn_rate < -entity->params->turn_limit) {
                    ai_state->turn_rate = -entity->params->turn_limit;
                }
            } else if (entity->move_command & MOVE_COMMAND_TURN_RIGHT) {
                ai_state->turn_rate += entity->params->turn_step_per_frame;
                if (entity->params->turn_limit < ai_state->turn_rate) {
                    ai_state->turn_rate = entity->params->turn_limit;
                }
            } else {
                ai_state->turn_rate = 0.0f;
            }
            direction[0] = ai_state->turn_rate;
            if (entity->move_command & MOVE_COMMAND_TURN_FAST) {
                direction[0] += direction[0];
            }
            entity->turn_delta_yaw += direction[0];
            ai_state->move_speed = 0.0f;
            break;
        case MOVE_CHOICE_NONE:
            Motion_Start(entity, 0, 0, entity->params->motion_blend_frames, 0);
            ai_state->turn_rate = 0.0f;
            ai_state->move_speed = 0.0f;
            break;
        }
        break;
    }
}

/* An action (busy bit 0x2000): plays animation 12 or, with ai_state.action_flags below 1, animation 11 to
   its end. FireElemental_StandAndAttack starts it when the player is more than 90 degrees off the
   heading. */
static void FireElemental_PlayTurnAnim(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;

    entity->busy_flags |= ENTITY_BUSY_2000;
    switch (entity->action_step) {
    case ACTION_STEP_START:
        if (ai_state->action_flags > 0) {
            Motion_Start(entity, 0xC, 0, entity->params->motion_blend_frames, 1);
        } else {
            Motion_Start(entity, 0xB, 0, entity->params->motion_blend_frames, 1);
        }
        entity->flags = (entity->flags & ~ENTITY_FLAG_MOTION_STEP_MODE_MASK) | ENTITY_FLAG_MOTION_STEP_MODE_1;
        entity->flags &= ~ENTITY_FLAG_LOOK_AT_TARGET;
        entity->action_step++;
    case ACTION_STEP_BLEND_IN:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        break;
    case ACTION_STEP_PLAY:
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            if (ENTITY_FLAG_BITS(entity)->own_think) {
                ai_state->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, ai_state->move, 1);
            }
        }
        break;
    }
}

/* The dodge action (busy bit 8): as EntityAi_Dodge; when it ends, one time in two and with the
   player within 30 degrees of the heading, it starts FireElemental_RangedAttack1 with record 1.
   FireElemental_StandAndAttack starts it; the think function uses EntityAi_Dodge. */
static void FireElemental_Dodge(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;

    entity->busy_flags |= ENTITY_BUSY_DODGE;
    switch (entity->action_step) {
    case ACTION_STEP_START:
        Motion_Start(entity, 0xA, 0, 5, 1);
        ENTITY_FLAG_BITS(entity)->motion_step_mode = (ENTITY_PARAMS_MOVEMENT_KIND(entity->params) == ENTITY_PARAMS_MOVEMENT_1) ? MOTION_STEP_MOVE_FLAT : MOTION_STEP_MOVE;
        ai_state->turn_rate = 0.0f;
        ENTITY_FLAGS(entity) &= ~ENTITY_FLAG_LOOK_AT_TARGET;
        entity->action_step++;
        break;
    case ACTION_STEP_BLEND_IN:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        break;
    case ACTION_STEP_PLAY:
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            if (!(rand() & 1) && ABS(EntityMath_WrapAngle(entity->heading_to_player - entity->model->rotation[1])) < DEG_TO_RAD(30)) {
                ENTITY_FLAGS(entity) = (ENTITY_FLAGS(entity) & ~(3L << 58)) | (1L << 58);
                EntityAi_StartAction(entity, FireElemental_RangedAttack1, 1);
            } else if (ENTITY_FLAG_BITS(entity)->own_think) {
                ai_state->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, ai_state->move, 1);
            }
        }
        break;
    }
}

/* Whether to dodge a magic: only while ai_state.action_counter runs (FireElemental_Think puts it to 30 when the player
   casts one, player_state.cast_launched_shot), with the player at least min_distance away (less the radius and 100),
   Entity.heading_to_player within max_angle of the heading and the entity within 30 degrees of the player's facing; then
   EntityParams.dodge_chance / 10 times in 100. */
static s32 FireElemental_ShouldDodgeMagic(Entity *entity, f32 min_distance, f32 max_angle) {
    f32 distance;
    f32 angle;
    f32 player_angle;

    if (entity->ai_state.action_counter <= 0) {
        return FALSE;
    }
    distance = entity->distance_to_player - (entity->radius + 100.0f);
    angle = EntityMath_AngleDifference(entity->model->rotation[1], entity->heading_to_player);
    player_angle = EntityMath_AngleDifference(player_state.rotation.y, EntityMath_AngleXZ(&player_state.position.x, entity->model->position));
    if (distance < min_distance) {
        return FALSE;
    }
    if (ABS(angle) > max_angle) {
        return FALSE;
    }
    if (ABS(player_angle) > DEG_TO_RAD(30)) {
        return FALSE;
    }
    return rand() % 100 < entity->params->dodge_chance / 10;
}

/* Helper of FireElemental_StandAndAttack: whether to dodge the player's swing: the player within
   max_distance and max_angle of the heading, player_weapon_step[0] set and the motion of held_item_model_ptr before
   frame 6, then a chance of EntityParams.dodge_chance / 5 in 100. */
static s32 FireElemental_ShouldDodge(Entity *entity, f32 max_distance, f32 max_angle) {
    f32 distance;
    f32 angle;

    distance = entity->distance_to_player - (entity->radius + 100.0f);
    angle = EntityMath_AngleDifference(entity->model->rotation[1], entity->heading_to_player);
    if (distance > max_distance) {
        return FALSE;
    }
    if (ABS(angle) > max_angle) {
        return FALSE;
    }
    if (player_weapon_step[0] == 0 || held_item_model_ptr->level_tables->frame >= 6) {
        return FALSE;
    }
    return rand() % 100 < entity->params->dodge_chance / 5;
}

/* The ranged attack with record 1 (busy bit 1): it takes the record from flag bits 58-59 as
   EntityAi_RangedAttack does, but every place that starts it does so for record 1. It turns to the
   player all through the blend and launches the shots with EntityShot_Launch along the reverse of its
   heading plus 5 degrees. */
static void FireElemental_RangedAttack1(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    s32 attack_index = ENTITY_FLAG_BITS(entity)->attack_index;
    EntityParamsUnk3D8 *unk3D8 = ENTITY_PARAMS_UNK3D8(entity->params, attack_index);
    f32 direction[4];
    s32 elapsed;

    entity->busy_flags |= ENTITY_BUSY_ATTACK;
    switch (entity->action_step) {
    case ACTION_STEP_START:
        Motion_Start(entity, attack_index + ENTITY_MOTION_RANGED_ATTACK, 0, entity->params->motion_blend_frames >> 1, 1);
        if (entity->params->flags & ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE) {
            ENTITY_FLAGS(entity) &= ~ENTITY_FLAG_MOTION_STEP_MODE_MASK;
        } else {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = (ENTITY_PARAMS_MOVEMENT_KIND(entity->params) == ENTITY_PARAMS_MOVEMENT_1) ? MOTION_STEP_MOVE_FLAT : MOTION_STEP_MOVE;
        }
        ENTITY_FLAGS(entity) &= ~ENTITY_FLAG_LOOK_AT_TARGET;
        entity->action_step++;
    case ACTION_STEP_BLEND_IN:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        EntityAi_TurnToHeading(entity, &ai_state->turn_rate, entity->heading_to_player);
        break;
    case ACTION_STEP_PLAY:
        if (unk3D8->turn_frames >= entity->model->level_tables->frame) {
            EntityAi_TurnToHeading(entity, &ai_state->turn_rate, entity->heading_to_player);
        } else {
            ai_state->turn_rate = 0.0f;
        }
        elapsed = (entity->model->level_tables->frame_fixed - entity->params->motion_speeds[entity->animation_index]) >> 8;
        Vector_Copy(direction, entity->model->rotation);
        direction[1] = EntityMath_WrapAngle(entity->model->rotation[1] - PI + DEG_TO_RAD(5));
        EntityShot_Launch(entity, unk3D8, elapsed, entity->model->level_tables->frame, direction);
        if (entity->model->level_tables->frame > 0 && unk3D8->attack_end_frame >= entity->model->level_tables->frame) {
            ENTITY_FLAGS(entity) |= ENTITY_FLAG_ATTACK_WINDOW;
        } else {
            ENTITY_FLAGS(entity) &= ~ENTITY_FLAG_ATTACK_WINDOW;
        }
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            EntityAi_SetAttackDelay(entity, unk3D8->attack_delay);
            if (ENTITY_FLAG_BITS(entity)->own_think) {
                ai_state->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, ai_state->move, 1);
            }
        }
        break;
    }
}

/* Helper of FireElemental_OnHit: sets ai_state.step_index and pause_timer to 1 in every other live entity
   with the same EntityParams that is within EntityParams.action_frames of this one, has pause_timer at 0 and has
   the player within EntitySpawnInfo.wander_radius of its spawn point. */
static void FireElemental_AlertSameKind(Entity *entity) {
    Entity *other;
    EntityAiState *ai_state;

    for (other = active_entities_head; other != NULL; other = other->next) {
        if (entity->params == other->params && ((s32)other->flags & ENTITY_FLAG_ACTIVE_LIST) && (ENTITY_FLAG_BITS(other)->live)
            && ((s32)(other->flags >> 2) & 3) == 0 && other != entity) {
            ai_state = &other->ai_state;
            if (ai_state->pause_timer == 0
                && EntityMath_DistanceXZ(entity->model->position, other->model->position) < (s16)entity->params->action_frames
                && EntityMath_DistanceXZ((float *)other->spawn_info, &player_state.position.x) < other->spawn_info->wander_radius) {
                ai_state->step_index = ai_state->pause_timer = 1;
            }
        }
    }
}

/* The on_hit handler: as EntityAi_OnHit, then FireElemental_AlertSameKind. */
static void FireElemental_OnHit(Entity *entity) {
    EntityAi_LowerWeightsAfterHit(entity);
    entity->target = NULL;
    ENTITY_FLAG_BITS(entity)->attacking = FALSE;
    ENTITY_FLAG_BITS(entity)->look_at_target = FALSE;
    if (ENTITY_FLAG_BITS(entity)->hit_strength == 1) {
        if (ENTITY_FLAG_BITS(entity)->attack_window) {
            EntityAi_SetReaction(entity, EntityAi_ReactionRewind, 0);
        } else {
            EntityAi_SetReaction(entity, EntityAi_ReactionLight, 0);
        }
    } else if (ENTITY_FLAG_BITS(entity)->hit_strength == 2) {
        EntityAi_SetReaction(entity, EntityAi_ReactionHeavy, 0);
    }
    EntityAi_SetState(entity, 2);
    FireElemental_AlertSameKind(entity);
}

/* An action (busy bit 0x3000) that FireElemental_Think starts while ai_state.pause_timer is 1: plays
   animation 5, then each frame guards or dodges the player's swing as FireElemental_ThinkCombat
   does, starts the attack EntityAi_PickAttack picks once EntityAi_PickCombatChoice chooses attacking (pause_timer becomes
   2; ranged record 1 is FireElemental_RangedAttack1), and else gives the backwards command towards
   the player and calls the move handler itself. Flag bit 53 ends it with pause_timer at 2. */
static void FireElemental_BackAwayFromPlayer(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 direction[4];
    s32 side;

    entity->busy_flags |= (ENTITY_BUSY_1000 | ENTITY_BUSY_2000);
    switch (entity->action_step) {
    case ACTION_STEP_START:
        Motion_Start(entity, 5, 0, entity->params->motion_blend_frames, 0);
        ENTITY_FLAGS(entity) = (ENTITY_FLAGS(entity) & ~ENTITY_FLAG_MOTION_STEP_MODE_MASK) | ENTITY_FLAG_MOTION_STEP_MODE_1;
        ai_state->turn_rate = 0.0f;
        ENTITY_FLAGS(entity) &= ~ENTITY_FLAG_LOOK_AT_TARGET;
        entity->action_step++;
        break;
    case ACTION_STEP_BLEND_IN:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        break;
    case FIRE_ELEMENTAL_BACK_AWAY_STEP_BACK:
        if (FireElemental_ShouldDodgeMagic(entity, 200.0f, DEG_TO_RAD(60))) {
            EntityAi_StartAction(entity, EntityAi_Dodge, 1);
            break;
        }
        if (EntityAi_ShouldGuard(entity, 300.0f, DEG_TO_RAD(30))) {
            EntityAi_StartAction(entity, EntityAi_Guard, 1);
            break;
        }
        if (EntityAi_ShouldDodge(entity, 300.0f, DEG_TO_RAD(30))) {
            EntityAi_StartAction(entity, EntityAi_Dodge, 1);
            break;
        }
        if (ENTITY_FLAG_BITS(entity)->far_from_spawn) {
            ai_state->pause_timer = 2;
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            if (ENTITY_FLAG_BITS(entity)->own_think) {
                ai_state->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, ai_state->move, 1);
            }
        }
        side = EntityAi_PickCombatChoice(entity, 0);
        ENTITY_FLAGS(entity) |= ENTITY_FLAG_LOOK_AT_TARGET;
        if (side == 0) {
            entity->prev_combat_choice = entity->combat_choice;
            entity->combat_choice = COMBAT_CHOICE_ATTACK;
            if (entity->prev_combat_choice != COMBAT_CHOICE_ATTACK) {
                entity->choice_change_frame = frame_counter;
            }
            EntityAi_LowerChoiceWeight(entity, 0);
            if (entity->combat_choice == COMBAT_CHOICE_ATTACK) {
                ai_state->pause_timer = 2;
                if (EntityAi_PickAttack(entity)) {
                    if (ENTITY_FLAG_BITS(entity)->attack_index == 1) {
                        EntityAi_StartAction(entity, FireElemental_RangedAttack1, 1);
                    } else {
                        EntityAi_StartAction(entity, EntityAi_RangedAttack, 1);
                    }
                } else {
                    EntityAi_StartAction(entity, EntityAi_MeleeAttack, 1);
                }
            }
            break;
        }
        entity->target = &player_state;
        side = EntityAi_TurnFromPush(entity, direction);
        if (side > 0) {
            entity->move_command = (MOVE_COMMAND_BACK | MOVE_COMMAND_TURN_RIGHT);
        } else if (side < 0) {
            entity->move_command = (MOVE_COMMAND_BACK | MOVE_COMMAND_TURN_LEFT);
        } else {
            entity->move_command = FireElemental_BackwardsCommand(entity, 1, entity->radius + 100.0f + 30.0f);
        }
        (ai_state->move)(entity);
        break;
    }
}

/* The move handler: as EntityAi_Move, but it only turns the entity and picks the animation, it does
   not move it; going backwards is animation 18. */
static void FireElemental_Move(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 turn;

    if (entity == player_state.stepped_on_entity && (entity->params->flags & ENTITY_PARAMS_FLAG_STOPS_WHEN_STOOD_ON)) {
        entity->move_command = 0;
    }
    if (entity->move_command & MOVE_COMMAND_TURN_LEFT) {
        ai_state->turn_rate -= entity->params->turn_step_per_frame;
        if (ai_state->turn_rate < -entity->params->turn_limit) {
            ai_state->turn_rate = -entity->params->turn_limit;
        }
    } else if (entity->move_command & MOVE_COMMAND_TURN_RIGHT) {
        ai_state->turn_rate += entity->params->turn_step_per_frame;
        if (ai_state->turn_rate > entity->params->turn_limit) {
            ai_state->turn_rate = entity->params->turn_limit;
        }
    } else {
        ai_state->turn_rate = 0.0f;
    }
    turn = ai_state->turn_rate;
    if (entity->move_command & MOVE_COMMAND_TURN_FAST) {
        turn += turn;
    }
    entity->turn_delta_yaw += turn;
    ai_state->move_speed = 0.0f;
    if (entity->move_command & MOVE_COMMAND_FORWARD) {
        if (entity->move_command & MOVE_COMMAND_RUN) {
            Motion_Start(entity, 4, 0, entity->params->motion_blend_frames >> 1, 0);
        } else {
            Motion_Start(entity, 3, 0, entity->params->motion_blend_frames, 0);
        }
    } else if (entity->move_command & MOVE_COMMAND_BACK) {
        Motion_Start(entity, 0x12, 0, entity->params->motion_blend_frames, 0);
    } else if (entity->move_command & MOVE_COMMAND_DOWN) {
        Motion_Start(entity, 8, 0, entity->params->motion_blend_frames, 0);
    } else if (entity->move_command & MOVE_COMMAND_UP) {
        Motion_Start(entity, 9, 0, entity->params->motion_blend_frames, 0);
    } else if (entity->move_command & MOVE_COMMAND_STEP_LEFT) {
        Motion_Start(entity, 6, 0, entity->params->motion_blend_frames, 0);
    } else if (entity->move_command & MOVE_COMMAND_STEP_RIGHT) {
        Motion_Start(entity, 7, 0, entity->params->motion_blend_frames, 0);
    } else if (entity->move_command & MOVE_COMMAND_TURNS) {
        Motion_Start(entity, 3, 0, entity->params->motion_blend_frames, 0);
    } else {
        Motion_Start(entity, 0, 0, entity->params->motion_blend_frames, 0);
    }
}

/* The movement command for going backwards: a turn bit (0x100 / 0x200) while the target is off the heading by
   more than the turn limit, and command bit 2 (backwards) while it is farther than reach; 0 without a target.
   The second parameter is not used. */
static s32 FireElemental_BackwardsCommand(Entity *entity, s32 unused, f32 reach) {
    f32 turn;
    f32 limit;
    f32 heading;
    s32 command;

    command = 0;
    if (entity->target == NULL) {
        return 0;
    }
    heading = EntityMath_HeadingTo(entity->model->position, entity->target);
    turn = EntityMath_AngleDifference(entity->model->rotation[1], heading);
    limit = entity->params->turn_limit;
    if (0.0f <= turn ? limit < turn : limit < -turn) {
        command = 0x100;
        if (0.0f <= turn) {
            command = 0x200;
        }
    }
    if (reach < EntityMath_DistanceXZ(entity->model->position, entity->target)) {
        command |= MOVE_COMMAND_BACK;
    }
    return command;
}

/* The think function while flag bit 53 is set: as EntityAi_ThinkReturn (walk back to the spawn
   point, turning away from whatever is pushing the entity), but with the backwards command of
   FireElemental_BackwardsCommand where the shared one gives the forwards one. */
static void FireElemental_ThinkReturn(Entity *entity, s32 run_now) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 to_spawn[4];
    f32 push[4];
    f32 direction[4];
    s32 turn;

    entity->action_timer = 0;
    ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
    ENTITY_FLAG_BITS(entity)->attacking = FALSE;
    ENTITY_FLAG_BITS(entity)->look_at_target = FALSE;
    turn = EntityAi_TurnFromPush(entity, direction);
    if (turn == 0) {
        entity->target = entity->spawn_info;
        entity->move_command = FireElemental_BackwardsCommand(entity, 1, entity->radius + 100.0f + 30.0f);
    } else {
        push[0] = push[1] = push[2] = push[3] = 0.0f;
        if (entity->flags & (ENTITY_FLAG_WALL_PUSH | ENTITY_FLAG_400000000000)) {
            push[0] += entity->wall_push[0];
            push[2] += entity->wall_push[2];
        }
        if (ENTITY_FLAG_BITS(entity)->entity_push) {
            push[0] += entity->entity_push[0];
            push[2] += entity->entity_push[2];
        }
        Vector_Normalize(push, push);
        Vector_Subtract(to_spawn, entity->spawn_info, entity->model->position);
        Vector_Normalize(to_spawn, to_spawn);
        Vector_Cross(to_spawn, push, to_spawn);
        if (to_spawn[1] <= 0.0f) {
            if (turn == -1) {
                turn = 1;
            }
        } else if (turn == 1) {
            turn = -1;
        }
        entity->target = entity->spawn_info;
        if (turn > 0) {
            entity->move_command = (MOVE_COMMAND_BACK | MOVE_COMMAND_TURN_RIGHT);
        } else if (turn < 0) {
            entity->move_command = (MOVE_COMMAND_BACK | MOVE_COMMAND_TURN_LEFT);
        } else {
            entity->move_command = FireElemental_BackwardsCommand(entity, 1, entity->radius + 100.0f + 30.0f);
        }
    }
    EntityAi_StartAction(entity, ai_state->move, run_now);
}

/* The ranged attack with record 0 (busy bit 1): animation 23, as EntityAi_RangedAttack, but the
   shots are launched with EntityShot_Launch along the reverse of its heading. FireElemental_StandAndAttack
   starts it. */
static void FireElemental_RangedAttack0(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    EntityParamsUnk3D8 *unk3D8 = &entity->params->ranged_attacks[0].record;
    f32 direction[4];
    s32 elapsed;

    entity->busy_flags |= ENTITY_BUSY_ATTACK;
    switch (entity->action_step) {
    case ACTION_STEP_START:
        Motion_Start(entity, 0x17, 0, entity->params->motion_blend_frames >> 1, 1);
        ENTITY_FLAGS(entity) = (ENTITY_FLAGS(entity) & ~ENTITY_FLAG_MOTION_STEP_MODE_MASK) | ENTITY_FLAG_MOTION_STEP_MODE_1;
        ENTITY_FLAGS(entity) &= ~ENTITY_FLAG_LOOK_AT_TARGET;
        entity->action_step++;
    case ACTION_STEP_BLEND_IN:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        if (unk3D8->turn_frames != 0) {
            EntityAi_TurnToHeading(entity, &ai_state->turn_rate, entity->heading_to_player);
        }
        break;
    case ACTION_STEP_PLAY:
        if (unk3D8->turn_frames >= entity->model->level_tables->frame) {
            EntityAi_TurnToHeading(entity, &ai_state->turn_rate, entity->heading_to_player);
        } else {
            ai_state->turn_rate = 0.0f;
        }
        elapsed = (entity->model->level_tables->frame_fixed - entity->params->motion_speeds[entity->animation_index]) >> 8;
        Vector_Copy(direction, entity->model->rotation);
        direction[1] = EntityMath_WrapAngle(direction[1] - PI);
        EntityShot_Launch(entity, unk3D8, elapsed, entity->model->level_tables->frame, direction);
        if (entity->model->level_tables->frame > 0 && unk3D8->attack_end_frame >= entity->model->level_tables->frame) {
            ENTITY_FLAGS(entity) |= ENTITY_FLAG_ATTACK_WINDOW;
        } else {
            ENTITY_FLAGS(entity) &= ~ENTITY_FLAG_ATTACK_WINDOW;
        }
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            EntityAi_SetAttackDelay(entity, unk3D8->attack_delay);
            if (ENTITY_FLAG_BITS(entity)->own_think) {
                ai_state->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, ai_state->move, 1);
            }
        }
        break;
    }
}
