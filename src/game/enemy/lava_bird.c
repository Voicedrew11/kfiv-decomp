/* LavaBird: the behaviour of entity type 68 (EntityParams.entity_type, the index into entity_type_handlers): its set-up
   function, which the table names, and the handlers it installs. Used by: Lava Bird. The game names no
   monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by
   hit points and experience, with resistances and drops where two monsters share those. */
#include "entity/entity.h"
#include "common.h"
#include "effect/shot.h"

#define ENTITY_WORK_AREA_SIZE 0xA0
void EntityAi_SetUpdate(char *, s32);
int EntityAi_StartAction(Entity *, void (*)(Entity *), int);
void Motion_ClearPartSlots(EntityModel *);
s32 Motion_StepModel(ModelInstance *, s32, s32);
void Motion_SetPartSlots(EntityModel *, s32, s32, s32);
void Vector_Copy(void *, void *);
extern s32 entity_type_handlers[][2];
extern u32 frame_counter;
int rand(void);
void EntityAi_SensePlayer(void *, int);
void EntityAi_CheckFarFromSpawn(char *);
void EntityAi_StepState(void *);
int EntityAi_StartSlotMotion(void *, void *, int, int, int, int, int);
int EntityAi_ThinkReturn(void *, int);
int EntityAi_TurnFromPush(void *, void *);
extern PlayerData player_state;
s32 EntityAi_PickCombatChoice(Entity *, s32);
void EntityAi_LowerChoiceWeight(Entity *, s32);
int EntityAi_PickAttack(char *);
void EntityPoint_PickNear(float, void *, void *, void *);
extern f32 identity_matrix[16];
float EntityMath_EaseAngleToward(float *, float, float);
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0RotMatrixZ(float *, float *, float);
void Vector_ApplyMatrix(void *, void *, void *);
float EntityMath_HeadingTo(float *, float *);
float EntityMath_AngleDifference(float, float);
void EntityEvent_StartEffectAtPoint(char **, int, u32, int);
void EntityAi_SetState(char *, u8);
s32 EntityAi_TurnToHeading(Entity *, f32 *, f32);
void EntityAi_SetAttackDelay(Entity *entity, s32 base);
float sqrtf(float);
float atan2f(float, float);
double atan2(double, double);
char *Model_GetSkinnedBonePoint(char *, u32);
float EntityMath_WrapAngle(float);
float EntityMath_DistanceXZ(void *, void *);
float EntityMath_AngleXZ(float *, char *);
f32 EntityMath_Approach(f32, f32, f32);
int Attack_LaunchMagicEffect(u16, void *, void *, void *, void *);
void EntityAi_LowerWeightsAfterHit(char *);
int EntityAi_SetReaction(Entity *, void (*)(Entity *), int);
void EntityEvent_StartFrameCues(Entity *, s32, s32, s32);
void EntityEvent_OnDeath(Entity *);
int EntityAi_StepSlotMotion(Entity *, EntityMotionSlot *, int, f32);
s32 Motion_StepSlot(Entity *, EntityAiMotionSlotBits *, s32, s32);
void EntityEvent_StartFrameEffects(Entity *, s32, s32, s32);
int EntityAi_LookAtPoint(void *, int, int, int, int, float);
int EntityAi_LookAtPlayer(Entity *, void *, s32, void *, f32);

static void LavaBird_Think(Entity *entity, s32 unknown);
static void LavaBird_ThinkCombat(Entity *entity, s32 unknown);
static void LavaBird_ThinkPursue(Entity *entity, s32 unknown);
static void LavaBird_ThinkIdle(Entity *entity, s32 unknown);
static void LavaBird_Move(Entity *entity);
static void LavaBird_ApplyMoveCommand(Entity *entity);
static s32 LavaBird_SteerToTargetCommand(Entity *entity, s32 mark_far, f32 range);
static void LavaBird_Dormant(Entity *entity);
static void LavaBird_RangedAttack(Entity *entity);
static s32 LavaBird_LaunchBothShots(Entity *entity, EntityParamsUnk3D8 *unk3D8, s32 frame_from, s32 frame_to, f32 *target, f32 pitch_rate, f32 yaw_rate);
static void LavaBird_OnHit(Entity *entity);
static void LavaBird_ReactionLight(Entity *entity);
static void LavaBird_ReactionHeavy(Entity *entity);
static void LavaBird_Death(Entity *entity);
static void LavaBird_Animate(Entity *entity);

/* Set-up of a type 68 entity: clears its work area and installs the type's handlers. */
void LavaBird_Init(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    EntityAiStateKind151D30 *work = (EntityAiStateKind151D30 *)&entity->ai_state;
    u8 *unkBC = entity->choice_weights;
    u8 *unkC0 = entity->attack_weights;
    char *unk138 = entity->model->look_at_part;
    s32 i;

    // match: in the three loops the pointer has to be formed inside the loop
    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        u8 *dest = (u8 *)&entity->ai_state;

        dest[i] = 0;
    }
    EntityAi_SetUpdate((char *)entity, entity_type_handlers[entity->params->entity_type][1]);
    ai_state->think = LavaBird_Think;
    ai_state->animate = LavaBird_Animate;
    ai_state->on_hit = LavaBird_OnHit;
    ai_state->move = LavaBird_Move;
    ai_state->death = LavaBird_Death;
    ai_state->dormant = LavaBird_Dormant;
    for (i = 4 - 1; i != -1; i--) {
        entity->choice_weights[i] = 0x80;
    }
    for (i = 6 - 1; i != -1; i--) {
        entity->attack_weights[i] = 0x80;
    }
    ENTITY_UNK5C(entity)->as_s32 = 0;
    ENTITY_FLAG_BITS(entity)->no_floor_lift = TRUE;
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
    Motion_SetPartSlots(entity->model, 0x10, 0x19, 1);
    work->motion_slot_0.bit5 = TRUE;
    work->motion_slot_0.bit6 = TRUE;
    work->motion_slot_1.bit6 = TRUE;
    Motion_StepModel(ENTITY_MODEL(entity), 0, 0);
}

