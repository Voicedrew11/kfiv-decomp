/* WaterElemental: the behaviour of entity type 51 (EntityParams.entity_type, the index into entity_type_handlers): its
   set-up function, which the table names, and the handlers it installs. Used by: Water Elemental. The game
   names no monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm
   records by hit points and experience, with resistances and drops where two monsters share those. */
#include "entity/entity.h"
#include "common.h"
#include "enemy/enemy.h"
#include "entity/entity_ai_handlers.h"
#include "math/vu0.h"
#include "model/motion.h"
#include <stdlib.h>
#include "entity/entity_math.h"
#include "entity/entity_ai.h"
#include "entity/entity_ai_choice.h"
#include "player/player.h"

// Local: entity_math.h says float * for DistanceXZ and AngleXZ, and callers pass the player block.

#define ENTITY_WORK_AREA_SIZE 0xA0

/* WaterElemental_RunAtPlayer: Entity.action_step (steps 0 and 1 are the shared ones) */
#define WATER_ELEMENTAL_RUN_STEP_CHASE 2       /* each frame guards, dodges or runs at the player */

static void WaterElemental_Think(Entity *entity, s32 run_now);
static void WaterElemental_ThinkCombat(Entity *entity, s32 run_now);
static void WaterElemental_PlayTurnAnim(Entity *entity);
static void WaterElemental_RecoverHealth(Entity *entity);
static s32 WaterElemental_ShouldDodgeMagic(Entity *entity, f32 min_distance, f32 max_angle);
static void WaterElemental_AlertSameKind(Entity *entity);
static void WaterElemental_OnHit(Entity *entity);
static void WaterElemental_RunAtPlayer(Entity *entity);

/* Set-up of a type 51 entity: clears its work area and installs the type's handlers. */
void WaterElemental_Init(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    u8 *choice_weights = entity->choice_weights;
    u8 *attack_weights = entity->attack_weights;
    char *unk138 = entity->model->look_at_part;
    s32 i;

    // match: the three fills name their target inside the loop; choice_weights and attack_weights above are
    // otherwise unused but retail computes them up front and copies them before each loop
    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        u8 *work_area = ENTITY_WORK_BYTES(entity);

        work_area[i] = 0;
    }
    EntityAi_SetUpdate(entity, entity_type_handlers[entity->params->entity_type][1]);
    ai_state->think = WaterElemental_Think;
    ai_state->animate = EntityAi_Animate;
    ai_state->on_hit = WaterElemental_OnHit;
    ai_state->move = EntityAi_Move;
    ai_state->dormant = EntityAi_Dormant;
    ai_state->death = EntityAi_Death;
    for (i = 3; i != -1; i--) {
        entity->choice_weights[i] = 0x80;
    }
    for (i = 5; i != -1; i--) {
        entity->attack_weights[i] = 0x80;
    }
    entity->target = NULL;
    entity->flags &= ~ENTITY_FLAG_NO_FLOOR_LIFT;
    if (ENTITY_FLAG_WORD(entity->spawn_info->spawn_flags) & (0x8000L << 43)) {
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
    Motion_StepModel(ENTITY_MODEL(entity), 0, 0);
}

/* The think handler: as EntityAi_Think, with WaterElemental_ThinkCombat for state 2. ai_state.action_flags
   is put to 30 while player_state.cast_launched_shot is set and counts down. While ai_state.pause_timer is 1
   (WaterElemental_AlertSameKind of another entity sets it; states 0 and 1 clear it) it only starts
   WaterElemental_RunAtPlayer. Once the pause in ai_state.mode_timer is over and Entity.hit_points is below
   EntityParams.near_distance percent of EntityParams.hit_points it starts WaterElemental_RecoverHealth with a
   chance of far_distance in 100. */
