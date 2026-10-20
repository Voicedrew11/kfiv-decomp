/* DarkBird: the behaviour of entity type 59 (EntityParams.entity_type, the index into entity_type_handlers): its set-up
   function, which the table names, and the handlers it installs. Used by: Dark Bird. The game names no
   monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by
   hit points and experience, with resistances and drops where two monsters share those. */
#include "entity/entity.h"
#include "common.h"
#include "effect/shot.h"

#define ENTITY_WORK_AREA_SIZE 0xA0
extern s32 entity_type_handlers[][2];
void EntityAi_SetUpdate(char *, s32);
int EntityAi_StartAction(Entity *, void (*)(Entity *), int);
void Motion_ClearPartSlots(EntityModel *);
void Motion_SetPartSlots(EntityModel *, s32, s32, s32);
s32 Motion_StepModel(ModelInstance *, s32, s32);
void Vector_Copy(void *, void *);
void EntityAi_Dormant(Entity *);
extern u32 frame_counter;
void EntityAi_SensePlayer(void *, int);
int EntityAi_StartSlotMotion(void *, void *, int, int, int, int, int);
void EntityAi_CheckFarFromSpawn(void *);
void EntityAi_StepState(void *);
int EntityAi_ThinkReturn(void *, int);
void EntityAi_ThinkPursue(Entity *, s32);
extern PlayerData player_state;
int rand(void);
s32 EntityAi_PickCombatChoice(Entity *, s32);
f32 EntityCollision_DistanceToPropFloor(void *, void *);
void EntityAi_LowerChoiceWeight(Entity *, s32);
int EntityAi_PickAttack(char *);
int EntityAi_TurnFromPush(void *, void *);
void EntityPoint_PickNear(float, void *, void *, void *);
extern f32 identity_matrix[16];
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0RotMatrixZ(float *, float *, float);
void Vector_ApplyMatrix(void *, void *, void *);
void Vector_Scale(float *, float *, float);
float EntityMath_HeadingTo(float *, float *);
float EntityMath_AngleDifference(float, float);
float sinf(float);
float cosf(float);
int Motion_Start(void *, int, int, u32, int);
void EntityAi_SetState(char *, u8);
float EntityMath_WrapAngle(float);
s32 EntityAi_TurnToHeading(Entity *, f32 *, f32);
int EntityAi_CheckMeleeHit(Entity *, void *, s32, s32, s32);
void EntityAi_SetAttackDelay(Entity *entity, s32 base);
double atan2(double, double);
char *Model_GetSkinnedBonePoint(char *, u32);
float EntityMath_DistanceXZ(void *, void *);
float sqrtf(float);
float atan2f(float, float);
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

static void DarkBird_Think(Entity *entity, s32 unknown);
static void DarkBird_ThinkCombat(Entity *entity, s32 unknown);
static void DarkBird_ThinkIdle(Entity *entity, s32 unknown);
static void DarkBird_Move(Entity *entity);
static void DarkBird_ApplyMoveCommand(Entity *entity);
static int DarkBird_SteerToTargetCommand(Entity *entity, s32 unused, f32 unused_weight);
static void DarkBird_DormantMoveToCircle(Entity *entity);
static void DarkBird_ApproachForMelee(Entity *entity);
static void DarkBird_MeleeAttack(Entity *entity);
static void DarkBird_RangedAttack(Entity *entity);
static s32 DarkBird_LaunchBothShots(Entity *entity, EntityParamsUnk3D8 *unk3D8, s32 frame_from, s32 frame_to, f32 *target, f32 pitch_rate, f32 yaw_rate);
static s32 DarkBird_LaunchThreeShots(Entity *entity, EntityParamsUnk3D8Full *record, s32 frame_from, s32 frame_to, f32 *angles);
static void DarkBird_OnHit(Entity *entity);
static void DarkBird_ReactionLight(Entity *entity);
static void DarkBird_ReactionHeavy(Entity *entity);
static void DarkBird_Death(Entity *entity);
static void DarkBird_Animate(Entity *entity);

/* Set-up of a type 59 entity: clears its work area and installs the type's handlers. */
void DarkBird_Init(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    EntityAiStateKind151D30 *slots = (EntityAiStateKind151D30 *)&entity->ai_state;
    u8 *unkBC = entity->choice_weights;
    u8 *unkC0 = entity->attack_weights;
    char *unk138 = entity->model->look_at_part;
    s32 i;

    // match: the three fills name their target inside the loop; unkBC and unkC0 above are
    // otherwise unused but retail computes them up front and copies them before each loop
    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        u8 *work_area = (u8 *)&entity->ai_state;

        work_area[i] = 0;
    }
    EntityAi_SetUpdate((char *)entity, entity_type_handlers[entity->params->entity_type][1]);
    ai_state->think = DarkBird_Think;
    ai_state->animate = DarkBird_Animate;
    ai_state->on_hit = DarkBird_OnHit;
    ai_state->move = DarkBird_Move;
    ai_state->death = DarkBird_Death;
    if (entity->spawn_info->spawn_mode == 1) {
        ai_state->dormant = DarkBird_DormantMoveToCircle;
    } else {
        ai_state->dormant = EntityAi_Dormant;
    }
    for (i = 3; i != -1; i--) {
        entity->choice_weights[i] = 0x80;
    }
    for (i = 5; i != -1; i--) {
        entity->attack_weights[i] = 0x80;
    }
    entity->flags |= ENTITY_FLAG_NO_FLOOR_LIFT;
    entity->target = NULL;
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
    Motion_SetPartSlots(entity->model, 6, 0xB, 1);
    slots->motion_slot_0.bit5 = TRUE;
    slots->motion_slot_0.bit6 = TRUE;
    slots->motion_slot_1.bit6 = TRUE;
    Motion_StepModel(ENTITY_MODEL(entity), 0, 0);
}

