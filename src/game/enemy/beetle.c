/* Beetle: the behaviour of entity type 5 (EntityParams.entity_type, the index into entity_type_handlers): its set-up
   function, which the table names, and the handlers it installs. Used by: Beetle Small, Beetle Very Large,
   Beetle White, Beetle Medium. The game names no monsters; these names are from
   https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by hit points and experience, with
   resistances and drops where two monsters share those. */
#include "entity/entity.h"
#include "common.h"

#define ENTITY_WORK_AREA_SIZE 0xA0
void EntityAi_SetUpdate(char *, s32);
int EntityAi_StartAction(Entity *, void (*)(Entity *), int);
void Motion_ClearPartSlots(EntityModel *);
int Motion_StepModel(EntityModel *, s32, s32);
void EntityAi_OnHit(Entity *);
void EntityAi_Move(Entity *);
void EntityAi_Dormant(Entity *);
void EntityAi_Death(Entity *);
extern s32 entity_type_handlers[][2];
int Motion_Step(void *, int);
int Motion_BlendToPose(EntityModel *, f32);
extern u32 frame_counter;
void EntityAi_SensePlayer(void *, int);
void EntityAi_CheckFarFromSpawn(char *);
void EntityAi_StepState(void *);
int EntityAi_ThinkReturn(void *, int);
void EntityAi_StandOnce(Entity *);
int rand(void);
void EntityPoint_PickNear(float, void *, void *, void *);
int EntityAi_TurnFromPush(void *, void *);
s32 EntityAi_MoveCommand(Entity *, s32, f32);
extern PlayerData player_state;
s32 EntityAi_ShouldGuard(Entity *, f32, f32);
s32 EntityAi_ShouldDodge(Entity *, f32, f32);
s32 EntityAi_PickCombatChoice(Entity *, s32);
void EntityAi_LowerChoiceWeight(Entity *, s32);
int EntityAi_PickAttack(char *);
void EntityAi_ClearTurnInPlace(Entity *);
float EntityMath_AngleDifference(float, float);
int EntityAi_TurnCommand(Entity *);
void EntityAi_Dodge(Entity *);
void EntityAi_Guard(Entity *);
void EntityAi_MeleeAttack(Entity *);
float EntityMath_EaseAngleToward(float *, float, float);
int Motion_Start(void *, int, int, u32, int);
s32 EntityAi_TurnToHeading(Entity *, f32 *, f32);
int EntityAi_LaunchShots(Entity *, EntityParamsUnk3D8 *, s32, s32);
void EntityAi_SetAttackDelay(Entity *, s32);

static void Beetle_Animate(Entity *entity);
static void Beetle_Think(Entity *entity, s32 unknown);
static void Beetle_ThinkWander(Entity *entity, s32 unknown);
static void Beetle_ThinkCombat(Entity *entity, s32 unknown);
static void Beetle_TurnParts7And12(void *entity_arg);
static void Beetle_TurnParts23To32(Entity *entity);
static void Beetle_RangedAttack0(Entity *entity);

/* Set-up of a type 5 entity: clears its work area and installs the type's handlers. */
void Beetle_Init(Entity *entity) {
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
    ai_state->think = Beetle_Think;
    ai_state->animate = Beetle_Animate;
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
    ai_state->pose_blend_weight = 0.5f;
    entity->model_alpha = (entity->params->flags & ENTITY_PARAMS_FLAG_HALF_MODEL_ALPHA) ? 0x40 : 0x80;
    entity->model->alpha = entity->model_alpha;
    Motion_ClearPartSlots(entity->model);
    Motion_StepModel(entity->model, 0, 0);
}

/* The animate handler: as EntityAi_Animate without the test of the EntityParams kind bits and
   without the head turn; Beetle_TurnParts7And12 and Beetle_TurnParts23To32 turn model parts
   instead. */
