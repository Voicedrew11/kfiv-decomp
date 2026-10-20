/* DarkSorcerer: the behaviour of entity type 53 (EntityParams.entity_type, the index into entity_type_handlers): its set-up
   function, which the table names, and the handlers it installs. Used by: Dark Sorcerer. The game names no
   monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by
   hit points and experience, with resistances and drops where two monsters share those. */
#include "entity/entity.h"
#include "common.h"
#include "map/map.h"

#define ENTITY_WORK_AREA_SIZE 0xA0
void EntityAi_SetUpdate(char *, s32);
int EntityAi_StartAction(Entity *, void (*)(Entity *), int);
void Motion_ClearPartSlots(EntityModel *);
s32 Motion_StepModel(ModelInstance *, s32, s32);
void Vector_Copy(void *, void *);
// match: the definition takes an Actor, a struct local to EntityAi_Animate.c
void EntityAi_Animate(Entity *);
extern s32 entity_type_handlers[][2];
void EntityAi_LowerWeightsAfterHit(char *);
void EntityAi_SetState(char *, u8);
int EntityAi_SetReaction(Entity *, void (*)(Entity *), int);
void EntityAi_ReactionRewind(Entity *);
void EntityAi_ReactionLight(Entity *);
void EntityAi_ReactionHeavy(Entity *);
extern u32 frame_counter;
// match: the size is unknown; a complete small type would be read through $gp
extern f32 dark_sorcerer_no_floor_height[];
void EntityAi_SensePlayer(void *, int);
void EntityAi_CheckFarFromSpawn(void *);
void EntityAi_StepState(void *);
int EntityAi_ThinkReturn(void *, int);
f32 EntityCollision_DistanceToPropFloor(void *, void *);
extern PlayerData player_state;
int rand(void);
s32 EntityAi_ShouldGuard(Entity *, f32, f32);
s32 EntityAi_ShouldDodge(Entity *, f32, f32);
s32 EntityAi_PickCombatChoice(Entity *, s32);
void EntityAi_LowerChoiceWeight(Entity *, s32);
int EntityAi_PickAttack(char *);
float EntityMath_WrapAngle(float);
float EntityMath_AngleXZ(float *, char *);
float EntityMath_AngleDifference(float, float);
s32 EntityAi_IsPushedFromFront(Entity *, f32);
int EntityAi_TurnCommand(Entity *entity);
int EntityAi_TurnFromPush(void *, void *);
s32 EntityAi_MoveCommand(Entity *, s32, f32);
void EntityAi_Dodge(Entity *);
void EntityAi_Guard(Entity *);
int Motion_Start(void *, int, int, u32, int);
extern f32 identity_matrix[16];
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0RotMatrixZ(float *, float *, float);
void Vector_ApplyMatrix(void *, void *, void *);
void Vector_Scale(float *, float *, float);
// match: the size is unknown; a complete small type would be read through $gp
extern u8 dark_sorcerer_fight_started[];
int EntityAi_CheckMeleeHit(Entity *, void *, s32, s32, s32);
void EntityAi_SetAttackDelay(Entity *, s32);
int EntityShot_LaunchAimed(Entity *, EntityParamsUnk3D8 *, s32, s32, f32 *, f32, f32);
int EntityAi_AnimationFrameCount(void *, int);
void EntityEvent_OnDeath(Entity *);
extern EntityActionWeights3 dark_sorcerer_action_weights;
int EntityAi_LaunchShot(void *, int, void *, void *);
int Sound_PlayMapAtRange(u8, u8, float *, float);
extern s32 current_map_slot;
extern MapSlot map_slots[];
void *Map_GetRecordById(int, int);
int Script_Start(int, int);

static void DarkSorcerer_OnHit(Entity *entity);
static void DarkSorcerer_Think(Entity *entity, s32 unknown);
static void DarkSorcerer_ThinkDecide(Entity *entity, s32 unknown);
static void DarkSorcerer_Move(Entity *entity);
static void DarkSorcerer_ApplyMoveCommand(Entity *entity);
static void DarkSorcerer_Dormant(Entity *entity);
static void DarkSorcerer_MeleeAttack(Entity *entity);
static void DarkSorcerer_RangedAttack(Entity *entity);
static void DarkSorcerer_Death(Entity *entity);
static s32 DarkSorcerer_PickAction(Entity *entity);
static void DarkSorcerer_PlayAnim17AndShoot(Entity *entity);
static void DarkSorcerer_LaunchShotsAtFixedPoints(Entity *entity);
static void DarkSorcerer_PlayAnim18AndShoot(Entity *entity);
static void DarkSorcerer_PlayAnim19StartScripts(Entity *entity);
static void DarkSorcerer_StartMapScripts(Entity *entity);