/* The think handler: as EntityAi_Think, with DarkBird_ThinkIdle for state 0 and
   DarkBird_ThinkCombat for state 2; the first time in state 2 (work byte unk7A) it starts
   DarkBird_ApproachForMelee instead. It keeps flag bit 28 set except in the death action, keeps
   animation 3 on the second motion slot outside attacks, heavy-hit reactions and the death, and
   calls DarkBird_ApplyMoveCommand while the entity is not busy or is in a hit reaction. */
static void DarkBird_Think(Entity *entity, s32 unknown) {
    EntityAiStateKind1740A8 *ai_state = (EntityAiStateKind1740A8 *)&entity->ai_state;

    if (frame_counter % 10U == entity->spawn_index % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (entity->busy_flags & ENTITY_BUSY_DEATH) {
        entity->flags &= ~ENTITY_FLAG_NO_FLOOR_LIFT;
    } else {
        entity->flags |= ENTITY_FLAG_NO_FLOOR_LIFT;
    }
    if (!(entity->busy_flags & (ENTITY_BUSY_DEATH | ENTITY_BUSY_REACTION_HEAVY | ENTITY_BUSY_ATTACK))) {
        EntityAi_StartSlotMotion(entity, &ai_state->motion_slot_1, 1, 3, 0, entity->params->motion_blend_frames, 0);
    }
    if (entity->busy_flags == 0) {
        EntityAi_CheckFarFromSpawn(entity);
        EntityAi_StepState(entity);
        if (ENTITY_FLAG_BITS(entity)->far_from_spawn) {
            EntityAi_ThinkReturn(entity, unknown);
        } else {
            switch (entity->state) {
            case 0:
                ai_state->think_flag = 0;
                DarkBird_ThinkIdle(entity, unknown);
                break;
            case 1:
                EntityAi_ThinkPursue(entity, unknown);
                break;
            case 2:
                if (ai_state->think_flag == 0) {
                    EntityAi_StartAction(entity, DarkBird_ApproachForMelee, unknown);
                    ai_state->think_flag = 1;
                } else {
                    DarkBird_ThinkCombat(entity, unknown);
                }
                break;
            default:
                DarkBird_ThinkIdle(entity, unknown);
                break;
            }
        }
    }
    if (entity->busy_flags == 0 || (entity->busy_flags & (ENTITY_BUSY_REACTION_LIGHT | ENTITY_BUSY_REACTION_HEAVY))) {
        DarkBird_ApplyMoveCommand(entity);
    }
}

/* The think function of state 2: as EntityAi_ThinkCombat without the guard and the dodge, and with
   the command of DarkBird_SteerToTargetCommand. It also picks the pitch command, work byte unk79: 1 or -1 at random
   (EntityParams.far_distance times in 100), 2 or -2 to keep the height in bounds (in mode 0 EntityCollision_DistanceToPropFloor
   between EntityParams.near_distance and unk434, in modes 1 and 2 the y against the spawn point's). A
   ranged attack is DarkBird_RangedAttack, but only when the player's y less its own is 200 or
   more; a melee one is DarkBird_MeleeAttack when that difference is between 100 and 400, else
   DarkBird_ApproachForMelee. */
static void DarkBird_ThinkCombat(Entity *entity, s32 unknown) {
    EntityAiStateKind1740A8 *ai_state = (EntityAiStateKind1740A8 *)&entity->ai_state;
    f32 direction[4];
    f32 height;
    s32 side;
    s32 turn;

    if (entity->params->flags & ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE) {
        ENTITY_FLAG_BITS(entity)->motion_step_mode = 0;
    } else {
        ENTITY_FLAG_BITS(entity)->motion_step_mode = 1;
    }
    if (entity->action_timer == 0) {
        if (rand() % 100 < entity->params->far_distance) {
            if (!(rand() & 1)) {
                entity->target = &player_state;
                entity->action_timer = rand() % 30 + 0x1E;
                ai_state->steer_to_target = 0;
                entity->combat_choice = 1;
                ai_state->climb_command = 1;
            } else {
                entity->target = &player_state;
                entity->action_timer = rand() % 30 + 0x1E;
                ai_state->steer_to_target = 0;
                entity->combat_choice = 1;
                ai_state->climb_command = -1;
            }
        } else if (entity->action_timer == 0) {
            ai_state->climb_command = 0;
        }
    }
    side = EntityAi_PickCombatChoice(entity, 0);
    if (entity->spawn_info->spawn_mode == 0) {
        if (EntityCollision_DistanceToPropFloor(entity->map_record, entity->model->position) < (f32)entity->params->near_distance ||
            ENTITY_FLAG_BITS(entity)->on_floor == 1) {
            side = 1;
            entity->target = &player_state;
            entity->action_timer = rand() % 30 + 0x1E;
            ai_state->steer_to_target = 0;
            entity->combat_choice = 1;
            ai_state->climb_command = 2;
        } else if (EntityCollision_DistanceToPropFloor(entity->map_record, entity->model->position) > (f32)(s16)entity->params->action_frames ||
                   ENTITY_FLAG_BITS(entity)->bit46 == 1) {
            side = 1;
            entity->target = &player_state;
            entity->action_timer = rand() % 30 + 0x1E;
            ai_state->steer_to_target = 0;
            entity->combat_choice = 1;
            ai_state->climb_command = -2;
        }
    } else if (entity->spawn_info->spawn_mode == 1 || entity->spawn_info->spawn_mode == 2) {
        if (entity->spawn_info->spawn_y - entity->model->position[1] <
                (f32)entity->spawn_info->map_object_id * (entity->spawn_info->second_map_object_id != 0 ? 100.0f : -100.0f) ||
            ENTITY_FLAG_BITS(entity)->on_floor == 1) {
            side = 1;
            entity->target = &player_state;
            entity->action_timer = rand() % 30 + 0x1E;
            ai_state->steer_to_target = 0;
            entity->combat_choice = 1;
            ai_state->climb_command = 2;
        } else if (entity->spawn_info->spawn_y - entity->model->position[1] > (f32)entity->spawn_info->variant * 100.0f ||
                   ENTITY_FLAG_BITS(entity)->bit46 == 1) {
            side = 1;
            entity->target = &player_state;
            entity->action_timer = rand() % 30 + 0x1E;
            ai_state->steer_to_target = 0;
            entity->combat_choice = 1;
            ai_state->climb_command = -2;
        }
    }
    ENTITY_FLAG_BITS(entity)->look_at_target = 1;
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
                if (player_state.position[1] - entity->model->position[1] < 200.0f) {
                    entity->target = &player_state;
                    entity->action_timer = rand() % 30 + 0x1E;
                    ai_state->steer_to_target = 0;
                    entity->combat_choice = 1;
                    ai_state->climb_command = 2;
                } else {
                    EntityAi_StartAction(entity, DarkBird_RangedAttack, unknown);
                    return;
                }
            } else {
                height = player_state.position[1] - entity->model->position[1];
                if (100.0f < height && height < 400.0f) {
                    EntityAi_StartAction(entity, DarkBird_MeleeAttack, unknown);
                } else {
                    EntityAi_StartAction(entity, DarkBird_ApproachForMelee, unknown);
                }
                return;
            }
            break;
        case 1:
            entity->target = &player_state;
            entity->action_timer = rand() % 30 + 0x3C;
            ai_state->steer_to_target = 0;
            break;
        default:
            entity->target = &player_state;
            entity->action_timer = rand() % 30 + 0x3C;
            ai_state->steer_to_target = 0;
            break;
        }
    }
    if (ai_state->steer_to_target == 0) {
        turn = EntityAi_TurnFromPush(entity, direction);
        if (turn > 0) {
            entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_RIGHT);
        } else if (turn < 0) {
            entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_LEFT);
        } else {
            // match: retail passes two more arguments here and takes the result as 16 bits
            entity->move_command = DarkBird_SteerToTargetCommand(entity, 0, 0.0f);
        }
    }
    EntityAi_StartAction(entity, ai_state->move, unknown);
}

