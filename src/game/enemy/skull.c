/* Skull: the behaviour of entity type 65 (Entity.params->unk02, the index into entity_type_handlers): its set-up
   function, which the table names, and the handlers it installs. Used by: Skull. The game names no monsters;
   these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by hit points
   and experience, with resistances and drops where two monsters share those. */
#include "entity/entity.h"
#include "common.h"
#include "effect/shot.h"

void EntityAi_SetUpdate(char *, s32);
int EntityAi_StartAction(Entity *, void (*)(Entity *), int);
void Motion_ClearPartSlots(EntityModel *);
int Motion_StepModel(EntityModel *, s32, s32);
void Vector_Copy(void *, void *);
int rand(void);
void EntityAi_OnHit(char *);
void EntityAi_Dormant(Entity *);
extern s32 entity_type_handlers[][2];
extern u32 frame_counter;
void EntityAi_SensePlayer(void *, int);
void EntityAi_CheckFarFromSpawn(char *);
void EntityAi_StepState(void *);
int EntityAi_ThinkReturn(void *, int);
int EntityAi_ThinkIdle(void *, int);
void EntityAi_ThinkPursue(Entity *, s32);
extern PlayerData player_state;
s32 EntityAi_ShouldGuard(Entity *, f32, f32);
s32 EntityAi_ShouldDodge(Entity *, f32, f32);
s32 EntityAi_PickCombatChoice(Entity *, s32);
void EntityAi_LowerChoiceWeight(Entity *, s32);
int EntityAi_PickAttack(char *);
void EntityAi_ClearTurnInPlace(Entity *);
int EntityAi_TurnCommand(Entity *);
int EntityAi_TurnFromPush(void *, void *);
s32 EntityAi_MoveCommand(Entity *, s32, f32);
float EntityMath_AngleDifference(float, float);
void EntityAi_Dodge(Entity *);
void EntityAi_Guard(Entity *);
void EntityAi_MeleeAttack(Entity *);
int Motion_Start(void *, int, int, u32, int);
void EntityEvent_OnDeath(Entity *);
extern f32 identity_matrix[16];
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0RotMatrixZ(float *, float *, float);
void Vector_ApplyMatrix(void *, void *, void *);
void Vector_Scale(float *, float *, float);
float EntityMath_WrapAngle(float);
int Motion_Step(void *, int);
// match: the callee is void, but retail keeps what it leaves in $2
int Motion_BlendToPose(EntityModel *, f32);
char *Model_GetSkinnedBonePoint(char *, u32);
int Attack_LaunchMagicEffect(u16, void *, void *, void *, void *);
void EntityEvent_StartEffectAtPoint(char **, int, u32, int);
s32 EntityAi_TurnToHeading(Entity *, f32 *, f32);
int EntityAi_LaunchShots(Entity *, EntityParamsUnk3D8 *, s32, s32);
void EntityAi_SetAttackDelay(Entity *, s32);

static void Skull_Think(Entity *entity, s32 unknown);
static void Skull_ThinkCombat(Entity *entity, s32 unknown);
static void Skull_Move(Entity *entity);
static void Skull_Death(Entity *entity);
static void Skull_ApplyMoveCommand(Entity *entity);
static void Skull_SwingPart3(Entity *entity);
static void Skull_Animate(Entity *entity);
static void Skull_RangedAttack0(Entity *entity);

