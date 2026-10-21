/* SkeletonSoldier: the behaviour of entity type 6 (EntityParams.entity_type, the index into entity_type_handlers): its
   set-up function, which the table names, and the handlers it installs. Used by: Skeleton Soldier
   Roundshield, Skeleton Soldier Squareshield, Skeleton Soldier Axe, High Skeleton Soldier. The game names no
   monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by
   hit points and experience, with resistances and drops where two monsters share those. */
#include "entity/entity.h"
#include "common.h"
#include "enemy/enemy.h"
#include "entity/entity_ai_handlers.h"
#include "math/vu0.h"
#include <stdlib.h>
#include "entity/entity_ai.h"
#include "entity/entity_ai_choice.h"
#include "model/motion.h"
#include "entity/entity_math.h"

/* EntitySpawnInfo.spawn_mode: picks the think handler and the start animation */
#define SKELETON_SOLDIER_SPAWN_MODE_NORMAL               0
#define SKELETON_SOLDIER_SPAWN_MODE_NO_STATES            1
#define SKELETON_SOLDIER_SPAWN_MODE_CLEARS_MAP_COLLISION 2
#define SKELETON_SOLDIER_SPAWN_MODE_CLEARS_COLLISION_AWAY 3


static void SkeletonSoldier_Think(Entity *entity, s32 run_now);
static void SkeletonSoldier_Dormant(Entity *entity);
static void SkeletonSoldier_PlayAnim17(Entity *entity);
static void SkeletonSoldier_ThinkClearFlag25Away(Entity *entity, s32 run_now);
static void SkeletonSoldier_DormantAnim18Then19(Entity *entity);
static void SkeletonSoldier_ThinkNoStates(Entity *entity, s32 run_now);
static void SkeletonSoldier_ThinkDecideAttackOrWait(Entity *entity, s32 run_now);

/* Set-up of a type 6 entity: clears its work area and installs the type's handlers. */
void SkeletonSoldier_Init(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    char *unk138 = entity->model->look_at_part;
    s32 i;

    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        ENTITY_WORK_BYTES(entity)[i] = 0;
    }
    EntityAi_SetUpdate(entity, entity_type_handlers[entity->params->entity_type][1]);
    ai_state->animate = EntityAi_Animate;
    ai_state->on_hit = EntityAi_OnHit;
    ai_state->move = EntityAi_Move;
    ai_state->dormant = SkeletonSoldier_Dormant;
    ai_state->death = EntityAi_Death;
    if (entity->spawn_info->spawn_mode == SKELETON_SOLDIER_SPAWN_MODE_NO_STATES) {
        ai_state->think = SkeletonSoldier_ThinkNoStates;
    } else if (entity->spawn_info->spawn_mode >= 2 && entity->spawn_info->spawn_mode <= 3) {
        ai_state->think = SkeletonSoldier_ThinkClearFlag25Away;
        ai_state->dormant = SkeletonSoldier_DormantAnim18Then19;
    } else {
        ai_state->think = SkeletonSoldier_Think;
    }
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
    Motion_StepModel(ENTITY_MODEL(entity), 0, 0);
}

/* The think handler: as EntityAi_Think; in state 2 with flag bit 32 set it first plays
   SkeletonSoldier_PlayAnim17 once (ai_state.mode_timer says whether it has; state 0 clears it). */