/* The think function of state 0: as EntityAi_ThinkIdle, always with a point to go to, the head
   following, the command of DarkBird_SteerToTargetCommand, and the pitch command (work byte unk79) picked as in
   DarkBird_ThinkCombat. */
static void DarkBird_ThinkIdle(Entity *entity, s32 unknown) {
    EntityAiStateKind1740A8 *ai_state = (EntityAiStateKind1740A8 *)&entity->ai_state;
    f32 direction[4];
    s32 turn;

    if (entity->params->flags & ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE) {
        ENTITY_FLAG_BITS(entity)->motion_step_mode = 0;
    } else {
        ENTITY_FLAG_BITS(entity)->motion_step_mode = 1;
    }
    ENTITY_FLAG_BITS(entity)->look_at_target = 1;
    if (entity->action_timer == 0) {
        EntityPoint_PickNear((f32)entity->spawn_info->wander_radius, ai_state->target_point, entity->spawn_info, entity->model->position);
        entity->target = ai_state->target_point;
        entity->action_timer = rand() % 60 + 0x3C;
        if (entity->spawn_info->spawn_mode == 0) {
            if (EntityCollision_DistanceToPropFloor(entity->map_record, entity->model->position) < (f32)entity->params->near_distance) {
                entity->target = &player_state;
                entity->action_timer = rand() % 30 + 0x1E;
                ai_state->steer_to_target = 0;
                entity->combat_choice = 1;
                ai_state->climb_command = 2;
            } else if (EntityCollision_DistanceToPropFloor(entity->map_record, entity->model->position) > (f32)(s16)entity->params->action_frames) {
                entity->target = &player_state;
                entity->action_timer = rand() % 30 + 0x1E;
                ai_state->steer_to_target = 0;
                entity->combat_choice = 1;
                ai_state->climb_command = -2;
            }
        } else if (entity->spawn_info->spawn_mode == 1 || entity->spawn_info->spawn_mode == 2) {
            if (entity->spawn_info->spawn_y - entity->model->position[1] <
                    (f32)entity->spawn_info->map_object_id * (entity->spawn_info->second_map_object_id != 0 ? 100.0f : -100.0f) ||
                ENTITY_FLAG_BITS(entity)->on_floor == 1) {
                entity->target = &player_state;
                entity->action_timer = rand() % 30 + 0x1E;
                ai_state->steer_to_target = 0;
                entity->combat_choice = 1;
                ai_state->climb_command = 2;
            } else if (entity->spawn_info->spawn_y - entity->model->position[1] > (f32)entity->spawn_info->variant * 100.0f ||
                       ENTITY_FLAG_BITS(entity)->bit46 == 1) {
                entity->target = &player_state;
                entity->action_timer = rand() % 30 + 0x1E;
                ai_state->steer_to_target = 0;
                entity->combat_choice = 1;
                ai_state->climb_command = -2;
            }
        }
        if (rand() % 100 < entity->params->far_distance) {
            if (!(rand() & 1)) {
                entity->target = &player_state;
                entity->action_timer = rand() % 30 + 0x1E;
                ai_state->steer_to_target = 0;
                entity->combat_choice = 1;
                ai_state->climb_command = 1;
            } else {
                entity->target = &player_state;
                entity->action_timer = rand() % 30 + 0x1E;
                ai_state->steer_to_target = 0;
                entity->combat_choice = 1;
                ai_state->climb_command = -1;
            }
        } else {
            ai_state->climb_command = 0;
        }
    }
    turn = EntityAi_TurnFromPush(entity, direction);
    if (turn > 0) {
        entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_RIGHT);
    } else if (turn < 0) {
        entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_LEFT);
    } else {
        // match: retail passes two more arguments here and takes the result as 16 bits
        entity->move_command = DarkBird_SteerToTargetCommand(entity, 0, 0.0f);
    }
    EntityAi_StartAction(entity, ai_state->move, unknown);
}