/* Set-up of a type 53 entity: clears its work area and installs the type's handlers. */
void DarkSorcerer_Init(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
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
    ai_state->think = DarkSorcerer_Think;
    ai_state->animate = EntityAi_Animate;
    ai_state->on_hit = DarkSorcerer_OnHit;
    ai_state->move = DarkSorcerer_Move;
    ai_state->dormant = DarkSorcerer_Dormant;
    ai_state->death = DarkSorcerer_Death;
    for (i = 3; i != -1; i--) {
        entity->choice_weights[i] = 0x80;
    }
    for (i = 5; i != -1; i--) {
        entity->attack_weights[i] = 0x80;
    }
    entity->target = NULL;
    ENTITY_FLAG_BITS(entity)->no_floor_lift = 0;
    EntityAi_StartAction(entity, ai_state->dormant, 1);
    if (unk138 != NULL) {
        Vector_Copy(ai_state->look_part_angles, unk138 + 0x50);
    }
    ai_state->pose_blend_weight = 0.5f;
    Motion_ClearPartSlots(entity->model);
    Motion_StepModel(ENTITY_MODEL(entity), 0, 0);
}

/* The on_hit handler: as EntityAi_OnHit, but it leaves the target and flag bit 10 as they are. */
static void DarkSorcerer_OnHit(Entity *entity) {
    u64 flags;
    u64 hit_strength;

    EntityAi_LowerWeightsAfterHit((char *)entity);
    entity->flags &= ~ENTITY_FLAG_LOOK_AT_TARGET;
    flags = entity->flags;
    hit_strength = (flags >> 36) & 3;
    if (hit_strength == 1) {
        if ((s32)(flags >> 19) & 1) {
            EntityAi_SetReaction(entity, EntityAi_ReactionRewind, 0);
        } else {
            EntityAi_SetReaction(entity, EntityAi_ReactionLight, 0);
        }
    } else if (hit_strength == 2) {
        EntityAi_SetReaction(entity, EntityAi_ReactionHeavy, 0);
    }
    EntityAi_SetState((char *)entity, 2);
}

/* The think handler: no states. While the entity is not busy it calls EntityAi_ThinkReturn when
   flag bit 53 is set, else DarkSorcerer_ThinkDecide. After that it keeps flag bit 28 set except in
   the death action; while the bit is set it adds command bit 0x20 or 0x10 by the height
   EntityCollision_DistanceToPropFloor reads (work byte unk7A: set above EntityParams.type_param_1 or when there is no reading,
   cleared below type_param_2); and it calls DarkSorcerer_ApplyMoveCommand while the entity is not busy or is in a hit
   reaction. Counts down work word unk6A. */
static void DarkSorcerer_Think(Entity *entity, s32 unknown) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 height;

    if (frame_counter % 10U == entity->spawn_index % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (entity->busy_flags == 0) {
        EntityAi_CheckFarFromSpawn(entity);
        EntityAi_StepState(entity);
        if (ENTITY_FLAG_BITS(entity)->far_from_spawn) {
            EntityAi_ThinkReturn(entity, unknown);
        } else {
            DarkSorcerer_ThinkDecide(entity, unknown);
        }
    }
    if (entity->busy_flags & ENTITY_BUSY_DEATH) {
        entity->flags &= ~ENTITY_FLAG_NO_FLOOR_LIFT;
    } else {
        entity->flags |= ENTITY_FLAG_NO_FLOOR_LIFT;
    }
    if (ENTITY_FLAG_BITS(entity)->no_floor_lift) {
        height = EntityCollision_DistanceToPropFloor(entity->map_record, entity->model->position);
        if (height != dark_sorcerer_no_floor_height[0]) {
            if (!((f32)entity->params->type_param_1 < height)) {
                if (height < (f32)entity->params->type_param_2) {
                    ai_state->step_index = 0;
                }
            } else {
                ai_state->step_index = 1;
            }
        } else {
            ai_state->step_index = 1;
        }
        if (ai_state->step_index != 0) {
            entity->move_command |= MOVE_COMMAND_UP;
        } else {
            entity->move_command |= MOVE_COMMAND_DOWN;
        }
    }
    if (entity->busy_flags == 0 || (entity->busy_flags & (ENTITY_BUSY_REACTION_LIGHT | ENTITY_BUSY_REACTION_HEAVY))) {
         DarkSorcerer_ApplyMoveCommand(entity);
    }
    if (ai_state->sub_timer > 0) {
        ai_state->sub_timer--;
    }
}

