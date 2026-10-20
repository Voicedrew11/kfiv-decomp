/* DragonFly: the behaviour of entity type 17 (EntityParams.entity_type, the index into entity_type_handlers): its set-up
   function, which the table names, and the handlers it installs. Used by: Dragon Fly. The game names no
   monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by
   hit points and experience, with resistances and drops where two monsters share those. */
#include "entity/entity.h"
#include "common.h"
#include "enemy/enemy.h"
#include "entity/entity_ai_handlers.h"
#include "entity/entity_aim.h"
#include "entity/entity_collision.h"
#include "entity/entity_math.h"
#include <libvu0.h>
#include "math/vu0.h"
#include <stdlib.h>

#define ENTITY_WORK_AREA_SIZE 0xA0
void EntityAi_SetUpdate(char *, s32);
int EntityAi_StartAction(Entity *, void (*)(Entity *), int);
void Motion_ClearPartSlots(EntityModel *);
s32 Motion_StepModel(ModelInstance *, s32, s32);
extern s32 entity_type_handlers[][2];
int Motion_Step(void *, int);
int Motion_BlendToPose(EntityModel *, f32);
void EntityAi_SensePlayer(void *, int);
void EntityAi_CheckFarFromSpawn3D(char *);
void EntityAi_StepState(void *);
int EntityAi_TurnFromPush(void *, void *);
s32 EntityAi_MoveCommand3D(Entity *, s32, f32);
int EntityAi_PushTurnCommand3D(Entity *, int);
extern PlayerData player_state;
s32 EntityAi_CanAttack(Entity *);
int EntityAi_PickAttack(char *);
float EntityAi_Accelerate(Entity *, float);
int Motion_Start(void *, int, int, u32, int);
s32 EntityAi_TurnToHeading(Entity *, f32 *, f32);
s32 EntityAi_CheckMeleeHit(Entity *, void *, s32, s32, s32);
void EntityAi_SetAttackDelay(Entity *, s32);
int EntityAi_LaunchShots(Entity *, EntityParamsUnk3D8 *, s32, s32);

static void DragonFly_Animate(Entity *entity);
static void DragonFly_Think(Entity *entity, s32 unknown);
static void DragonFly_ThinkReturn(Entity *entity, s32 unknown);
static void DragonFly_ThinkWander(Entity *entity, s32 unknown);
static void DragonFly_ThinkCombat(Entity *entity, s32 unknown);
static void DragonFly_Move(Entity *entity);
static void DragonFly_MeleeAttack(Entity *entity);
static void DragonFly_RangedAttack(Entity *entity);

/* Set-up of a type 17 entity: clears its work area and installs the type's handlers. */
void DragonFly_Init(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    u8 *unkBC = entity->choice_weights;
    u8 *unkC0 = entity->attack_weights;
    s32 i;

    // match: in the three loops the pointer has to be formed inside the loop
    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        u8 *dest = (u8 *)&entity->ai_state;

        dest[i] = 0;
    }
    EntityAi_SetUpdate((char *)entity, entity_type_handlers[entity->params->entity_type][1]);
    ai_state->think = DragonFly_Think;
    ai_state->animate = DragonFly_Animate;
    ai_state->on_hit = EntityAi_OnHit;
    ai_state->move = DragonFly_Move;
    ai_state->dormant = EntityAi_Dormant;
    ai_state->death = EntityAi_Death;
    for (i = 4 - 1; i != -1; i--) {
        entity->choice_weights[i] = 0x80;
    }
    for (i = 6 - 1; i != -1; i--) {
        entity->attack_weights[i] = 0x80;
    }
    ENTITY_UNK5C(entity)->as_s32 = 0;
    if (entity->spawn_info->spawn_flags & (0x8000L << 43)) {
        EntityAi_StartAction(entity, ai_state->dormant, 1);
    } else {
        EntityAi_StartAction(entity, ai_state->move, 0);
    }
    ENTITY_FLAG_BITS(entity)->no_floor_lift = TRUE;
    ai_state->pose_blend_weight = 0.5f;
    entity->model_alpha = (entity->params->flags & ENTITY_PARAMS_FLAG_HALF_MODEL_ALPHA) ? 0x40 : 0x80;
    entity->model->alpha = entity->model_alpha;
    Motion_ClearPartSlots(entity->model);
    Motion_StepModel(ENTITY_MODEL(entity), 0, 0);
}

