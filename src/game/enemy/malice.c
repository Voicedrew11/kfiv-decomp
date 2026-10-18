/* Malice: the behaviour of entity type 58 (Entity.params->unk02, the index into entity_type_handlers): its set-up
   function, which the table names, and the handlers it installs. Used by: Malice. The game names no monsters;
   these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by hit points
   and experience, with resistances and drops where two monsters share those. */
#include "entity/entity.h"
#include "common.h"
#include "effect/effect.h"

void EntityAi_SetUpdate(char *, s32);
int EntityAi_StartAction(Entity *, void (*)(Entity *), int);
void Motion_ClearPartSlots(EntityModel *);
int Motion_StepModel(EntityModel *, s32, s32);
void Vector_Copy(void *, void *);
// match: the definition takes an Actor, a struct local to EntityAi_Animate.c
void EntityAi_Animate(void);
extern s32 entity_type_handlers[][2];
extern u32 frame_counter;
extern f32 D_00446814[];
void EntityAi_SensePlayer(void *, int);
void EntityAi_CheckFarFromSpawn(char *);
void EntityAi_StepState(void *);
int EntityAi_ThinkReturn(void *, int);
int EntityAi_ThinkIdle(void *, int);
void EntityAi_ThinkPursue(Entity *, s32);
f32 EntityCollision_DistanceToPropFloor(void *, void *);
extern PlayerData player_state;
int rand(void);
s32 EntityAi_ShouldGuard(Entity *, f32, f32);
s32 EntityAi_ShouldDodge(Entity *, f32, f32);
s32 EntityAi_PickCombatChoice(Entity *, s32);
void EntityAi_LowerChoiceWeight(Entity *, s32);
int EntityAi_PickAttack(char *);
float EntityMath_DistanceXZ(void *, void *);
void EntityPoint_PickNearFromDistance(f32 *, f32 *, f32, f32);
void EntityAi_ClearTurnInPlace(Entity *);
float EntityMath_AngleDifference(float, float);
int EntityAi_TurnCommand(Entity *);
int EntityAi_TurnFromPush(void *, void *);
s32 EntityAi_MoveCommand(Entity *, s32, f32);
void EntityAi_Dodge(Entity *);
void EntityAi_Guard(Entity *);
void EntityAi_MeleeAttack(Entity *);
void EntityAi_RangedAttack(Entity *);
int Motion_Start(void *, int, int, u32, int);
extern f32 identity_matrix[16];
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0RotMatrixZ(float *, float *, float);
void Vector_ApplyMatrix(void *, void *, void *);
void Vector_Scale(float *, float *, float);
extern f32 D_00446818[];
void EntityEvent_StartEffectAtPoint(char **, int, u32, int);
float EntityMath_AngleXZ(float *, char *);
float EntityMath_WrapAngle(float);
float EntityMath_HeadingTo(float *, float *);
void EntityAi_LowerWeightsAfterHit(char *);
int EntityAi_SetReaction(char *, void (*)(void), int);
void EntityAi_SetState(char *, u8);
void EntityAi_ReactionRewind(Entity *);
void EntityAi_ReactionLight(Entity *);
void EntityAi_ReactionHeavy(Entity *);
float atanf(float);
s32 EntityAi_TurnToHeading(Entity *, f32 *, f32);
int EntityShot_Launch(Entity *, EntityParamsUnk3D8 *, s32, s32, f32 *);
void EntityAi_SetAttackDelay(Entity *, s32);
void EntityEvent_OnDeath(Entity *);

static void Malice_Think(Entity *entity, s32 unknown);
static void Malice_ThinkCombat(Entity *entity, s32 unknown);
static void Malice_Move(Entity *entity);
static void Malice_ApplyMoveCommand(Entity *entity);
static void Malice_ToggleFadeLevel(Entity *entity);
static void Malice_FadeOutAndRelocate(Entity *entity);
static void Malice_FadeBackIn(Entity *entity);
static void Malice_OnHit(Entity *entity);
static void Malice_Dormant(Entity *entity);
static void Malice_RangedAttack0(Entity *entity);
static void Malice_Death(Entity *entity);