/* Decides for DarkSorcerer_Think: guards or dodges the player's swing and picks among attacking,
   closing in and waiting as EntityAi_ThinkCombat does, with the type's own attacks
   (DarkSorcerer_RangedAttack, DarkSorcerer_MeleeAttack). Then, unless
   DarkSorcerer_PickAction starts one of its three actions, it gives the command for the way
   of closing in kept in work byte unk78 (0 forward, 1 backwards, 2 and 3 sideways with command bits
   4 and 8, 4 turning only) and starts the move action. */
static void DarkSorcerer_ThinkDecide(Entity *entity, s32 unknown) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 direction[4];
    s32 side;
    s32 turn;

    ENTITY_FLAG_BITS(entity)->motion_step_mode = 0;
    if (EntityAi_ShouldGuard(entity, 300.0f, 0.5235988f)) {
        EntityAi_StartAction(entity, EntityAi_Guard, 1);
        return;
    }
    if (EntityAi_ShouldDodge(entity, 300.0f, 0.5235988f)) {
        EntityAi_StartAction(entity, EntityAi_Dodge, 1);
        return;
    }
    side = EntityAi_PickCombatChoice(entity, 0);
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
                EntityAi_StartAction(entity, DarkSorcerer_RangedAttack, unknown);
            } else {
                EntityAi_StartAction(entity, DarkSorcerer_MeleeAttack, unknown);
            }
            return;
        case 1:
            entity->target = &player_state;
            if ((f32)entity->params->far_distance < entity->distance_to_player) {
                ai_state->move_choice = 0;
            } else if (entity->distance_to_player < (f32)entity->params->near_distance) {
                ai_state->move_choice = 1;
            } else if (EntityMath_AngleDifference(player_state.heading,
                                     EntityMath_AngleXZ(player_state.position, (char *)entity->model->position)) > 0.0f) {
                ai_state->move_choice = 2;
            } else {
                ai_state->move_choice = 3;
            }
            entity->action_timer = entity->params->type_param_0;
            break;
        case 2:
            entity->target = &player_state;
            entity->action_timer = rand() % 30 + 30;
            ai_state->move_choice = 4;
            break;
        default:
            ai_state->move_choice = -1;
            break;
        }
    }
    if (DarkSorcerer_PickAction(entity)) {
        return;
    }
    switch (ai_state->move_choice) {
    case 0:
        turn = EntityAi_TurnFromPush(entity, direction);
        if (turn > 0) {
            entity->move_command |= (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_RIGHT);
        } else if (turn < 0) {
            entity->move_command |= (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_LEFT);
        } else {
            entity->move_command |= EntityAi_MoveCommand(entity, 0, entity->radius + 100.0f + 30.0f);
        }
        break;
    case 1:
        if (EntityAi_IsPushedFromFront(entity, EntityMath_WrapAngle(entity->model->rotation[1] + 3.1415927f))) {
            ai_state->move_choice = rand() % 3 + 2;
        } else {
            entity->move_command |= ((u16 (*)(Entity *))EntityAi_TurnCommand)(entity); // match: read as unsigned here
            entity->move_command |= MOVE_COMMAND_BACK;
        }
        break;
    case 2:
        entity->move_command |= ((u16 (*)(Entity *))EntityAi_TurnCommand)(entity); // match: read as unsigned here
        entity->move_command |= MOVE_COMMAND_STEP_LEFT;
        if ((f32)entity->params->far_distance < entity->distance_to_player) {
            entity->move_command |= MOVE_COMMAND_FORWARD;
        } else if (entity->distance_to_player < (f32)entity->params->near_distance) {
            entity->move_command |= MOVE_COMMAND_BACK;
        }
        break;
    case 3:
        entity->move_command |= ((u16 (*)(Entity *))EntityAi_TurnCommand)(entity); // match: read as unsigned here
        entity->move_command |= MOVE_COMMAND_STEP_RIGHT;
        if ((f32)entity->params->far_distance < entity->distance_to_player) {
            entity->move_command |= MOVE_COMMAND_FORWARD;
        } else if (entity->distance_to_player < (f32)entity->params->near_distance) {
            entity->move_command |= MOVE_COMMAND_BACK;
        }
        break;
    case 4:
        entity->move_command |= EntityAi_TurnCommand(entity);
        break;
    }
    EntityAi_StartAction(entity, ai_state->move, unknown);
}