/* The move handler: keeps animation 3 on both motion slots, flag bits 11 to 13 at 0. DarkBird_ApplyMoveCommand
   does the turning and the moving. */
static void DarkBird_Move(Entity *entity) {
    entity->flags &= ~ENTITY_FLAG_MOTION_STEP_MODE_MASK;
    EntityAi_StartSlotMotion(entity, entity->ai_state.motion_slot_0, 0, 3, 0, entity->params->motion_blend_frames, 0);
    EntityAi_StartSlotMotion(entity, entity->ai_state.motion_slot_1, 1, 3, 0, entity->params->motion_blend_frames, 0);
}

/* Carries out the movement command in Entity.move_command; DarkBird_Think and
   DarkBird_ApproachForMelee call it (the move handler only keeps the animation). It turns the
   entity, eases its pitch towards 10 degrees for each unit of work byte unk79 (back to level at 0),
   and speeds up or slows down along its axes by EntityParams.speed_step_per_frame a frame up to unk54. */
static void DarkBird_ApplyMoveCommand(Entity *entity) {
    EntityAiStateKind1719F8 *ai_state = (EntityAiStateKind1719F8 *)&entity->ai_state;

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
    } else if (entity->params->turn_step_per_frame < ai_state->turn_rate) {
        ai_state->turn_rate -= entity->params->turn_step_per_frame;
    } else if (ai_state->turn_rate < -entity->params->turn_step_per_frame) {
        ai_state->turn_rate += entity->params->turn_step_per_frame;
    } else {
        ai_state->turn_rate = 0.0f;
    }
    entity->turn_delta_yaw += ai_state->turn_rate;
    if (ai_state->climb_command > 0) {
        if (ABS((f32)ai_state->climb_command) * -0.17453292f < entity->model->rotation[0]) {
            ai_state->pitch_speed -= 0.006981317f;
            if (ai_state->pitch_speed < -0.017453292f) {
                ai_state->pitch_speed = -0.017453292f;
            }
        } else {
            ai_state->pitch_speed = 0.0f;
        }
    } else if (ai_state->climb_command < 0) {
        if (entity->model->rotation[0] < ABS((f32)ai_state->climb_command) * 0.17453292f) {
            ai_state->pitch_speed += 0.006981317f;
            if (0.017453292f < ai_state->pitch_speed) {
                ai_state->pitch_speed = 0.017453292f;
            }
        } else {
            ai_state->pitch_speed = 0.0f;
        }
    } else if (ai_state->climb_command == 0) {
        if (0.017453292f < entity->model->rotation[0]) {
            ai_state->pitch_speed = -0.017453292f;
        } else if (entity->model->rotation[0] < -0.017453292f) {
            ai_state->pitch_speed = 0.017453292f;
        } else {
            ai_state->pitch_speed = -entity->model->rotation[0];
        }
    }
    entity->turn_delta_pitch += ai_state->pitch_speed;
    ENTITY_SCRATCH_VECTOR[0] = ENTITY_SCRATCH_VECTOR[1] = ENTITY_SCRATCH_VECTOR[2] = ENTITY_SCRATCH_VECTOR[3] = 0.0f;
    if (entity->move_command & MOVE_COMMAND_FORWARD) {
        ai_state->forward_speed -= entity->params->speed_step_per_frame;
        if (ai_state->forward_speed < -entity->params->max_speed) {
            ai_state->forward_speed = -entity->params->max_speed;
        }
    } else if (entity->move_command & MOVE_COMMAND_BACK) {
        ai_state->forward_speed += entity->params->speed_step_per_frame;
        if (entity->params->max_speed < ai_state->forward_speed) {
            ai_state->forward_speed = entity->params->max_speed;
        }
    } else if (ai_state->forward_speed > entity->params->speed_step_per_frame) {
        ai_state->forward_speed -= entity->params->speed_step_per_frame;
    } else if (ai_state->forward_speed < -entity->params->speed_step_per_frame) {
        ai_state->forward_speed += entity->params->speed_step_per_frame;
    } else {
        ai_state->forward_speed = 0.0f;
    }
    if (entity->move_command & MOVE_COMMAND_STEP_LEFT) {
        ai_state->side_speed += entity->params->speed_step_per_frame;
        if (entity->params->max_speed < ai_state->side_speed) {
            ai_state->side_speed = entity->params->max_speed;
        }
    } else if (entity->move_command & MOVE_COMMAND_STEP_RIGHT) {
        ai_state->side_speed -= entity->params->speed_step_per_frame;
        if (ai_state->side_speed < -entity->params->max_speed) {
            ai_state->side_speed = -entity->params->max_speed;
        }
    } else if (entity->params->speed_step_per_frame < ai_state->side_speed) {
        ai_state->side_speed -= entity->params->speed_step_per_frame;
    } else if (ai_state->side_speed < -entity->params->speed_step_per_frame) {
        ai_state->side_speed += entity->params->speed_step_per_frame;
    } else {
        ai_state->side_speed = 0.0f;
    }
    ENTITY_SCRATCH_VECTOR[0] = ai_state->side_speed;
    ENTITY_SCRATCH_VECTOR[1] = 0.0f;
    ENTITY_SCRATCH_VECTOR[2] = ai_state->forward_speed;
    sceVu0RotMatrixZ(ENTITY_SCRATCH_MATRIX, identity_matrix, entity->model->rotation[2]);
    sceVu0RotMatrixX(ENTITY_SCRATCH_MATRIX, ENTITY_SCRATCH_MATRIX, entity->model->rotation[0]);
    sceVu0RotMatrixY(ENTITY_SCRATCH_MATRIX, ENTITY_SCRATCH_MATRIX, entity->model->rotation[1]);
    Vector_ApplyMatrix(ENTITY_SCRATCH_VECTOR, ENTITY_SCRATCH_MATRIX, ENTITY_SCRATCH_VECTOR);
    if (entity->move_command & MOVE_COMMAND_RUN) {
        Vector_Scale(ENTITY_SCRATCH_VECTOR, ENTITY_SCRATCH_VECTOR, 2.0f);
    }
    entity->move_delta_x += ENTITY_SCRATCH_VECTOR[0];
    entity->move_delta_y += ENTITY_SCRATCH_VECTOR[1];
    entity->move_delta_z += ENTITY_SCRATCH_VECTOR[2];
}