/* Set-up of a type 58 entity: clears its work area and installs the type's handlers. */
void Malice_Init(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    char *unk138 = entity->model->look_at_part;
    s32 i;

    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        ENTITY_WORK_BYTES(entity)[i] = 0;
    }
    EntityAi_SetUpdate((char *)entity, entity_type_handlers[entity->params->entity_type][1]);
    ai_state->think = Malice_Think;
    ai_state->animate = EntityAi_Animate;
    ai_state->on_hit = (void (*)(void))Malice_OnHit;
    ai_state->move = (void (*)(void))Malice_Move;
    ai_state->dormant = (void (*)(void))Malice_Dormant;
    ai_state->death = (void (*)(void))Malice_Death;
    for (i = 3; i != -1; i--) {
        entity->choice_weights[i] = 0x80;
    }
    for (i = 5; i != -1; i--) {
        entity->attack_weights[i] = 0x80;
    }
    ENTITY_FLAG_BITS(entity)->bit28 = 1;
    entity->target = NULL;
    if (ENTITY_SPAWN_FLAGS(entity) & (0x8000L << 43)) {
        EntityAi_StartAction(entity, (void (*)(Entity *))ai_state->dormant, 1);
    } else {
        EntityAi_StartAction(entity, (void (*)(Entity *))ai_state->move, 0);
    }
    if (unk138 != NULL) {
        Vector_Copy(ai_state->look_part_angles, unk138 + 0x50);
    }
    ai_state->pose_blend_weight = 0.5f;
    entity->model_alpha = 0;
    entity->model->alpha = 0.0f;
    Motion_ClearPartSlots(entity->model);
    Motion_StepModel(entity->model, 0, 0);
}

/* The think handler: as EntityAi_Think, with Malice_ThinkCombat for state 2. It also keeps
   flag bits 18, 20, 21, 26 and 27 set while work byte unk7B is set or work word unk68 is 1, counts
   down work word unk6A, adds command bit 0x10 or 0x20 by what EntityCollision_DistanceToPropFloor reads at the entity's
   position against EntityParams.unk43C and unk43E (work byte unk7A), and calls
   Malice_ApplyMoveCommand while the entity is not busy or has one of the busy bits 0x3006. */
static void Malice_Think(Entity *entity, s32 unknown) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 height;

    if (frame_counter % 10U == entity->spawn_index % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (ai_state->action_flags != 0 || ai_state->pause_timer == 1) {
        ENTITY_FLAG_BITS(entity)->bit20 = 1;
        ENTITY_FLAG_BITS(entity)->bit18 = 1;
        ENTITY_FLAG_BITS(entity)->bit21 = 1;
        ENTITY_FLAG_BITS(entity)->bit26 = 1;
        ENTITY_FLAG_BITS(entity)->bit27 = 1;
    } else {
        ENTITY_FLAG_BITS(entity)->bit20 = 0;
        ENTITY_FLAG_BITS(entity)->bit18 = 0;
        ENTITY_FLAG_BITS(entity)->bit21 = 0;
        ENTITY_FLAG_BITS(entity)->bit26 = 0;
        ENTITY_FLAG_BITS(entity)->bit27 = 0;
    }
    if (ai_state->sub_timer > 0) {
        ai_state->sub_timer--;
    }
    ENTITY_FLAG_BITS(entity)->bit28 = 1;
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
                Malice_ThinkCombat(entity, unknown);
                break;
            default:
                EntityAi_ThinkIdle(entity, unknown);
                break;
            }
        }
    }
    if (ENTITY_FLAG_BITS(entity)->bit28) {
        height = EntityCollision_DistanceToPropFloor(entity->map_record, entity->model->position);
        if (height != D_00446814[0]) {
            if (entity->params->unk43C < height) {
                ai_state->step_index = 1;
            } else if (height < entity->params->unk43E) {
                ai_state->step_index = 0;
            }
            if (ai_state->step_index != 0) {
                entity->move_command |= MOVE_COMMAND_UP;
            } else {
                entity->move_command |= MOVE_COMMAND_DOWN;
            }
        } else {
            ai_state->step_index = 0;
            entity->move_command |= MOVE_COMMAND_DOWN;
        }
    }
    if (entity->busy_flags == 0 || (entity->busy_flags & (ENTITY_BUSY_1000 | ENTITY_BUSY_2000 | ENTITY_BUSY_REACTION_LIGHT | ENTITY_BUSY_REACTION_HEAVY))) {
        Malice_ApplyMoveCommand(entity);
    }
}