/* The move handler: only picks the animation, 6 or 7 for command bits 4 and 8, 3 forward, 5
   backwards, else 0. DarkSorcerer_ApplyMoveCommand does the turning and the moving. */
static void DarkSorcerer_Move(Entity *entity) {
    u16 command = entity->move_command;

    if (command & MOVE_COMMAND_STEP_LEFT) {
        Motion_Start(entity, 6, 0, entity->params->motion_blend_frames, 0);
        return;
    }
    if (command & MOVE_COMMAND_STEP_RIGHT) {
        Motion_Start(entity, 7, 0, entity->params->motion_blend_frames, 0);
        return;
    }
    if (command & MOVE_COMMAND_FORWARD) {
        Motion_Start(entity, 3, 0, entity->params->motion_blend_frames, 0);
        return;
    }
    if (command & MOVE_COMMAND_BACK) {
        Motion_Start(entity, 5, 0, entity->params->motion_blend_frames, 0);
        return;
    }
    Motion_Start(entity, 0, 0, entity->params->motion_blend_frames, 0);
}

/* Carries out the movement command in Entity.move_command; DarkSorcerer_Think, the attacks and the three
   PlayAnim actions call it. It turns the entity and speeds up or slows down along its three axes by
   EntityParams.speed_step_per_frame a frame up to unk54 (work floats unk44, unk48 and unk4C; in y half the step
   and four fifths of the limit), twice as fast for command bit 0x40. */
static void DarkSorcerer_ApplyMoveCommand(Entity *entity) {
    EntityAiStateKind16BF98 *ai_state = (EntityAiStateKind16BF98 *)&entity->ai_state;
    f32 turn = 0.0f;
    f32 step;
    f32 limit;

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
    limit = entity->params->max_speed * 0.8f;
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
    } else if (entity->params->speed_step_per_frame < ai_state->forward_speed) {
        ai_state->forward_speed -= entity->params->speed_step_per_frame;
    } else if (ai_state->forward_speed < -entity->params->speed_step_per_frame) {
        ai_state->forward_speed += entity->params->speed_step_per_frame;
    } else {
        ai_state->forward_speed = 0.0f;
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
        ai_state->vertical_speed -= step;
        if (ai_state->vertical_speed < -limit) {
            ai_state->vertical_speed = -limit;
        }
    } else if (entity->move_command & MOVE_COMMAND_UP) {
        ai_state->vertical_speed += step;
        if (limit < ai_state->vertical_speed) {
            ai_state->vertical_speed = limit;
        }
    } else if (step < ai_state->vertical_speed) {
        ai_state->vertical_speed -= step;
    } else if (ai_state->vertical_speed < -step) {
        ai_state->vertical_speed += step;
    } else {
        ai_state->vertical_speed = 0.0f;
    }
    ENTITY_SCRATCH_VECTOR[0] = ai_state->move_speed;
    ENTITY_SCRATCH_VECTOR[1] = ai_state->vertical_speed;
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

/* The dormant action (busy bit 0x20), which the set-up function always starts: animation 0 with
   flag bits 18, 20 and 21 set and Entity.model_alpha at 0x20 until dark_sorcerer_fight_started[0] is not 0; then it brings
   unkD6 up to 0x80 over 20 frames (counted in work byte unk7B), clears the three bits and thinks.
   */