/* The think handler: as EntityAi_Think, with the type's own functions for the three states. It
   keeps flag bit 28 set except in the death action, and animation 3 on the second motion slot
   outside heavy-hit reactions and the death; every 60th frame it draws work float unk54 (0 to 9).
   After the state's function, every frame outside the death, it gives the command of LavaBird_SteerToTargetCommand
   (or the turn away from a push) and calls LavaBird_ApplyMoveCommand. */
static void LavaBird_Think(Entity *entity, s32 unknown) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 direction[4];
    s32 side;

    if (frame_counter % 10U == entity->spawn_index % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (frame_counter % 60U == entity->spawn_index % 10U) {
        ai_state->pitch_speed = rand() % 10;
    }
    if (entity->busy_flags & ENTITY_BUSY_DEATH) {
        ENTITY_FLAG_BITS(entity)->no_floor_lift = FALSE;
    } else {
        ENTITY_FLAG_BITS(entity)->no_floor_lift = TRUE;
    }
    if (!(entity->busy_flags & (ENTITY_BUSY_DEATH | ENTITY_BUSY_REACTION_HEAVY))) {
        EntityAi_StartSlotMotion(entity, &((EntityAiStateKind151D30 *)ai_state)->motion_slot_1, 1, 3, 5, entity->params->motion_blend_frames, 0);
    }
    if (entity->busy_flags == 0) {
        EntityAi_CheckFarFromSpawn((char *)entity);
        EntityAi_StepState(entity);
        if (ENTITY_FLAG_BITS(entity)->far_from_spawn) {
            EntityAi_ThinkReturn(entity, unknown);
        } else {
            switch (entity->state) {
            case 0:
                LavaBird_ThinkIdle(entity, unknown);
                break;
            case 1:
                LavaBird_ThinkPursue(entity, unknown);
                break;
            case 2:
                LavaBird_ThinkCombat(entity, unknown);
                break;
            default:
                LavaBird_ThinkIdle(entity, unknown);
                break;
            }
        }
    }
    if (entity->busy_flags & ENTITY_BUSY_DEATH) {
        ai_state->move_speed = 0.0f;
        ai_state->part_angle_x = 0.0f;
    } else {
        side = EntityAi_TurnFromPush(entity, direction);
        if (side > 0) {
            entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_RIGHT);
        } else if (side < 0) {
            entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_LEFT);
        } else {
            // match: called like EntityAi_MoveCommand, with a speed and a mode the callee ignores
            entity->move_command = LavaBird_SteerToTargetCommand(entity, 0, 0.0f);
        }
        LavaBird_ApplyMoveCommand(entity);
    }
}

/* The think function of state 2: as EntityAi_ThinkCombat without the guard and the dodge, with the
   command of LavaBird_SteerToTargetCommand. It also picks the pitch command, work byte unk79: 1 or -1 at random
   (EntityParams.far_distance times in 100), 2 while the player's y less its own is below
   EntityParams.near_distance, -2 while it is above unk434. Its only attack is LavaBird_RangedAttack,
   when EntityAi_PickAttack draws a ranged one. */
static void LavaBird_ThinkCombat(Entity *entity, s32 unknown) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 direction[4];
    s32 next_action = 0; // match: the unused initial value changes the instruction count that sizes a compiler hash table, and with it two registers
    s32 side;

    ENTITY_FLAG_BITS(entity)->motion_step_mode = 0;
    if (entity->action_timer == 0) {
        if (rand() % 100 < entity->params->far_distance) {
            if (!(rand() & 1)) {
                entity->target = &player_state;
                entity->action_timer = rand() % 30 + 30;
                ai_state->move_choice = 0;
                entity->combat_choice = 1;
                ai_state->turn_in_place = 1;
            } else {
                entity->target = &player_state;
                entity->action_timer = rand() % 30 + 30;
                ai_state->move_choice = 0;
                entity->combat_choice = 1;
                ai_state->turn_in_place = -1;
            }
        } else if (entity->action_timer == 0) {
            ai_state->turn_in_place = 0;
        }
    }
    next_action = EntityAi_PickCombatChoice(entity, 0);
    if (player_state.position[1] - entity->model->position[1] < entity->params->near_distance) {
        entity->target = &player_state;
        entity->action_timer = rand() % 30 + 30;
        ai_state->move_choice = 0;
        next_action = 1;
        entity->combat_choice = next_action;
        ai_state->turn_in_place = 2;
    }
    if (player_state.position[1] - entity->model->position[1] > (s16)entity->params->action_frames) { // match: read signed here
        entity->target = &player_state;
        entity->action_timer = rand() % 30 + 30;
        ai_state->move_choice = 0;
        next_action = 1;
        entity->combat_choice = next_action;
        ai_state->turn_in_place = -2;
    }
    ENTITY_FLAG_BITS(entity)->look_at_target = TRUE;
    if (entity->action_timer == 0 || next_action == 0) {
        entity->prev_combat_choice = entity->combat_choice;
        entity->combat_choice = next_action;
        if (entity->combat_choice != entity->prev_combat_choice) {
            entity->choice_change_frame = frame_counter;
        }
        EntityAi_LowerChoiceWeight(entity, next_action);
        switch (entity->combat_choice) {
        case 0:
            if (EntityAi_PickAttack((char *)entity)) {
                EntityAi_StartAction(entity, LavaBird_RangedAttack, unknown);
            }
            return;
        case 1:
            entity->target = &player_state;
            entity->action_timer = rand() % 30 + 60;
            ai_state->move_choice = 0;
            break;
        default:
            entity->target = &player_state;
            entity->action_timer = rand() % 30 + 60;
            ai_state->move_choice = 0;
            break;
        }
    }
    if (entity->combat_choice == 1) {
        side = EntityAi_TurnFromPush(entity, direction);
        if (ai_state->move_choice == 0) {
            if (side > 0) {
                entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_RIGHT);
            } else if (side < 0) {
                entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_LEFT);
            } else {
                // match: called like EntityAi_MoveCommand, with a speed and a mode the callee ignores
                entity->move_command = LavaBird_SteerToTargetCommand(entity, 0, 0.0f);
            }
        }
    }
    EntityAi_StartAction(entity, ai_state->move, unknown);
}