/* Set-up of a type 65 entity: clears its work area and installs the type's handlers. */
void Skull_Init(Entity *entity) {
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
    EntityAi_SetUpdate((char *)entity, entity_type_handlers[entity->params->entity_type][1]);
    ai_state->think = Skull_Think;
    ai_state->animate = (void (*)(void))Skull_Animate;
    ai_state->on_hit = (void (*)(void))EntityAi_OnHit;
    ai_state->move = (void (*)(void))Skull_Move;
    ai_state->dormant = (void (*)(void))EntityAi_Dormant;
    ai_state->death = (void (*)(void))Skull_Death;
    for (i = 4 - 1; i != -1; i--) {
        entity->choice_weights[i] = 0x80;
    }
    for (i = 6 - 1; i != -1; i--) {
        entity->attack_weights[i] = 0x80;
    }
    ENTITY_UNK5C(entity)->as_s32 = 0;
    if (entity->spawn_info->spawn_flags & (0x8000L << 43)) {
        EntityAi_StartAction(entity, (void (*)(Entity *))ai_state->dormant, 1);
    } else {
        EntityAi_StartAction(entity, (void (*)(Entity *))ai_state->move, 0);
    }
    if (unk138 != NULL) {
        Vector_Copy(ai_state->look_part_angles, unk138 + 0x50);
    }
    ai_state->pose_blend_weight = 0.5f;
    entity->model_alpha = (entity->params->flags & ENTITY_PARAMS_FLAG_HALF_MODEL_ALPHA) ? 0x40 : 0x80;
    entity->model->alpha = entity->model_alpha;
    ai_state->sub_timer = rand() % 30;
    Motion_ClearPartSlots(entity->model);
    Motion_StepModel(entity->model, 0, 0);
}

/* The think handler: as EntityAi_Think, with Skull_ThinkCombat for state 2. It keeps flag
   bits 22 and 28 set except in the death action. While bit 28 is set and no attack runs it adds
   command bit 0x20 or 0x10 (work byte unk7A), chosen anew every 20 to 34 frames (work word unk6A)
   so as to keep its y between 300 and 100 below the player's; then Skull_ApplyMoveCommand moves
   the entity. When work word unk68 has run below 0 it draws new values for
   Skull_SwingPart3 (work bytes unk7B and unk7C). */
static void Skull_Think(Entity *entity, s32 unknown) {
    EntityAiState *ai_state = &entity->ai_state;

    if (frame_counter % 10U == entity->spawn_index % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (entity->busy_flags & ENTITY_BUSY_DEATH) {
        ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit22 = FALSE;
    } else {
        ENTITY_FLAG_BITS(entity)->bit28 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit22 = TRUE;
    }
    if (entity->busy_flags == 0) {
        EntityAi_CheckFarFromSpawn((char *)entity);
        EntityAi_StepState(entity);
        if (ENTITY_FLAG_BITS(entity)->bit53) {
            EntityAi_ThinkReturn(entity, unknown);
        } else {
            switch (entity->state) {
            case 0:
                EntityAi_ThinkIdle(entity, unknown);
                break;
            case 1:
                EntityAi_ThinkPursue(entity, unknown);
                break;
            case 2:
                Skull_ThinkCombat(entity, unknown);
                break;
            default:
                EntityAi_ThinkIdle(entity, unknown);
                break;
            }
        }
    }
    if (ai_state->sub_timer > 0) {
        ai_state->sub_timer--;
    }
    if (ENTITY_FLAG_BITS(entity)->bit28 && !(entity->busy_flags & ENTITY_BUSY_ATTACK)) {
        if (ai_state->sub_timer <= 0) {
            if (entity->model->position[1] < player_state.position[1] - 300.0f) {
                ai_state->step_index = 1;
            } else if (player_state.position[1] - 100.0f < entity->model->position[1]) {
                ai_state->step_index = 0;
            } else {
                ai_state->step_index = ai_state->step_index == 0;
            }
            ai_state->sub_timer = rand() % 15 + 20;
        }
        if (ai_state->step_index != 0) {
            entity->move_command |= MOVE_COMMAND_UP;
        } else {
            entity->move_command |= MOVE_COMMAND_DOWN;
        }
    }
    Skull_ApplyMoveCommand(entity);
    if (ai_state->pause_timer < 0) {
        ai_state->action_counter = (rand() % 10 + 10) * 2;
        if (rand() % 10 == 1) {
            ai_state->action_flags = 0;
        } else {
            ai_state->action_flags = rand() % entity->params->far_distance;
        }
        ai_state->pause_timer = ai_state->action_counter * 2;
    }
}

/* The think function of state 2: as EntityAi_ThinkCombat with Skull_RangedAttack0 as the
   ranged attack. It attacks only while its y is between 300 and 50 below the player's; otherwise it
   sets the choice of command bit 0x20 or 0x10 (work byte unk7A) for 15 to 29 frames. One time in
   three it closes in with command bit 0x40. */
static void Skull_ThinkCombat(Entity *entity, s32 unknown) {
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
    if (EntityAi_ShouldGuard(entity, 300.0f, 0.5235988f)) {
        EntityAi_StartAction(entity, EntityAi_Guard, 1);
        return;
    }
    if (EntityAi_ShouldDodge(entity, 300.0f, 0.5235988f)) {
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
        case 0:
            if (entity->model->position[1] < player_state.position[1] - 50.0f && player_state.position[1] - 300.0f < entity->model->position[1]) {
                if (EntityAi_PickAttack((char *)entity)) {
                    EntityAi_StartAction(entity, Skull_RangedAttack0, unknown);
                } else {
                    EntityAi_StartAction(entity, EntityAi_MeleeAttack, unknown);
                }
                return;
            }
            if (entity->model->position[1] < player_state.position[1]) {
                ai_state->step_index = 1;
            } else {
                ai_state->step_index = 0;
            }
            ai_state->sub_timer = rand() % 15 + 15;
            break;
        case 1:
            entity->target = &player_state;
            entity->action_timer = rand() % 60 + 120;
            if (rand() % 3 == 0) {
                ai_state->move_choice = 1;
            } else {
                ai_state->move_choice = 0;
            }
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
    if (entity->combat_choice == 1) {
        EntityAi_ClearTurnInPlace(entity);
        if ((s32)(entity->flags >> 31) & 1) {
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
            side = EntityAi_TurnFromPush(entity, direction);
            switch (ai_state->move_choice) {
            case 0:
                if (side > 0) {
                    entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_RIGHT);
                } else if (side < 0) {
                    entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_LEFT);
                } else {
                    entity->move_command = EntityAi_MoveCommand(entity, 0, entity->radius + 100.0f + 30.0f);
                }
                break;
            case 1:
                if (side > 0) {
                    entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_RUN | MOVE_COMMAND_TURN_RIGHT);
                } else if (side < 0) {
                    entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_RUN | MOVE_COMMAND_TURN_LEFT);
                } else {
                    entity->move_command = EntityAi_MoveCommand(entity, 1, entity->radius + 100.0f + 30.0f);
                }
                break;
            }
        }
    }