static void Beetle_Animate(Entity *entity) {
    u32 blend_frames; // match: converted to float as unsigned
    s64 flags;
    EntityAiState *ai_state;

    flags = entity->flags;
    ai_state = &entity->ai_state;
    if (ENTITY_FLAG_BITS(entity)->animates) {
        blend_frames = entity->model->level_tables->blend_left;
        if (blend_frames != 0) {
            ENTITY_FLAG_BITS(entity)->motion_ended = Motion_BlendToPose(entity->model, (entity->busy_flags & ENTITY_BUSY_REACTION_LIGHT) ? ai_state->pose_blend_weight : 1.0f / (f32)blend_frames);
        } else {
            ENTITY_FLAG_BITS(entity)->motion_ended = Motion_Step(entity, (s32)(flags >> 11) & 7);
        }
        Beetle_TurnParts7And12(entity);
        Beetle_TurnParts23To32(entity);
    }
}

/* The think handler: as EntityAi_Think, with Beetle_ThinkCombat for state 2 and Beetle_ThinkWander
   for states 0 and 1; when it comes to state 0 from another state it starts the shared action
   EntityAi_StandOnce instead. */
static void Beetle_Think(Entity *entity, s32 unknown) {
    if (frame_counter % 10U == entity->spawn_index % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (entity->busy_flags != 0) {
        return;
    }
    EntityAi_CheckFarFromSpawn((char *)entity);
    EntityAi_StepState(entity);
    if (ENTITY_FLAG_BITS(entity)->far_from_spawn) {
        EntityAi_ThinkReturn(entity, unknown);
        return;
    }
    switch (entity->state) {
    case 0:
        if (entity->prev_state != 0) {
            EntityAi_StartAction(entity, EntityAi_StandOnce, unknown);
        } else {
            Beetle_ThinkWander(entity, unknown);
        }
        return;
    case 1:
        break;
    case 2:
        Beetle_ThinkCombat(entity, unknown);
        return;
    }
    Beetle_ThinkWander(entity, unknown);
}

/* The think function of every state but 2 (Beetle_Think calls it): wanders. Each time the
   timer runs out it picks a point near the spawn point (EntityPoint_PickNear; 79 times in 100, for 120 to
   179 frames) or no target (60 to 89 frames), takes the command of EntityAi_MoveCommand towards it (or the
   turn away from a push, EntityAi_TurnFromPush) and starts the move action. */
static void Beetle_ThinkWander(Entity *entity, s32 unknown) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 direction[4];
    s32 turn;

    ENTITY_FLAG_BITS(entity)->motion_step_mode = 1;
    if (entity->action_timer == 0) {
        if (rand() % 100 > 20) {
            EntityPoint_PickNear(entity->spawn_info->wander_radius, ai_state->target_point, entity->spawn_info, entity->model->position);
            entity->target = ai_state->target_point;
            entity->action_timer = rand() % 60 + 120;
        } else {
            entity->target = NULL;
            entity->action_timer = rand() % 30 + 60;
        }
    }
    turn = EntityAi_TurnFromPush(entity, direction);
    if (turn > 0) {
        entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_RIGHT);
    } else if (turn < 0) {
        entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_LEFT);
    } else {
        entity->move_command = EntityAi_MoveCommand(entity, 0, 50.0f);
    }
    EntityAi_StartAction(entity, ai_state->move, unknown);
}

/* The think function of state 2: as EntityAi_ThinkCombat with Beetle_RangedAttack0 as the
   ranged attack; it leaves flag bits 9 and 10 as they are, and closes in with command bit 0x40
   (EntityAi_MoveCommand with range 0, far marked). */
static void Beetle_ThinkCombat(Entity *entity, s32 unknown) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 direction[4];
    f32 angle;
    s32 side;
    s32 turn;

    ENTITY_FLAG_BITS(entity)->motion_step_mode = 1;
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
                EntityAi_StartAction(entity, Beetle_RangedAttack0, unknown);
            } else {
                EntityAi_StartAction(entity, EntityAi_MeleeAttack, unknown);
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
        EntityAi_ClearTurnInPlace(entity);
        if (ENTITY_FLAG_BITS(entity)->turn_in_place) {
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
                entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_RUN | MOVE_COMMAND_TURN_RIGHT);
            } else if (turn < 0) {
                entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_RUN | MOVE_COMMAND_TURN_LEFT);
            } else {
                entity->move_command = EntityAi_MoveCommand(entity, 1, 0.0f);
            }
        }
    }
done:
    EntityAi_StartAction(entity, ai_state->move, unknown);
}

/* Helper of Beetle_Animate: eases work float unk48 towards 20 degrees on the side the command
   turns to (0 with no turn bit) and puts half of it in angle unk50[1] of model parts 7 and 12. */