/* The think function of state 1: as EntityAi_ThinkPursue, with flag bits 11 to 13 at 0 and the
   command of LavaBird_SteerToTargetCommand. */
static void LavaBird_ThinkPursue(Entity *entity, s32 unknown) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 direction[2];
    s32 turn;

    ENTITY_FLAG_BITS(entity)->motion_step_mode = 0;
    ENTITY_FLAG_BITS(entity)->attacking = FALSE;
    ENTITY_FLAG_BITS(entity)->look_at_target = FALSE;
    if (entity->action_timer == 0) {
        entity->target = &player_state;
    }
    turn = EntityAi_TurnFromPush(entity, direction);
    if (turn > 0) {
        entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_RIGHT);
    } else if (turn < 0) {
        entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_LEFT);
    } else {
        entity->move_command = LavaBird_SteerToTargetCommand(entity, 0, 50.0f);
    }
    EntityAi_StartAction(entity, ai_state->move, unknown);
}

/* The think function of state 0: as EntityAi_ThinkIdle, always with a point to go to, the head
   following, the command of LavaBird_SteerToTargetCommand, and the pitch command (work byte unk79) picked as in
   LavaBird_ThinkCombat. */
static void LavaBird_ThinkIdle(Entity *entity, s32 unknown) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 direction[4];
    s32 side;

    ENTITY_FLAG_BITS(entity)->motion_step_mode = 0;
    ENTITY_FLAG_BITS(entity)->look_at_target = TRUE;
    if (entity->action_timer == 0) {
        EntityPoint_PickNear((f32)entity->spawn_info->wander_radius, ai_state->target_point, entity->spawn_info, entity->model->position);
        entity->target = ai_state->target_point;
        entity->action_timer = rand() % 60 + 60;
        if (player_state.position[1] - entity->model->position[1] < entity->params->near_distance) {
            entity->target = &player_state;
            entity->action_timer = rand() % 30 + 30;
            ai_state->move_choice = 0;
            entity->combat_choice = 1;
            ai_state->turn_in_place = 2;
        }
        if (player_state.position[1] - entity->model->position[1] > (s16)entity->params->action_frames) { // match: read signed here
            entity->target = &player_state;
            entity->action_timer = rand() % 30 + 30;
            ai_state->move_choice = 0;
            entity->combat_choice = 1;
            ai_state->turn_in_place = -2;
        }
        if (rand() % 100 < entity->params->far_distance) {
            if (!(rand() & 1)) {
                entity->target = &player_state;
                entity->action_timer = rand() % 30 + 30;
                ai_state->move_choice = 0;
                entity->combat_choice = 1;
                ai_state->turn_in_place = 1;
            } else {
                entity->target = &player_state;
                entity->action_timer = rand() % 30 + 30;
                ai_state->move_choice = 0;
                entity->combat_choice = 1;
                ai_state->turn_in_place = -1;
            }
        } else {
            ai_state->turn_in_place = 0;
        }
    }
    side = EntityAi_TurnFromPush(entity, direction);
    if (side > 0) {
        entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_RIGHT);
    } else if (side < 0) {
        entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_LEFT);
    } else {
        // match: called like EntityAi_MoveCommand, with a speed and a mode the callee ignores
        entity->move_command = LavaBird_SteerToTargetCommand(entity, 0, 0.0f);
    }
    EntityAi_StartAction(entity, ai_state->move, unknown);
}

/* The move handler: keeps animation 3 on both motion slots, flag bits 11 to 13 at 0. LavaBird_ApplyMoveCommand
   does the turning and the moving. */
static void LavaBird_Move(Entity *entity) {
    entity->flags &= ~ENTITY_FLAG_MOTION_STEP_MODE_MASK;
    EntityAi_StartSlotMotion(entity, entity->ai_state.motion_slot_0, 0, 3, 5, ENTITY_CONTROLLER(entity)->motion_blend_frames, 0);
    EntityAi_StartSlotMotion(entity, entity->ai_state.motion_slot_1, 1, 3, 5, ENTITY_CONTROLLER(entity)->motion_blend_frames, 0);
}