done:
    EntityAi_StartAction(entity, (void (*)(Entity *))ai_state->move, unknown);
}

/* The move handler: only picks the animation, 3 with command bit 1, else 0.
   Skull_ApplyMoveCommand does the turning and the moving. */
static void Skull_Move(Entity *entity) {
    if (entity->move_command & MOVE_COMMAND_FORWARD) {
        Motion_Start(entity, 3, 0, entity->params->motion_blend_frames, 0);
    } else {
        Motion_Start(entity, 0, 0, entity->params->motion_blend_frames, 0);
    }
}

/* The death action (busy bit 0x40): animation 16, the fade over 20 frames and the removal, as
   EntityAi_Death with other flag settings; it sets Entity.collision_radius to 80. */
static void Skull_Death(Entity *entity) {
    entity->busy_flags = ENTITY_BUSY_DEATH;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x10, 0, 5, 1);
        entity->flags &= ~ENTITY_FLAG_MOTION_STEP_MODE_MASK;
        entity->flags |= ENTITY_FLAG_ANIMATES;
        entity->flags |= ENTITY_FLAG_100000;
        entity->flags = (entity->flags & ~ENTITY_FLAG_DYING_MASK) | ENTITY_FLAG_DYING_1;
        entity->flags |= ENTITY_FLAG_NO_PLAYER_COLLISION;
        entity->flags |= ENTITY_FLAG_NO_ENTITY_COLLISION;
        entity->flags &= ~ENTITY_FLAG_NO_FLOOR_LIFT;
        entity->collision_radius = 80.0f;
        entity->flags &= ~ENTITY_FLAG_400000;
        entity->action_step++;
    case 1:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            ENTITY_FLAG_BITS(entity)->bit14 = 0;
            ENTITY_FLAG_BITS(entity)->bits2 = 2;
            ENTITY_FLAG_BITS(entity)->bit4 = 1;
            entity->action_timer = 20;
            EntityEvent_OnDeath(entity);
            entity->action_step++;
        }
        break;
    case 3:
        if (entity->action_timer != 0) {
            entity->model->alpha = (f32)entity->model_alpha * (f32)entity->action_timer / 20.0f;
        } else {
            entity->flags |= ENTITY_FLAG_DYING_MASK;
            entity->flags &= ~ENTITY_FLAG_LIVE;
        }
        break;
    }
}

