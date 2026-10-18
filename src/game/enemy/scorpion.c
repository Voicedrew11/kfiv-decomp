/* Scorpion: the behaviour of entity type 16 (EntityParams.entity_type, the index into entity_type_handlers): its set-up
   function, which the table names, and the handlers it installs. Used by: Scorpion. The game names no
   monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by
   hit points and experience, with resistances and drops where two monsters share those. */
#include "entity/entity.h"
#include "common.h"

void EntityAi_SetUpdate(char *, s32);
int EntityAi_StartAction(Entity *, void (*)(Entity *), int);
void Motion_ClearPartSlots(EntityModel *);
int Motion_StepModel(EntityModel *, s32, s32);
void EntityAi_OnHit(char *);
void EntityAi_Dormant(Entity *);
void EntityAi_Death(Entity *);
extern s32 entity_type_handlers[][2];
int Motion_Step(void *, int);
int Motion_BlendToPose(EntityModel *, f32);
extern u32 frame_counter;
int EntityAi_ThinkReturn(void *, int);
void EntityAi_StepState(void *);
void EntityAi_SensePlayer(void *, int);
void EntityAi_CheckFarFromSpawn(void *);
int rand(void);
int EntityAi_TurnFromPush(void *, void *);
void EntityPoint_PickNear(float, void *, void *, void *);
s16 EntityAi_MoveCommand(float, void *, int); // match: this file only matches with this type; EntityAi_MoveCommand.c defines it differently
extern PlayerData player_state;
s32 EntityAi_ShouldGuard(Entity *, f32, f32);
s32 EntityAi_ShouldDodge(Entity *, f32, f32);
s32 EntityAi_PickCombatChoice(Entity *, s32);
void EntityAi_LowerChoiceWeight(Entity *, s32);
int EntityAi_PickAttack(char *);
float EntityMath_WrapAngle(float);
float EntityMath_AngleXZ(float *, char *);
float EntityMath_AngleDifference(float, float);
int EntityAi_TurnCommand(Entity *);
void EntityAi_Dodge(Entity *);
void EntityAi_Guard(Entity *);
void EntityAi_MeleeAttack(Entity *);
void EntityAi_RangedAttack(Entity *);
void EntityAi_ClearTurnInPlace(Entity *);
extern f32 identity_matrix[16];
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0RotMatrixZ(float *, float *, float);
void Vector_ApplyMatrix(void *, void *, void *);
void Vector_Normalize(void *, void *);
void Vector_Scale(float *, float *, float);
float EntityAi_Accelerate(Entity *, float);
int Motion_Start(void *, int, int, u32, int);

static void Scorpion_Animate(Entity *entity);
static void Scorpion_Think(Entity *entity, s32 unknown);
static int Scorpion_ThinkWander(Entity *entity, s32 unknown);
static int Scorpion_ThinkCombat(Entity *entity, s32 unknown);
static void Scorpion_Move(Entity *entity);

/* Set-up of a type 16 entity: clears its work area and installs the type's handlers. */
void Scorpion_Init(Entity *entity) {
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
    ai_state->think = (void (*)(Entity *, s32))Scorpion_Think;
    ai_state->animate = (void (*)(void))Scorpion_Animate;
    ai_state->on_hit = (void (*)(void))EntityAi_OnHit;
    ai_state->move = (void (*)(void))Scorpion_Move;
    ai_state->dormant = (void (*)(void))EntityAi_Dormant;
    ai_state->death = (void (*)(void))EntityAi_Death;
    for (i = 4 - 1; i != -1; i--) {
        entity->choice_weights[i] = 0x80;
    }
    for (i = 6 - 1; i != -1; i--) {
        entity->attack_weights[i] = 0x80;
    }
    ENTITY_UNK5C(entity)->as_s32 = 0;
    ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
    if (entity->spawn_info->spawn_flags & (0x8000L << 43)) {
        EntityAi_StartAction(entity, (void (*)(Entity *))ai_state->dormant, 1);
    } else {
        EntityAi_StartAction(entity, (void (*)(Entity *))ai_state->move, 0);
    }
    ai_state->pose_blend_weight = 0.5f;
    entity->model_alpha = (entity->params->flags & ENTITY_PARAMS_FLAG_HALF_MODEL_ALPHA) ? 0x40 : 0x80;
    entity->model->alpha = entity->model_alpha;
    Motion_ClearPartSlots(entity->model);
    Motion_StepModel(entity->model, 0, 0);
}