/* Carries out the movement command in Entity.move_command; LavaBird_Think and LavaBird_Dormant
   call it (the move handler only keeps the animation). It turns the entity, rolls it to one side or
   the other with the turn bits, eases its pitch towards 10 degrees for each unit of work byte unk79
   (back to level at 0), and speeds up or slows down along its axes by EntityParams.speed_step_per_frame a frame up
   to unk54. */
static void LavaBird_ApplyMoveCommand(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 roll;
    f32 step;

    ENTITY_FLAG_BITS(entity)->motion_step_mode = 0;
    if (entity->move_command & MOVE_COMMAND_TURN_LEFT) {
        ai_state->turn_rate -= entity->params->turn_step_per_frame;
        if (ai_state->turn_rate < -entity->params->turn_limit) {
            ai_state->turn_rate = -entity->params->turn_limit;
        }
        roll = DEG_TO_RAD(ai_state->pitch_speed + 50.0f);
    } else if (entity->move_command & MOVE_COMMAND_TURN_RIGHT) {
        ai_state->turn_rate += entity->params->turn_step_per_frame;
        if (entity->params->turn_limit < ai_state->turn_rate) {
            ai_state->turn_rate = entity->params->turn_limit;
        }
        roll = DEG_TO_RAD(-50.0f - ai_state->pitch_speed);
    } else {
        if (entity->params->turn_step_per_frame < ai_state->turn_rate) {
            ai_state->turn_rate -= entity->params->turn_step_per_frame;
        } else if (ai_state->turn_rate < -entity->params->turn_step_per_frame) {
            ai_state->turn_rate += entity->params->turn_step_per_frame;
        } else {
            ai_state->turn_rate = 0.0f;
        }
        roll = 0.0f;
    }
    entity->turn_delta_yaw += ai_state->turn_rate;
    EntityMath_EaseAngleToward(&entity->model->rotation[2], roll, 60.0f);
    if (ai_state->turn_in_place > 0) {
        if (ABS((f32)ai_state->turn_in_place) * -DEG_TO_RAD(10) < entity->model->rotation[0]) {
            ai_state->part_angle_y -= 0.006981317f;
            if (ai_state->part_angle_y < -DEG_TO_RAD(1)) {
                ai_state->part_angle_y = -DEG_TO_RAD(1);
            }
        } else {
            ai_state->part_angle_y = 0.0f;
        }
    } else if (ai_state->turn_in_place < 0) {
        if (entity->model->rotation[0] < ABS((f32)ai_state->turn_in_place) * DEG_TO_RAD(10)) {
            ai_state->part_angle_y += 0.006981317f;
            if (DEG_TO_RAD(1) < ai_state->part_angle_y) {
                ai_state->part_angle_y = DEG_TO_RAD(1);
            }
        } else {
            ai_state->part_angle_y = 0.0f;
        }
    } else if (ai_state->turn_in_place == 0) {
        if (DEG_TO_RAD(1) < entity->model->rotation[0]) {
            ai_state->part_angle_y = -DEG_TO_RAD(1);
        } else if (entity->model->rotation[0] < -DEG_TO_RAD(1)) {
            ai_state->part_angle_y = DEG_TO_RAD(1);
        } else {
            ai_state->part_angle_y = -entity->model->rotation[0];
        }
    }
    entity->turn_delta_pitch += ai_state->part_angle_y;
    ENTITY_SCRATCH_VECTOR[0] = ENTITY_SCRATCH_VECTOR[1] = ENTITY_SCRATCH_VECTOR[2] = ENTITY_SCRATCH_VECTOR[3] = 0.0f;
    step = entity->params->speed_step_per_frame;
    if (entity->move_command & MOVE_COMMAND_FORWARD) {
        ai_state->part_angle_x -= step;
        if (ai_state->part_angle_x < -entity->params->max_speed) {
            ai_state->part_angle_x = -entity->params->max_speed;
        }
    } else if (entity->move_command & MOVE_COMMAND_BACK) {
        ai_state->part_angle_x += step;
        if (entity->params->max_speed < ai_state->part_angle_x) {
            ai_state->part_angle_x = entity->params->max_speed;
        }
    } else if (step < ai_state->part_angle_x) {
        ai_state->part_angle_x -= step;
    } else if (ai_state->part_angle_x < -step) {
        ai_state->part_angle_x += step;
    } else {
        ai_state->part_angle_x = 0.0f;
    }
    if (entity->move_command & MOVE_COMMAND_STEP_LEFT) {
        ai_state->move_speed += entity->params->speed_step_per_frame;
        if (entity->params->max_speed < ai_state->move_speed) {
            ai_state->move_speed = entity->params->max_speed;
        }
    } else if (entity->move_command & MOVE_COMMAND_STEP_RIGHT) {
        ai_state->move_speed -= entity->params->speed_step_per_frame;
        if (ai_state->move_speed < -entity->params->max_speed) {
            ai_state->move_speed = -entity->params->max_speed;
        }
    } else if (entity->params->speed_step_per_frame < ai_state->move_speed) {
        ai_state->move_speed -= entity->params->speed_step_per_frame;
    } else if (ai_state->move_speed < -entity->params->speed_step_per_frame) {
        ai_state->move_speed += entity->params->speed_step_per_frame;
    } else {
        ai_state->move_speed = 0.0f;
    }
    ENTITY_SCRATCH_VECTOR[0] = ai_state->move_speed;
    ENTITY_SCRATCH_VECTOR[1] = 0.0f;
    ENTITY_SCRATCH_VECTOR[2] = ai_state->part_angle_x;
    sceVu0RotMatrixZ(ENTITY_SCRATCH_MATRIX, identity_matrix, entity->model->rotation[2]);
    sceVu0RotMatrixX(ENTITY_SCRATCH_MATRIX, ENTITY_SCRATCH_MATRIX, entity->model->rotation[0]);
    sceVu0RotMatrixY(ENTITY_SCRATCH_MATRIX, ENTITY_SCRATCH_MATRIX, entity->model->rotation[1]);
    Vector_ApplyMatrix(ENTITY_SCRATCH_VECTOR, ENTITY_SCRATCH_MATRIX, ENTITY_SCRATCH_VECTOR);
    // match: a test whose result is never used (DarkSorcerer_ApplyMoveCommand doubles the speed on this bit); it leaves
    // no code, but the scratchpad reads below only get their own address loads when it is here
    if (entity->move_command & MOVE_COMMAND_RUN) {
        step += step;
    }
    entity->move_delta_x += ENTITY_SCRATCH_VECTOR[0];
    entity->move_delta_y += ENTITY_SCRATCH_VECTOR[1];
    entity->move_delta_z += ENTITY_SCRATCH_VECTOR[2];
}