/* The think function of state 2: as EntityAi_ThinkCombat, but first it starts
   Malice_ToggleFadeLevel (when work byte unk7B is 1 and work word unk6A has run out, or with a
   chance of EntityParams.unk43A in 100 when the timer is at 0) or Malice_FadeOutAndRelocate (a
   chance of unk438 in 100). It attacks only with unk7B at 0 and the player within 200 in y, and
   else goes to a point EntityPoint_PickNearFromDistance picks; ranged record 0 is Malice_RangedAttack0. Closing
   in never runs. */
static void Malice_ThinkCombat(Entity *entity, s32 unknown) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 retreat_point[4];
    f32 destination[4];
    f32 direction[4];
    EntityModel *model;
    f32 height_difference;
    f32 angle;
    s32 side;
    s32 turn;

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
    if (ai_state->action_flags == 1 && ai_state->sub_timer == 0) {
        EntityAi_StartAction(entity, Malice_ToggleFadeLevel, 1);
        return;
    }
    if (entity->action_timer == 0 && rand() % 100 < entity->params->unk43A) {
        EntityAi_StartAction(entity, Malice_ToggleFadeLevel, 1);
        return;
    }
    if (entity->action_timer == 0 && rand() % 100 < entity->params->far_distance) {
        EntityAi_StartAction(entity, Malice_FadeOutAndRelocate, 1);
        return;
    }
    side = EntityAi_PickCombatChoice(entity, 0);
    ENTITY_FLAG_BITS(entity)->bit9 = 1;
    if (entity->action_timer == 0 || side == 0) {
        entity->prev_combat_choice = entity->combat_choice;
        entity->combat_choice = side;
        if (entity->combat_choice != entity->prev_combat_choice) {
            entity->choice_change_frame = frame_counter;
        }
        EntityAi_LowerChoiceWeight(entity, side);
        switch (entity->combat_choice) {
        case 0:
            model = entity->model;
            height_difference = player_state.position[1] - model->position[1];
            if (ABS(height_difference) > 200.0f) {
                EntityPoint_PickNearFromDistance(retreat_point, model->position, EntityMath_DistanceXZ(model->position, entity->spawn_info), entity->radius * 4.0f);
                entity->target = retreat_point;
                entity->action_timer = rand() % 30 + 30;
                ai_state->move_choice = 0;
                entity->combat_choice = 1;
            } else if (ai_state->action_flags == 0) {
                if (EntityAi_PickAttack((char *)entity)) {
                    if (ENTITY_FLAG_BITS(entity)->bits58 == 0) {
                        EntityAi_StartAction(entity, Malice_RangedAttack0, unknown);
                    } else {
                        EntityAi_StartAction(entity, EntityAi_RangedAttack, unknown);
                    }
                } else {
                    EntityAi_StartAction(entity, EntityAi_MeleeAttack, unknown);
                }
                return;
            } else {
                EntityPoint_PickNearFromDistance(destination, model->position, EntityMath_DistanceXZ(model->position, entity->spawn_info), entity->radius * 4.0f);
                entity->target = destination;
                entity->action_timer = rand() % 30 + 60;
                ai_state->move_choice = 0;
                entity->combat_choice = 1;
            }
            break;
        case 1:
            entity->target = &player_state;
            entity->action_timer = rand() % 30 + 60;
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
            turn = EntityAi_TurnFromPush(entity, direction);
            if (turn > 0) {
                entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_RIGHT);
            } else if (turn < 0) {
                entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_LEFT);
            } else {
                entity->move_command = EntityAi_MoveCommand(entity, 0, entity->radius + 100.0f + 30.0f);
            }
        }
    }