/* The movement command for flying at the target: forward (1) always, with a turn bit (0x101 / 0x201) while the
   target is off the heading by more than the turn limit; 0 without a target. */
static int DarkBird_SteerToTargetCommand(Entity *entity, s32 unused, f32 unused_weight) {
    float angle;
    float limit;
    int result;

    if (entity->target == NULL) {
        return 0;
    }
    result = 1;
    angle = EntityMath_HeadingTo(&ENTITY_MODEL(entity)->position.x, entity->target);
    angle = EntityMath_AngleDifference(ENTITY_MODEL(entity)->rotation.y, angle);
    limit = ENTITY_CONTROLLER(entity)->turn_limit;
    if (0.0f <= angle ? limit < angle : limit < -angle) {
        result = 0x101;
        if (0.0f <= angle) {
            result = 0x201;
        }
    }
    return result;
}

/* The dormant action of a spawn in mode 1 (EntitySpawnInfo.spawn_mode is 1; busy bit 0x20): holds
   animation 1 as EntityAi_Dormant does; when the player is within EntitySpawnInfo.wake_distance or flag bit
   43 is set it goes to state 2 at once, puts the entity at a random point of the circle of radius
   2000 round the origin (x and z) and thinks. */
static void DarkBird_DormantMoveToCircle(Entity *entity) {
    EntityAiStateKind1740A8 *ai_state = (EntityAiStateKind1740A8 *)&entity->ai_state;
    f32 angle;

    entity->busy_flags |= ENTITY_BUSY_DORMANT;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 1, 0, 0, 1);
        ENTITY_FLAG_BITS(entity)->motion_step_mode = 0;
        ai_state->turn_rate = 0;
        ENTITY_FLAG_BITS(entity)->bit21 = 1;
        ENTITY_FLAG_BITS(entity)->bit22 = 1;
        ENTITY_FLAG_BITS(entity)->no_wall_collision = 1;
        ENTITY_FLAG_BITS(entity)->no_second_wall_collision = 1;
        ENTITY_FLAG_BITS(entity)->bit18 = 1;
        ENTITY_FLAG_BITS(entity)->no_floor_lift = 1;
        ENTITY_FLAG_BITS(entity)->attacking = 0;
        ENTITY_FLAG_BITS(entity)->look_at_target = 0;
        entity->action_step++;
        break;
    case 1:
        if (entity->distance_to_player < (f32)entity->spawn_info->wake_distance || ENTITY_FLAG_BITS(entity)->hit_by_player) {
            ENTITY_FLAG_BITS(entity)->hit_strength = 0;
            EntityAi_SetState((char *)entity, 2);
            ENTITY_FLAG_BITS(entity)->bit21 = 0;
            ENTITY_FLAG_BITS(entity)->bit22 = 0;
            ENTITY_FLAG_BITS(entity)->no_wall_collision = 0;
            ENTITY_FLAG_BITS(entity)->no_second_wall_collision = 0;
            ENTITY_FLAG_BITS(entity)->bit18 = 0;
            if (ENTITY_PARAMS_MOVEMENT_KIND(entity->params) == ENTITY_PARAMS_MOVEMENT_WALK) {
                ENTITY_FLAG_BITS(entity)->no_floor_lift = 0;
            }
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            angle = EntityMath_WrapAngle((f32)(rand() % 361) * 0.017453292f);
            entity->model->position[0] = sinf(angle) * 2000.0f;
            entity->model->position[2] = cosf(angle) * 2000.0f;
            if (ENTITY_FLAG_BITS(entity)->own_think) {
                ai_state->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, ai_state->move, 1);
            }
        }
        break;
    }
}

/* An action (busy bits 0x3000): animation 3 on both motion slots; for up to 212 frames it goes for
   the player (DarkBird_SteerToTargetCommand, DarkBird_ApplyMoveCommand) with the pitch command that brings the player's y less
   its own between 200 and 400, and starts DarkBird_MeleeAttack once the player is within the
   radius plus 350 and that difference is between 100 and 400. */