/* The animate handler: as EntityAi_Animate without the test of the EntityParams kind bits and
   without the head turn. */
static void DragonFly_Animate(Entity *entity) {
    u32 blend_frames; // match: converted to float as unsigned
    EntityAiState *ai_state;

    ai_state = &entity->ai_state;
    if (ENTITY_FLAG_BITS(entity)->animates) {
        blend_frames = entity->model->level_tables->blend_left;
        if (blend_frames != 0) {
            ENTITY_FLAG_BITS(entity)->motion_ended = Motion_BlendToPose(entity->model, (entity->busy_flags & (ENTITY_BUSY_GUARD | ENTITY_BUSY_REACTION_LIGHT)) ? ai_state->pose_blend_weight : 1.0f / (f32)blend_frames);
        } else {
            ENTITY_FLAG_BITS(entity)->motion_ended = Motion_Step(entity, ENTITY_FLAG_BITS(entity)->motion_step_mode);
        }
    }
}

/* The think handler: as EntityAi_Think, with EntityAi_CheckFarFromSpawn3D where that one calls EntityAi_CheckFarFromSpawn,
   DragonFly_ThinkReturn while flag bit 53 is set, DragonFly_ThinkCombat for state 2 and
   DragonFly_ThinkWander for the other states. It also moves the entity in y so that the ground
   EntityCollision_DistanceToGround finds is no nearer than Entity.collision_radius. */
static void DragonFly_Think(Entity *entity, s32 unknown) {
    f32 floor_distance;

    if (frame_counter % 10U == entity->spawn_index % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (entity->busy_flags != 0) {
        return;
    }
    EntityAi_CheckFarFromSpawn3D((char *)entity);
    EntityAi_StepState(entity);
    floor_distance = EntityCollision_DistanceToGround(entity->model->position);
    if (floor_distance < entity->collision_radius) {
        entity->model->position[1] -= entity->collision_radius - floor_distance;
    }
    if (ENTITY_FLAG_BITS(entity)->far_from_spawn) {
        DragonFly_ThinkReturn(entity, unknown);
        return;
    }
    switch (entity->state) {
    case 0:
    case 1:
        break;
    case 2:
        DragonFly_ThinkCombat(entity, unknown);
        return;
    }
    DragonFly_ThinkWander(entity, unknown);
}

/* The think function while flag bit 53 is set: as EntityAi_ThinkReturn, with flag bits 11 to 13 at
   0, the command of EntityAi_MoveCommand3D (far marked), and, while it is pushed, command bit 0x40 and bit
   0x20 or 0x10 by the y of the push. */
static void DragonFly_ThinkReturn(Entity *entity, s32 unknown) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 to_spawn[4];
    f32 push[4];
    f32 direction[4];
    s32 turn;

    entity->action_timer = 0;
    ENTITY_FLAG_BITS(entity)->motion_step_mode = 0;
    turn = EntityAi_TurnFromPush(entity, direction);
    if (turn == 0) {
        entity->target = entity->spawn_info;
        entity->move_command = EntityAi_MoveCommand3D(entity, 1, 50.0f);
    } else {
        push[0] = push[1] = push[2] = push[3] = 0.0f;
        if (entity->flags & (ENTITY_FLAG_WALL_PUSH | ENTITY_FLAG_400000000000)) {
            push[0] += entity->wall_push[0];
            push[1] += entity->wall_push[1];
            push[2] += entity->wall_push[2];
        }
        if (ENTITY_FLAG_BITS(entity)->entity_push) {
            push[0] += entity->entity_push[0];
            push[1] += entity->entity_push[1];
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
            entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_RUN | MOVE_COMMAND_TURN_RIGHT);
            if (push[1] > 0.0f) {
                entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_UP | MOVE_COMMAND_RUN | MOVE_COMMAND_TURN_RIGHT);
            } else if (push[1] < 0.0f) {
                entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_DOWN | MOVE_COMMAND_RUN | MOVE_COMMAND_TURN_RIGHT);
            }
        } else if (turn < 0) {
            entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_RUN | MOVE_COMMAND_TURN_LEFT);
            if (push[1] > 0.0f) {
                entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_UP | MOVE_COMMAND_RUN | MOVE_COMMAND_TURN_LEFT);
            } else if (push[1] < 0.0f) {
                entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_DOWN | MOVE_COMMAND_RUN | MOVE_COMMAND_TURN_LEFT);
            }
        } else {
            entity->move_command = EntityAi_MoveCommand3D(entity, 1, 50.0f);
        }
    }
    EntityAi_StartAction(entity, ai_state->move, unknown);
}