static void DarkSorcerer_Dormant(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;

    entity->busy_flags |= ENTITY_BUSY_DORMANT;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0, 0, 0, 1);
        entity->flags &= ~ENTITY_FLAG_MOTION_STEP_MODE_MASK;
        entity->flags |= ENTITY_FLAG_200000;
        entity->flags |= ENTITY_FLAG_100000;
        entity->flags |= ENTITY_FLAG_40000;
        entity->model_alpha = 0x20;
        entity->flags |= ENTITY_FLAG_NO_FLOOR_LIFT;
        ai_state->action_flags = 0;
        entity->action_step++;
    case 1:
        if (dark_sorcerer_fight_started[0] != 0) {
            entity->action_step++;
        }
        break;
    case 2:
        entity->model_alpha = ai_state->action_flags * 0x60 / 20 + 0x20;
        ai_state->action_flags++;
        if (ai_state->action_flags > 20) {
            entity->flags &= ~ENTITY_FLAG_200000;
            entity->flags &= ~ENTITY_FLAG_100000;
            entity->flags &= ~ENTITY_FLAG_40000;
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
    entity->model->alpha = entity->model_alpha;
}

/* The melee attack (busy bit 1): as EntityAi_MeleeAttack, but it turns by adding the command of
   EntityAi_TurnCommand, and calls DarkSorcerer_ApplyMoveCommand every frame. */
static void DarkSorcerer_MeleeAttack(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    s32 kind = (s32)(entity->flags >> 58) & 3;
    EntityParamsUnk240 *unk240 = &entity->params->melee_attacks[kind];
    s32 elapsed;

    entity->busy_flags |= ENTITY_BUSY_ATTACK;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, kind | 0x14, 0, entity->params->motion_blend_frames >> 1, 1);
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
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        if (unk240->hit_start_frame != 0) {
            entity->move_command |= EntityAi_TurnCommand(entity);
        }
        break;
    case 2:
        if (unk240->hit_start_frame >= entity->model->level_tables->frame) {
            entity->move_command |= EntityAi_TurnCommand(entity);
        }
        elapsed = (entity->model->level_tables->frame_fixed - entity->params->motion_speeds[entity->animation_index]) >> 8;
        if (!(ENTITY_FLAG_BITS(entity)->melee_0_hit)) {
            ENTITY_FLAG_BITS(entity)->melee_0_hit = EntityAi_CheckMeleeHit(entity, unk240, 0, elapsed, entity->model->level_tables->frame);
        }
        if (!(ENTITY_FLAG_BITS(entity)->melee_1_hit)) {
            ENTITY_FLAG_BITS(entity)->melee_1_hit = EntityAi_CheckMeleeHit(entity, unk240, 1, elapsed, entity->model->level_tables->frame);
        }
        if (!(ENTITY_FLAG_BITS(entity)->melee_2_hit)) {
            ENTITY_FLAG_BITS(entity)->melee_2_hit = EntityAi_CheckMeleeHit(entity, unk240, 2, elapsed, entity->model->level_tables->frame);
        }
        if (entity->model->level_tables->frame > 0 && unk240->hit_end_frame >= entity->model->level_tables->frame) {
            entity->flags |= ENTITY_FLAG_80000;
        } else {
            entity->flags &= ~ENTITY_FLAG_80000;
        }
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            EntityAi_SetAttackDelay(entity, unk240->attack_delay);
            if (ENTITY_FLAG_BITS(entity)->own_think) {
                ai_state->think(entity, 1);
                return;
            }
            EntityAi_StartAction(entity, ai_state->move, 1);
            return;
        }
        break;
    }
     DarkSorcerer_ApplyMoveCommand(entity);
}

/* The ranged attack (busy bit 1): as EntityAi_RangedAttack, but the shots are turned towards a
   point 190 below the player (EntityShot_LaunchAimed); it turns and moves as DarkSorcerer_MeleeAttack does.
   */