static void WaterElemental_Think(Entity *entity, s32 run_now) {
    EntityAiState *ai_state;

    ai_state = &entity->ai_state;
    if (frame_counter % 10U == entity->spawn_index % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (ai_state->mode_timer > 0) {
        ai_state->mode_timer--;
    }
    if (player_state.cast_launched_shot != 0) {
        ai_state->action_flags = 30;
    }
    if (ai_state->action_flags > 0) {
        ai_state->action_flags--;
    }
    if (ai_state->pause_timer == 1) {
        if (entity->busy_flags == 0) {
            EntityAi_StartAction(entity, WaterElemental_RunAtPlayer, 1);
        }
        return;
    }
    if (entity->busy_flags == 0) {
        EntityAi_CheckFarFromSpawn(entity);
        EntityAi_StepState(entity);
        if (ai_state->mode_timer == 0 && (s32)(entity->hit_points * 100.0f / entity->params->hit_points) < entity->params->near_distance
            && rand() % 100 < entity->params->far_distance) {
            EntityAi_StartAction(entity, WaterElemental_RecoverHealth, 1);
            return;
        }
        if (ENTITY_FLAG_BITS(entity)->far_from_spawn) {
            EntityAi_ThinkReturn(entity, run_now);
        } else {
            switch (entity->state) {
            case ENTITY_STATE_UNAWARE:
                ai_state->pause_timer = 0;
                EntityAi_ThinkIdle(entity, run_now);
                break;
            case ENTITY_STATE_SEARCHING:
                ai_state->pause_timer = 0;
                EntityAi_ThinkPursue(entity, run_now);
                break;
            case ENTITY_STATE_NOTICED:
                WaterElemental_ThinkCombat(entity, run_now);
                break;
            default:
                EntityAi_ThinkIdle(entity, run_now);
                break;
            }
        }
    }
}

/* The think function of state 2: as EntityAi_ThinkCombat, but it first starts EntityAi_Dodge when
   WaterElemental_ShouldDodgeMagic says so, or on every fifteenth frame with the player nearer than the radius plus
   400 and less than 90 degrees off its heading. Closing in is at the run; it starts
   WaterElemental_PlayTurnAnim now and then when the player is more than 90 degrees off its heading.
   */
static void WaterElemental_ThinkCombat(Entity *entity, s32 run_now) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 direction[4];
    f32 angle;
    s32 mode;
    s32 side;

    if (entity->params->flags & ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE) {
        entity->flags &= ~ENTITY_FLAG_MOTION_STEP_MODE_MASK;
    } else {
        entity->flags = (entity->flags & ~ENTITY_FLAG_MOTION_STEP_MODE_MASK) | ENTITY_FLAG_MOTION_STEP_MODE_1;
    }
    if ((entity->distance_to_player < entity->radius + 400.0f
         && ABS(EntityMath_WrapAngle(entity->heading_to_player - entity->model->rotation[1])) < HALF_PI && frame_counter % 15U == 0
         && rand() % 50 < 80)
        || WaterElemental_ShouldDodgeMagic(entity, 200.0f, DEG_TO_RAD(60))) {
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
    mode = EntityAi_PickCombatChoice(entity, 0);
    entity->flags |= ENTITY_FLAG_LOOK_AT_TARGET;
    if (entity->action_timer == 0 || mode == 0) {
        entity->prev_combat_choice = entity->combat_choice;
        entity->combat_choice = mode;
        if (entity->combat_choice != entity->prev_combat_choice) {
            entity->choice_change_frame = frame_counter;
        }
        EntityAi_LowerChoiceWeight(entity, mode);
        switch (entity->combat_choice) {
        case COMBAT_CHOICE_ATTACK:
            if (EntityAi_PickAttack(entity)) {
                EntityAi_StartAction(entity, EntityAi_RangedAttack, run_now);
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
            side = EntityAi_TurnFromPush(entity, direction);
            if (side > 0) {
                entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_RUN | MOVE_COMMAND_TURN_RIGHT);
            } else if (side < 0) {
                entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_RUN | MOVE_COMMAND_TURN_LEFT);
            } else {
                if (ABS(EntityMath_WrapAngle(entity->heading_to_player - entity->model->rotation[1])) > HALF_PI
                    && frame_counter % 15U == 0 && !(rand() & 1)) {
                    ai_state->step_index = (EntityMath_WrapAngle(entity->heading_to_player - entity->model->rotation[1]) > 0.0f) ? 1 : -1;
                    EntityAi_StartAction(entity, WaterElemental_PlayTurnAnim, run_now);
                    return;
                }
                entity->move_command = EntityAi_MoveCommand(entity, 1, entity->radius + 100.0f + 30.0f);
            }
        }
    }
    EntityAi_StartAction(entity, ai_state->move, run_now);
}

/* An action (busy bit 0x2000): plays animation 12 or, with ai_state.step_index below 1, animation 11 to
   its end. WaterElemental_ThinkCombat starts it when the player is more than 90 degrees off the
   heading. */