/* The think function of every state but 2 (DragonFly_Think calls it): wanders. Each time the
   timer runs out it picks a point near the spawn point (EntityPoint_PickNearWithHeight; 79 times in 100) or no
   target, takes the command of EntityAi_MoveCommand3D towards it (or of EntityAi_PushTurnCommand3D for the turn away from
   a push) and starts the move action. */
static void DragonFly_ThinkWander(Entity *entity, s32 unknown) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 direction[4];
    s32 turn;

    ENTITY_FLAG_BITS(entity)->motion_step_mode = 0;
    if (entity->action_timer == 0) {
        if (rand() % 100 > 20) {
            EntityPoint_PickNearWithHeight(ai_state->target_point, entity->spawn_info->wander_radius, entity->spawn_info->wander_height_range, entity->spawn_info, entity->model->position);
            entity->target = ai_state->target_point;
            entity->action_timer = rand() % 60 + 120;
        } else {
            entity->target = NULL;
            entity->action_timer = rand() % 30 + 30;
        }
    }
    turn = EntityAi_TurnFromPush(entity, direction);
    if (turn != 0) {
        entity->move_command = EntityAi_PushTurnCommand3D(entity, turn);
    } else {
        entity->move_command = EntityAi_MoveCommand3D(entity, 1, 50.0f);
    }
    EntityAi_StartAction(entity, ai_state->move, unknown);
}

/* The think function of state 2: it has none of EntityAi_ThinkCombat's guard, dodge and draw among
   the actions. Whenever the pause Entity.attack_delay_frames is over and EntityAi_CanAttack finds the player in reach
   (and the way is not 4) it starts DragonFly_RangedAttack or DragonFly_MeleeAttack, as
   EntityAi_PickAttack draws. Otherwise it gives the command for one of the ways kept in work byte unk78:
   1 forward at the level of the player's y less 190 plus work byte unk7B; 2 forward to the level
   work float unk4C (the player's y less 220 to 919) until within 200 of that point; 3 (after an
   attack) forward for 90 frames; 4 forward with command bits 0x40 and 0x10 until the player's y is
   more than 700 above its own. */