done:
    EntityAi_StartAction(entity, (void (*)(Entity *))ai_state->move, unknown);
}

/* The move handler: only picks the animation, 3 going forward and 0 otherwise;
   Malice_ApplyMoveCommand moves the entity. */
static void Malice_Move(Entity *entity) {
    if (entity == player_state.stood_on_entity) {
        if (entity->params->flags & ENTITY_PARAMS_FLAG_STOPS_WHEN_STOOD_ON) {
            entity->move_command = 0;
        }
    }
    if (entity->move_command & MOVE_COMMAND_FORWARD) {
        Motion_Start(entity, 3, 0, entity->params->motion_blend_frames, 0);
        return;
    }
    Motion_Start(entity, 0, 0, entity->params->motion_blend_frames, 0);
}

/* Helper of Malice_Think: carries out the command in Entity.move_command. It turns the entity and
   speeds up or slows down along its three axes by EntityParams.speed_step_per_frame a frame up to unk54 (work
   floats unk44, unk48 and unk4C; in y half the step and a fifth of the limit), twice as fast for
   command bit 0x40. */
static void Malice_ApplyMoveCommand(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 turn;
    f32 step;
    f32 limit;

    turn = 0.0f;
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
    turn = ai_state->turn_rate;
    if (entity->move_command & MOVE_COMMAND_TURN_FAST) {
        turn += turn;
    }
    entity->turn_delta_yaw += turn;
    ENTITY_SCRATCH_VECTOR[0] = ENTITY_SCRATCH_VECTOR[1] = ENTITY_SCRATCH_VECTOR[2] = ENTITY_SCRATCH_VECTOR[3] = 0.0f;
    step = entity->params->speed_step_per_frame * 0.5f;
    limit = entity->params->max_speed * 0.2f;
    if (entity->move_command & MOVE_COMMAND_FORWARD) {
        ai_state->part_angle_y -= entity->params->speed_step_per_frame;
        if (ai_state->part_angle_y < -entity->params->max_speed) {
            ai_state->part_angle_y = -entity->params->max_speed;
        }
    } else if (entity->move_command & MOVE_COMMAND_BACK) {
        ai_state->part_angle_y += entity->params->speed_step_per_frame;
        if (entity->params->max_speed < ai_state->part_angle_y) {
            ai_state->part_angle_y = entity->params->max_speed;
        }
    } else if (entity->params->speed_step_per_frame < ai_state->part_angle_y) {
        ai_state->part_angle_y -= entity->params->speed_step_per_frame;
    } else if (ai_state->part_angle_y < -entity->params->speed_step_per_frame) {
        ai_state->part_angle_y += entity->params->speed_step_per_frame;
    } else {
        ai_state->part_angle_y = 0.0f;
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
    if (entity->move_command & MOVE_COMMAND_DOWN) {
        ai_state->part_angle_x -= step;
        if (ai_state->part_angle_x < -limit) {
            ai_state->part_angle_x = -limit;
        }
    } else if (entity->move_command & MOVE_COMMAND_UP) {
        ai_state->part_angle_x += step;
        if (limit < ai_state->part_angle_x) {
            ai_state->part_angle_x = limit;
        }
    } else if (step < ai_state->part_angle_x) {
        ai_state->part_angle_x -= step;
    } else if (ai_state->part_angle_x < -step) {
        ai_state->part_angle_x += step;
    } else {
        ai_state->part_angle_x = 0.0f;
    }
    ENTITY_SCRATCH_VECTOR[0] = ai_state->move_speed;
    ENTITY_SCRATCH_VECTOR[1] = ai_state->part_angle_x;
    ENTITY_SCRATCH_VECTOR[2] = ai_state->part_angle_y;
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

/* An action (busy bit 0x1000): plays animation 1 while Entity.model_alpha goes by 1 a frame up to 0x30
   (work byte unk7B set) or down to 0x10 (clear), then flips unk7B, sets work word unk6A to
   EntityParams.near_distance and thinks. */
static void Malice_ToggleFadeLevel(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;

    entity->busy_flags |= ENTITY_BUSY_1000;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 1, 0, entity->params->motion_blend_frames, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->action_step++;
    case 1:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (ai_state->action_flags != 0) {
            entity->model_alpha++;
        } else {
            entity->model_alpha--;
        }
        if ((ai_state->action_flags == 1 && entity->model_alpha >= 0x30) || (ai_state->action_flags == 0 && entity->model_alpha <= 0x10)) {
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            entity->model_alpha = ai_state->action_flags ? 0x30 : 0x10;
            ai_state->action_flags = ai_state->action_flags == 0;
            ai_state->sub_timer = entity->params->near_distance;
            if (ENTITY_FLAG_BITS(entity)->bit5) {
                ai_state->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))ai_state->move, 1);
            }
        }
        break;
    }
}