/* Helper of Skull_Think: turns and moves the entity as the command says, the speeds eased in
   work floats unk44 (sideways), unk48 (y) and unk4C (forward). While the entity is busy with
   anything but a hit reaction only command bits 0x10 and 0x20 count, and the turn and the other
   speeds ease out. */
static void Skull_ApplyMoveCommand(Entity *entity) {
    EntityAiStateKind16BF98 *ai_state = (EntityAiStateKind16BF98 *)&entity->ai_state;
    f32 turn = 0.0f;
    f32 step;
    f32 limit;

    if (entity->busy_flags == 0 || (entity->busy_flags & (ENTITY_BUSY_REACTION_LIGHT | ENTITY_BUSY_REACTION_HEAVY))) {
        if (entity->move_command & MOVE_COMMAND_TURN_LEFT) {
            ai_state->unk1C -= entity->params->turn_step_per_frame;
            if (ai_state->unk1C < -entity->params->turn_limit) {
                ai_state->unk1C = -entity->params->turn_limit;
            }
        } else if (entity->move_command & MOVE_COMMAND_TURN_RIGHT) {
            ai_state->unk1C += entity->params->turn_step_per_frame;
            if (entity->params->turn_limit < ai_state->unk1C) {
                ai_state->unk1C = entity->params->turn_limit;
            }
        } else if (entity->params->turn_step_per_frame < ai_state->unk1C) {
            ai_state->unk1C -= entity->params->turn_step_per_frame;
        } else if (ai_state->unk1C < -entity->params->turn_step_per_frame) {
            ai_state->unk1C += entity->params->turn_step_per_frame;
        } else {
            ai_state->unk1C = 0.0f;
        }
        turn = ai_state->unk1C;
        if (entity->move_command & MOVE_COMMAND_TURN_FAST) {
            turn += turn;
        }
        entity->turn_delta_yaw += turn;
        if (entity->move_command & MOVE_COMMAND_FORWARD) {
            ai_state->unk4C -= entity->params->speed_step_per_frame;
            if (ai_state->unk4C < -entity->params->max_speed) {
                ai_state->unk4C = -entity->params->max_speed;
            }
        } else if (entity->move_command & MOVE_COMMAND_BACK) {
            ai_state->unk4C += entity->params->speed_step_per_frame;
            if (entity->params->max_speed < ai_state->unk4C) {
                ai_state->unk4C = entity->params->max_speed;
            }
        } else if (ai_state->unk4C > entity->params->speed_step_per_frame) {
            ai_state->unk4C -= entity->params->speed_step_per_frame;
        } else if (ai_state->unk4C < -entity->params->speed_step_per_frame) {
            ai_state->unk4C += entity->params->speed_step_per_frame;
        } else {
            ai_state->unk4C = 0.0f;
        }
        if (entity->move_command & MOVE_COMMAND_STEP_LEFT) {
            ai_state->unk44 += entity->params->speed_step_per_frame;
            if (entity->params->max_speed < ai_state->unk44) {
                ai_state->unk44 = entity->params->max_speed;
            }
        } else if (entity->move_command & MOVE_COMMAND_STEP_RIGHT) {
            ai_state->unk44 -= entity->params->speed_step_per_frame;
            if (ai_state->unk44 < -entity->params->max_speed) {
                ai_state->unk44 = -entity->params->max_speed;
            }
        } else if (ai_state->unk44 > entity->params->speed_step_per_frame) {
            ai_state->unk44 -= entity->params->speed_step_per_frame;
        } else if (ai_state->unk44 < -entity->params->speed_step_per_frame) {
            ai_state->unk44 += entity->params->speed_step_per_frame;
        } else {
            ai_state->unk44 = 0.0f;
        }
    } else {
        if (entity->params->turn_step_per_frame < ai_state->unk1C) {
            ai_state->unk1C -= entity->params->turn_step_per_frame;
        } else if (ai_state->unk1C < -entity->params->turn_step_per_frame) {
            ai_state->unk1C += entity->params->turn_step_per_frame;
        } else {
            ai_state->unk1C = 0.0f;
        }
        if (ai_state->unk4C > entity->params->speed_step_per_frame) {
            ai_state->unk4C -= entity->params->speed_step_per_frame;
        } else if (ai_state->unk4C < -entity->params->speed_step_per_frame) {
            ai_state->unk4C += entity->params->speed_step_per_frame;
        } else {
            ai_state->unk4C = 0.0f;
        }
        if (entity->params->speed_step_per_frame < ai_state->unk44) {
            ai_state->unk44 -= entity->params->speed_step_per_frame;
        } else if (ai_state->unk44 < -entity->params->speed_step_per_frame) {
            ai_state->unk44 += entity->params->speed_step_per_frame;
        } else {
            ai_state->unk44 = 0.0f;
        }
    }
    ENTITY_SCRATCH_VECTOR[0] = ENTITY_SCRATCH_VECTOR[1] = ENTITY_SCRATCH_VECTOR[2] = ENTITY_SCRATCH_VECTOR[3] = 0.0f;
    step = entity->params->speed_step_per_frame * 0.8f;
    limit = entity->params->max_speed * 0.5f;
    if (entity->move_command & MOVE_COMMAND_DOWN) {
        ai_state->unk48 -= step;
        if (ai_state->unk48 < -limit) {
            ai_state->unk48 = -limit;
        }
    } else if (entity->move_command & MOVE_COMMAND_UP) {
        ai_state->unk48 += step;
        if (limit < ai_state->unk48) {
            ai_state->unk48 = limit;
        }
    } else if (step < ai_state->unk48) {
        ai_state->unk48 -= step;
    } else if (ai_state->unk48 < -step) {
        ai_state->unk48 += step;
    } else {
        ai_state->unk48 = 0.0f;
    }
    ENTITY_SCRATCH_VECTOR[0] = ai_state->unk44;
    ENTITY_SCRATCH_VECTOR[1] = ai_state->unk48;
    ENTITY_SCRATCH_VECTOR[2] = ai_state->unk4C;
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

/* Helper of Skull_Animate: swings angle unk50[0] of model part 3 up to work byte unk7B
   degrees and back to 0 over twice work byte unk7C frames, counted down in work word unk68; the
   angle is kept in work float unk50. */
static void Skull_SwingPart3(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    EntityModelPart80 *part = &((EntityModelPart80 *)entity->model->parts)[3];
    f32 angle;

    if (part != NULL) {
        if (ai_state->pause_timer >= ai_state->action_counter) {
            ai_state->part_angle_z += (f32)ai_state->action_flags / (f32)ai_state->action_counter * 0.017453292f;
            if (ai_state->action_flags * 0.017453292f < ai_state->part_angle_z) {
                ai_state->part_angle_z = ai_state->action_flags * 0.017453292f;
            }
        } else {
            ai_state->part_angle_z -= (f32)ai_state->action_flags / (f32)ai_state->action_counter * 0.017453292f;
            if (ai_state->part_angle_z < 0.0f) {
                ai_state->part_angle_z = 0.0f;
            }
        }
        angle = ai_state->part_angle_z;
        part->unk50[0] = angle;
        part->unk50[0] = EntityMath_WrapAngle(angle);
        ai_state->pause_timer--;
    }
}

/* The animate handler: as EntityAi_Animate without the head turn. Skull_SwingPart3 swings
   model part 3 while the entity is not busy. Every tenth frame it launches shot 0xD0 from the point
   Model_GetSkinnedBonePoint gives for 0xFF (Attack_LaunchMagicEffect); on the other frames it starts effect 0x39D at model
   point 0xFF. */
static void Skull_Animate(Entity *entity) {
    u32 blend_frames; // match: converted to float as unsigned
    s64 flags;
    EntityAiState *ai_state;
    EntityUnk1B1390 *shot;
    f32 unused[4]; /* retail reserves the 0x10 bytes */

    flags = entity->flags;
    ai_state = &entity->ai_state;
    if (ENTITY_FLAG_BITS(entity)->bit14) {
        if (ENTITY_PARAMS_MOVEMENT_KIND(entity->params) == ENTITY_PARAMS_MOVEMENT_FIXED) {
            entity->flags = flags & ~ENTITY_FLAG_MOTION_STEP_MODE_MASK;
        }
        blend_frames = entity->model->level_tables->blend_left;
        if (blend_frames != 0) {
            ENTITY_FLAG_BITS(entity)->bit34 = Motion_BlendToPose(entity->model, (entity->busy_flags & (ENTITY_BUSY_GUARD | ENTITY_BUSY_REACTION_LIGHT)) ? ai_state->pose_blend_weight : 1.0f / (f32)blend_frames);
        } else {
            ENTITY_FLAG_BITS(entity)->bit34 = Motion_Step(entity, ENTITY_FLAG_BITS(entity)->bits11);
        }
        if (entity->busy_flags == 0 && entity->model->level_tables->blend_left == 0) {
            Skull_SwingPart3(entity);
        } else {
            ai_state->part_angle_z = 0.0f;
        }
        if (frame_counter % 10U == 0) {
            shot = Shot_Alloc();
            shot->unk00 = 1;
            shot->unk20 = 0;
            shot->unk22 = 0;
            shot->unk28 = entity->params->unk238;
            Attack_LaunchMagicEffect(0xD0, shot, Model_GetSkinnedBonePoint((char *)entity->model, 0xFF), entity->model->rotation, entity);
        } else {
            EntityEvent_StartEffectAtPoint((char **)entity, 0x39D, 0xFF, (int)entity->model->rotation);
        }
    }
}

/* The ranged attack with record 0 (busy bit 1): animation 23; from frame EntityParams.unk43A to
   unk43C launches the record's shots every other frame. */
static void Skull_RangedAttack0(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    EntityParamsUnk3D8 *unk3D8 = &entity->params->ranged_attacks[0].record;

    entity->busy_flags |= ENTITY_BUSY_ATTACK;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x17, 0, entity->params->motion_blend_frames >> 1, 1);
        ENTITY_FLAGS(entity) = (ENTITY_FLAGS(entity) & ~0x3800) | 0x800;
        ENTITY_FLAGS(entity) &= ~0x200;
        entity->action_step++;
    case 1:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        if (unk3D8->unk01 != 0) {
            EntityAi_TurnToHeading(entity, &ai_state->turn_rate, entity->heading_to_player);
        }
        break;
    case 2:
        if (unk3D8->unk01 >= entity->model->level_tables->frame) {
            EntityAi_TurnToHeading(entity, &entity->ai_state.turn_rate, entity->heading_to_player);
        } else {
            ai_state->turn_rate = 0.0f;
        }
        if (entity->model->level_tables->frame >= entity->params->unk43A && entity->model->level_tables->frame <= entity->params->unk43C &&
            (frame_counter & 1)) {
            EntityAi_LaunchShots(entity, unk3D8, entity->params->unk43A, entity->params->unk43C);
        }
        if (entity->model->level_tables->frame > 0 && unk3D8->unk03 >= entity->model->level_tables->frame) {
            ENTITY_FLAGS(entity) |= 0x80000;
        } else {
            ENTITY_FLAGS(entity) &= ~0x80000;
        }
        if ((s32)(ENTITY_FLAGS(entity) >> 34) & 1) {
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            EntityAi_SetAttackDelay(entity, unk3D8->unk02);
            if ((s32)(ENTITY_FLAGS(entity) >> 5) & 1) {
                ai_state->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))ai_state->move, 1);
            }
        }
        break;
    }
}