static void DragonFly_ThinkCombat(Entity *entity, s32 unknown) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 target[4];
    f32 direction[4];
    f32 delta; /* angle to the wanted heading, then height to the wanted level */
    s32 turn;
    s32 roll;

    ENTITY_FLAG_BITS(entity)->motion_step_mode = 0;
    if (ai_state->move_choice != 4 && entity->attack_delay_frames == 0 && EntityAi_CanAttack(entity)) {
        if (EntityAi_PickAttack((char *)entity)) {
            EntityAi_StartAction(entity, DragonFly_RangedAttack, unknown);
        } else {
            EntityAi_StartAction(entity, DragonFly_MeleeAttack, unknown);
        }
        ai_state->part_angle_x = 0.0f;
        ai_state->move_choice = 3;
        return;
    }
    if (ai_state->move_choice == 0) {
        if (rand() % 100 < 75) {
            roll = rand() % 60;
            ai_state->move_choice = 1;
            ai_state->action_flags = roll;
        } else {
            ai_state->part_angle_y = player_state.position[1] - 220.0f - (f32)(rand() % 700);
            if (entity->radius + 100.0f + 200.0f < entity->distance_to_player) {
                ai_state->step_index = 1;
            } else {
                ai_state->step_index = 0;
            }
            entity->action_timer = 120;
            ai_state->move_choice = 2;
        }
    }
    turn = EntityAi_TurnFromPush(entity, direction);
    if (turn != 0) {
        entity->move_command = EntityAi_PushTurnCommand3D(entity, turn);
    } else {
        switch (ai_state->move_choice) {
        case 1:
            delta = EntityMath_AngleDifference(entity->model->rotation[1], entity->heading_to_player);
            if (ABS(delta) > entity->params->turn_limit) {
                if (delta >= 0.0f) {
                    entity->move_command |= MOVE_COMMAND_TURN_RIGHT;
                } else {
                    entity->move_command |= MOVE_COMMAND_TURN_LEFT;
                }
            }
            delta = player_state.position[1] - 190.0f + (f32)ai_state->action_flags - entity->model->position[1];
            if (delta > 10.0f) {
                entity->move_command |= MOVE_COMMAND_UP;
            } else if (delta < -10.0f) {
                entity->move_command |= MOVE_COMMAND_DOWN;
            }
            entity->move_command |= MOVE_COMMAND_FORWARD;
            if (entity->radius + 100.0f + 100.0f < entity->distance_to_player) {
                entity->move_command |= MOVE_COMMAND_RUN;
            }
            break;
        case 2:
            delta = EntityMath_AngleDifference(entity->model->rotation[1], entity->heading_to_player);
            if (ABS(delta) > entity->params->turn_limit) {
                if (delta >= 0.0f) {
                    entity->move_command |= MOVE_COMMAND_TURN_RIGHT;
                } else {
                    entity->move_command |= MOVE_COMMAND_TURN_LEFT;
                }
            }
            delta = ai_state->part_angle_y - entity->model->position[1];
            if (delta > 30.0f) {
                entity->move_command |= MOVE_COMMAND_UP;
            } else if (delta < -30.0f) {
                entity->move_command |= MOVE_COMMAND_DOWN;
            }
            entity->move_command |= MOVE_COMMAND_FORWARD;
            if (ai_state->step_index != 0) {
                entity->move_command |= MOVE_COMMAND_RUN;
            }
            Vector_Copy(target, &player_state);
            target[1] = ai_state->part_angle_y;
            // match: the shared prototype has no result; it returns the distance
            if (EntityMath_Distance(target, entity->model->position) <= 200.0f) {
                ai_state->move_choice = 0;
            }
            break;
        case 3:
            delta = EntityMath_AngleDifference(entity->model->rotation[1], entity->heading_to_player);
            if (ABS(delta) > entity->params->turn_limit) {
                if (delta >= 0.0f) {
                    entity->move_command |= MOVE_COMMAND_TURN_RIGHT;
                } else {
                    entity->move_command |= MOVE_COMMAND_TURN_LEFT;
                }
            }
            if (entity->radius + 100.0f + 100.0f < entity->distance_to_player) {
                entity->move_command |= (MOVE_COMMAND_FORWARD | MOVE_COMMAND_RUN);
            }
            ai_state->pause_timer++;
            if (ai_state->pause_timer > 90) {
                ai_state->pause_timer = 0;
                ai_state->move_choice = 4;
            }
            break;
        case 4:
            delta = EntityMath_AngleDifference(entity->model->rotation[1], entity->heading_to_player);
            if (ABS(delta) > entity->params->turn_limit) {
                if (delta >= 0.0f) {
                    entity->move_command |= MOVE_COMMAND_TURN_RIGHT;
                } else {
                    entity->move_command |= MOVE_COMMAND_TURN_LEFT;
                }
            }
            delta = player_state.position[1] - entity->model->position[1];
            entity->move_command |= (MOVE_COMMAND_FORWARD | MOVE_COMMAND_DOWN | MOVE_COMMAND_RUN);
            if (delta > 700.0f) {
                ai_state->move_choice = 0;
            }
            break;
        default:
            ai_state->move_choice = 0;
            break;
        }
    }
    if (entity->handler != ai_state->move) {
        EntityAi_StartAction(entity, ai_state->move, unknown);
    }
}