/* A second action that plays animation 1 (busy bit 0x2000): sets work word unk68 to 1 and flag bits
   18, 20, 21 and 27, calls EntityEvent_StartEffectAtPoint with 0x234 and takes Entity.model_alpha down to 0; meanwhile it
   looks for a point round the spawn point (EntityPoint_PickNearFromDistance, EntityCollision_DistanceToPropFloor) more than 45 degrees off
   the player's facing. Then it puts the entity there if it found one and starts
   Malice_FadeBackIn. */
static void Malice_FadeOutAndRelocate(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 *destination;
    f32 height;
    f32 yaw;

    entity->busy_flags |= ENTITY_BUSY_2000;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 1, 0, entity->params->motion_blend_frames, 0);
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
        ENTITY_FLAG_BITS(entity)->bits36 = 0;
        ai_state->pause_timer = 1;
        ai_state->turn_rate = 0.0f;
        ENTITY_FLAG_BITS(entity)->bit10 = 0;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->action_step++;
        break;
    case 1:
        if (entity->model->level_tables->blend_left == 0) {
            ENTITY_FLAG_BITS(entity)->bit20 = 1;
            ENTITY_FLAG_BITS(entity)->bit18 = 1;
            ENTITY_FLAG_BITS(entity)->bit21 = 1;
            ENTITY_FLAG_BITS(entity)->bit27 = 1;
            ai_state->action_counter = 0;
            ai_state->reload_timer = 0;
            EntityEvent_StartEffectAtPoint((char **)entity, 0x234, 0, (int)entity->model->rotation);
            entity->action_step++;
        }
        break;
    case 2:
        if (ai_state->action_counter == 0) {
            destination = ai_state->target_point;
            EntityPoint_PickNearFromDistance(destination, (f32 *)entity->spawn_info, EntityMath_DistanceXZ(entity->model->position, entity->spawn_info), entity->spawn_info->wander_radius);
            height = EntityCollision_DistanceToPropFloor(entity->map_record, destination);
            if (height != D_00446818[0]) {
                yaw = EntityMath_AngleXZ(player_state.position, (char *)destination);
                if (ABS(EntityMath_WrapAngle(yaw - player_state.heading)) > 0.7853982f) {
                    ai_state->target_point[1] += height - (f32)(rand() % 200);
                    ai_state->action_counter = 1;
                }
            }
        }
        entity->model_alpha -= (ai_state->action_flags != 0) ? 2 : 4;
        if (entity->model_alpha > 0x80) {
            entity->model_alpha = 0;
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            if (ai_state->action_counter == 1) {
                Vector_Copy(entity->model->position, ai_state->target_point);
            }
            EntityAi_StartAction(entity, Malice_FadeBackIn, 1);
        }
        break;
    }
}