/* The movement command for flying at the target: forward (1) always, with a turn bit (0x101 / 0x201) while the
   target is off the heading by more than the turn limit; 0 without a target. Called like EntityAi_MoveCommand;
   the last two parameters are not used. */
static s32 LavaBird_SteerToTargetCommand(Entity *entity, s32 mark_far, f32 range) {
    s32 result;
    f32 angle;
    f32 limit;

    if (entity->target == NULL) {
        return 0;
    }
    result = 1;
    angle = EntityMath_HeadingTo(entity->model->position, entity->target);
    angle = EntityMath_AngleDifference(entity->model->rotation[1], angle);
    limit = entity->params->turn_limit;
    if ((0.0f <= angle) ? (limit < angle) : (limit < -angle)) {
        if (0.0f <= angle) {
            result = 0x201;
        } else {
            result = 0x101;
        }
    }
    return result;
}

/* The dormant action (busy bit 0x20), on the motion slots: holds animation 1 until the player is
   within EntitySpawnInfo.wake_distance or flag bit 43 is set; then it starts effect 0x166 at model point
   0xFF and moves the entity in y by a speed that starts at EntityParams.type_param_0 and falls off, plays
   animation 2 to its end while LavaBird_ApplyMoveCommand moves it with command 1, goes to state 2 and thinks.
   */
static void LavaBird_Dormant(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    EntityAiStateKind151D30 *slots = (EntityAiStateKind151D30 *)ai_state;

    entity->busy_flags |= ENTITY_BUSY_DORMANT;
    switch (entity->action_step) {
    case 0:
        EntityAi_StartSlotMotion(entity, &slots->motion_slot_0, 0, 1, 0, 0, 1);
        EntityAi_StartSlotMotion(entity, &slots->motion_slot_1, 1, 1, 0, 0, 1);
        ENTITY_FLAG_BITS(entity)->motion_step_mode = 0;
        ai_state->turn_rate = 0.0f;
        ENTITY_FLAG_BITS(entity)->bit21 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit22 = TRUE;
        ENTITY_FLAG_BITS(entity)->no_wall_collision = TRUE;
        ENTITY_FLAG_BITS(entity)->no_second_wall_collision = TRUE;
        ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
        ENTITY_FLAG_BITS(entity)->attacking = FALSE;
        ENTITY_FLAG_BITS(entity)->look_at_target = FALSE;
        entity->action_step++;
        break;
    case 1:
        if (entity->distance_to_player < entity->spawn_info->wake_distance || ENTITY_FLAG_BITS(entity)->hit_by_player) {
            ai_state->part_angle_z = entity->params->type_param_0;
            EntityEvent_StartEffectAtPoint((char **)entity, 0x166, 0xFF, (int)entity->model->rotation);
            entity->action_step++;
        }
        break;
    case 2:
        entity->model->position[1] -= ai_state->part_angle_z;
        ai_state->part_angle_z -= 3.0f;
        if (ai_state->part_angle_z <= 0.0f) {
            EntityAi_StartSlotMotion(entity, &slots->motion_slot_0, 0, 2, 0, entity->params->motion_blend_frames, 1);
            EntityAi_StartSlotMotion(entity, &slots->motion_slot_1, 1, 2, 0, entity->params->motion_blend_frames, 1);
            entity->action_step++;
        }
        break;
    case 3:
        entity->model->position[1] -= ai_state->part_angle_z;
        ai_state->part_angle_z -= 1.0f;
        if (slots->motion_slot_0.frames_left == 0) {
            entity->action_step++;
        }
        break;
    case 4:
        if (slots->motion_slot_0.frame < 0x32) {
            entity->model->position[1] -= ai_state->part_angle_z;
            ai_state->part_angle_z -= 0.1f;
        }
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            ENTITY_FLAG_BITS(entity)->hit_strength = 0;
            EntityAi_SetState((char *)entity, 2);
            ENTITY_FLAG_BITS(entity)->bit21 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit22 = FALSE;
            ENTITY_FLAG_BITS(entity)->no_wall_collision = FALSE;
            ENTITY_FLAG_BITS(entity)->no_second_wall_collision = FALSE;
            ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            if (ENTITY_FLAG_BITS(entity)->own_think) {
                ai_state->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, ai_state->move, 1);
            }
        }
        entity->move_command = MOVE_COMMAND_FORWARD;
        LavaBird_ApplyMoveCommand(entity);
        break;
    }
}