/* The animate handler: as EntityAi_Animate without the test of the EntityParams kind bits and
   without the head turn. */
static void Scorpion_Animate(Entity *entity) {
    u32 blend_frames; // match: converted to float as unsigned
    s64 flags;
    EntityAiState *ai_state;

    flags = entity->flags;
    ai_state = &entity->ai_state;
    if (ENTITY_FLAG_BITS(entity)->bit14) {
        blend_frames = entity->model->level_tables->blend_left;
        if (blend_frames != 0) {
            ENTITY_FLAG_BITS(entity)->bit34 = Motion_BlendToPose(entity->model, (entity->busy_flags & (ENTITY_BUSY_GUARD | ENTITY_BUSY_REACTION_LIGHT)) ? ai_state->pose_blend_weight : 1.0f / (f32)blend_frames);
        } else {
            ENTITY_FLAG_BITS(entity)->bit34 = Motion_Step(entity, (s32)(flags >> 11) & 7);
        }
    }
}

/* The think handler: as EntityAi_Think, with Scorpion_ThinkCombat for state 2 and Scorpion_ThinkWander
   for the other states. */
static void Scorpion_Think(Entity *entity, s32 unknown) {
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
    if (ENTITY_FLAG_BITS(entity)->bit53) {
        EntityAi_ThinkReturn(entity, unknown);
        return;
    }
    switch (entity->state) {
    case 0:
    case 1:
        break;
    case 2:
        Scorpion_ThinkCombat(entity, unknown);
        return;
    }
    Scorpion_ThinkWander(entity, unknown);
}

/* The think function of every state but 2 (Scorpion_Think calls it): wanders. Each time the
   timer runs out it picks a point near the spawn point (EntityPoint_PickNear; 21 times in 100, for 60 to
   89 frames) or no target (120 to 179 frames), takes the command of EntityAi_MoveCommand towards it (or
   the turn away from a push, EntityAi_TurnFromPush) and starts the move action. */
static int Scorpion_ThinkWander(Entity *entity, s32 unknown) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 direction[4];
    s32 turn;

    if (entity->params->flags & ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE) {
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
    } else {
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
    }
    if (entity->action_timer == 0) {
        if (rand() % 100 <= 20) {
            EntityPoint_PickNear(entity->spawn_info->wander_radius, ai_state->target_point, entity->spawn_info, entity->model->position);
            entity->target = ai_state->target_point;
            entity->action_timer = rand() % 30 + 60;
        } else {
            entity->target = NULL;
            entity->action_timer = rand() % 60 + 120;
        }
    }
    turn = EntityAi_TurnFromPush(entity, direction);
    if (turn > 0) {
        entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_RIGHT);
    } else if (turn < 0) {
        entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_LEFT);
    } else {
        entity->move_command = EntityAi_MoveCommand(50.0f, entity, 0);
    }
    EntityAi_StartAction(entity, (void (*)(Entity *))ai_state->move, unknown);
}

/* The think function of state 2: guard, dodge and the choice among attacking, closing in and
   waiting as in EntityAi_ThinkCombat; the head does not follow. Closing in has three ways, kept in
   work byte unk78: 0 turns on the spot until Entity.heading_to_player is within 10 degrees of the heading (flag
   bits 11 to 13 at 3); 1 goes for the player; 2 moves sideways (command bit 4 or 8, by the side of
   the player's facing the entity is on, work byte unk7A) until flag bit 45, 47 or 48 is set, which
   puts it on way 1. */
