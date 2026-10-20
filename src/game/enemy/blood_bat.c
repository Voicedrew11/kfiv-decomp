/* BloodBat: the behaviour of entity type 10 (EntityParams.entity_type, the index into entity_type_handlers): its set-up
   function, which the table names, and the handlers it installs. Used by: Blood Bat. The game names no
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

#define ENTITY_WORK_AREA_SIZE 0xA0
void EntityAi_SetUpdate(char *, s32);
int EntityAi_StartAction(Entity *, void (*)(Entity *), int);
void Motion_ClearPartSlots(EntityModel *);
int Motion_StepModel(EntityModel *, s32, s32);
extern s32 entity_type_handlers[][2];
int Motion_Step(void *, int);
int Motion_BlendToPose(EntityModel *, f32);
void EntityAi_StepState(void *);
void EntityAi_SensePlayer(void *, int);
void EntityAi_CheckFarFromSpawn3D(char *);
int EntityAi_TurnFromPush(void *, void *);
s32 EntityAi_MoveCommand3D(Entity *, s32, f32);
int EntityAi_PushTurnCommand3D(Entity *, int);
extern PlayerData player_state;
s32 EntityAi_ShouldGuard(Entity *, f32, f32);
s32 EntityAi_ShouldDodge(Entity *, f32, f32);
s32 EntityAi_PickCombatChoice(Entity *, s32);
void EntityAi_LowerChoiceWeight(Entity *, s32);
int EntityAi_PickAttack(char *);
float EntityAi_Accelerate(Entity *, float);
int Motion_Start(void *, int, int, u32, int);
void EntityAi_SetState(char *, u8);

static void BloodBat_Animate(Entity *entity);
static void BloodBat_Think(Entity *entity, s32 unknown);
static int BloodBat_ThinkReturn(void *entity_arg, int unknown);
static void BloodBat_ThinkWander(Entity *entity, s32 unknown);
static int BloodBat_ThinkCombat(void *entity_arg, int unknown);
static void BloodBat_Move(Entity *entity);
static void BloodBat_Dormant(Entity *entity);

/* Set-up of a type 10 entity: clears its work area and installs the type's handlers. */
void BloodBat_Init(Entity *entity) {
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
    ai_state->think = BloodBat_Think;
    ai_state->animate = BloodBat_Animate;
    ai_state->on_hit = EntityAi_OnHit;
    ai_state->move = BloodBat_Move;
    ai_state->dormant = BloodBat_Dormant;
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
    Motion_StepModel(entity->model, 0, 0);
}

/* The animate handler: as EntityAi_Animate without the head turn. */
static void BloodBat_Animate(Entity *entity) {
    u32 blend_frames; // match: converted to float as unsigned
    s64 flags;
    EntityAiState *ai_state;

    flags = entity->flags;
    ai_state = &entity->ai_state;
    if (ENTITY_FLAG_BITS(entity)->animates) {
        if (ENTITY_PARAMS_MOVEMENT_KIND(entity->params) == ENTITY_PARAMS_MOVEMENT_FIXED) {
            entity->flags = flags & ~ENTITY_FLAG_MOTION_STEP_MODE_MASK;
        }
        blend_frames = entity->model->level_tables->blend_left;
        if (blend_frames != 0) {
            ENTITY_FLAG_BITS(entity)->motion_ended = Motion_BlendToPose(entity->model, (entity->busy_flags & (ENTITY_BUSY_GUARD | ENTITY_BUSY_REACTION_LIGHT)) ? ai_state->pose_blend_weight : 1.0f / (f32)blend_frames);
        } else {
            ENTITY_FLAG_BITS(entity)->motion_ended = Motion_Step(entity, ENTITY_FLAG_BITS(entity)->motion_step_mode);
        }
    }
}

/* The think handler: as EntityAi_Think, with EntityAi_CheckFarFromSpawn3D where that one calls EntityAi_CheckFarFromSpawn,
   BloodBat_ThinkReturn while flag bit 53 is set, BloodBat_ThinkCombat for state 2 and
   BloodBat_ThinkWander for the other states. */