/* The ranged attack (busy bit 1): as EntityAi_RangedAttack on the first motion slot, flag bits 11
   to 13 at 0; on the frame of the record's first shot LavaBird_LaunchBothShots launches at
   the player. */
static void LavaBird_RangedAttack(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    EntityAiStateKind151D30 *slots = (EntityAiStateKind151D30 *)ai_state;
    EntityParamsUnk3D8 *unk3D8 = ENTITY_PARAMS_UNK3D8(entity->params, ENTITY_FLAG_BITS(entity)->attack_index);
    s32 frame;
    s32 launch_frame;
    s32 current_frame;
    f32 unused_vector[4]; // match: the frame has 16 unused bytes, and they keep the tail calls from becoming jumps

    entity->busy_flags |= ENTITY_BUSY_ATTACK;
    switch (entity->action_step) {
    case 0:
        EntityAi_StartSlotMotion(entity, &((EntityAiStateKind151D30 *)&entity->ai_state)->motion_slot_0, 0, ENTITY_FLAG_BITS(entity)->attack_index + 0x17, 0, entity->params->motion_blend_frames >> 1, 1);
        ENTITY_FLAG_BITS(entity)->motion_step_mode = 0;
        entity->action_step++;
    case 1:
        if (slots->motion_slot_0.frames_left == 0) {
            entity->action_step++;
        }
        if (unk3D8->turn_frames != 0) {
            EntityAi_TurnToHeading(entity, &ai_state->turn_rate, entity->heading_to_player);
        }
        break;
    case 2:
        if (slots->motion_slot_0.frame <= unk3D8->turn_frames) {
            EntityAi_TurnToHeading(entity, &entity->ai_state.turn_rate, entity->heading_to_player);
        } else {
            ai_state->turn_rate = 0.0f;
        }
        frame = (slots->motion_slot_0.frame_fixed - entity->params->motion_speeds[entity->animation_index]) >> 8;
        current_frame = slots->motion_slot_0.frame;
        launch_frame = ((EntityParamsUnk3D8Full *)unk3D8)->launches[0].frame;
        if (launch_frame != 0 && frame < launch_frame && launch_frame <= current_frame) {
            LavaBird_LaunchBothShots(entity, unk3D8, frame, current_frame, player_state.position, DEG_TO_RAD(45), DEG_TO_RAD(45));
        }
        if (slots->motion_slot_0.frame > 0 && unk3D8->attack_end_frame >= slots->motion_slot_0.frame) {
            ENTITY_FLAG_BITS(entity)->attack_window = 1;
        } else {
            ENTITY_FLAG_BITS(entity)->attack_window = 0;
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

/* Helper of LavaBird_RangedAttack: launches both shots of a record at once, whatever the
   animation frame, each from its point and turned towards `target` by at most the two rates. */
static s32 LavaBird_LaunchBothShots(Entity *entity, EntityParamsUnk3D8 *unk3D8, s32 frame_from, s32 frame_to, f32 *target, f32 pitch_rate, f32 yaw_rate) {
    EntityParamsUnk3D8Full *record = (EntityParamsUnk3D8Full *)unk3D8;
    EntityUnk1B1390 *shot;
    f32 angles[4];
    f32 *from;
    f32 *tip;
    f32 delta_x;
    f32 delta_y;
    f32 delta_z;
    f32 pitch;
    f32 yaw;
    s32 i;

    if (entity->ailment_4_frames != 0 && record->ignores_ailment_4 == 0) {
        return 0;
    }
    for (i = 0; i < 2; i++) {
        shot = Shot_Alloc();
        shot->player_magic = shot->player_power = 0;
        shot->is_magic_shot = 1;
        shot->shot_params = entity->params->shot_params;
        from = (f32 *)Model_GetSkinnedBonePoint((char *)entity->model, record->launches[i].bone);
        tip = (f32 *)Model_GetSkinnedBonePoint((char *)entity->model, record->launches[i].bone + 1);
        if (from != NULL && tip != NULL) {
            if (record->launches[i].bone != 0xFF) {
                delta_x = tip[0] - from[0];
                delta_y = tip[1] - from[1];
                delta_z = tip[2] - from[2];
                angles[0] = -atan2f(delta_y, sqrtf(delta_x * delta_x + delta_z * delta_z));
                angles[1] = EntityMath_WrapAngle(-atan2f(delta_z, delta_x) + HALF_PI);
                angles[2] = angles[3] = 0.0f;
            } else {
                Vector_Copy(angles, entity->model->rotation);
                angles[1] = EntityMath_WrapAngle(angles[1] + PI);
            }
            if (target != NULL) {
                pitch = EntityMath_WrapAngle(-atan2(target[1] - from[1], EntityMath_DistanceXZ(from, target)));
                yaw = EntityMath_AngleXZ(from, (char *)target);
                angles[0] = EntityMath_Approach(angles[0], pitch, pitch_rate);
                angles[1] = EntityMath_Approach(angles[1], yaw, yaw_rate);
            }
            Attack_LaunchMagicEffect(record->launches[i].magic_effect, shot, from, angles, entity);
        }
    }
    return 0;
}

/* The on_hit handler: as EntityAi_OnHit with the type's own two reactions; a light hit starts
   LavaBird_ReactionLight whether flag bit 19 is set or not. */
static void LavaBird_OnHit(Entity *entity) {
    EntityAi_LowerWeightsAfterHit((char *)entity);
    entity->target = NULL;
    ENTITY_FLAG_BITS(entity)->attacking = FALSE;
    ENTITY_FLAG_BITS(entity)->look_at_target = FALSE;
    if (ENTITY_FLAG_BITS(entity)->hit_strength == 1) {
        EntityAi_SetReaction(entity, LavaBird_ReactionLight, 0);
    } else if (ENTITY_FLAG_BITS(entity)->hit_strength == 2) {
        EntityAi_SetReaction(entity, LavaBird_ReactionHeavy, 0);
    }
    EntityAi_SetState((char *)entity, 2);
}

/* The reaction to a light hit (busy bit 2): as EntityAi_ReactionLight, on the first motion slot,
   flag bits 11 to 13 at 0. */
static void LavaBird_ReactionLight(Entity *entity) {
    EntityAiStateKind1740A8 *ai_state = (EntityAiStateKind1740A8 *)&entity->ai_state;

    entity->busy_flags = ENTITY_BUSY_REACTION_LIGHT;
    switch (entity->reaction_step) {
    case 0:
        EntityEvent_StartFrameCues(entity, 0xE, 0, 0);
        EntityAi_StartSlotMotion(entity, &((EntityAiStateKind1740A8 *)&entity->ai_state)->motion_slot_0, 0, 0xE, 0, 5, 1);
        ENTITY_FLAG_BITS(entity)->motion_step_mode = 0;
        ai_state->turn_rate = 0;
        ENTITY_FLAG_BITS(entity)->animates = TRUE;
        ENTITY_FLAG_BITS(entity)->attack_window = FALSE;
        ENTITY_FLAG_BITS(entity)->guarding = FALSE;
        ENTITY_FLAG_BITS(entity)->attacking = FALSE;
        ENTITY_FLAG_BITS(entity)->look_at_target = FALSE;
        entity->reaction_step++;
        break;
    case 1:
        if (ai_state->motion_slot_0.frames_left == 0) {
            entity->reaction_step++;
        }
        break;
    case 2:
        EntityAi_SetAttackDelay(entity, entity->params->attack_delay);
        EntityAi_SetReaction(entity, NULL, 0);
        entity->busy_flags = 0;
        entity->move_command = 0;
        entity->action_timer = 0;
        if (ENTITY_FLAG_BITS(entity)->own_think) {
            entity->ai_state.think(entity, 1);
        } else {
            EntityAi_StartAction(entity, ai_state->move, 1);
        }
        break;
    }
}

/* The reaction to a heavy hit (busy bit 4): as EntityAi_ReactionHeavy, on the motion slots, flag
   bits 11 to 13 at 0. */
static void LavaBird_ReactionHeavy(Entity *entity) {
    EntityAiStateKind1740A8 *ai_state = (EntityAiStateKind1740A8 *)&entity->ai_state;

    entity->busy_flags = ENTITY_BUSY_REACTION_HEAVY;
    switch (entity->reaction_step) {
    case 0:
        EntityAi_StartSlotMotion(entity, &((EntityAiStateKind1740A8 *)&entity->ai_state)->motion_slot_0, 0, 0xF, 0, 5, 1);
        EntityAi_StartSlotMotion(entity, &((EntityAiStateKind1740A8 *)&entity->ai_state)->motion_slot_1, 1, 0xF, 0, 5, 1);
        ENTITY_FLAG_BITS(entity)->motion_step_mode = 0;
        ai_state->turn_rate = 0;
        ENTITY_FLAG_BITS(entity)->animates = TRUE;
        ENTITY_FLAG_BITS(entity)->attack_window = FALSE;
        ENTITY_FLAG_BITS(entity)->guarding = FALSE;
        if (!ENTITY_PARAMS_BITS(entity)->bit25 && !ENTITY_PARAMS_BITS(entity)->bit26) {
            ENTITY_FLAG_BITS(entity)->no_floor_lift = FALSE;
        }
        ENTITY_FLAG_BITS(entity)->attacking = FALSE;
        ENTITY_FLAG_BITS(entity)->look_at_target = FALSE;
        entity->reaction_step++;
        break;
    case 1:
        if (ai_state->motion_slot_0.frames_left == 0) {
            entity->reaction_step++;
        }
        break;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            EntityAi_SetAttackDelay(entity, entity->params->attack_delay);
            EntityAi_SetReaction(entity, NULL, 0);
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

/* The death action (busy bit 0x40): as EntityAi_Death, on the motion slots, flag bits 11 to 13 at
   0. */
static void LavaBird_Death(Entity *entity) {
    EntityAiStateKind1740A8 *ai_state = (EntityAiStateKind1740A8 *)&entity->ai_state;

    entity->busy_flags = ENTITY_BUSY_DEATH;
    switch (entity->action_step) {
    case 0:
        EntityAi_StartSlotMotion(entity, &((EntityAiStateKind1740A8 *)&entity->ai_state)->motion_slot_0, 0, 0x10, 0, 5, 1);
        EntityAi_StartSlotMotion(entity, &((EntityAiStateKind1740A8 *)&entity->ai_state)->motion_slot_1, 1, 0x10, 0, 5, 1);
        ENTITY_FLAG_BITS(entity)->motion_step_mode = 0;
        if (!ENTITY_PARAMS_BITS(entity)->bit25 && ENTITY_PARAMS_BITS(entity)->bit26) {
            entity->collision_radius = 20.0f;
        }
        ENTITY_FLAG_BITS(entity)->animates = TRUE;
        ENTITY_FLAG_BITS(entity)->bit20 = TRUE;
        ENTITY_FLAG_BITS(entity)->dying = 1;
        ENTITY_FLAG_BITS(entity)->no_player_collision = TRUE;
        ENTITY_FLAG_BITS(entity)->no_entity_collision = TRUE;
        ENTITY_FLAG_BITS(entity)->no_floor_lift = FALSE;
        ENTITY_FLAG_BITS(entity)->look_at_target = FALSE;
        entity->action_step++;
    case 1:
        if (ai_state->motion_slot_0.frames_left == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            ENTITY_FLAG_BITS(entity)->animates = FALSE;
            ENTITY_FLAG_BITS(entity)->dying = 2;
            ENTITY_FLAG_BITS(entity)->no_respawn = TRUE;
            entity->action_timer = 20;
            EntityEvent_OnDeath(entity);
            entity->action_step++;
        }
        break;
    case 3:
        if (entity->action_timer != 0) {
            entity->model->alpha = (f32)entity->model_alpha * (f32)entity->action_timer / 20.0f;
        } else {
            ENTITY_FLAG_BITS(entity)->dying = 3;
            ENTITY_FLAG_BITS(entity)->live = FALSE;
        }
        break;
    }
}

/* The animate handler: puts flag bits 11 to 13 to 0, then plays the two motion slots of the work
   area (unk80 and unk88) where EntityAi_Animate plays the model's one motion, then turns the head
   (EntityAi_LookAtPoint with the target while flag bit 9 is set and there is one). */
static void LavaBird_Animate(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    EntityAiStateKind151D30 *slots = (EntityAiStateKind151D30 *)ai_state;
    f32 limits[4];
    f32 unused_vector[4]; // match: the frame has 16 unused bytes
    s32 previous;
    f32 rate;

    ENTITY_FLAG_BITS(entity)->motion_step_mode = 0;
    if (!ENTITY_FLAG_BITS(entity)->animates) {
        return;
    }
    if (slots->motion_slot_0.frames_left != 0) {
        if (entity->busy_flags & (ENTITY_BUSY_GUARD | ENTITY_BUSY_REACTION_LIGHT)) {
            rate = ai_state->pose_blend_weight;
        } else {
            rate = 1.0f / slots->motion_slot_0.frames_left;
        }
        // match: the caller passes a float the callee does not take
        ENTITY_FLAG_BITS(entity)->motion_ended = ((int (*)(void *, void *, int, float))EntityAi_StepSlotMotion)(entity, &slots->motion_slot_0, 0, rate);
    } else {
        previous = slots->motion_slot_0.frame_fixed;
        ENTITY_FLAG_BITS(entity)->motion_ended = Motion_StepSlot(entity, &((EntityAiStateKind151D30 *)&entity->ai_state)->motion_slot_0, 0, ENTITY_FLAG_BITS(entity)->motion_step_mode);
        if (slots->motion_slot_0.index != 3) {
            EntityEvent_StartFrameEffects(entity, slots->motion_slot_0.index, previous, slots->motion_slot_0.frame_fixed);
            EntityEvent_StartFrameCues(entity, slots->motion_slot_0.index, previous, slots->motion_slot_0.frame_fixed);
        }
    }
    if (slots->motion_slot_1.frames_left != 0) {
        if (entity->busy_flags & (ENTITY_BUSY_GUARD | ENTITY_BUSY_REACTION_LIGHT)) {
            rate = ai_state->pose_blend_weight;
        } else {
            rate = 1.0f / slots->motion_slot_1.frames_left;
        }
        // match: the caller passes a float the callee does not take
        ((int (*)(void *, void *, int, float))EntityAi_StepSlotMotion)(entity, &slots->motion_slot_1, 1, rate);
    } else {
        previous = slots->motion_slot_1.frame_fixed;
        Motion_StepSlot(entity, &slots->motion_slot_1, 1, ENTITY_FLAG_BITS(entity)->motion_step_mode);
        if (slots->motion_slot_1.index == 3) {
            EntityEvent_StartFrameEffects(entity, slots->motion_slot_1.index, previous, slots->motion_slot_1.frame_fixed);
            EntityEvent_StartFrameCues(entity, slots->motion_slot_1.index, previous, slots->motion_slot_1.frame_fixed);
        }
    }
    limits[1] = DEG_TO_RAD(entity->params->pitch_limit_deg);
    limits[0] = DEG_TO_RAD(-entity->params->pitch_limit_deg);
    limits[3] = DEG_TO_RAD(entity->params->yaw_limit_deg);
    limits[2] = DEG_TO_RAD(-entity->params->yaw_limit_deg);
    if (ENTITY_FLAG_BITS(entity)->look_at_target && entity->target != NULL) {
        EntityAi_LookAtPoint(entity, (int)ai_state->look_part_angles, ENTITY_FLAG_BITS(entity)->look_at_target, (int)entity->target, (int)limits, 12.0f);
    } else {
        EntityAi_LookAtPlayer(entity, ai_state->look_part_angles, ENTITY_FLAG_BITS(entity)->look_at_target, NULL, 8.0f);
    }
}