static void DarkSorcerer_RangedAttack(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    s32 kind = (s32)(ENTITY_FLAG_WORD(entity->flags) >> 58) & 3;
    EntityParamsUnk3D8 *unk3D8 = ENTITY_PARAMS_UNK3D8(entity->params, kind);
    f32 target[4];
    s32 elapsed;

    entity->busy_flags |= ENTITY_BUSY_ATTACK;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, kind + 0x17, 0, entity->params->motion_blend_frames >> 1, 1);
        if (entity->params->flags & ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE) {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = (ENTITY_PARAMS_MOVEMENT_KIND(entity->params) == ENTITY_PARAMS_MOVEMENT_1) ? 2 : 1;
        }
        ENTITY_FLAG_BITS(entity)->look_at_target = 0;
        entity->action_step++;
    case 1:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        if (unk3D8->turn_frames != 0) {
            entity->move_command |= EntityAi_TurnCommand(entity);
        }
        break;
    case 2:
        if (unk3D8->turn_frames >= entity->model->level_tables->frame) {
            entity->move_command |= EntityAi_TurnCommand(entity);
        }
        elapsed = (entity->model->level_tables->frame_fixed - entity->params->motion_speeds[entity->animation_index]) >> 8;
        Vector_Copy(target, &player_state);
        target[1] -= 190.0f;
        EntityShot_LaunchAimed(entity, unk3D8, elapsed, entity->model->level_tables->frame, target, 0.7853982f, 0.0f);
        if (entity->model->level_tables->frame > 0 && unk3D8->attack_end_frame >= entity->model->level_tables->frame) {
            entity->flags |= ENTITY_FLAG_80000;
        } else {
            entity->flags &= ~ENTITY_FLAG_80000;
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
            return;
        }
        break;
    }
     DarkSorcerer_ApplyMoveCommand(entity);
}

/* The death action (busy bit 0x40): animation 16; once it is past frame EntityParams.action_frames it
   calls EntityEvent_OnDeath and fades the model out over the rest of the motion (the frames left, kept in
   work word unk68), then removes the entity when the motion has ended. */
static void DarkSorcerer_Death(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;

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
        entity->flags &= ~ENTITY_FLAG_LOOK_AT_TARGET;
        entity->action_step++;
    case 1:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if ((s16)entity->params->action_frames < entity->model->level_tables->frame) {
            entity->action_timer = ai_state->pause_timer = EntityAi_AnimationFrameCount(entity, entity->animation_index) - entity->model->level_tables->frame;
            ENTITY_FLAG_BITS(entity)->dying = 2;
            ENTITY_FLAG_BITS(entity)->no_respawn = 1;
            EntityEvent_OnDeath(entity);
            entity->action_step++;
        }
        break;
    case 3:
        entity->model->alpha = (f32)entity->model_alpha * (f32)entity->action_timer / (f32)ai_state->pause_timer;
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            ENTITY_FLAG_BITS(entity)->animates = 0;
            ENTITY_FLAG_BITS(entity)->dying = 3;
            ENTITY_FLAG_BITS(entity)->live = 0;
        }
        break;
    }
}

/* Helper of DarkSorcerer_ThinkDecide: once the pause in work word unk6A is over, draws one of three
   actions by the weights in dark_sorcerer_action_weights (the third weight is 90 while the player is outside a fixed
   rectangle) and starts it: DarkSorcerer_PlayAnim19StartScripts, DarkSorcerer_PlayAnim18AndShoot or
   DarkSorcerer_PlayAnim17AndShoot. Returns whether it started one. */
static s32 DarkSorcerer_PickAction(Entity *entity) {
    EntityActionWeights3 chances = dark_sorcerer_action_weights;
    s32 choice;
    s32 total;
    s32 i;
    s32 roll;
    s32 sum;

    if (entity->ai_state.sub_timer > 0) {
        return FALSE;
    }
    if (player_state.position[0] < -16900.0f || player_state.position[0] > -16500.0f || player_state.position[2] < 2500.0f
        || player_state.position[2] > 3600.0f) {
        chances.weights[2] = 90;
    }
    total = chances.weights[0] + chances.weights[1] + chances.weights[2];
    roll = rand() % total;
    choice = 0;
    sum = 0;
    for (i = 0; i < 3; i++) {
        sum += chances.weights[i];
        if (roll < sum) {
            choice = i;
            break;
        }
    }
    if (choice == 0) {
        EntityAi_StartAction(entity, DarkSorcerer_PlayAnim19StartScripts, 1);
    } else if (choice == 1) {
        EntityAi_StartAction(entity, DarkSorcerer_PlayAnim18AndShoot, 1);
    } else {
        EntityAi_StartAction(entity, DarkSorcerer_PlayAnim17AndShoot, 1);
    }
    return TRUE;
}