static void BloodBat_Think(Entity *entity, s32 unknown) {
    u32 phase;

    phase = frame_counter % 10U;
    if (phase == entity->spawn_index % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (entity->busy_flags != 0) {
        return;
    }
    EntityAi_CheckFarFromSpawn3D((char *)entity);
    EntityAi_StepState(entity);
    if (ENTITY_FLAG_BITS(entity)->far_from_spawn) {
        BloodBat_ThinkReturn(entity, unknown);
        return;
    }
    switch (entity->state) {
    case 0:
    case 1:
        break;
    case 2:
        BloodBat_ThinkCombat(entity, unknown);
        return;
    }
    BloodBat_ThinkWander(entity, unknown);
}

/* The think function while flag bit 53 is set: walks back to the spawn point, turning away from
   whatever is pushing the entity, as EntityAi_ThinkReturn; with flag bits 11 to 13 at 0, the
   command of EntityAi_MoveCommand3D, and, while it is pushed, command bit 0x20 or 0x10 by the y of the push.
   */
static int BloodBat_ThinkReturn(void *entity_arg, int unknown) {
    Entity *entity = entity_arg;
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
        entity->move_command = EntityAi_MoveCommand3D(entity, 0, 50.0f);
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
            entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_RIGHT);
            if (push[1] > 0.0f) {
                entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_UP | MOVE_COMMAND_TURN_RIGHT);
            } else if (push[1] < 0.0f) {
                entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_DOWN | MOVE_COMMAND_TURN_RIGHT);
            }
        } else if (turn < 0) {
            entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_LEFT);
            if (push[1] > 0.0f) {
                entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_UP | MOVE_COMMAND_TURN_LEFT);
            } else if (push[1] < 0.0f) {
                entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_DOWN | MOVE_COMMAND_TURN_LEFT);
            }
        } else {
            entity->move_command = EntityAi_MoveCommand3D(entity, 0, 50.0f);
        }
    }
    EntityAi_StartAction(entity, ai_state->move, unknown);
}

/* The think function of every state but 2 (BloodBat_Think calls it): wanders. Each time the
   timer runs out it picks a point near the spawn point (EntityPoint_PickNearWithHeight) for 120 to 179 frames, takes
   the command of EntityAi_MoveCommand3D towards it (or of EntityAi_PushTurnCommand3D for the turn away from a push) and
   starts the move action. */
static void BloodBat_ThinkWander(Entity *entity, s32 unknown) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 direction[2];
    s32 turn;
    f32 *point;
    EntitySpawnInfo *spawn_info;

    ENTITY_FLAG_BITS(entity)->motion_step_mode = 0;
    if (entity->action_timer == 0) {
        spawn_info = entity->spawn_info;
        point = entity->ai_state.target_point;
        EntityPoint_PickNearWithHeight(point, (f32)spawn_info->wander_radius, (f32)spawn_info->wander_height_range, spawn_info, entity->model->position);
        entity->target = point;
        entity->action_timer = rand() % 60 + 120;
    }
    turn = EntityAi_TurnFromPush(entity, direction);
    if (turn != 0) {
        entity->move_command = EntityAi_PushTurnCommand3D(entity, turn);
    } else {
        entity->move_command = EntityAi_MoveCommand3D(entity, 0, 50.0f);
    }
    EntityAi_StartAction(entity, ai_state->move, unknown);
}

/* The think function of state 2: guard, dodge and the choice among attacking, closing in and
   waiting as in EntityAi_ThinkCombat, with flag bits 11 to 13 at 0; the head does not follow.
   Closing in, it goes forward turning to Entity.heading_to_player, with command bit 0x20 or 0x10 to bring its y
   to the player's less work float unk4C (EntityParams.yaw_limit_deg give or take unk434), and it chooses
   anew once the player is within the radius plus 130. */
