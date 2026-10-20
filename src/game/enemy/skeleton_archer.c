/* SkeletonArcher: the behaviour of entity type 9 (EntityParams.entity_type, the index into entity_type_handlers): its
   set-up function, which the table names, and the handlers it installs. Used by: Skeleton Archer Small,
   Skeleton Archer Large. The game names no monsters; these names are from
   https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by hit points and experience, with
   resistances and drops where two monsters share those. */
#include "entity/entity.h"
#include "common.h"
#include "enemy/enemy.h"
#include "entity/entity_ai_handlers.h"
#include "entity/entity_math.h"
#include <math.h>
#include "math/vu0.h"
#include <stdlib.h>
#include "entity/entity_ai_choice.h"
#include "entity/entity_ai.h"
#include "model/motion.h"
#include "player/player.h"


static void SkeletonArcher_Animate(Entity *entity);
static void SkeletonArcher_PitchPartToPlayer(Entity *entity);
static void SkeletonArcher_Think(Entity *entity, s32 run_now);
static void SkeletonArcher_ThinkCombat(Entity *entity, s32 run_now);
static void SkeletonArcher_RangedAttack0(Entity *entity);

/* Set-up of a type 9 entity: clears its work area and installs the type's handlers. */
void SkeletonArcher_Init(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    u8 *unkBC = entity->choice_weights;
    u8 *unkC0 = entity->attack_weights;
    char *unk138 = entity->model->look_at_part;
    s32 i;

    // match: in the three loops the pointer has to be formed inside the loop
    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        u8 *dest = (u8 *)&entity->ai_state;

        dest[i] = 0;
    }
    EntityAi_SetUpdate(entity, entity_type_handlers[entity->params->entity_type][1]);
    ai_state->think = SkeletonArcher_Think;
    ai_state->animate = SkeletonArcher_Animate;
    ai_state->on_hit = EntityAi_OnHit;
    ai_state->move = EntityAi_Move;
    ai_state->dormant = EntityAi_Dormant;
    ai_state->death = EntityAi_Death;
    for (i = 4 - 1; i != -1; i--) {
        entity->choice_weights[i] = 0x80;
    }
    for (i = 6 - 1; i != -1; i--) {
        entity->attack_weights[i] = 0x80;
    }
    ENTITY_UNK5C(entity)->as_s32 = 0;
    ENTITY_FLAG_BITS(entity)->no_floor_lift = FALSE;
    if (entity->spawn_info->spawn_flags & (0x8000L << 43)) {
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

/* The animate handler: as EntityAi_Animate; during animation 23 of an attack (busy bit 1), once
   blended in, SkeletonArcher_PitchPartToPlayer pitches model part 11, and otherwise work float unk48 is
   cleared. */
static void SkeletonArcher_Animate(Entity *entity) {
    f32 limits[4];
    s32 look_at_target;
    u32 blend_frames; // match: converted to float as unsigned
    EntityAiState *ai_state;

    ai_state = &entity->ai_state;
    if (ENTITY_FLAG_BITS(entity)->animates) {
        blend_frames = entity->model->level_tables->blend_left;
        if (blend_frames != 0) {
            ENTITY_FLAG_BITS(entity)->motion_ended = Motion_BlendToPose(ENTITY_MODEL(entity), (entity->busy_flags & ENTITY_BUSY_REACTION_LIGHT) ? ai_state->pose_blend_weight : 1.0f / (f32)blend_frames);
        } else {
            ENTITY_FLAG_BITS(entity)->motion_ended = Motion_Step(entity, ENTITY_FLAG_BITS(entity)->motion_step_mode);
        }
        if ((entity->busy_flags & ENTITY_BUSY_ATTACK) && entity->animation_index == 0x17 && entity->model->level_tables->blend_left == 0) {
            SkeletonArcher_PitchPartToPlayer(entity);
        } else {
            ai_state->part_angle_x = 0.0f;
        }
        limits[0] = DEG_TO_RAD(-entity->params->pitch_limit_deg);
        limits[1] = DEG_TO_RAD(entity->params->pitch_limit_deg);
        limits[2] = DEG_TO_RAD(-entity->params->yaw_limit_deg);
        limits[3] = DEG_TO_RAD(entity->params->yaw_limit_deg);
        look_at_target = (s32)(entity->flags >> 9);
        if (look_at_target & 1) {
            EntityAi_LookAtPlayer(entity, ai_state->look_part_angles, look_at_target & 1, limits, 12.0f);
        } else {
            EntityAi_LookAtPlayer(entity, ai_state->look_part_angles, 0, NULL, 8.0f);
        }
    }
}

/* Helper of SkeletonArcher_Animate: pitches model part 11 towards the player, at most 50 degrees
   either way (work float unk48, rate 10). */
static void SkeletonArcher_PitchPartToPlayer(Entity *entity) {
    EntityModel *model = entity->model;
    EntityModelPart80 *part = &((EntityModelPart80 *)model->parts)[11];
    EntityAiState *ai_state = &entity->ai_state;
    f32 pitch;

    if (part != NULL) {
        pitch = EntityMath_WrapAngle(atan2(player_state.position.y - model->position[1], entity->distance_to_player));
        if (DEG_TO_RAD(50) < pitch) {
            pitch = DEG_TO_RAD(50);
        } else if (pitch < -DEG_TO_RAD(50)) {
            pitch = -DEG_TO_RAD(50);
        }
        EntityMath_EaseAngleToward(&ai_state->part_angle_x, pitch, 10.0f);
        part->part_angles[0] = ai_state->part_angle_x;
    }
}

/* The think handler: as EntityAi_Think, with SkeletonArcher_ThinkCombat for state 2. */
static void SkeletonArcher_Think(Entity *entity, s32 run_now) {
    if (frame_counter % 10U == entity->spawn_index % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (entity->busy_flags != 0) {
        return;
    }
    EntityAi_CheckFarFromSpawn(entity);
    EntityAi_StepState(entity);
    if (ENTITY_FLAG_BITS(entity)->far_from_spawn) {
        EntityAi_ThinkReturn(entity, run_now);
        return;
    }
    switch (entity->state) {
    case 0:
        EntityAi_ThinkIdle(entity, run_now);
        return;
    case 1:
        EntityAi_ThinkPursue(entity, run_now);
        return;
    case 2:
        SkeletonArcher_ThinkCombat(entity, run_now);
        return;
    }
    EntityAi_ThinkIdle(entity, run_now);
}

/* The think function of state 2: as EntityAi_ThinkCombat, but ranged record 0 is
   SkeletonArcher_RangedAttack0 (flag bit 10 is set for a ranged attack), and closing in does not use
   EntityAi_ClearTurnInPlace, takes its command from EntityAi_TurnCommand where the shared one calls EntityAi_MoveCommand, and
   never runs. */
static void SkeletonArcher_ThinkCombat(Entity *entity, s32 run_now) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 direction[4];
    s32 side;
    s32 turn;

    ENTITY_FLAG_BITS(entity)->motion_step_mode = 1;
    if (EntityAi_ShouldGuard(entity, 300.0f, DEG_TO_RAD(30))) {
        EntityAi_StartAction(entity, EntityAi_Guard, 1);
        return;
    }
    if (EntityAi_ShouldDodge(entity, 300.0f, DEG_TO_RAD(30))) {
        EntityAi_StartAction(entity, EntityAi_Dodge, 1);
        return;
    }
    side = EntityAi_PickCombatChoice(entity, 0);
    ENTITY_FLAG_BITS(entity)->attacking = FALSE;
    ENTITY_FLAG_BITS(entity)->look_at_target = TRUE;
    if (entity->action_timer == 0 || side == 0) {
        entity->prev_combat_choice = entity->combat_choice;
        entity->combat_choice = side;
        if (entity->combat_choice != entity->prev_combat_choice) {
            entity->choice_change_frame = frame_counter;
        }
        EntityAi_LowerChoiceWeight(entity, side);
        switch (entity->combat_choice) {
        case 0:
            if (EntityAi_PickAttack(entity)) {
                ENTITY_FLAG_BITS(entity)->attacking = TRUE;
                if (ENTITY_FLAG_BITS(entity)->attack_index == 0) {
                    EntityAi_StartAction(entity, SkeletonArcher_RangedAttack0, run_now);
                } else {
                    EntityAi_StartAction(entity, EntityAi_RangedAttack, run_now);
                }
            } else {
                EntityAi_StartAction(entity, EntityAi_MeleeAttack, run_now);
            }
            return;
        case 1:
            entity->target = &player_state;
            entity->action_timer = rand() % 60 + 120;
            ai_state->move_choice = 0;
            break;
        case 2:
            entity->action_timer = rand() % 30 + 30;
            ai_state->move_choice = -1;
            break;
        default:
            ai_state->move_choice = -1;
            break;
        }
    }
    if (ai_state->move_choice == 0) {
        turn = EntityAi_TurnFromPush(entity, direction);
        if (turn > 0) {
            entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_RIGHT);
        } else if (turn < 0) {
            entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_LEFT);
        } else {
            entity->move_command = EntityAi_TurnCommand(entity);
        }
    }
    EntityAi_StartAction(entity, ai_state->move, run_now);
}