static void DarkBird_ApproachForMelee(Entity *entity) {
    EntityAiStateKind1740A8 *ai_state = (EntityAiStateKind1740A8 *)&entity->ai_state;
    f32 direction[4];
    f32 height;
    s32 turn;

    entity->busy_flags |= (ENTITY_BUSY_1000 | ENTITY_BUSY_2000);
    switch (entity->action_step) {
    case 0:
        EntityAi_StartSlotMotion(entity, &ai_state->motion_slot_0, 0, 3, 0, entity->params->motion_blend_frames, 0);
        EntityAi_StartSlotMotion(entity, &ai_state->motion_slot_1, 1, 3, 0, entity->params->motion_blend_frames, 0);
        ENTITY_FLAG_BITS(entity)->motion_step_mode = 0;
        ENTITY_FLAG_BITS(entity)->attacking = 1;
        ENTITY_FLAG_BITS(entity)->look_at_target = 1;
        entity->target = &player_state;
        ai_state->action_frames = 0;
        entity->action_step++;
    case 1:
        if (entity->distance_to_player < entity->radius + 250.0f + 100.0f) {
            f32 above = player_state.position[1] - entity->model->position[1];

            if (100.0f < above && above < 400.0f) {
                entity->busy_flags = 0;
                entity->move_command = 0;
                entity->action_timer = 0;
                EntityAi_StartAction(entity, DarkBird_MeleeAttack, 1);
                return;
            }
        }
        if (ai_state->action_frames++ >= 0xD3) {
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            if (ENTITY_FLAG_BITS(entity)->own_think) {
                ai_state->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, ai_state->move, 1);
            }
            return;
        }
        turn = EntityAi_TurnFromPush(entity, direction);
        if (turn > 0) {
            entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_RIGHT);
        } else if (turn < 0) {
            entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_LEFT);
        } else {
            // match: retail passes two more arguments here and takes the result as 16 bits
            entity->move_command = DarkBird_SteerToTargetCommand(entity, 0, 0.0f);
        }
        height = player_state.position[1] - entity->model->position[1];
        if (height < 200.0f || ENTITY_FLAG_BITS(entity)->on_floor == 1 || ENTITY_FLAG_BITS(entity)->wall_push == 1) {
            ai_state->climb_command = 2;
        } else if (400.0f < height || ENTITY_FLAG_BITS(entity)->bit46 == 1) {
            ai_state->climb_command = -2;
        } else {
            ai_state->climb_command = 0;
        }
        DarkBird_ApplyMoveCommand(entity);
        break;
    }
}