static int BloodBat_ThinkCombat(void *entity_arg, int unknown) {
    Entity *entity = entity_arg;
    EntityAiState *ai_state = &entity->ai_state;
    f32 direction[4];
    f32 delta; /* angle to the wanted heading, then height to the wanted level */
    s32 side;
    s32 turn;

    ENTITY_FLAG_BITS(entity)->motion_step_mode = 0;
    if (EntityAi_ShouldGuard(entity, 300.0f, DEG_TO_RAD(30))) {
        EntityAi_StartAction(entity, EntityAi_Guard, 1);
        return;
    }
    if (EntityAi_ShouldDodge(entity, 300.0f, DEG_TO_RAD(30))) {
        EntityAi_StartAction(entity, EntityAi_Dodge, 1);
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
            if (EntityAi_PickAttack((char *)entity)) {
                EntityAi_StartAction(entity, EntityAi_RangedAttack, unknown);
            } else {
                EntityAi_StartAction(entity, EntityAi_MeleeAttack, unknown);
            }
            return;
        case 1:
            entity->target = &player_state;
            ai_state->part_angle_y = entity->params->yaw_limit_deg;
            ai_state->part_angle_y += rand() % (ENTITY_CONTROLLER(entity)->action_frames * 2) - ENTITY_CONTROLLER(entity)->action_frames;
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
        if (turn != 0) {
            entity->move_command = EntityAi_PushTurnCommand3D(entity, turn);
        } else {
            delta = EntityMath_AngleDifference(entity->model->rotation[1], entity->heading_to_player);
            if (ABS(delta) > entity->params->turn_limit) {
                if (delta >= 0.0f) {
                    entity->move_command |= MOVE_COMMAND_TURN_RIGHT;
                } else {
                    entity->move_command |= MOVE_COMMAND_TURN_LEFT;
                }
            }
            delta = player_state.position[1] - ai_state->part_angle_y - entity->model->position[1];
            if (ABS(delta) > entity->params->max_speed) {
                if (delta >= 0.0f) {
                    entity->move_command |= MOVE_COMMAND_UP;
                } else {
                    entity->move_command |= MOVE_COMMAND_DOWN;
                }
            }
            entity->move_command |= MOVE_COMMAND_FORWARD;
            if (entity->distance_to_player < entity->radius + 100.0f + 30.0f) {
                entity->action_timer = 0;
            }
        }
    }
    EntityAi_StartAction(entity, ai_state->move, unknown);
}

/* The move handler: as EntityAi_Move, with the y part of the movement eased in work float unk48,
   and with animations 4, 3 (command bit 1) and 0 only. */
static void BloodBat_Move(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 *vector;
    f32 turn;
    f32 amount;
    f32 climb_step;

    turn = 0.0f;
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
    if (ENTITY_FLAG_BITS(entity)->motion_step_mode == 0) {
        vector = ENTITY_SCRATCH_VECTOR;
        vector[0] = vector[1] = vector[2] = vector[3] = 0.0f;
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
    } else {
        ai_state->move_speed = 0.0f;
        ai_state->part_angle_x = 0.0f;
    }
    if (entity->move_command & MOVE_COMMAND_FORWARD) {
        if (entity->move_command & MOVE_COMMAND_RUN) {
            Motion_Start(entity, 4, 0, entity->params->motion_blend_frames >> 1, 0);
        } else {
            Motion_Start(entity, 3, 0, entity->params->motion_blend_frames, 0);
        }
    } else {
        Motion_Start(entity, 0, 0, entity->params->motion_blend_frames, 0);
    }
}

/* The dormant action (busy bit 0x20): holds animation 1 until the player is within
   EntitySpawnInfo.wake_distance or flag bit 43 is set, then goes to state 2 and thinks at once; it plays no
   animation 2. */
static void BloodBat_Dormant(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;

    entity->busy_flags |= ENTITY_BUSY_DORMANT;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 1, 0, 0, 1);
        ENTITY_FLAG_BITS(entity)->motion_step_mode = 0;
        ai_state->turn_rate = 0.0f;
        ENTITY_FLAG_BITS(entity)->bit22 = TRUE;
        ENTITY_FLAG_BITS(entity)->no_wall_collision = TRUE;
        ENTITY_FLAG_BITS(entity)->no_second_wall_collision = TRUE;
        ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
        entity->action_step++;
        break;
    case 1:
        if (entity->distance_to_player < entity->spawn_info->wake_distance || ENTITY_FLAG_BITS(entity)->hit_by_player) {
            EntityAi_SetState((char *)entity, 2);
            ENTITY_FLAG_BITS(entity)->bit22 = FALSE;
            ENTITY_FLAG_BITS(entity)->no_wall_collision = FALSE;
            ENTITY_FLAG_BITS(entity)->no_second_wall_collision = FALSE;
            ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
            ENTITY_FLAG_BITS(entity)->hit_strength = 0;
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