static int Scorpion_ThinkCombat(Entity *entity, s32 unknown) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 heading;
    f32 angle;
    s32 side;
    s32 turn;
    s32 mode;

    if (entity->params->flags & ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE) {
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
    } else {
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
    }
    if (EntityAi_ShouldGuard(entity, 300.0f, 0.5235988f)) {
        EntityAi_StartAction(entity, EntityAi_Guard, 1);
        return;
    }
    if (EntityAi_ShouldDodge(entity, 300.0f, 0.5235988f)) {
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
            angle = EntityMath_AngleDifference(entity->model->rotation[1], entity->heading_to_player);
            if (ABS(angle) > 0.17453292f) {
                entity->target = &player_state;
                entity->action_timer = rand() % 60 + 60;
                ai_state->move_choice = 0;
            } else {
                heading = EntityMath_AngleXZ(player_state.position, (char *)entity->model->position) - player_state.heading;
                heading = EntityMath_WrapAngle(heading);
                if (ABS(heading) > 0.7853982f || rand() % 100 < entity->params->pitch_limit_deg) {
                    entity->target = &player_state;
                    entity->action_timer = rand() % 60 + 120;
                    ai_state->move_choice = 1;
                } else {
                    if (heading >= 0.0f) {
                        ai_state->step_index = 0;
                    } else {
                        ai_state->step_index = 1;
                    }
                    entity->action_timer = rand() % 30 + 60;
                    ai_state->move_choice = 2;
                }
            }
            break;
        case 2:
            entity->action_timer = 30;
            ai_state->move_choice = -1;
            break;
        default:
            ai_state->move_choice = -1;
            break;
        }
    }
    mode = entity->combat_choice;
    if (mode == 1) {
        EntityAi_ClearTurnInPlace(entity);
        if (ENTITY_FLAG_BITS(entity)->bit31) {
            angle = EntityMath_AngleDifference(entity->model->rotation[1], entity->heading_to_player);
            if (ai_state->turn_in_place == 0) {
                ai_state->turn_in_place = ABS(angle) > 0.5235988f;
                if (ai_state->turn_in_place == 0) {
                    goto done;
                }
            }
            entity->move_command = EntityAi_TurnCommand(entity);
            if (entity->move_command == 0) {
                ai_state->turn_in_place = 0;
            }
        } else {
            turn = EntityAi_TurnFromPush(entity, &heading);
            switch (ai_state->move_choice) {
            case 0:
                angle = EntityMath_AngleDifference(entity->model->rotation[1], entity->heading_to_player);
                if (ABS(angle) > entity->params->turn_limit) {
                    if (angle >= 0.0f) {
                        entity->move_command |= MOVE_COMMAND_TURN_RIGHT;
                    } else {
                        entity->move_command |= MOVE_COMMAND_TURN_LEFT;
                    }
                }
                if (ABS(angle) <= 0.17453292f) {
                    entity->action_timer = 0;
                }
                ENTITY_FLAG_BITS(entity)->bits11 = 3;
                break;
            case 1:
                if (turn > 0) {
                    entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_RIGHT);
                } else if (turn < 0) {
                    entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_LEFT);
                } else {
                    entity->move_command = EntityAi_MoveCommand(entity->radius + 100.0f + 30.0f, entity, 0);
                }
                break;
            case 2:
                angle = EntityMath_AngleDifference(entity->model->rotation[1], entity->heading_to_player);
                if (ABS(angle) > entity->params->turn_limit) {
                    if (angle >= 0.0f) {
                        entity->move_command |= MOVE_COMMAND_TURN_RIGHT;
                    } else {
                        entity->move_command |= MOVE_COMMAND_TURN_LEFT;
                    }
                }
                if (entity->flags & (ENTITY_FLAG_WALL_PUSH | ENTITY_FLAG_OBJECT_PUSH | ENTITY_FLAG_ENTITY_PUSH)) {
                    ai_state->move_choice = 1;
                } else if (ai_state->step_index != 0) {
                    entity->move_command |= MOVE_COMMAND_STEP_RIGHT;
                } else {
                    entity->move_command |= MOVE_COMMAND_STEP_LEFT;
                }
                break;
            }
        }
    }
done:
    EntityAi_StartAction(entity, (void (*)(Entity *))ai_state->move, unknown);
}

/* The move handler: as EntityAi_Move, but it turns the heading only while the command has more than
   the two turn bits, and a turn alone gets animation 11 or 12. */