/* The melee attack (busy bit 1): as EntityAi_MeleeAttack, on the motion slots. */
static void DarkBird_MeleeAttack(Entity *entity) {
    EntityAiStateKind1740A8 *ai_state = (EntityAiStateKind1740A8 *)&entity->ai_state;
    s32 kind = (s32)(ENTITY_FLAG_WORD(entity->flags) >> 58) & 3;
    EntityParamsUnk240 *unk240 = &entity->params->melee_attacks[kind];
    s32 elapsed;

    entity->busy_flags |= ENTITY_BUSY_ATTACK;
    switch (entity->action_step) {
    case 0:
        EntityAi_StartSlotMotion(entity, &ai_state->motion_slot_0, 0, kind | ENTITY_ANIMATION_ATTACK_BASE, 0, entity->params->motion_blend_frames >> 1, 1);
        EntityAi_StartSlotMotion(entity, &ai_state->motion_slot_1, 1, ((s32)(entity->flags >> 58) & 3) | ENTITY_ANIMATION_ATTACK_BASE, 0, entity->params->motion_blend_frames >> 1, 1);
        if (entity->params->flags & ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE) {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = (ENTITY_PARAMS_MOVEMENT_KIND(entity->params) == ENTITY_PARAMS_MOVEMENT_1) ? 2 : 1;
        }
        ENTITY_FLAG_BITS(entity)->melee_0_hit = 0;
        ENTITY_FLAG_BITS(entity)->melee_1_hit = 0;
        ENTITY_FLAG_BITS(entity)->melee_2_hit = 0;
        ENTITY_FLAG_BITS(entity)->look_at_target = 0;
        entity->action_step++;
    case 1:
        if (ai_state->motion_slot_0.frames_left == 0) {
            entity->action_step++;
        }
        if (unk240->hit_start_frame != 0) {
            EntityAi_TurnToHeading(entity, (f32 *)&ai_state->turn_rate, entity->heading_to_player);
        }
        break;
    case 2:
        if (ai_state->motion_slot_0.frame <= unk240->hit_start_frame) {
            EntityAi_TurnToHeading(entity, (f32 *)&ai_state->turn_rate, entity->heading_to_player);
        } else {
            ai_state->turn_rate = 0;
        }
        elapsed = (ai_state->motion_slot_0.frame_fixed - entity->params->motion_speeds[entity->animation_index]) >> 8;
        if (!(ENTITY_FLAG_BITS(entity)->melee_0_hit)) {
            ENTITY_FLAG_BITS(entity)->melee_0_hit = EntityAi_CheckMeleeHit(entity, unk240, 0, elapsed, ai_state->motion_slot_0.frame);
        }
        if (!(ENTITY_FLAG_BITS(entity)->melee_1_hit)) {
            ENTITY_FLAG_BITS(entity)->melee_1_hit = EntityAi_CheckMeleeHit(entity, unk240, 1, elapsed, ai_state->motion_slot_0.frame);
        }
        if (!(ENTITY_FLAG_BITS(entity)->melee_2_hit)) {
            ENTITY_FLAG_BITS(entity)->melee_2_hit = EntityAi_CheckMeleeHit(entity, unk240, 2, elapsed, ai_state->motion_slot_0.frame);
        }
        if (ai_state->motion_slot_0.frame > 0 && unk240->hit_end_frame >= ai_state->motion_slot_0.frame) {
            entity->flags |= ENTITY_FLAG_ATTACK_WINDOW;
        } else {
            entity->flags &= ~ENTITY_FLAG_ATTACK_WINDOW;
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

/* The ranged attack (busy bit 1): as EntityAi_RangedAttack, on the motion slots, with launches of
   its own. Record 1: on the frame of the record's first shot and on frames EntityParams.type_param_0 and
   type_param_1, DarkBird_LaunchThreeShots launches, pitched at the player and up to 14 degrees to
   either side of the reverse of the heading. The other records: on the frame of the first shot
   DarkBird_LaunchBothShots launches at a point 200 below the player. The pause Entity.attack_delay_frames
   is the record's unk02 as it is. */
static void DarkBird_RangedAttack(Entity *entity) {
    EntityAiStateKind1719F8 *ai_state = (EntityAiStateKind1719F8 *)&entity->ai_state;
    s32 kind = (s32)(ENTITY_FLAG_WORD(entity->flags) >> 58) & 3;
    EntityParamsUnk3D8Full *record = (EntityParamsUnk3D8Full *)ENTITY_PARAMS_UNK3D8(entity->params, kind);
    f32 target[4];
    f32 angles[4];
    f32 *point;
    s32 frame;
    s32 elapsed;
    s32 launch_frame;
    s32 i;

    entity->busy_flags |= ENTITY_BUSY_ATTACK;
    switch (entity->action_step) {
    case 0:
        EntityAi_StartSlotMotion(entity, &ai_state->motion_slot_0, 0, kind + 0x17, 0, entity->params->motion_blend_frames >> 1, 1);
        EntityAi_StartSlotMotion(entity, &ai_state->motion_slot_1, 1, ((s32)(entity->flags >> 58) & 3) + 0x17, 0, entity->params->motion_blend_frames >> 1, 1);
        if (entity->params->flags & ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE) {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = (ENTITY_PARAMS_MOVEMENT_KIND(entity->params) == ENTITY_PARAMS_MOVEMENT_1) ? 2 : 1;
        }
        ENTITY_FLAG_BITS(entity)->look_at_target = 0;
        entity->action_step++;
    case 1:
        if (ai_state->motion_slot_0.frames_left == 0) {
            entity->action_step++;
        }
        if (record->turn_frames != 0) {
            EntityAi_TurnToHeading(entity, &ai_state->turn_rate, entity->heading_to_player);
        }
        break;
    case 2:
        if (ai_state->motion_slot_0.frame <= record->turn_frames) {
            EntityAi_TurnToHeading(entity, &ai_state->turn_rate, entity->heading_to_player);
        } else {
            ai_state->turn_rate = 0.0f;
        }
        frame = ai_state->motion_slot_0.frame;
        elapsed = (ai_state->motion_slot_0.frame_fixed - entity->params->motion_speeds[entity->animation_index]) >> 8;
        if (((s32)(entity->flags >> 58) & 3) == 1) {
            for (i = 0; i < 3; i++) {
                switch (i) {
                case 0:
                    launch_frame = record->launches[0].frame;
                    break;
                case 1:
                    launch_frame = entity->params->type_param_0;
                    break;
                case 2:
                    launch_frame = entity->params->type_param_1;
                    break;
                }
                if (launch_frame != 0 && elapsed < launch_frame && frame >= launch_frame) {
                    point = (f32 *)Model_GetSkinnedBonePoint((char *)entity->model, 0);
                    angles[0] = EntityMath_WrapAngle(-atan2(player_state.position[1] - point[1], EntityMath_DistanceXZ(point, &player_state)));
                    if (!(rand() & 1)) {
                        angles[1] = EntityMath_WrapAngle(entity->model->rotation[1] + 3.1415927f + (f32)(rand() % 15) * 0.017453292f);
                    } else {
                        angles[1] = EntityMath_WrapAngle(entity->model->rotation[1] + 3.1415927f - (f32)(rand() % 15) * 0.017453292f);
                    }
                    angles[2] = 0.0f;
                    DarkBird_LaunchThreeShots(entity, record, elapsed, ai_state->motion_slot_0.frame, angles);
                }
            }
        } else {
            Vector_Copy(target, &player_state);
            target[1] -= 200.0f;
            if (record->launches[0].frame != 0 && elapsed < record->launches[0].frame && frame >= record->launches[0].frame) {
                DarkBird_LaunchBothShots(entity, (EntityParamsUnk3D8 *)record, elapsed, ai_state->motion_slot_0.frame, target, 0.7853982f, 0.0f);
            }
        }
        if (ai_state->motion_slot_0.frame > 0 && record->attack_end_frame >= ai_state->motion_slot_0.frame) {
            entity->flags |= ENTITY_FLAG_ATTACK_WINDOW;
        } else {
            entity->flags &= ~ENTITY_FLAG_ATTACK_WINDOW;
        }
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            entity->attack_delay_frames = record->attack_delay;
            if (ENTITY_FLAG_BITS(entity)->own_think) {
                ai_state->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, ai_state->move, 1);
            }
        }
        break;
    }
}

/* Helper of DarkBird_RangedAttack: launches both shots of a record at once, whatever the
   animation frame, each from its point and turned towards `target` by at most the two rates. */
static s32 DarkBird_LaunchBothShots(Entity *entity, EntityParamsUnk3D8 *unk3D8, s32 frame_from, s32 frame_to, f32 *target, f32 pitch_rate, f32 yaw_rate) {
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
                angles[1] = EntityMath_WrapAngle(-atan2f(delta_z, delta_x) + 1.5707964f);
                angles[2] = angles[3] = 0.0f;
            } else {
                Vector_Copy(angles, entity->model->rotation);
                angles[1] = EntityMath_WrapAngle(angles[1] + 3.1415927f);
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

/* Helper of DarkBird_RangedAttack: launches the three shots of a record at once, whatever the
   animation frame, each from its point, all at the given angles. */
static s32 DarkBird_LaunchThreeShots(Entity *entity, EntityParamsUnk3D8Full *record, s32 frame_from, s32 frame_to, f32 *angles) {
    EntityUnk1B1390 *shot;
    char *point;
    s32 i;

    if (entity->ailment_4_frames != 0 && record->ignores_ailment_4 == 0) {
        return 0;
    }
    for (i = 0; i < 3; i++) {
        shot = Shot_Alloc();
        shot->is_magic_shot = 1;
        shot->player_power = 0;
        shot->player_magic = 0;
        shot->shot_params = entity->params->shot_params;
        point = Model_GetSkinnedBonePoint((char *)entity->model, record->launches[i].bone);
        if (point != NULL) {
            Attack_LaunchMagicEffect(record->launches[i].magic_effect, shot, point, angles, entity);
        }
    }
    return 0;
}

/* The on_hit handler: as EntityAi_OnHit with the type's own two reactions; a light hit starts
   DarkBird_ReactionLight whether flag bit 19 is set or not. */
static void DarkBird_OnHit(Entity *entity) {
    EntityAi_LowerWeightsAfterHit((char *)entity);
    entity->target = NULL;
    entity->flags &= ~ENTITY_FLAG_ATTACKING;
    entity->flags &= ~ENTITY_FLAG_LOOK_AT_TARGET;
    if (((s32)(entity->flags >> 36) & 3) == 1) {
        EntityAi_SetReaction(entity, DarkBird_ReactionLight, 0);
    } else if (((s32)(entity->flags >> 36) & 3) == 2) {
        EntityAi_SetReaction(entity, DarkBird_ReactionHeavy, 0);
    }
    EntityAi_SetState((char *)entity, 2);
}

/* The reaction to a light hit (busy bit 2): as EntityAi_ReactionLight, on the motion slots
   (animation 14 on the first, 3 on the second). */
static void DarkBird_ReactionLight(Entity *entity) {
    EntityAiStateKind1740A8 *ai_state = (EntityAiStateKind1740A8 *)&entity->ai_state;

    entity->busy_flags = ENTITY_BUSY_REACTION_LIGHT;
    switch (entity->reaction_step) {
    case 0:
        EntityEvent_StartFrameCues(entity, 0xE, 0, 0);
        EntityAi_StartSlotMotion(entity, &ai_state->motion_slot_0, 0, 0xE, 0, 5, 1);
        EntityAi_StartSlotMotion(entity, &ai_state->motion_slot_1, 1, 3, 0, 5, 0);
        if (entity->params->flags & ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE) {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = (ENTITY_PARAMS_MOVEMENT_KIND(entity->params) == ENTITY_PARAMS_MOVEMENT_1) ? 2 : 1;
        }
        ai_state->turn_rate = 0;
        ENTITY_FLAG_BITS(entity)->animates = 1;
        ENTITY_FLAG_BITS(entity)->attack_window = 0;
        ENTITY_FLAG_BITS(entity)->guarding = 0;
        ENTITY_FLAG_BITS(entity)->attacking = 0;
        ENTITY_FLAG_BITS(entity)->look_at_target = 0;
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
            ai_state->think(entity, 1);
        } else {
            EntityAi_StartAction(entity, ai_state->move, 1);
        }
        break;
    }
}

/* The reaction to a heavy hit (busy bit 4): as EntityAi_ReactionHeavy, on the motion slots. */
static void DarkBird_ReactionHeavy(Entity *entity) {
    EntityAiStateKind1740A8 *ai_state = (EntityAiStateKind1740A8 *)&entity->ai_state;

    entity->busy_flags = ENTITY_BUSY_REACTION_HEAVY;
    switch (entity->reaction_step) {
    case 0:
        EntityAi_StartSlotMotion(entity, &ai_state->motion_slot_0, 0, 0xF, 0, 5, 1);
        EntityAi_StartSlotMotion(entity, &ai_state->motion_slot_1, 1, 0xF, 0, 5, 1);
        if (entity->params->flags & ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE) {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = 0;
        } else {
            s32 kind = 2;

            if (!(ENTITY_PARAMS_BITS(entity)->bit25 && !ENTITY_PARAMS_BITS(entity)->bit26)) {
                kind = 1;
            }
            ENTITY_FLAG_BITS(entity)->motion_step_mode = kind;
        }
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
                ai_state->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, ai_state->move, 1);
            }
        }
        break;
    }
}

