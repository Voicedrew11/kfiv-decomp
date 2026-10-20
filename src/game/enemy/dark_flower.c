/* DarkFlower: the behaviour of entity type 71 (EntityParams.entity_type, the index into entity_type_handlers): its set-up
   function, which the table names, and the handlers it installs. Used by: Dark Flower. The game names no
   monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by
   hit points and experience, with resistances and drops where two monsters share those. */
#include "entity/entity.h"
#include "common.h"
#include "enemy/enemy.h"
#include "entity/entity_ai_handlers.h"
#include "entity/entity_aim.h"
#include "entity/entity_math.h"
#include <libvu0.h>
#include "math/vu0.h"
#include <stdlib.h>
#include "entity/entity_ai.h"
#include "entity/entity_ai_choice.h"
#include "model/motion.h"
#include "player/player.h"

static void DarkFlower_Think(Entity *entity, s32 run_now);
static void DarkFlower_ThinkWander(Entity *entity, s32 run_now);
static void DarkFlower_ThinkCombat(Entity *entity, s32 run_now);
static void DarkFlower_Move(Entity *entity);
static void DarkFlower_RangedAttack0(Entity *entity);

/* Set-up of a type 71 entity: clears its work area and installs the type's handlers. */
void DarkFlower_Init(Entity *entity) {
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
    ai_state->think = DarkFlower_Think;
    ai_state->animate = EntityAi_Animate;
    ai_state->on_hit = EntityAi_OnHit;
    ai_state->move = DarkFlower_Move;
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

/* The think handler: as EntityAi_Think, with DarkFlower_ThinkWander for every state but 2 and
   DarkFlower_ThinkCombat for state 2. */
static void DarkFlower_Think(Entity *entity, s32 run_now) {
    u32 phase;

    phase = frame_counter % 10U;
    if (phase == entity->spawn_index % 10U) {
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
    case 1:
        break;
    case 2:
        DarkFlower_ThinkCombat(entity, run_now);
        return;
    }
    DarkFlower_ThinkWander(entity, run_now);
}

/* The think function of every state but 2 (DarkFlower_Think calls it): wanders. Each time the
   timer runs out it picks a point near the spawn point (EntityPoint_PickNear; 79 times in 100, for 240 to
   329 frames) or no target (90 to 149 frames), takes the command of EntityAi_MoveCommand towards it and
   starts the move action. It does not go for the player. */
static void DarkFlower_ThinkWander(Entity *entity, s32 run_now) {
    EntityAiState *ai_state = &entity->ai_state;

    ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_IN_PLACE;
    if (entity->action_timer == 0) {
        if (rand() % 100 >= 21) {
            f32 *point = ai_state->target_point;

            EntityPoint_PickNear(entity->spawn_info->wander_radius, point, entity->spawn_info, entity->model->position);
            entity->target = point;
            entity->action_timer = rand() % 90 + 240;
        } else {
            entity->target = NULL;
            entity->action_timer = rand() % 60 + 90;
        }
    }
    entity->move_command = EntityAi_MoveCommand(entity, 0, 50.0f);
    EntityAi_StartAction(entity, ai_state->move, run_now);
}

/* The think function of state 2: as EntityAi_ThinkCombat without the dodge; ranged record 0 is
   DarkFlower_RangedAttack0, and instead of closing in on the player it goes to a point
   EntityPoint_PickNear picks round the spawn point. */
static void DarkFlower_ThinkCombat(Entity *entity, s32 run_now) {
    EntityAiState *ai_state = &entity->ai_state;
    s32 side;

    ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_IN_PLACE;
    if (EntityAi_ShouldGuard(entity, 300.0f, DEG_TO_RAD(30))) {
        EntityAi_StartAction(entity, EntityAi_Guard, 1);
        return;
    }
    side = EntityAi_PickCombatChoice(entity, 0);
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
                if (ENTITY_FLAG_BITS(entity)->attack_index == 0) {
                    EntityAi_StartAction(entity, DarkFlower_RangedAttack0, run_now);
                } else {
                    EntityAi_StartAction(entity, EntityAi_RangedAttack, run_now);
                }
            } else {
                EntityAi_StartAction(entity, EntityAi_MeleeAttack, run_now);
            }
            return;
        case 1:
            EntityPoint_PickNear(entity->spawn_info->wander_radius, ai_state->target_point, entity->spawn_info, entity->model->position);
            entity->target = ai_state->target_point;
            entity->action_timer = rand() % 60 + 210;
            ai_state->move_choice = 0;
            break;
        case 2:
            entity->action_timer = rand() % 60 + 90;
            ai_state->move_choice = -1;
            break;
        default:
            ai_state->move_choice = -1;
            break;
        }
    }
    if (ai_state->move_choice == 0) {
        entity->move_command = EntityAi_MoveCommand(entity, 0, 50.0f);
    }
    EntityAi_StartAction(entity, ai_state->move, run_now);
}