static void SkeletonSoldier_Think(Entity *entity, s32 run_now) {
    EntityAiState *ai_state = &entity->ai_state;

    if (frame_counter % 10U == entity->spawn_index % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (entity->busy_flags == 0) {
        EntityAi_CheckFarFromSpawn(entity);
        EntityAi_StepState(entity);
        if (ENTITY_FLAG_BITS(entity)->far_from_spawn) {
            EntityAi_ThinkReturn(entity, run_now);
        } else {
            switch (entity->state) {
            case ENTITY_STATE_UNAWARE:
                EntityAi_ThinkIdle(entity, run_now);
                ai_state->mode_timer = 0;
                break;
            case ENTITY_STATE_SEARCHING:
                EntityAi_ThinkPursue(entity, run_now);
                break;
            case ENTITY_STATE_NOTICED:
                if (ENTITY_FLAG_BITS(entity)->player_in_reach && ai_state->mode_timer == 0) {
                    EntityAi_StartAction(entity, SkeletonSoldier_PlayAnim17, run_now);
                    ai_state->mode_timer = 1;
                } else {
                    EntityAi_ThinkCombat(entity, run_now);
                }
                break;
            default:
                EntityAi_ThinkIdle(entity, run_now);
                break;
            }
        }
    }
}

/* The dormant action (busy bit 0x20): as EntityAi_Dormant, but with EntitySpawnInfo.spawn_mode set the
   animations are 18 and 19 instead of 1 and 2; flag bit 20 is set and flag bit 17 clear while it
   lasts. */
static void SkeletonSoldier_Dormant(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;

    entity->busy_flags |= ENTITY_BUSY_DORMANT;
    switch (entity->action_step) {
    case ACTION_STEP_START:
        if (entity->spawn_info->spawn_mode != SKELETON_SOLDIER_SPAWN_MODE_NORMAL) {
            Motion_Start(entity, 0x12, 0, 0, 1);
        } else {
            Motion_Start(entity, 1, 0, 0, 1);
        }
        if (entity->params->flags & ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE) {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_IN_PLACE;
        } else {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = ENTITY_PARAMS_MOVEMENT_KIND(entity->params) == ENTITY_PARAMS_MOVEMENT_1 ? MOTION_STEP_MOVE_FLAT : MOTION_STEP_MOVE;
        }
        ENTITY_FLAG_BITS(entity)->bit21 = 1;
        ENTITY_FLAG_BITS(entity)->bit20 = 1;
        ai_state->turn_rate = 0.0f;
        ENTITY_FLAG_BITS(entity)->bit22 = 1;
        ENTITY_FLAG_BITS(entity)->no_wall_collision = 1;
        ENTITY_FLAG_BITS(entity)->no_second_wall_collision = 1;
        ENTITY_FLAG_BITS(entity)->bit18 = 1;
        ENTITY_FLAG_BITS(entity)->draw_afterimage = 0;
        ENTITY_FLAG_BITS(entity)->no_floor_lift = 1;
        ENTITY_FLAG_BITS(entity)->look_at_target = 0;
        entity->action_step++;
        break;
    case DORMANT_STEP_SLEEP:
        if (entity->distance_to_player < entity->spawn_info->wake_distance || ENTITY_FLAG_BITS(entity)->hit_by_player) {
            if (entity->spawn_info->spawn_mode != SKELETON_SOLDIER_SPAWN_MODE_NORMAL) {
                Motion_Start(entity, 0x13, 0, entity->params->motion_blend_frames, 0);
            } else {
                Motion_Start(entity, 2, 0, entity->params->motion_blend_frames, 0);
            }
            if (entity->params->flags & ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE) {
                ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_IN_PLACE;
            } else {
                ENTITY_FLAG_BITS(entity)->motion_step_mode = ENTITY_PARAMS_MOVEMENT_KIND(entity->params) == ENTITY_PARAMS_MOVEMENT_1 ? MOTION_STEP_MOVE_FLAT : MOTION_STEP_MOVE;
            }
            ENTITY_FLAG_BITS(entity)->hit_strength = 0;
            entity->action_step++;
        }
        break;
    case DORMANT_STEP_BLEND_IN:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        break;
    case DORMANT_STEP_WAKE:
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            EntityAi_SetState(entity, 2);
            ENTITY_FLAG_BITS(entity)->bit21 = 0;
            ENTITY_FLAG_BITS(entity)->bit20 = 0;
            ENTITY_FLAG_BITS(entity)->bit22 = 0;
            ENTITY_FLAG_BITS(entity)->no_wall_collision = 0;
            ENTITY_FLAG_BITS(entity)->no_second_wall_collision = 0;
            ENTITY_FLAG_BITS(entity)->bit18 = 0;
            ENTITY_FLAG_BITS(entity)->draw_afterimage = 1;
            if (ENTITY_PARAMS_MOVEMENT_KIND(entity->params) == ENTITY_PARAMS_MOVEMENT_WALK) {
                ENTITY_FLAG_BITS(entity)->no_floor_lift = 0;
            }
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            if (ENTITY_FLAG_BITS(entity)->own_think) {
                entity->ai_state.think(entity, 1);
            } else {
                EntityAi_StartAction(entity, ai_state->move, 1);
            }
        }
        break;
    }
}

/* An action (busy bit 0x1000): plays animation 17 to its end with flag bit 9 clear. The think
   handlers start it once on entering state 2. */