/* A third action that plays animation 1 (busy bit 0x3000), started by Malice_FadeOutAndRelocate:
   waits EntityParams.action_frames frames, turns the entity to the player, calls EntityEvent_StartEffectAtPoint with 0x235
   and takes Entity.model_alpha back up to 0x30 (0x10 with work byte unk7B set), then clears work word
   unk68 and thinks. */
static void Malice_FadeBackIn(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;

    entity->busy_flags |= (ENTITY_BUSY_1000 | ENTITY_BUSY_2000);
    switch (entity->action_step) {
    case 0:
        ENTITY_FLAG_BITS(entity)->bits36 = 0;
        ai_state->mode_timer = 0;
        entity->model_alpha = 0;
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        entity->action_step++;
        break;
    case 1:
        if ((s16)entity->params->action_frames < ai_state->mode_timer++) {
            Motion_Start(entity, 1, 0, 0, 0);
            entity->model->rotation[1] = EntityMath_HeadingTo(entity->model->position, (f32 *)&player_state);
            EntityEvent_StartEffectAtPoint((char **)entity, 0x235, 0, (int)entity->model->rotation);
            entity->action_step++;
        }
        break;
    case 2:
        entity->model_alpha++;
        if (entity->model_alpha > (ai_state->action_flags != 0 ? 0x10 : 0x30)) {
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            entity->model_alpha = ai_state->action_flags ? 0x10 : 0x30;
            ai_state->pause_timer = 0;
            if (ENTITY_FLAG_BITS(entity)->bit5) {
                ai_state->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))ai_state->move, 1);
            }
        }
        break;
    }
}

/* The on_hit handler: the same as EntityAi_OnHit. */
static void Malice_OnHit(Entity *entity) {
    EntityAi_LowerWeightsAfterHit((char *)entity);
    entity->target = NULL;
    ENTITY_FLAG_BITS(entity)->bit10 = 0;
    ENTITY_FLAG_BITS(entity)->bit9 = 0;
    if (ENTITY_FLAG_BITS(entity)->bits36 == 1) {
        if (ENTITY_FLAG_BITS(entity)->bit19) {
            EntityAi_SetReaction((char *)entity, (void (*)(void))EntityAi_ReactionRewind, 0);
        } else {
            EntityAi_SetReaction((char *)entity, (void (*)(void))EntityAi_ReactionLight, 0);
        }
    } else if (ENTITY_FLAG_BITS(entity)->bits36 == 2) {
        EntityAi_SetReaction((char *)entity, (void (*)(void))EntityAi_ReactionHeavy, 0);
    }
    EntityAi_SetState((char *)entity, 2);
}

/* The dormant action (busy bit 0x20): as EntityAi_Dormant, but in place of animation 2 it takes
   Entity.model_alpha up by 1 a frame to 0x30. */
static void Malice_Dormant(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;

    entity->busy_flags |= ENTITY_BUSY_DORMANT;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 1, 0, 0, 1);
        if (entity->params->flags & ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->bits11 = ENTITY_PARAMS_MOVEMENT_KIND(entity->params) == ENTITY_PARAMS_MOVEMENT_1 ? 2 : 1;
        }
        ai_state->turn_rate = 0.0f;
        ENTITY_FLAG_BITS(entity)->bit21 = 1;
        ENTITY_FLAG_BITS(entity)->bit22 = 1;
        ENTITY_FLAG_BITS(entity)->bit23 = 1;
        ENTITY_FLAG_BITS(entity)->bit24 = 1;
        ENTITY_FLAG_BITS(entity)->bit18 = 1;
        ENTITY_FLAG_BITS(entity)->bit28 = 1;
        ENTITY_FLAG_BITS(entity)->bit10 = 0;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->action_step++;
        break;
    case 1:
        if (entity->distance_to_player < entity->spawn_info->wake_distance || ENTITY_FLAG_BITS(entity)->bit43) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
            ENTITY_FLAG_BITS(entity)->bits36 = 0;
            entity->action_step++;
        }
        break;
    case 2:
        entity->model_alpha++;
        if (entity->model_alpha >= 0x30) {
            entity->model_alpha = 0x30;
            EntityAi_SetState((char *)entity, 2);
            ENTITY_FLAG_BITS(entity)->bit21 = 0;
            ENTITY_FLAG_BITS(entity)->bit22 = 0;
            ENTITY_FLAG_BITS(entity)->bit23 = 0;
            ENTITY_FLAG_BITS(entity)->bit24 = 0;
            ENTITY_FLAG_BITS(entity)->bit18 = 0;
            if (ENTITY_PARAMS_MOVEMENT_KIND(entity->params) == ENTITY_PARAMS_MOVEMENT_WALK) {
                ENTITY_FLAG_BITS(entity)->bit28 = 0;
            }
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            if (ENTITY_FLAG_BITS(entity)->bit5) {
                entity->ai_state.think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))ai_state->move, 1);
            }
        }
        break;
    }
}