/* An action (busy bit 0x1000) that DarkSorcerer_PickAction starts: plays animation 17 to its
   end; at frame 40 it calls DarkSorcerer_LaunchShotsAtFixedPoints and sets the pause, work word unk6A, to
   90. Calls DarkSorcerer_ApplyMoveCommand every frame. */
static void DarkSorcerer_PlayAnim17AndShoot(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;

    entity->busy_flags |= ENTITY_BUSY_1000;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x11, 0, entity->params->motion_blend_frames >> 1, 1);
        entity->flags &= ~ENTITY_FLAG_MOTION_STEP_MODE_MASK;
        entity->flags &= ~ENTITY_FLAG_LOOK_AT_TARGET;
        entity->action_step++;
    case 1:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (entity->model->level_tables->frame >= 0x28) {
            DarkSorcerer_LaunchShotsAtFixedPoints(entity);
            ai_state->sub_timer = 0x5A;
            entity->action_step++;
        }
        break;
    case 3:
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            if (ENTITY_FLAG_BITS(entity)->own_think) {
                ai_state->think(entity, 1);
                return;
            }
            EntityAi_StartAction(entity, ai_state->move, 1);
            return;
        }
        break;
    }
     DarkSorcerer_ApplyMoveCommand(entity);
}

/* Helper of DarkSorcerer_PlayAnim17AndShoot: launches shot 0xCA from 18 fixed points along the four sides
   of a rectangle (EntityAi_LaunchShot) and calls Sound_PlayMapAtRange with sound 0x42 at five fixed points. */
static void DarkSorcerer_LaunchShotsAtFixedPoints(Entity *entity) {
    f32 position[4];
    f32 direction[4];
    s32 i;

    position[0] = -17300.0f;
    position[1] = 2930.0f;
    position[2] = 1800.0f;
    position[3] = 0.0f;
    direction[0] = 0.0f;
    direction[1] = 0.0f;
    direction[2] = 0.0f;
    direction[3] = 0.0f;
    for (i = -17300; i <= -16100; i += 400) {
        position[0] = i;
        EntityAi_LaunchShot(entity, 0xCA, position, direction);
    }
    position[0] = -17300.0f;
    position[1] = 2930.0f;
    position[2] = 4200.0f;
    position[3] = 0.0f;
    direction[0] = 0.0f;
    direction[1] = 3.1415927f;
    direction[2] = 0.0f;
    direction[3] = 0.0f;
    for (i = -17300; i <= -16100; i += 400) {
        position[0] = i;
        EntityAi_LaunchShot(entity, 0xCA, position, direction);
    }
    position[0] = -17700.0f;
    position[1] = 2830.0f;
    position[2] = 2000.0f;
    position[3] = 0.0f;
    direction[0] = 0.0f;
    direction[1] = 1.5707964f;
    direction[2] = 0.0f;
    direction[3] = 0.0f;
    for (i = 2000; i <= 4000; i += 400) {
        position[2] = i;
        EntityAi_LaunchShot(entity, 0xCA, position, direction);
    }
    position[0] = -15700.0f;
    position[1] = 2830.0f;
    position[2] = 3200.0f;
    position[3] = 0.0f;
    direction[0] = 0.0f;
    direction[1] = -1.5707964f;
    direction[2] = 0.0f;
    direction[3] = 0.0f;
    for (i = 3200; i <= 4000; i += 400) {
        position[2] = i;
        EntityAi_LaunchShot(entity, 0xCA, position, direction);
    }
    position[0] = -15700.0f;
    position[1] = 2830.0f;
    position[2] = 2000.0f;
    position[3] = 0.0f;
    EntityAi_LaunchShot(entity, 0xCA, position, direction);
    position[0] = -15700.0f;
    position[1] = 2830.0f;
    position[2] = 3600.0f;
    position[3] = 0.0f;
    Sound_PlayMapAtRange(0x42, 0, position, 2000.0f);
    position[0] = -16900.0f;
    position[1] = 2930.0f;
    position[2] = 4200.0f;
    position[3] = 0.0f;
    Sound_PlayMapAtRange(0x42, 0, position, 2000.0f);
    position[0] = -17700.0f;
    position[1] = 2830.0f;
    position[2] = 3200.0f;
    position[3] = 0.0f;
    Sound_PlayMapAtRange(0x42, 0, position, 2000.0f);
    position[0] = -17300.0f;
    position[1] = 2930.0f;
    position[2] = 2200.0f;
    position[3] = 0.0f;
    Sound_PlayMapAtRange(0x42, 0, position, 2000.0f);
    position[0] = -15700.0f;
    position[1] = 2830.0f;
    position[2] = 2000.0f;
    position[3] = 0.0f;
    Sound_PlayMapAtRange(0x42, 0, position, 2000.0f);
}