static void SkeletonSoldier_PlayAnim17(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;

    entity->busy_flags |= ENTITY_BUSY_1000;
    ENTITY_FLAG_BITS(entity)->look_at_target = FALSE;
    switch (entity->action_step) {
    case ACTION_STEP_START:
        Motion_Start(entity, 0x11, 0, entity->params->motion_blend_frames, 0);
        if (entity->params->flags & ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE) {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_IN_PLACE;
        } else {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = ENTITY_PARAMS_MOVEMENT_KIND(entity->params) == ENTITY_PARAMS_MOVEMENT_1 ? MOTION_STEP_MOVE_FLAT : MOTION_STEP_MOVE;
        }
        ai_state->turn_rate = 0.0f;
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
            if (ENTITY_FLAG_BITS(entity)->own_think) {
                entity->ai_state.think(entity, 1);
            } else {
                EntityAi_StartAction(entity, ai_state->move, 1);
            }
        }
        break;
    }
}

/* The think handler the set-up function installs for EntitySpawnInfo.spawn_mode 2 and 3: as
   SkeletonSoldier_Think; with EntitySpawnInfo.spawn_mode 3 it also clears flag bit 25 once the entity is more than 180 from
   its spawn point (SkeletonSoldier_DormantAnim18Then19 leaves the bit set in that mode). What flag bit
   25 does is not known; scripts read and set it (ScriptCmd_SetEntityFlags, ScriptCmd_GetEntityFlags). */
static void SkeletonSoldier_ThinkClearFlag25Away(Entity *entity, s32 run_now) {
    EntityAiState *ai_state = &entity->ai_state;

    if (frame_counter % 10U == entity->spawn_index % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (entity->spawn_info->spawn_mode == SKELETON_SOLDIER_SPAWN_MODE_CLEARS_COLLISION_AWAY && EntityMath_DistanceXZ(entity->model->position, &entity->spawn_info->spawn_x) > 180.0f) {
        ENTITY_FLAG_BITS(entity)->no_map_object_collision = 0;
    }
    if (entity->busy_flags == 0) {
        EntityAi_CheckFarFromSpawn(entity);
        EntityAi_StepState(entity);
        if (ENTITY_FLAG_BITS(entity)->far_from_spawn) {
            EntityAi_ThinkReturn(entity, run_now);
        } else {
            switch (entity->state) {
            case ENTITY_STATE_UNAWARE:
                EntityAi_ThinkIdle(entity, run_now);
                ai_state->mode_timer = 0;
                break;
            case ENTITY_STATE_SEARCHING:
                EntityAi_ThinkPursue(entity, run_now);
                break;
            case ENTITY_STATE_NOTICED:
                if (ENTITY_FLAG_BITS(entity)->player_in_reach && ai_state->mode_timer == 0) {
                    EntityAi_StartAction(entity, SkeletonSoldier_PlayAnim17, run_now);
                    ai_state->mode_timer = 1;
                } else {
                    EntityAi_ThinkCombat(entity, run_now);
                }
                break;
            default:
                EntityAi_ThinkIdle(entity, run_now);
                break;
            }
        }
    }
}

/* The dormant action (busy bit 0x20) the set-up function installs for EntitySpawnInfo.spawn_mode 2 and
   3: as SkeletonSoldier_Dormant with animations 18 and 19; it also sets flag bit 25, and clears it at
   the end only when EntitySpawnInfo.spawn_mode is 2. */
static void SkeletonSoldier_DormantAnim18Then19(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;

    entity->busy_flags |= ENTITY_BUSY_DORMANT;
    switch (entity->action_step) {
    case ACTION_STEP_START:
        Motion_Start(entity, 0x12, 0, 0, 1);
        if (entity->params->flags & ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE) {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_IN_PLACE;
        } else {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = ENTITY_PARAMS_MOVEMENT_KIND(entity->params) == ENTITY_PARAMS_MOVEMENT_1 ? MOTION_STEP_MOVE_FLAT : MOTION_STEP_MOVE;
        }
        ENTITY_FLAG_BITS(entity)->bit21 = 1;
        ENTITY_FLAG_BITS(entity)->bit20 = 1;
        ai_state->turn_rate = 0.0f;
        ENTITY_FLAG_BITS(entity)->bit22 = 1;
        ENTITY_FLAG_BITS(entity)->no_wall_collision = 1;
        ENTITY_FLAG_BITS(entity)->no_second_wall_collision = 1;
        ENTITY_FLAG_BITS(entity)->no_map_object_collision = 1;
        ENTITY_FLAG_BITS(entity)->bit18 = 1;
        ENTITY_FLAG_BITS(entity)->draw_afterimage = 0;
        ENTITY_FLAG_BITS(entity)->no_floor_lift = 1;
        ENTITY_FLAG_BITS(entity)->look_at_target = 0;
        entity->action_step++;
        break;
    case DORMANT_STEP_SLEEP:
        if (entity->distance_to_player < entity->spawn_info->wake_distance || ENTITY_FLAG_BITS(entity)->hit_by_player) {
            Motion_Start(entity, 0x13, 0, entity->params->motion_blend_frames, 0);
            if (entity->params->flags & ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE) {
                ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_IN_PLACE;
            } else {
                ENTITY_FLAG_BITS(entity)->motion_step_mode = ENTITY_PARAMS_MOVEMENT_KIND(entity->params) == ENTITY_PARAMS_MOVEMENT_1 ? MOTION_STEP_MOVE_FLAT : MOTION_STEP_MOVE;
            }
            ENTITY_FLAG_BITS(entity)->hit_strength = 0;
            entity->action_step++;
        }
        break;
    case DORMANT_STEP_BLEND_IN:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        break;
    case DORMANT_STEP_WAKE:
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            EntityAi_SetState(entity, 2);
            ENTITY_FLAG_BITS(entity)->bit21 = 0;
            ENTITY_FLAG_BITS(entity)->bit20 = 0;
            ENTITY_FLAG_BITS(entity)->bit22 = 0;
            ENTITY_FLAG_BITS(entity)->no_wall_collision = 0;
            ENTITY_FLAG_BITS(entity)->no_second_wall_collision = 0;
            if (entity->spawn_info->spawn_mode == SKELETON_SOLDIER_SPAWN_MODE_CLEARS_MAP_COLLISION) {
                ENTITY_FLAG_BITS(entity)->no_map_object_collision = 0;
            }
            ENTITY_FLAG_BITS(entity)->bit18 = 0;
            ENTITY_FLAG_BITS(entity)->draw_afterimage = 1;
            if (ENTITY_PARAMS_MOVEMENT_KIND(entity->params) == ENTITY_PARAMS_MOVEMENT_WALK) {
                ENTITY_FLAG_BITS(entity)->no_floor_lift = 0;
            }
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            if (ENTITY_FLAG_BITS(entity)->own_think) {
                entity->ai_state.think(entity, 1);
            } else {
                EntityAi_StartAction(entity, ai_state->move, 1);
            }
        }
        break;
    }
}