/* The move handler: as EntityAi_Move, but it moves whatever flag bits 11 to 13 say, and the y part
   of the movement is eased in work float unk48. */
static void DragonFly_Move(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 *vector;
    f32 turn;
    f32 amount;
    f32 climb_step;

    vector = ENTITY_SCRATCH_VECTOR;
    turn = vector[0] = vector[1] = vector[2] = vector[3] = 0.0f;
    climb_step = 0.02f;
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
        ai_state->turn_rate = turn;
    }
    turn = ai_state->turn_rate;
    if (entity->move_command & MOVE_COMMAND_TURN_FAST) {
        turn += turn;
    }
    entity->turn_delta_yaw += turn;
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
    if (entity->move_command & MOVE_COMMAND_UP_DOWN) {
        amount = 0.0f;
        if (entity->move_command & MOVE_COMMAND_DOWN) {
            ai_state->part_angle_x -= climb_step;
            if (ai_state->part_angle_x < -1.0f) {
                ai_state->part_angle_x = -1.0f;
            }
            amount = ai_state->part_angle_x;
        } else if (entity->move_command & MOVE_COMMAND_UP) {
            ai_state->part_angle_x += climb_step;
            if (1.0f < ai_state->part_angle_x) {
                ai_state->part_angle_x = 1.0f;
            }
            amount = ai_state->part_angle_x;
        }
        vector[1] += amount;
    }
    if (entity->move_command & MOVE_COMMAND_DIRECTIONS) {
        ai_state->move_speed = EntityAi_Accelerate(entity, ai_state->move_speed);
        amount = ai_state->move_speed;
        if (entity->move_command & MOVE_COMMAND_RUN) {
            amount += amount;
        }
        sceVu0RotMatrixZ(ENTITY_SCRATCH_MATRIX, identity_matrix, entity->model->rotation[2]);
        sceVu0RotMatrixX(ENTITY_SCRATCH_MATRIX, ENTITY_SCRATCH_MATRIX, entity->model->rotation[0]);
        sceVu0RotMatrixY(ENTITY_SCRATCH_MATRIX, ENTITY_SCRATCH_MATRIX, entity->model->rotation[1]);
        Vector_ApplyMatrix(ENTITY_SCRATCH_VECTOR, ENTITY_SCRATCH_MATRIX, ENTITY_SCRATCH_VECTOR);
        Vector_Normalize(ENTITY_SCRATCH_VECTOR, ENTITY_SCRATCH_VECTOR);
        Vector_Scale(ENTITY_SCRATCH_VECTOR, ENTITY_SCRATCH_VECTOR, amount);
        entity->move_delta_x += ENTITY_SCRATCH_VECTOR[0];
        entity->move_delta_y += ENTITY_SCRATCH_VECTOR[1];
        entity->move_delta_z += ENTITY_SCRATCH_VECTOR[2];
    } else {
        ai_state->move_speed = 0.0f;
        ai_state->part_angle_x = 0.0f;
    }
    if (entity->move_command & MOVE_COMMAND_FORWARD) {
        if (entity->move_command & MOVE_COMMAND_RUN) {
            Motion_Start(entity, 4, 0, (s32)((f32)entity->params->motion_blend_frames * 0.5f), 0);
        } else {
            Motion_Start(entity, 3, 0, entity->params->motion_blend_frames, 0);
        }
    } else if (entity->move_command & MOVE_COMMAND_BACK) {
        Motion_Start(entity, 5, 0, entity->params->motion_blend_frames, 0);
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

/* The melee attack (busy bit 1): as EntityAi_MeleeAttack with flag bits 11 to 13 at 0; it leaves
   flag bit 9 as it is. */
static void DragonFly_MeleeAttack(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    EntityParamsUnk240 *unk240 = &entity->params->melee_attacks[ENTITY_FLAG_BITS(entity)->attack_index];
    s32 frame;

    entity->busy_flags |= ENTITY_BUSY_ATTACK;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, ENTITY_FLAG_BITS(entity)->attack_index | ENTITY_MOTION_ATTACK_BASE, 0, entity->params->motion_blend_frames >> 1, 1);
        ENTITY_FLAG_BITS(entity)->motion_step_mode = 0;
        ENTITY_FLAG_BITS(entity)->melee_0_hit = 0;
        ENTITY_FLAG_BITS(entity)->melee_1_hit = 0;
        ENTITY_FLAG_BITS(entity)->melee_2_hit = 0;
        entity->action_step++;
    case 1:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        if (unk240->hit_start_frame != 0) {
            EntityAi_TurnToHeading(entity, &ai_state->turn_rate, entity->heading_to_player);
        }
        break;
    case 2:
        if (unk240->hit_start_frame >= entity->model->level_tables->frame) {
            EntityAi_TurnToHeading(entity, &ai_state->turn_rate, entity->heading_to_player);
        } else {
            ai_state->turn_rate = 0.0f;
        }
        frame = (entity->model->level_tables->frame_fixed - entity->params->motion_speeds[entity->animation_index]) >> 8;
        if (!ENTITY_FLAG_BITS(entity)->melee_0_hit) {
            ENTITY_FLAG_BITS(entity)->melee_0_hit = EntityAi_CheckMeleeHit(entity, unk240, 0, frame, entity->model->level_tables->frame);
        }
        if (!ENTITY_FLAG_BITS(entity)->melee_1_hit) {
            ENTITY_FLAG_BITS(entity)->melee_1_hit = EntityAi_CheckMeleeHit(entity, unk240, 1, frame, entity->model->level_tables->frame);
        }
        if (!ENTITY_FLAG_BITS(entity)->melee_2_hit) {
            ENTITY_FLAG_BITS(entity)->melee_2_hit = EntityAi_CheckMeleeHit(entity, unk240, 2, frame, entity->model->level_tables->frame);
        }
        if (entity->model->level_tables->frame > 0 && unk240->hit_end_frame >= entity->model->level_tables->frame) {
            ENTITY_FLAG_BITS(entity)->attack_window = 1;
        } else {
            ENTITY_FLAG_BITS(entity)->attack_window = 0;
        }
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            EntityAi_SetAttackDelay(entity, unk240->attack_delay);
            if (ENTITY_FLAG_BITS(entity)->own_think) {
                ai_state->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, ai_state->move, 1);
            }
        }
        break;
    }
}

/* The ranged attack (busy bit 1): as EntityAi_RangedAttack with flag bits 11 to 13 at 0; it leaves
   flag bit 9 as it is. */
static void DragonFly_RangedAttack(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    s32 kind = ENTITY_FLAG_BITS(entity)->attack_index;
    EntityParamsUnk3D8 *unk3D8 = ENTITY_PARAMS_UNK3D8(entity->params, kind);

    entity->busy_flags |= ENTITY_BUSY_ATTACK;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, kind + 0x17, 0, entity->params->motion_blend_frames >> 1, 1);
        ENTITY_FLAGS(entity) &= ~ENTITY_FLAG_MOTION_STEP_MODE_MASK;
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
            EntityAi_TurnToHeading(entity, &ai_state->turn_rate, entity->heading_to_player);
        } else {
            ai_state->turn_rate = 0.0f;
        }
        EntityAi_LaunchShots(entity, unk3D8,
                      (entity->model->level_tables->frame_fixed - entity->params->motion_speeds[entity->animation_index]) >> 8,
                      entity->model->level_tables->frame);
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