/* The move handler: as EntityAi_Move, but the entity moves along the bearing to its target instead
   of along its heading, not up or down, and the animation is always 0. */
static void DarkFlower_Move(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 *vector;
    f32 turn;
    f32 heading;
    f32 amount;

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
        if (entity->params->turn_limit < ai_state->turn_rate) {
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
    ENTITY_SCRATCH_VECTOR[0] = ENTITY_SCRATCH_VECTOR[1] = ENTITY_SCRATCH_VECTOR[2] = ENTITY_SCRATCH_VECTOR[3] = 0.0f;
    vector = ENTITY_SCRATCH_VECTOR;
    if (entity->move_command & MOVE_COMMAND_FORWARD_BACK) {
        amount = 0.0f;
        if (entity->move_command & MOVE_COMMAND_FORWARD) {
            amount = -1.0f;
        } else if (entity->move_command & MOVE_COMMAND_BACK) {
            amount = 1.0f;
        }
        vector[2] += amount;
    }
    if (entity->move_command & MOVE_COMMAND_SIDEWAYS) {
        amount = 0.0f;
        if (entity->move_command & MOVE_COMMAND_STEP_LEFT) {
            amount = 1.0f;
        } else if (entity->move_command & MOVE_COMMAND_STEP_RIGHT) {
            amount = -1.0f;
        }
        vector[0] += amount;
    }
    if (entity->move_command & (MOVE_COMMAND_FORWARD_BACK | MOVE_COMMAND_SIDEWAYS)) {
        ai_state->move_speed = EntityAi_Accelerate(entity, ai_state->move_speed);
        amount = ai_state->move_speed;
        if (entity->move_command & MOVE_COMMAND_RUN) {
            amount += amount;
        }
        if (entity->target != NULL) {
            heading = EntityMath_HeadingTo(entity->model->position, entity->target);
            sceVu0RotMatrixY(ENTITY_SCRATCH_MATRIX, identity_matrix, heading);
            Vector_ApplyMatrix(ENTITY_SCRATCH_VECTOR, ENTITY_SCRATCH_MATRIX, ENTITY_SCRATCH_VECTOR);
            Vector_Normalize(ENTITY_SCRATCH_VECTOR, ENTITY_SCRATCH_VECTOR);
            Vector_Scale(ENTITY_SCRATCH_VECTOR, ENTITY_SCRATCH_VECTOR, amount);
            entity->move_delta_x += ENTITY_SCRATCH_VECTOR[0];
            entity->move_delta_y += ENTITY_SCRATCH_VECTOR[1];
            entity->move_delta_z += ENTITY_SCRATCH_VECTOR[2];
        }
    } else {
        ai_state->move_speed = 0.0f;
    }
    Motion_Start(entity, 0, 0, entity->params->motion_blend_frames, 0);
}

/* The ranged attack with record 0 (busy bit 1): animation 23, no turning. From frame
   EntityParams.pitch_limit_deg to unk432 it launches shots (EntityAi_LaunchShots) every unk434 frames, taking
   records 1 and 0 in turn (work byte unk7A), or record 0 every frame when unk434 is 0.
   DarkFlower_ThinkCombat starts it for record 0 only. */
static void DarkFlower_RangedAttack0(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    EntityParamsUnk3D8 *unk3D8 = ENTITY_PARAMS_UNK3D8(entity->params, 0);
    EntityParamsUnk3D8 *second_unk3D8 = ENTITY_PARAMS_UNK3D8(entity->params, 1);

    entity->busy_flags |= ENTITY_BUSY_ATTACK;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x17, 0, entity->params->motion_blend_frames >> 1, 1);
        entity->flags &= ~ENTITY_FLAG_MOTION_STEP_MODE_MASK;
        entity->action_step++;
    case 1:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (entity->model->level_tables->frame >= entity->params->pitch_limit_deg && entity->model->level_tables->frame <= entity->params->yaw_limit_deg) {
            if ((s16)entity->params->action_frames == 0) {
                EntityAi_LaunchShots(entity, unk3D8, 30, 50);
            } else if (frame_counter % (s16)entity->params->action_frames == 0) {
                if (ai_state->step_index != 0) {
                    EntityAi_LaunchShots(entity, unk3D8, 30, 50);
                } else {
                    EntityAi_LaunchShots(entity, second_unk3D8, 30, 50);
                }
                ai_state->step_index ^= 1;
            }
        }
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