/* The ranged attack with record 0 (busy bit 1): animation 23; from frame 28 to 60 it launches the
   record's shots every other frame with EntityShot_Launch, pitched at the player and along the reverse
   of its heading. */
static void Malice_RangedAttack0(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    EntityParamsUnk3D8 *unk3D8 = &entity->params->ranged_attacks[0].record;
    f32 direction[4];

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
            EntityAi_TurnToHeading(entity, &ai_state->turn_rate, entity->heading_to_player);
        } else {
            ai_state->turn_rate = 0.0f;
        }
        if (entity->model->level_tables->frame >= 0x1C && entity->model->level_tables->frame <= 0x3C) {
            Vector_Copy(direction, entity->model->rotation);
            direction[0] = atanf((entity->model->position[1] - player_state.position[1]) / entity->distance_to_player);
            direction[1] = EntityMath_WrapAngle(direction[1] - 3.1415927f);
            if (frame_counter & 1) {
                EntityShot_Launch(entity, unk3D8, 0x1C, 0x3C, direction);
            }
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

/* The death action (busy bit 0x40): as EntityAi_Death, but it frees the entity's effects
   (Effect_FreeForOwner) when EntityParams.respawn_effect_id is neither 0 nor 0xFFFF and leaves flag bit 28
   alone. */
static void Malice_Death(Entity *entity) {
    EntityParams *params;

    entity->busy_flags = ENTITY_BUSY_DEATH;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x10, 0, 5, 1);
        params = entity->params;
        if (params->flags & ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->bits11 = ENTITY_PARAMS_MOVEMENT_KIND(params) == ENTITY_PARAMS_MOVEMENT_1 ? 2 : 1;
        }
        if (ENTITY_PARAMS_MOVEMENT_KIND(params) == ENTITY_PARAMS_MOVEMENT_FLY) {
            entity->collision_radius = 20.0f;
        }
        ENTITY_FLAG_BITS(entity)->bit14 = 1;
        ENTITY_FLAG_BITS(entity)->bit20 = 1;
        ENTITY_FLAG_BITS(entity)->bits2 = 1;
        ENTITY_FLAG_BITS(entity)->bit27 = 1;
        ENTITY_FLAG_BITS(entity)->bit26 = 1;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        if (params->respawn_effect_id != 0 && params->respawn_effect_id != 0xFFFF) {
            Effect_FreeForOwner(entity);
        }
        entity->action_step++;
    case 1:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            entity->action_timer = 20;
            ENTITY_FLAG_BITS(entity)->bit14 = 0;
            ENTITY_FLAG_BITS(entity)->bits2 = 2;
            ENTITY_FLAG_BITS(entity)->bit4 = 1;
            EntityEvent_OnDeath(entity);
            entity->action_step++;
        }
        break;
    case 3:
        if (entity->action_timer != 0) {
            entity->model->alpha = (f32)entity->model_alpha * (f32)entity->action_timer / 20.0f;
        } else {
            ENTITY_FLAG_BITS(entity)->bits2 = 3;
            ENTITY_FLAG_BITS(entity)->bit1 = 0;
        }
        break;
    }
}