/* An action (busy bit 0x2000) that DarkSorcerer_PickAction starts: plays animation 18 to its
   end; at frame 16 it launches shot 0xC9 from the entity's position along the model's angles
   (EntityAi_LaunchShot) and sets the pause, work word unk6A, to 90. Calls DarkSorcerer_ApplyMoveCommand every frame. */
static void DarkSorcerer_PlayAnim18AndShoot(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;

    entity->busy_flags |= ENTITY_BUSY_2000;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x12, 0, entity->params->motion_blend_frames >> 1, 1);
        entity->flags &= ~ENTITY_FLAG_MOTION_STEP_MODE_MASK;
        entity->flags &= ~ENTITY_FLAG_LOOK_AT_TARGET;
        entity->action_step++;
    case 1:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (entity->model->level_tables->frame >= 0x10) {
            EntityAi_LaunchShot(entity, 0xC9, entity->model->position, entity->model->rotation);
            ai_state->sub_timer = 0x5A;
            entity->action_step++;
        }
        break;
    case 3:
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            if (ENTITY_FLAG_BITS(entity)->own_think) {
                ai_state->think(entity, 1);
                return;
            }
            EntityAi_StartAction(entity, ai_state->move, 1);
            return;
        }
        break;
    }
     DarkSorcerer_ApplyMoveCommand(entity);
}

/* An action (busy bit 0x4000) that DarkSorcerer_PickAction starts: plays animation 19 to its
   end; at frame 24 it calls DarkSorcerer_StartMapScripts and sets the pause, work word unk6A, to
   90. Calls DarkSorcerer_ApplyMoveCommand every frame. */
static void DarkSorcerer_PlayAnim19StartScripts(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;

    entity->busy_flags |= ENTITY_BUSY_4000;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x13, 0, entity->params->motion_blend_frames >> 1, 1);
        entity->flags &= ~ENTITY_FLAG_MOTION_STEP_MODE_MASK;
        entity->flags &= ~ENTITY_FLAG_LOOK_AT_TARGET;
        entity->action_step++;
    case 1:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (entity->model->level_tables->frame >= 0x18) {
            DarkSorcerer_StartMapScripts(entity);
            ai_state->sub_timer = 0x5A;
            entity->action_step++;
        }
        break;
    case 3:
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            if (ENTITY_FLAG_BITS(entity)->own_think) {
                ai_state->think(entity, 1);
                return;
            }
            EntityAi_StartAction(entity, ai_state->move, 1);
            return;
        }
        break;
    }
     DarkSorcerer_ApplyMoveCommand(entity);
}

/* Helper of DarkSorcerer_PlayAnim19StartScripts: when the entity's map is the current one, starts script
   unk4E + 10000 of the map record player_state.unk252 picks, and of up to two of the records
   Map_GetRecordById gives for 0x6B to 0x88, drawn at random. */
static void DarkSorcerer_StartMapScripts(Entity *entity) {
    MapSlot *slot = &map_slots[current_map_slot];
    MapRecord *record = &((MapRecord *)slot->records)[player_state.map_record_index];
    s32 remaining;
    s32 chance;
    s32 i;
    s32 slots_left;
    s32 roll_range;

    if (record->prop_id != 0 && entity->group_id == current_map_slot) {
        Script_Start(record->prop_id + 10000, 0);
    }
    slots_left = 0x1D;
    i = 0;
    remaining = 2;
    roll_range = 100;
    chance = 200;
    for (; i < 0x1E; i++) {
        if (rand() % roll_range < chance / slots_left) {
            record = Map_GetRecordById(entity->group_id, i + 0x6B);
            if (record != NULL && record->prop_id != 0 && entity->group_id == current_map_slot) {
                chance -= 100;
                remaining--;
                Script_Start(record->prop_id + 10000, 0);
            }
        }
        if (remaining <= 0) {
            break;
        }
        slots_left--;
        if (slots_left <= 0) {
            break;
        }
    }
}