static void Scorpion_Move(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 *vector;
    f32 turn;
    f32 amount;
    s32 command;
    s32 first_command;

    if (entity == player_state.stood_on_entity && (entity->params->flags & ENTITY_PARAMS_FLAG_STOPS_WHEN_STOOD_ON)) {
        entity->move_command = 0;
    }
    // match: read through the union, and again below once the turn speed is stored
    first_command = ENTITY_COMMAND(entity);
    if (first_command & ~MOVE_COMMAND_TURNS) {
        if (first_command & MOVE_COMMAND_TURN_LEFT) {
            ai_state->turn_rate -= entity->params->turn_step_per_frame;
            if (ai_state->turn_rate < -entity->params->turn_limit) {
                ai_state->turn_rate = -entity->params->turn_limit;
            }
        } else if (first_command & MOVE_COMMAND_TURN_RIGHT) {
            ai_state->turn_rate += entity->params->turn_step_per_frame;
            if (entity->params->turn_limit < ai_state->turn_rate) {
                ai_state->turn_rate = entity->params->turn_limit;
            }
        } else {
            ai_state->turn_rate = 0.0f;
        }
        command = entity->move_command;
        turn = ai_state->turn_rate;
        if (command & MOVE_COMMAND_TURN_FAST) {
            turn += turn;
        }
        entity->turn_delta_yaw += turn;
    } else {
        command = first_command;
    }
    if (ENTITY_FLAG_BITS(entity)->bits11 == 0) {
        vector = ENTITY_SCRATCH_VECTOR;
        vector[0] = vector[1] = vector[2] = vector[3] = 0.0f;
        if (command & MOVE_COMMAND_FORWARD_BACK) {
            amount = 0.0f;
            if (command & MOVE_COMMAND_FORWARD) {
                amount = -1.0f;
            } else if (command & MOVE_COMMAND_BACK) {
                amount = 1.0f;
            }
            vector[2] += amount;
        }
        if (command & MOVE_COMMAND_SIDEWAYS) {
            amount = 0.0f;
            if (command & MOVE_COMMAND_STEP_LEFT) {
                amount = 1.0f;
            } else if (command & MOVE_COMMAND_STEP_RIGHT) {
                amount = -1.0f;
            }
            vector[0] += amount;
        }
        if (command & MOVE_COMMAND_UP_DOWN) {
            amount = 0.0f;
            if (command & MOVE_COMMAND_DOWN) {
                amount = -1.0f;
            } else if (command & MOVE_COMMAND_UP) {
                amount = 1.0f;
            }
            vector[1] += amount;
        }
        if (command & MOVE_COMMAND_DIRECTIONS) {
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
            command = entity->move_command;
            entity->move_delta_x += ENTITY_SCRATCH_VECTOR[0];
            entity->move_delta_y += ENTITY_SCRATCH_VECTOR[1];
            entity->move_delta_z += ENTITY_SCRATCH_VECTOR[2];
        } else {
            ai_state->move_speed = 0.0f;
        }
    } else {
        ai_state->move_speed = 0.0f;
    }
    if (command & MOVE_COMMAND_FORWARD) {
        if (command & MOVE_COMMAND_RUN) {
            Motion_Start(entity, 4, 0, entity->params->motion_blend_frames >> 1, 0);
        } else {
            Motion_Start(entity, 3, 0, entity->params->motion_blend_frames, 0);
        }
    } else if (command & MOVE_COMMAND_BACK) {
        Motion_Start(entity, 5, 0, entity->params->motion_blend_frames, 0);
    } else if (command & MOVE_COMMAND_DOWN) {
        Motion_Start(entity, 8, 0, entity->params->motion_blend_frames, 0);
    } else if (command & MOVE_COMMAND_UP) {
        Motion_Start(entity, 9, 0, entity->params->motion_blend_frames, 0);
    } else if (command & MOVE_COMMAND_STEP_LEFT) {
        Motion_Start(entity, 6, 0, entity->params->motion_blend_frames, 0);
    } else if (command & MOVE_COMMAND_STEP_RIGHT) {
        Motion_Start(entity, 7, 0, entity->params->motion_blend_frames, 0);
    } else if (command & MOVE_COMMAND_TURN_LEFT) {
        Motion_Start(entity, 0xB, 0, entity->params->motion_blend_frames, 0);
    } else if (command & MOVE_COMMAND_TURN_RIGHT) {
        Motion_Start(entity, 0xC, 0, entity->params->motion_blend_frames, 0);
    } else {
        Motion_Start(entity, 0, 0, entity->params->motion_blend_frames, 0);
    }
}