static void WaterElemental_PlayTurnAnim(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;

    entity->busy_flags |= ENTITY_BUSY_2000;
    switch (entity->action_step) {
    case ACTION_STEP_START:
        if (ai_state->step_index > 0) {
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

/* An action (busy bit 0x1000) that WaterElemental_Think starts when Entity.hit_points is below
   EntityParams.near_distance percent of EntityParams.hit_points: plays animation 18 to its end; on frame
   EntityParams.type_param_2 it adds EntityParams.type_param_0 percent of EntityParams.hit_points to Entity.hit_points, once, and sets
   the pause in ai_state.mode_timer to type_param_1. Entity.hit_points is the entity's health: Entity_ResetToSpawn
   loads it from EntityParams.hit_points, Attack_HitEntity takes the damage of a hit off it, and
   EntityAi_Update starts the death action when it is 0. */
static void WaterElemental_RecoverHealth(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;

    entity->busy_flags |= ENTITY_BUSY_1000;
    switch (entity->action_step) {
    case ACTION_STEP_START:
        Motion_Start(entity, 0x12, 0, entity->params->motion_blend_frames >> 1, 1);
        ENTITY_FLAGS(entity) = (ENTITY_FLAGS(entity) & ~ENTITY_FLAG_MOTION_STEP_MODE_MASK) | ENTITY_FLAG_MOTION_STEP_MODE_1;
        ai_state->turn_rate = 0.0f;
        ENTITY_FLAGS(entity) &= ~ENTITY_FLAG_LOOK_AT_TARGET;
        ai_state->action_counter = 0;
        entity->action_step++;
        break;
    case ACTION_STEP_BLEND_IN:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        break;
    case ACTION_STEP_PLAY:
        if (ai_state->action_counter == 0 && entity->model->level_tables->frame == entity->params->type_param_2) {
            entity->hit_points += (s32)(entity->params->hit_points * (f32)entity->params->type_param_0 / 100.0f);
            ai_state->mode_timer = entity->params->type_param_1;
            ai_state->action_counter = 1;
        }
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

/* Whether to dodge a magic: only while ai_state.action_flags is 1 to 15 (WaterElemental_Think puts it to 30 when the
   player casts one, player_state.cast_launched_shot), with the player at least min_distance away (less the radius and 100),
   Entity.heading_to_player within max_angle of the heading and the entity within 30 degrees of the player's facing; then
   EntityParams.dodge_chance / 10 times in 100. */
static s32 WaterElemental_ShouldDodgeMagic(Entity *entity, f32 min_distance, f32 max_angle) {
    f32 distance;
    f32 angle;
    f32 player_angle;

    if ((u8)(entity->ai_state.action_flags - 1) >= 15) {
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

/* Helper of WaterElemental_OnHit: sets ai_state.pause_timer to 1 in every other live entity with the same
   EntityParams that is within EntityParams.action_frames of this one, has ai_state.pause_timer at 0 and has the player
   within EntitySpawnInfo.wander_radius of its spawn point. */
static void WaterElemental_AlertSameKind(Entity *entity) {
    Entity *other;

    for (other = active_entities_head; other != NULL; other = other->next) {
        if (entity->params == other->params && ((s32)other->flags & ENTITY_FLAG_ACTIVE_LIST) && (ENTITY_FLAG_BITS(other)->live)
            && ((s32)(other->flags >> 2) & 3) == 0 && other != entity) {
            EntityAiState *ai_state = &other->ai_state;

            if (ai_state->pause_timer == 0
                && EntityMath_DistanceXZ(entity->model->position, other->model->position)
                       < (s16)entity->params->action_frames
                && EntityMath_DistanceXZ(&other->spawn_info->spawn_x, &player_state.position.x) < other->spawn_info->wander_radius) {
                ai_state->pause_timer = 1;
            }
        }
    }
}

/* The on_hit handler: as EntityAi_OnHit, then WaterElemental_AlertSameKind. */
static void WaterElemental_OnHit(Entity *entity) {
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
    WaterElemental_AlertSameKind(entity);
}

/* An action (busy bit 0x3000) that WaterElemental_Think starts while ai_state.pause_timer is 1: plays
   animation 3, then each frame guards or dodges the player's swing, starts the attack EntityAi_PickAttack
   picks once EntityAi_PickCombatChoice chooses attacking (pause_timer becomes 2), and else gives the command to run
   at the player and calls the move handler itself. Flag bit 53 ends it with pause_timer at 2. */
static void WaterElemental_RunAtPlayer(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 direction[4];
    s32 side;

    entity->busy_flags |= (ENTITY_BUSY_1000 | ENTITY_BUSY_2000);
    switch (entity->action_step) {
    case ACTION_STEP_START:
        Motion_Start(entity, 3, 0, entity->params->motion_blend_frames, 0);
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
    case WATER_ELEMENTAL_RUN_STEP_CHASE:
        if (WaterElemental_ShouldDodgeMagic(entity, 200.0f, DEG_TO_RAD(60))) {
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
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            ai_state->pause_timer = 2;
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
                    EntityAi_StartAction(entity, EntityAi_RangedAttack, 1);
                } else {
                    EntityAi_StartAction(entity, EntityAi_MeleeAttack, 1);
                }
            }
            break;
        }
        entity->target = &player_state;
        side = EntityAi_TurnFromPush(entity, direction);
        if (side > 0) {
            entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_RUN | MOVE_COMMAND_TURN_RIGHT);
        } else if (side < 0) {
            entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_RUN | MOVE_COMMAND_TURN_LEFT);
        } else {
            entity->move_command = EntityAi_MoveCommand(entity, 1, entity->radius + 100.0f + 30.0f);
        }
        (ai_state->move)(entity);
        break;
    }
}