/* The death action (busy bit 0x40): as EntityAi_Death, on the motion slots. */
static void DarkBird_Death(Entity *entity) {
    EntityAiStateKind1740A8 *ai_state = (EntityAiStateKind1740A8 *)&entity->ai_state;

    entity->busy_flags = ENTITY_BUSY_DEATH;
    switch (entity->action_step) {
    case 0:
        EntityAi_StartSlotMotion(entity, &ai_state->motion_slot_0, 0, 0x10, 0, 5, 1);
        EntityAi_StartSlotMotion(entity, &ai_state->motion_slot_1, 1, 0x10, 0, 5, 1);
        if (entity->params->flags & ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE) {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = 0;
        } else {
            s32 kind = 2;

            if (!(ENTITY_PARAMS_BITS(entity)->bit25 && !ENTITY_PARAMS_BITS(entity)->bit26)) {
                kind = 1;
            }
            ENTITY_FLAG_BITS(entity)->motion_step_mode = kind;
        }
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

/* The animate handler: plays the two motion slots of the work area (unk80 and unk88) where
   EntityAi_Animate plays the model's one motion, then turns the head (EntityAi_LookAtPoint with the target
   while flag bit 9 is set and there is one). */
static void DarkBird_Animate(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    EntityAiStateKind151D30 *slots = (EntityAiStateKind151D30 *)ai_state;
    f32 limits[4];
    f32 unused_vector[4]; // match: retail reserves 16 more bytes of stack than it uses
    s32 previous;
    f32 rate;

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
    limits[1] = entity->params->pitch_limit_deg * 0.017453292f;
    limits[0] = -entity->params->pitch_limit_deg * 0.017453292f;
    limits[3] = entity->params->yaw_limit_deg * 0.017453292f;
    limits[2] = -entity->params->yaw_limit_deg * 0.017453292f;
    if (ENTITY_FLAG_BITS(entity)->look_at_target && entity->target != NULL) {
        EntityAi_LookAtPoint(entity, (int)ai_state->look_part_angles, ENTITY_FLAG_BITS(entity)->look_at_target, (int)entity->target, (int)limits, 12.0f);
    } else {
        EntityAi_LookAtPlayer(entity, ai_state->look_part_angles, ENTITY_FLAG_BITS(entity)->look_at_target, NULL, 8.0f);
    }
}