/* The ranged attack with record 0 (busy bit 1): animation 23, as EntityAi_RangedAttack; from frame
   31 to 41 it also calls EntityAi_SpawnBoneEffect with 0x3ED and 8. */
static void SkeletonArcher_RangedAttack0(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    EntityParamsUnk3D8 *unk3D8 = &entity->params->ranged_attacks[0].record;

    entity->busy_flags |= ENTITY_BUSY_ATTACK;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x17, 0, entity->params->motion_blend_frames >> 1, 1);
        if (entity->params->flags & ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE) {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = (ENTITY_PARAMS_MOVEMENT_KIND(entity->params) == ENTITY_PARAMS_MOVEMENT_1) ? 2 : 1;
        }
        ENTITY_FLAG_BITS(entity)->look_at_target = 0;
        entity->action_step++;
    case 1:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        if (unk3D8->turn_frames != 0) {
            EntityAi_TurnToHeading(entity, &ai_state->turn_rate, entity->heading_to_player);
        }
        break;
    case 2:
        if (unk3D8->turn_frames >= entity->model->level_tables->frame) {
            EntityAi_TurnToHeading(entity, &entity->ai_state.turn_rate, entity->heading_to_player);
        } else {
            ai_state->turn_rate = 0.0f;
        }
        if (entity->model->level_tables->frame >= 0x1F && entity->model->level_tables->frame < 0x2A) {
            EntityAi_SpawnBoneEffect(entity, 0x3ED, 8);
        }
        EntityAi_LaunchShots(entity, unk3D8, (entity->model->level_tables->frame_fixed - entity->params->motion_speeds[entity->animation_index]) >> 8,
                      entity->model->level_tables->frame);
        if (entity->model->level_tables->frame > 0 && unk3D8->attack_end_frame >= entity->model->level_tables->frame) {
            entity->flags |= ENTITY_FLAG_ATTACK_WINDOW;
        } else {
            entity->flags &= ~ENTITY_FLAG_ATTACK_WINDOW;
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