static void Beetle_TurnParts7And12(void *entity_arg) {
    Entity *entity = entity_arg;
    EntityAiState *ai_state = &entity->ai_state;
    EntityModelPart80 *part;
    f32 angle;
    f32 limit;
    s32 i;

    limit = 0.34906584f;
    if (entity->move_command & MOVE_COMMAND_TURN_RIGHT) {
        angle = limit;
    } else if (entity->move_command & MOVE_COMMAND_TURN_LEFT) {
        angle = -limit;
    } else {
        angle = 0.0f;
    }
    limit = 0.34906584f;
    if (angle > limit) {
        angle = limit;
    } else if (angle < -limit) {
        angle = -limit;
    }
    EntityMath_EaseAngleToward(&ai_state->part_angle_x, angle, 10.0f);
    part = (EntityModelPart80 *)entity->model->parts;
    for (i = 0; i < ENTITY_MODEL(entity)->part_count; i++) {
        if (i == 7 || i == 12) {
            part->part_angles[1] = ai_state->part_angle_x * 0.5f;
        }
        part++;
    }
}

/* Helper of Beetle_Animate: eases work float unk4C towards 20 degrees on the side the command
   turns to (0 with no turn bit) and puts minus half of it in angle unk50[1] of model parts 23, 28,
   31 and 32 (the name gives the first and the last of the four). */
static void Beetle_TurnParts23To32(Entity *entity) {
    f32 limit;
    f32 lean;
    EntityAiState *ai_state;
    EntityModelPart80 *part;
    u16 command;
    u16 part_count;
    s32 i;

    f32 limit2;
    limit = 0.34906584f;
    command = entity->move_command;
    ai_state = &entity->ai_state;
    if (command & MOVE_COMMAND_TURN_RIGHT) {
        lean = limit;
    } else {
        lean = (command & MOVE_COMMAND_TURN_LEFT) ? -limit : 0.0f;
    }
    limit2 = 0.34906584f;
    if (limit2 < lean || (limit2 = -limit2, lean < limit2)) {
        lean = limit2;
    }
    EntityMath_EaseAngleToward(&ai_state->part_angle_y, lean, 20.0f);
    part_count = ENTITY_MODEL(entity)->part_count;
    part = (EntityModelPart80 *)entity->model->parts;
    for (i = 0; i < part_count; i++) {
        if (i == 0x17 || i == 0x1C || i == 0x1F || i == 0x20) {
            part->part_angles[1] = -ai_state->part_angle_y * 0.5f;
        }
        part++;
    }
}

/* The ranged attack with record 0 (busy bit 1): animation 23; from frame EntityParams.pitch_limit_deg to
   unk432 it launches the record's shots on every frame whose number is not a multiple of
   EntityParams.action_frames. */
static void Beetle_RangedAttack0(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    EntityParamsUnk3D8 *unk3D8 = &entity->params->ranged_attacks[0].record;

    entity->busy_flags |= ENTITY_BUSY_ATTACK;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x17, 0, entity->params->motion_blend_frames >> 1, 1);
        ENTITY_FLAGS(entity) = (ENTITY_FLAGS(entity) & ~ENTITY_FLAG_MOTION_STEP_MODE_MASK) | ENTITY_FLAG_MOTION_STEP_MODE_1;
        ENTITY_FLAGS(entity) &= ~ENTITY_FLAG_LOOK_AT_TARGET;
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
        if (entity->model->level_tables->frame >= entity->params->pitch_limit_deg
            && entity->model->level_tables->frame <= entity->params->yaw_limit_deg) {
            if (ENTITY_CONTROLLER(entity)->action_frames == 0) {
                ENTITY_CONTROLLER(entity)->action_frames++;
            }
            if (frame_counter % ENTITY_CONTROLLER(entity)->action_frames != 0) {
                EntityAi_LaunchShots(entity, unk3D8, entity->params->pitch_limit_deg, entity->params->yaw_limit_deg);
            }
        }
        if (entity->model->level_tables->frame > 0 && unk3D8->attack_end_frame >= entity->model->level_tables->frame) {
            ENTITY_FLAGS(entity) |= 0x80000;
        } else {
            ENTITY_FLAGS(entity) &= ~0x80000;
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