/* The think handler of mode 1 (EntitySpawnInfo.spawn_mode is 1): as EntityAi_Think without the states;
   while flag bit 53 is clear it calls SkeletonSoldier_ThinkDecideAttackOrWait. */
static void SkeletonSoldier_ThinkNoStates(Entity *entity, s32 run_now) {
    u32 phase;

    phase = frame_counter % 10U;
    if (phase == entity->spawn_index % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (entity->busy_flags != 0) {
        return;
    }
    if (ENTITY_PARAMS_MOVEMENT_KIND(entity->params) != ENTITY_PARAMS_MOVEMENT_FIXED) {
        EntityAi_CheckFarFromSpawn(entity);
    }
    EntityAi_StepState(entity);
    if (ENTITY_FLAG_BITS(entity)->far_from_spawn) {
        EntityAi_ThinkReturn(entity, run_now);
        return;
    }
    SkeletonSoldier_ThinkDecideAttackOrWait(entity, run_now);
}

/* Decides for SkeletonSoldier_ThinkNoStates: as EntityAi_ThinkCombat, but it never closes in: when
   EntityAi_PickCombatChoice does not choose the attack it waits 30 to 59 frames. The motion never moves the
   entity. */
static void SkeletonSoldier_ThinkDecideAttackOrWait(Entity *entity, s32 run_now) {
    EntityAiState *ai_state = &entity->ai_state;
    s32 side;

    ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_IN_PLACE;
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
        if (entity->combat_choice == COMBAT_CHOICE_ATTACK) {
            if (EntityAi_PickAttack(entity)) {
                EntityAi_StartAction(entity, EntityAi_RangedAttack, run_now);
            } else {
                EntityAi_StartAction(entity, EntityAi_MeleeAttack, run_now);
            }
            return;
        }
        entity->action_timer = rand() % 30 + 30;
        ai_state->move_choice = MOVE_CHOICE_NONE;
    }
    EntityAi_StartAction(entity, ai_state->move, run_now);
}
