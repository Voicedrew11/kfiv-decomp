/* DarkSuccubus: the behaviour of entity type 54 (EntityParams.entity_type, the index into entity_type_handlers): its set-up
   function, which the table names, and the handlers it installs. Used by: Dark Succubus. The game names no
   monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by
   hit points and experience, with resistances and drops where two monsters share those. */
#include "entity/entity.h"
#include "common.h"
#include "entity/entity_world.h"

#define ENTITY_WORK_AREA_SIZE 0xA0
void EntityAi_SetUpdate(char *, s32);
int EntityAi_StartAction(Entity *, void (*)(Entity *), int);
void Motion_ClearPartSlots(EntityModel *);
s32 Motion_StepModel(ModelInstance *, s32, s32);
void Vector_Copy(void *, void *);
void EntityAi_Dormant(Entity *);
extern s32 entity_type_handlers[][2];
void EntityAi_LowerWeightsAfterHit(char *);
int EntityAi_SetReaction(Entity *, void (*)(Entity *), int);
void EntityAi_SetState(char *, u8);
void EntityAi_ReactionRewind(Entity *);
int Motion_Step(void *, int);
int Motion_BlendToPose(EntityModel *, f32);
int EntityAi_LookAtPlayer(Entity *, void *, s32, void *, f32);
extern u32 frame_counter;
extern PlayerData player_state;
extern u8 dark_succubus_fight_started[];
// match: the size is unknown; a complete small type would be read through $gp
extern f32 dark_succubus_no_floor_height_think[];
int rand(void);
void EntityAi_SensePlayer(void *, int);
void EntityAi_CheckFarFromSpawn(void *);
void EntityAi_StepState(void *);
s32 EntityAi_MoveCommand(Entity *, s32, f32);
int EntityAi_ThinkReturn(void *, int);
int EntityAi_ThinkIdle(void *, int);
void EntityAi_ThinkPursue(Entity *, s32);
f32 EntityCollision_DistanceToPropFloor(void *, void *);
void EntityEvent_StartCue(Entity *, s32, s32, s32);
s32 EntityAi_ShouldDodge(Entity *, f32, f32);
float EntityMath_AngleXZ(float *, char *);
float EntityMath_AngleDifference(float angle_from, float angle_to);
s32 EntityAi_CanMelee(Entity *);
s32 EntityAi_PickMeleeAttack(Entity *);
s32 EntityAi_PickCombatChoice(Entity *, s32);
void EntityAi_LowerChoiceWeight(Entity *, s32);
int EntityAi_TurnFromPush(void *, void *);
float EntityMath_WrapAngle(float);
s32 EntityAi_IsPushedFromFront(Entity *, f32);
int EntityAi_TurnCommand(Entity *);
int EntityAi_PickAttack(char *);
int Motion_Start(void *, int, int, u32, int);
extern f32 identity_matrix[16];
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0RotMatrixZ(float *, float *, float);
void Vector_ApplyMatrix(void *, void *, void *);
void Vector_Scale(float *, float *, float);
void EntityEvent_StartFrameCues(Entity *, s32, s32, s32);
void EntityAi_SetAttackDelay(Entity *, s32);
void EntityEvent_OnDeath(Entity *);
s32 EntityAi_CheckMeleeHit(Entity *, void *, s32, s32, s32);
int EntityShot_LaunchAimed(Entity *, EntityParamsUnk3D8 *, s32, s32, f32 *, f32, f32);
// match: the size is unknown (a complete small type would be read through $gp) and it is
// const: the store to unk54 may then be scheduled after the lui of this address
extern const f32 dark_succubus_no_floor_height_arc[];
f32 EntityMath_Approach(f32, f32, f32);
int EntityAi_AnimationFrameCount(void *, int);

static void DarkSuccubus_OnHit(Entity *entity);
static void DarkSuccubus_Animate(Entity *entity);
static void DarkSuccubus_Think(Entity *entity, s32 unknown);
static void DarkSuccubus_ThinkCombat(Entity *entity, s32 unknown);
static void DarkSuccubus_ThinkCombatCircleNear(Entity *entity, s32 unknown);
static void DarkSuccubus_ThinkCombatKeepAwayWhenFaced(Entity *entity, s32 unknown);
static void DarkSuccubus_Move(Entity *entity);
static void DarkSuccubus_ApplyMoveCommand(Entity *entity);
static void DarkSuccubus_ReactionLight(Entity *entity);
static void DarkSuccubus_ReactionHeavy(Entity *entity);
static void DarkSuccubus_Death(Entity *entity);
static void DarkSuccubus_MeleeAttack(Entity *entity);
static void DarkSuccubus_RangedAttack(Entity *entity);
static void DarkSuccubus_MoveAlongArc(Entity *entity);
static void DarkSuccubus_Dodge(Entity *entity);
static void DarkSuccubus_DodgeAnim29Or30(Entity *entity);
static void DarkSuccubus_PlayAnim18Turning(Entity *entity);
static void DarkSuccubus_FadeOut(Entity *entity);
static s32 DarkSuccubus_ShouldDodgeAnim29Or30(Entity *entity, f32 reach, f32 angle_limit);
static void DarkSuccubus_CountLiveSpawns(Entity *entity);

/* Set-up of a type 54 entity: clears its work area and installs the type's handlers. */
void DarkSuccubus_Init(Entity *entity) {
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
    ai_state->think = DarkSuccubus_Think;
    ai_state->animate = DarkSuccubus_Animate;
    ai_state->on_hit = DarkSuccubus_OnHit;
    ai_state->move = DarkSuccubus_Move;
    ai_state->dormant = EntityAi_Dormant;
    ai_state->death = DarkSuccubus_Death;
    for (i = 3; i != -1; i--) {
        entity->choice_weights[i] = 0x80;
    }
    for (i = 5; i != -1; i--) {
        entity->attack_weights[i] = 0x80;
    }
    entity->target = NULL;
    entity->flags |= ENTITY_FLAG_NO_FLOOR_LIFT;
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
    // match: called as returning an int, which keeps $2 busy after the call
    Motion_StepModel(ENTITY_MODEL(entity), 0, 0);
    entity->flags |= ENTITY_FLAG_40000;
}

/* The on_hit handler: as EntityAi_OnHit with the type's own reactions to a light and a heavy hit;
   it keeps the target, clears work word unk68, sets work word unk6A to EntityParams.far_distance and
   Entity.model_alpha to 0x80. */
static void DarkSuccubus_OnHit(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;

    EntityAi_LowerWeightsAfterHit((char *)entity);
    entity->flags &= ~ENTITY_FLAG_LOOK_AT_TARGET;
    if (((s32)(entity->flags >> 36) & 3) == 1) {
        if ((s32)(entity->flags >> 19) & 1) {
            EntityAi_SetReaction(entity, EntityAi_ReactionRewind, 0);
        } else {
            EntityAi_SetReaction(entity, DarkSuccubus_ReactionLight, 0);
        }
    } else if (((s32)(entity->flags >> 36) & 3) == 2) {
        EntityAi_SetReaction(entity, DarkSuccubus_ReactionHeavy, 0);
    }
    EntityAi_SetState((char *)entity, 2);
    ai_state->pause_timer = 0;
    ai_state->sub_timer = entity->params->far_distance;
    entity->model_alpha = 0x80;
}

/* The animate handler: as EntityAi_Animate, after DarkSuccubus_ApplyMoveCommand has carried out the
   movement command. */
static void DarkSuccubus_Animate(Entity *entity) {
    f32 limits[4];
    s32 look_at_target;
    u32 blend_frames; // match: converted to float as unsigned
    s64 flags;
    EntityAiState *ai_state;

    DarkSuccubus_ApplyMoveCommand(entity);
    flags = entity->flags;
    ai_state = &entity->ai_state;
    if (ENTITY_FLAG_BITS(entity)->bit14) {
        blend_frames = entity->model->level_tables->blend_left;
        if (blend_frames != 0) {
            ENTITY_FLAG_BITS(entity)->bit34 = Motion_BlendToPose(entity->model, (entity->busy_flags & (ENTITY_BUSY_GUARD | ENTITY_BUSY_REACTION_LIGHT)) ? ai_state->pose_blend_weight : 1.0f / (f32)blend_frames);
        } else {
            ENTITY_FLAG_BITS(entity)->bit34 = Motion_Step(entity, (s32)(flags >> 11) & 7);
        }
        look_at_target = ENTITY_FLAG_BITS(entity)->bit9;
        limits[1] = entity->params->pitch_limit_deg * 0.017453292f;
        limits[0] = -entity->params->pitch_limit_deg * 0.017453292f;
        limits[3] = entity->params->yaw_limit_deg * 0.017453292f;
        limits[2] = -entity->params->yaw_limit_deg * 0.017453292f;
        if (look_at_target) {
            EntityAi_LookAtPlayer(entity, ai_state->look_part_angles, look_at_target, limits, 12.0f);
        } else {
            EntityAi_LookAtPlayer(entity, ai_state->look_part_angles, -1, NULL, 8.0f);
        }
    }
}

/* The think handler: as EntityAi_Think, with three functions for state 2, picked by work byte unk7C
   (DarkSuccubus_CountLiveSpawns sets it): DarkSuccubus_ThinkCombat at 0, DarkSuccubus_ThinkCombatCircleNear at 1,
   DarkSuccubus_ThinkCombatKeepAwayWhenFaced above. Until the map script sets dark_succubus_fight_started it
   does nothing but set model flag 0x800, flag bits 18 and 21 and Entity.model_alpha to 0x80. With the player's x at 6600 or more it goes back to its spawn
   point instead of thinking. It adds command bit 0x10 or 0x20 by what EntityCollision_DistanceToPropFloor reads at the
   entity's position (work byte unk7A). While work word unk68 counts down, model flag 0x800 and flag
   bit 21 are set and Entity.model_alpha is 0x80; work word unk6A is the pause after it. With unk7C at 0
   the handler starts that time itself now and then, taking unkD6 down over 20 frames first. Work
   word unk6C is put to EntityParams.type_param_0 while player_state.unk259 is set and counts down. */
static void DarkSuccubus_Think(Entity *entity, s32 unknown) {
    EntityAiState *ai_state = &entity->ai_state;
    EntityAiStateKind15F8C0 *timers = (EntityAiStateKind15F8C0 *)ai_state;
    f32 height;

    if (frame_counter % 10U == entity->spawn_index % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (dark_succubus_fight_started[0] == 0) {
        entity->model->draw_flags |= 0x800;
        ((EntityFlagWord *)&entity->spawn_info->spawn_flags)->bytes[3] = 0x1E;
        entity->model_alpha = 0x80;
        ENTITY_FLAGS(entity) |= 0x200000;
        ENTITY_FLAGS(entity) |= 0x40000;
        return;
    }
    ENTITY_FLAGS(entity) &= ~0x40000;
    if (timers->think_timer > 0) {
        timers->think_timer--;
    }
    if (player_state.magic_shot_launched != 0) {
        timers->think_timer = entity->params->type_param_0;
    }
    DarkSuccubus_CountLiveSpawns(entity);
    if (entity->busy_flags == 0) {
        EntityAi_CheckFarFromSpawn(entity);
        EntityAi_StepState(entity);
        if (player_state.position[0] >= 6600.0f) {
            ENTITY_FLAGS(entity) |= 0x40000;
            ENTITY_FLAGS(entity) &= ~0x3800;
            entity->target = entity->spawn_info;
            entity->move_command = EntityAi_MoveCommand(entity, 0, 500.0f) | (MOVE_COMMAND_RUN | MOVE_COMMAND_TURN_FAST);
            EntityAi_StartAction(entity, ai_state->move, unknown);
        } else if ((s32)(ENTITY_FLAGS(entity) >> 53) & 1) {
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
                if (timers->combat_mode == 0) {
                    DarkSuccubus_ThinkCombat(entity, unknown);
                } else if (timers->combat_mode == 1) {
                    DarkSuccubus_ThinkCombatCircleNear(entity, unknown);
                } else {
                    DarkSuccubus_ThinkCombatKeepAwayWhenFaced(entity, unknown);
                }
                break;
            default:
                EntityAi_ThinkIdle(entity, unknown);
                break;
            }
        }
    }
    if (((s32)(ENTITY_FLAGS(entity) >> 28) & 1) && !(entity->busy_flags & ENTITY_BUSY_1000)) {
        height = EntityCollision_DistanceToPropFloor(entity->map_record, entity->model->position);
        if (height != dark_succubus_no_floor_height_think[0]) {
            if (height > 45.0f) {
                timers->rising = 1;
            } else if (height < 35.0f) {
                timers->rising = 0;
                timers->dipped_low = 1;
            }
            if (timers->rising != 0) {
                entity->move_command |= MOVE_COMMAND_UP;
            } else {
                entity->move_command |= MOVE_COMMAND_DOWN;
            }
        }
    }
    if (timers->combat_mode == 0) {
        if (timers->vanish_frames == 0 && timers->vanish_cooldown == 0 && timers->fading_out == 0 && rand() % 100 < 60) {
            timers->fading_out = 1;
            timers->fade_frames = 20;
        }
        if (timers->fading_out != 0) {
            if (timers->fade_frames != 0) {
                entity->model_alpha = (timers->fade_frames << 7) / 20;
                timers->fade_frames--;
            } else {
                timers->vanish_frames = entity->params->action_frames;
                timers->fading_out = 0;
                EntityEvent_StartCue(entity, 7, 0, 0);
            }
        }
    }
    if (timers->vanish_frames != 0) {
        entity->model->draw_flags |= 0x800;
        ((EntityFlagWord *)&entity->spawn_info->spawn_flags)->bytes[3] = 0x1E;
        entity->model_alpha = 0x80;
        ENTITY_FLAGS(entity) |= 0x200000;
        if (--timers->vanish_frames <= 0) {
            timers->vanish_cooldown = entity->params->far_distance;
        }
    } else {
        entity->model->draw_flags &= ~0x800;
        ((EntityFlagWord *)&entity->spawn_info->spawn_flags)->bytes[3] = 0x1F;
        ENTITY_FLAGS(entity) &= ~0x200000;
        if (timers->vanish_cooldown != 0) {
            timers->vanish_cooldown--;
        }
    }
}

/* The think function of state 2 while work byte unk7C is 0 (DarkSuccubus_ThinkCombatCircleNear and DarkSuccubus_ThinkCombatKeepAwayWhenFaced are the
   other two): starts DarkSuccubus_DodgeAnim29Or30 when DarkSuccubus_ShouldDodgeAnim29Or30 says so and DarkSuccubus_Dodge for
   the player's swing; starts DarkSuccubus_MeleeAttack when EntityAi_CanMelee finds a melee record in
   reach (EntityAi_PickMeleeAttack picks it); else, each time the timer runs out, starts
   DarkSuccubus_MoveAlongArc with the player nearer than 350 and Entity.heading_to_player within 40 degrees of
   the player's facing, or goes sideways round the player for 30 frames (work byte unk78: 2 or 3 the
   side, 1 backwards, 0 towards, 4 turning only). */
static void DarkSuccubus_ThinkCombat(Entity *entity, s32 unknown) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 direction[4];
    f32 angle;
    s32 mode;
    s32 side;

    entity->flags &= ~ENTITY_FLAG_MOTION_STEP_MODE_MASK;
    if (DarkSuccubus_ShouldDodgeAnim29Or30(entity, 600.0f, 0.5235988f)) {
        EntityAi_StartAction(entity, DarkSuccubus_DodgeAnim29Or30, 1);
        return;
    }
    if (EntityAi_ShouldDodge(entity, 300.0f, 1.0471976f)) {
        EntityAi_StartAction(entity, DarkSuccubus_Dodge, 1);
        return;
    }
    entity->flags |= ENTITY_FLAG_LOOK_AT_TARGET;
    EntityMath_AngleDifference(player_state.heading, EntityMath_AngleXZ(player_state.position, (char *)entity->model->position));
    ai_state->keep_min_distance = 100.0f;
    ai_state->keep_max_distance = 250.0f;
    if (entity->attack_delay_frames == 0 && EntityAi_CanMelee(entity)) {
        EntityAi_PickMeleeAttack(entity);
        EntityAi_StartAction(entity, DarkSuccubus_MeleeAttack, unknown);
        return;
    }
    mode = EntityAi_PickCombatChoice(entity, 0);
    if (entity->action_timer == 0) {
        entity->prev_combat_choice = entity->combat_choice;
        entity->combat_choice = mode;
        if (entity->combat_choice != entity->prev_combat_choice) {
            entity->choice_change_frame = frame_counter;
        }
        EntityAi_LowerChoiceWeight(entity, mode);
        if (entity->combat_choice == 1) {
            entity->target = &player_state;
            angle = EntityMath_AngleDifference(player_state.heading, entity->heading_to_player);
            if (entity->distance_to_player < 350.0f && ABS(angle) < 0.6981317f) {
                EntityAi_StartAction(entity, DarkSuccubus_MoveAlongArc, unknown);
                return;
            }
            if (EntityMath_AngleDifference(player_state.heading, EntityMath_AngleXZ(player_state.position, (char *)entity->model->position)) > 0.0f) {
                ai_state->move_choice = 2;
            } else {
                ai_state->move_choice = 3;
            }
            entity->action_timer = 30;
        } else {
            ai_state->move_choice = -1;
        }
    }
    switch (ai_state->move_choice) {
    case 0:
        side = EntityAi_TurnFromPush(entity, direction);
        if (side > 0) {
            entity->move_command |= (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_RIGHT);
        } else if (side < 0) {
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
        if (EntityAi_IsPushedFromFront(entity, EntityMath_WrapAngle(entity->model->rotation[1] - 1.5707964f))) {
            ai_state->move_choice = 3;
        } else {
            entity->move_command |= ((u16 (*)(Entity *))EntityAi_TurnCommand)(entity); // match: read as unsigned here
            entity->move_command |= MOVE_COMMAND_STEP_LEFT;
            if (entity->distance_to_player > ai_state->keep_max_distance) {
                entity->move_command |= MOVE_COMMAND_FORWARD;
            } else if (entity->distance_to_player < ai_state->keep_min_distance) {
                entity->move_command |= MOVE_COMMAND_BACK;
            }
        }
        break;
    case 3:
        if (EntityAi_IsPushedFromFront(entity, EntityMath_WrapAngle(entity->model->rotation[1] + 1.5707964f))) {
            ai_state->move_choice = 2;
        } else {
            entity->move_command |= ((u16 (*)(Entity *))EntityAi_TurnCommand)(entity); // match: read as unsigned here
            entity->move_command |= MOVE_COMMAND_STEP_RIGHT;
            if (entity->distance_to_player > ai_state->keep_max_distance) {
                entity->move_command |= MOVE_COMMAND_FORWARD;
            } else if (entity->distance_to_player < ai_state->keep_min_distance) {
                entity->move_command |= MOVE_COMMAND_BACK;
            }
        }
        break;
    case 4:
        entity->move_command |= EntityAi_TurnCommand(entity);
        break;
    }
    EntityAi_StartAction(entity, ai_state->move, unknown);
}

/* The think function of state 2 while one to three of the six entities DarkSuccubus_CountLiveSpawns counts are
   alive (work byte unk7C 1). It dodges as DarkSuccubus_ThinkCombat and attacks (DarkSuccubus_RangedAttack,
   DarkSuccubus_MeleeAttack) when EntityAi_PickCombatChoice chooses to; closing in it starts
   DarkSuccubus_MoveAlongArc with the player nearer than 350 and Entity.heading_to_player within 40 degrees of his facing,
   else now and then DarkSuccubus_FadeOut, else goes sideways round the player for 30 frames, keeping 100 to 250
   from him (work byte unk78). */
static void DarkSuccubus_ThinkCombatCircleNear(Entity *entity, s32 unknown) {
    EntityAiState *ai_state = &entity->ai_state;
    EntityAiStateKind15F8C0 *timers = (EntityAiStateKind15F8C0 *)ai_state;
    f32 direction[4];
    f32 angle;
    s32 mode;
    s32 side;
    s32 may_strike;

    entity->flags &= ~ENTITY_FLAG_MOTION_STEP_MODE_MASK;
    if (DarkSuccubus_ShouldDodgeAnim29Or30(entity, 600.0f, 0.5235988f)) {
        EntityAi_StartAction(entity, DarkSuccubus_DodgeAnim29Or30, 1);
        return;
    }
    if (EntityAi_ShouldDodge(entity, 300.0f, 1.0471976f)) {
        EntityAi_StartAction(entity, DarkSuccubus_Dodge, 1);
        return;
    }
    entity->flags |= ENTITY_FLAG_LOOK_AT_TARGET;
    EntityMath_AngleDifference(player_state.heading, EntityMath_AngleXZ(player_state.position, (char *)entity->model->position));
    may_strike = TRUE;
    ai_state->keep_min_distance = 100.0f;
    ai_state->keep_max_distance = 250.0f;
    mode = EntityAi_PickCombatChoice(entity, 0);
    if (entity->action_timer == 0 || mode == 0) {
        entity->prev_combat_choice = entity->combat_choice;
        entity->combat_choice = mode;
        if (entity->combat_choice != entity->prev_combat_choice) {
            entity->choice_change_frame = frame_counter;
        }
        EntityAi_LowerChoiceWeight(entity, mode);
        switch (entity->combat_choice) {
        case 0:
            if (EntityAi_PickAttack((char *)entity)) {
                EntityAi_StartAction(entity, DarkSuccubus_RangedAttack, unknown);
            } else {
                EntityAi_StartAction(entity, DarkSuccubus_MeleeAttack, unknown);
            }
            return;
        case 1:
            entity->target = &player_state;
            angle = EntityMath_AngleDifference(player_state.heading, entity->heading_to_player);
            if (may_strike && entity->distance_to_player < 350.0f && ABS(angle) < 0.6981317f) {
                EntityAi_StartAction(entity, DarkSuccubus_MoveAlongArc, unknown);
                return;
            }
            if (timers->vanish_frames == 0 && timers->vanish_cooldown == 0 && rand() % 100 < 60) {
                EntityAi_StartAction(entity, DarkSuccubus_FadeOut, unknown);
                return;
            }
            if (EntityMath_AngleDifference(player_state.heading, EntityMath_AngleXZ(player_state.position, (char *)entity->model->position)) > 0.0f) {
                ai_state->move_choice = 2;
            } else {
                ai_state->move_choice = 3;
            }
            entity->action_timer = 30;
            break;
        case 2:
            entity->target = &player_state;
            entity->action_timer = 30;
            ai_state->move_choice = 4;
            break;
        default:
            ai_state->move_choice = -1;
            break;
        }
    }
    switch (ai_state->move_choice) {
    case 0:
        side = EntityAi_TurnFromPush(entity, direction);
        if (side > 0) {
            entity->move_command |= (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_RIGHT);
        } else if (side < 0) {
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
        if (EntityAi_IsPushedFromFront(entity, EntityMath_WrapAngle(entity->model->rotation[1] - 1.5707964f))) {
            ai_state->move_choice = 3;
        } else {
            entity->move_command |= ((u16 (*)(Entity *))EntityAi_TurnCommand)(entity); // match: read as unsigned here
            entity->move_command |= MOVE_COMMAND_STEP_LEFT;
            if (entity->distance_to_player > ai_state->keep_max_distance) {
                entity->move_command |= MOVE_COMMAND_FORWARD;
            } else if (entity->distance_to_player < ai_state->keep_min_distance) {
                entity->move_command |= MOVE_COMMAND_BACK;
            }
        }
        break;
    case 3:
        if (EntityAi_IsPushedFromFront(entity, EntityMath_WrapAngle(entity->model->rotation[1] + 1.5707964f))) {
            ai_state->move_choice = 2;
        } else {
            entity->move_command |= ((u16 (*)(Entity *))EntityAi_TurnCommand)(entity); // match: read as unsigned here
            entity->move_command |= MOVE_COMMAND_STEP_RIGHT;
            if (entity->distance_to_player > ai_state->keep_max_distance) {
                entity->move_command |= MOVE_COMMAND_FORWARD;
            } else if (entity->distance_to_player < ai_state->keep_min_distance) {
                entity->move_command |= MOVE_COMMAND_BACK;
            }
        }
        break;
    case 4:
        entity->move_command |= EntityAi_TurnCommand(entity);
        break;
    }
    EntityAi_StartAction(entity, ai_state->move, unknown);
}

/* The think function of state 2 while four to six of them are alive (work byte unk7C 2): as
   DarkSuccubus_ThinkCombatCircleNear, but while the entity is within 50 degrees of the player's facing and not
   faded out (work word unk68 0) it keeps 500 to 800 from him, closing in or going backwards before it goes
   sideways, and only then may start DarkSuccubus_MoveAlongArc. */
static void DarkSuccubus_ThinkCombatKeepAwayWhenFaced(Entity *entity, s32 unknown) {
    EntityAiState *ai_state = &entity->ai_state;
    EntityAiStateKind15F8C0 *timers = (EntityAiStateKind15F8C0 *)ai_state;
    f32 direction[4];
    f32 angle;
    s32 mode;
    s32 side;
    s32 may_strike;

    entity->flags &= ~ENTITY_FLAG_MOTION_STEP_MODE_MASK;
    if (DarkSuccubus_ShouldDodgeAnim29Or30(entity, 600.0f, 0.5235988f)) {
        EntityAi_StartAction(entity, DarkSuccubus_DodgeAnim29Or30, 1);
        return;
    }
    if (EntityAi_ShouldDodge(entity, 300.0f, 1.0471976f)) {
        EntityAi_StartAction(entity, DarkSuccubus_Dodge, 1);
        return;
    }
    entity->flags |= ENTITY_FLAG_LOOK_AT_TARGET;
    angle = EntityMath_AngleDifference(player_state.heading, EntityMath_AngleXZ(player_state.position, (char *)entity->model->position));
    if (ABS(angle) > 0.87266463f || ai_state->pause_timer != 0) {
        ai_state->keep_min_distance = 100.0f;
        ai_state->keep_max_distance = 250.0f;
        may_strike = FALSE;
    } else {
        ai_state->keep_min_distance = 500.0f;
        ai_state->keep_max_distance = 800.0f;
        may_strike = TRUE;
    }
    mode = EntityAi_PickCombatChoice(entity, 0);
    if (entity->action_timer == 0 || mode == 0) {
        entity->prev_combat_choice = entity->combat_choice;
        entity->combat_choice = mode;
        if (entity->combat_choice != entity->prev_combat_choice) {
            entity->choice_change_frame = frame_counter;
        }
        EntityAi_LowerChoiceWeight(entity, mode);
        switch (entity->combat_choice) {
        case 0:
            if (EntityAi_PickAttack((char *)entity)) {
                EntityAi_StartAction(entity, DarkSuccubus_RangedAttack, unknown);
            } else {
                EntityAi_StartAction(entity, DarkSuccubus_MeleeAttack, unknown);
            }
            return;
        case 1:
            entity->target = &player_state;
            angle = EntityMath_AngleDifference(player_state.heading, entity->heading_to_player);
            if (may_strike && entity->distance_to_player < 350.0f && ABS(angle) < 0.6981317f) {
                EntityAi_StartAction(entity, DarkSuccubus_MoveAlongArc, unknown);
                return;
            }
            if (entity->distance_to_player > ai_state->keep_max_distance) {
                ai_state->move_choice = 0;
            } else if (entity->distance_to_player < ai_state->keep_min_distance) {
                ai_state->move_choice = 1;
            } else {
                if (timers->vanish_frames == 0 && timers->vanish_cooldown == 0 && rand() % 100 < 60) {
                    EntityAi_StartAction(entity, DarkSuccubus_FadeOut, unknown);
                    return;
                }
                if (EntityMath_AngleDifference(player_state.heading, EntityMath_AngleXZ(player_state.position, (char *)entity->model->position)) > 0.0f) {
                    ai_state->move_choice = 2;
                } else {
                    ai_state->move_choice = 3;
                }
            }
            entity->action_timer = 30;
            break;
        case 2:
            entity->target = &player_state;
            entity->action_timer = 30;
            ai_state->move_choice = 4;
            break;
        default:
            ai_state->move_choice = -1;
            break;
        }
    }
    switch (ai_state->move_choice) {
    case 0:
        side = EntityAi_TurnFromPush(entity, direction);
        if (side > 0) {
            entity->move_command |= (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_RIGHT);
        } else if (side < 0) {
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
        if (EntityAi_IsPushedFromFront(entity, EntityMath_WrapAngle(entity->model->rotation[1] - 1.5707964f))) {
            ai_state->move_choice = 3;
        } else {
            entity->move_command |= ((u16 (*)(Entity *))EntityAi_TurnCommand)(entity); // match: read as unsigned here
            entity->move_command |= MOVE_COMMAND_STEP_LEFT;
            if (entity->distance_to_player > ai_state->keep_max_distance) {
                entity->move_command |= MOVE_COMMAND_FORWARD;
            } else if (entity->distance_to_player < ai_state->keep_min_distance) {
                entity->move_command |= MOVE_COMMAND_BACK;
            }
        }
        break;
    case 3:
        if (EntityAi_IsPushedFromFront(entity, EntityMath_WrapAngle(entity->model->rotation[1] + 1.5707964f))) {
            ai_state->move_choice = 2;
        } else {
            entity->move_command |= ((u16 (*)(Entity *))EntityAi_TurnCommand)(entity); // match: read as unsigned here
            entity->move_command |= MOVE_COMMAND_STEP_RIGHT;
            if (entity->distance_to_player > ai_state->keep_max_distance) {
                entity->move_command |= MOVE_COMMAND_FORWARD;
            } else if (entity->distance_to_player < ai_state->keep_min_distance) {
                entity->move_command |= MOVE_COMMAND_BACK;
            }
        }
        break;
    case 4:
        entity->move_command |= EntityAi_TurnCommand(entity);
        break;
    }
    EntityAi_StartAction(entity, ai_state->move, unknown);
}

/* The move handler: only picks the animation (6 or 7 sideways, 3 forwards, 5 backwards, else 0);
   DarkSuccubus_ApplyMoveCommand moves the entity. */
static void DarkSuccubus_Move(Entity *entity) {
    if (entity == player_state.stood_on_entity) {
        if (entity->params->flags & ENTITY_PARAMS_FLAG_STOPS_WHEN_STOOD_ON) {
            entity->move_command = 0;
        }
    }
    if (entity->move_command & MOVE_COMMAND_STEP_LEFT) {
        Motion_Start(entity, 6, 0, entity->params->motion_blend_frames, 0);
        return;
    }
    if (entity->move_command & MOVE_COMMAND_STEP_RIGHT) {
        Motion_Start(entity, 7, 0, entity->params->motion_blend_frames, 0);
        return;
    }
    if (entity->move_command & MOVE_COMMAND_FORWARD) {
        Motion_Start(entity, 3, 0, entity->params->motion_blend_frames, 0);
        return;
    }
    if (entity->move_command & MOVE_COMMAND_BACK) {
        Motion_Start(entity, 5, 0, entity->params->motion_blend_frames, 0);
        return;
    }
    Motion_Start(entity, 0, 0, entity->params->motion_blend_frames, 0);
}

/* Helper of DarkSuccubus_Animate: carries out the command in Entity.move_command. It turns the entity and
   speeds up or slows down along its three axes by EntityParams.speed_step_per_frame a frame up to unk54 (work
   floats unk44, unk48 and unk4C; in y half the step and half the limit), twice as fast for command
   bit 0x40. */
static void DarkSuccubus_ApplyMoveCommand(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 turn;
    f32 step;
    f32 limit;

    if (entity == player_state.stood_on_entity) {
        entity->move_command = 0;
    }
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
    limit = entity->params->max_speed * 0.5f;
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

/* The reaction to a light hit (busy bit 2): as EntityAi_ReactionLight, but the motion never moves
   the entity and the turn rate is kept. */
static void DarkSuccubus_ReactionLight(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;

    entity->busy_flags = ENTITY_BUSY_REACTION_LIGHT;
    switch (entity->reaction_step) {
    case 0:
        EntityEvent_StartFrameCues(entity, 0xE, 0, 0);
        Motion_Start(entity, 0xE, 0, 5, 1);
        entity->flags &= ~ENTITY_FLAG_MOTION_STEP_MODE_MASK;
        entity->flags |= ENTITY_FLAG_ANIMATES;
        entity->flags &= ~ENTITY_FLAG_80000;
        entity->flags &= ~ENTITY_FLAG_GUARDING;
        entity->flags &= ~ENTITY_FLAG_LOOK_AT_TARGET;
        entity->reaction_step++;
        break;
    case 1:
        if (entity->model->level_tables->blend_left == 0) {
            entity->reaction_step++;
        }
        break;
    case 2:
        EntityAi_SetAttackDelay(entity, entity->params->attack_delay);
        EntityAi_SetReaction(entity, NULL, 0);
        entity->busy_flags = 0;
        entity->move_command = 0;
        entity->action_timer = 0;
        if ((s32)(entity->flags >> 5) & 1) {
            ai_state->think(entity, 1);
        } else {
            EntityAi_StartAction(entity, ai_state->move, 1);
        }
        break;
    }
}

/* The reaction to a heavy hit (busy bit 4): as EntityAi_ReactionHeavy, but the motion never moves
   the entity and the turn rate is kept. */
static void DarkSuccubus_ReactionHeavy(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;

    entity->busy_flags = ENTITY_BUSY_REACTION_HEAVY;
    switch (entity->reaction_step) {
    case 0:
        Motion_Start(entity, 0xF, 0, 5, 1);
        entity->flags &= ~ENTITY_FLAG_MOTION_STEP_MODE_MASK;
        entity->flags |= ENTITY_FLAG_ANIMATES;
        entity->flags &= ~ENTITY_FLAG_80000;
        entity->flags &= ~ENTITY_FLAG_GUARDING;
        entity->flags &= ~ENTITY_FLAG_LOOK_AT_TARGET;
        entity->reaction_step++;
        break;
    case 1:
        if (entity->model->level_tables->blend_left == 0) {
            entity->reaction_step++;
        }
        break;
    case 2:
        if ((s32)(entity->flags >> 34) & 1) {
            EntityAi_SetAttackDelay(entity, entity->params->attack_delay);
            EntityAi_SetReaction(entity, NULL, 0);
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            if ((s32)(entity->flags >> 5) & 1) {
                ai_state->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, ai_state->move, 1);
            }
        }
        break;
    }
}

/* The death action (busy bit 0x40): as EntityAi_Death with other flag settings; it first clears
   model flag 0x800 and work word unk68 and puts Entity.model_alpha back to 0x80. */
static void DarkSuccubus_Death(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;

    entity->busy_flags = ENTITY_BUSY_DEATH;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x10, 0, 5, 1);
        ENTITY_FLAGS(entity) &= ~0x3800;
        entity->model->draw_flags &= ~0x800;
        ai_state->pause_timer = 0;
        ((EntityFlagWord *)&entity->spawn_info->spawn_flags)->bytes[3] = 0x1F;
        entity->model_alpha = 0x80;
        ENTITY_FLAGS(entity) |= 0x4000;
        ENTITY_FLAGS(entity) |= 0x100000;
        ENTITY_FLAGS(entity) = (ENTITY_FLAGS(entity) & ~0xC) | 4;
        ENTITY_FLAGS(entity) |= 0x8000000;
        ENTITY_FLAGS(entity) |= 0x4000000;
        ENTITY_FLAGS(entity) &= ~0x10000000;
        ENTITY_FLAGS(entity) &= ~0x200;
        entity->action_step++;
    case 1:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if ((s32)(ENTITY_FLAGS(entity) >> 34) & 1) {
            ENTITY_FLAGS(entity) &= ~0x4000;
            ENTITY_FLAGS(entity) = (ENTITY_FLAGS(entity) & ~0xC) | 8;
            ENTITY_FLAGS(entity) |= 0x10;
            entity->action_timer = 20;
            EntityEvent_OnDeath(entity);
            entity->action_step++;
        }
        break;
    case 3:
        if (entity->action_timer != 0) {
            entity->model->alpha = (f32)entity->model_alpha * (f32)entity->action_timer / 20.0f;
        } else {
            ENTITY_FLAGS(entity) |= 0xC;
            ENTITY_FLAGS(entity) &= ~2;
        }
        break;
    }
}

/* The melee attack (busy bit 1): as EntityAi_MeleeAttack, but it blends in faster and, where that
   one turns the entity to the player, adds the command of EntityAi_TurnCommand. */
static void DarkSuccubus_MeleeAttack(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    EntityParamsUnk240 *unk240 = &entity->params->melee_attacks[ENTITY_FLAG_BITS(entity)->bits58];
    s32 frame;

    entity->busy_flags |= ENTITY_BUSY_ATTACK;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, ENTITY_FLAG_BITS(entity)->bits58 | 0x14, 0, entity->params->motion_blend_frames >> 2, 1);
        if (entity->params->flags & ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->bits11 = ENTITY_PARAMS_MOVEMENT_KIND(entity->params) == ENTITY_PARAMS_MOVEMENT_1 ? 2 : 1;
        }
        ENTITY_FLAG_BITS(entity)->bit54 = 0;
        ENTITY_FLAG_BITS(entity)->bit55 = 0;
        ENTITY_FLAG_BITS(entity)->bit56 = 0;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->action_step++;
    case 1:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        if (unk240->hit_start_frame != 0) {
            entity->move_command |= EntityAi_TurnCommand(entity);
        }
        return;
    case 2:
        if (unk240->hit_start_frame >= entity->model->level_tables->frame) {
            entity->move_command |= EntityAi_TurnCommand(entity);
        }
        frame = (entity->model->level_tables->frame_fixed - entity->params->motion_speeds[entity->animation_index]) >> 8;
        if (!ENTITY_FLAG_BITS(entity)->bit54) {
            ENTITY_FLAG_BITS(entity)->bit54 = EntityAi_CheckMeleeHit(entity, unk240, 0, frame, entity->model->level_tables->frame);
        }
        if (!ENTITY_FLAG_BITS(entity)->bit55) {
            ENTITY_FLAG_BITS(entity)->bit55 = EntityAi_CheckMeleeHit(entity, unk240, 1, frame, entity->model->level_tables->frame);
        }
        if (!ENTITY_FLAG_BITS(entity)->bit56) {
            ENTITY_FLAG_BITS(entity)->bit56 = EntityAi_CheckMeleeHit(entity, unk240, 2, frame, entity->model->level_tables->frame);
        }
        if (entity->model->level_tables->frame > 0 && unk240->hit_end_frame >= entity->model->level_tables->frame) {
            ENTITY_FLAG_BITS(entity)->bit19 = 1;
        } else {
            ENTITY_FLAG_BITS(entity)->bit19 = 0;
        }
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            EntityAi_SetAttackDelay(entity, unk240->attack_delay);
            if (ENTITY_FLAG_BITS(entity)->bit5) {
                ai_state->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, ai_state->move, 1);
            }
        }
        break;
    }
}

/* The ranged attack (busy bit 1): as EntityAi_RangedAttack, but it blends in faster, adds the
   command of EntityAi_TurnCommand where that one turns the entity, and aims its shots at a point 190 below
   the player's position, within 45 and 10 degrees (EntityShot_LaunchAimed). */
static void DarkSuccubus_RangedAttack(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    s32 attack_index = (s32)(entity->flags >> 58) & 3;
    EntityParamsUnk3D8 *unk3D8 = ENTITY_PARAMS_UNK3D8(entity->params, attack_index);
    f32 target[4];
    s32 elapsed;

    entity->busy_flags |= ENTITY_BUSY_ATTACK;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, attack_index + 0x17, 0, entity->params->motion_blend_frames >> 2, 1);
        if (entity->params->flags & ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE) {
            ENTITY_FLAGS(entity) &= ~0x3800;
        } else {
            ENTITY_FLAG_BITS(entity)->bits11 = (ENTITY_PARAMS_MOVEMENT_KIND(entity->params) == ENTITY_PARAMS_MOVEMENT_1) ? 2 : 1;
        }
        ENTITY_FLAGS(entity) &= ~0x200;
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
        Vector_Copy(target, player_state.position);
        target[1] -= 190.0f;
        EntityShot_LaunchAimed(entity, unk3D8, elapsed, entity->model->level_tables->frame, target, 0.7853982f, 0.17453292f);
        if (entity->model->level_tables->frame > 0 && unk3D8->attack_end_frame >= entity->model->level_tables->frame) {
            ENTITY_FLAGS(entity) |= 0x80000;
        } else {
            ENTITY_FLAGS(entity) &= ~0x80000;
        }
        if ((s32)(ENTITY_FLAGS(entity) >> 34) & 1) {
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            EntityAi_SetAttackDelay(entity, unk3D8->attack_delay);
            if ((s32)(ENTITY_FLAGS(entity) >> 5) & 1) {
                ai_state->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, ai_state->move, 1);
            }
        }
        break;
    }
}

/* An action (busy bit 0x1000) that moves the entity itself in an arc: forwards along the heading it
   starts with at up to twice EntityParams.max_speed, and in y. In animation 8 its speed in y goes to
   -20 (up: EntityCollision_DistanceToPropFloor, the distance to the floor below, grows) until EntityCollision_DistanceToPropFloor reads 400 or
   more (or nothing), then in animation 4 the speed in y goes to 20, turning to the player, until
   flag bit 44 or 50 is set, then animation 9 to its end. */
static void DarkSuccubus_MoveAlongArc(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 height;
    s32 run = FALSE;

    entity->busy_flags |= ENTITY_BUSY_1000;
    entity->flags &= ~ENTITY_FLAG_MOTION_STEP_MODE_MASK;
    entity->flags &= ~ENTITY_FLAG_LOOK_AT_TARGET;
    height = EntityCollision_DistanceToPropFloor(entity->map_record, entity->model->position);
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 8, 0, entity->params->motion_blend_frames, 1);
        ai_state->part_angle_z = EntityMath_WrapAngle(entity->model->rotation[1] + 3.1415927f);
        ai_state->pitch_speed = ai_state->part_angle_x;
        ai_state->heading_speed = 0.0f;
        ai_state->move_speed = 0.0f;
        ai_state->part_angle_x = 0.0f;
        ai_state->part_angle_y = 0.0f;
        ai_state->turn_rate = 0.0f;
        entity->action_step++;
    case 1:
        Motion_Start(entity, 8, 0, entity->params->motion_blend_frames, 0);
        ai_state->heading_speed = EntityMath_Approach(ai_state->heading_speed, entity->params->max_speed, entity->params->speed_step_per_frame);
        ai_state->pitch_speed = EntityMath_Approach(ai_state->pitch_speed, -20.0f, 2.0f);
        if (height == dark_succubus_no_floor_height_arc[0] || height >= 400.0f) {
            entity->action_step++;
        }
        break;
    case 2:
        Motion_Start(entity, 4, 0, entity->params->motion_blend_frames, 0);
        ai_state->heading_speed = EntityMath_Approach(ai_state->heading_speed, entity->params->max_speed, entity->params->speed_step_per_frame);
        ai_state->pitch_speed = EntityMath_Approach(ai_state->pitch_speed, 20.0f, 2.0f);
        if (ai_state->pitch_speed > 0.0f) {
            entity->move_command |= ((u16 (*)(Entity *))EntityAi_TurnCommand)(entity); // match: read as unsigned here
            entity->move_command |= MOVE_COMMAND_TURN_FAST;
        }
        if (entity->flags & (ENTITY_FLAG_ON_FLOOR | ENTITY_FLAG_ON_TOP)) {
            Motion_Start(entity, 9, 0, 0, 1);
            ai_state->heading_speed = ai_state->pitch_speed = 0.0f;
            entity->action_step++;
        }
        break;
    case 3:
        if ((s32)(entity->flags >> 34) & 1) {
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            if ((s32)(entity->flags >> 5) & 1) {
                entity->ai_state.think(entity, 1);
            } else {
                EntityAi_StartAction(entity, ai_state->move, 1);
            }
        }
        break;
    }
    ENTITY_SCRATCH_VECTOR[1] = ai_state->pitch_speed;
    ENTITY_SCRATCH_VECTOR[2] = ai_state->heading_speed + ai_state->heading_speed;
    ENTITY_SCRATCH_VECTOR[0] = ENTITY_SCRATCH_VECTOR[3] = 0.0f;
    sceVu0RotMatrixY(ENTITY_SCRATCH_MATRIX, identity_matrix, ai_state->part_angle_z);
    Vector_ApplyMatrix(ENTITY_SCRATCH_VECTOR, ENTITY_SCRATCH_MATRIX, ENTITY_SCRATCH_VECTOR);
    // match: never taken, but until the compiler works that out the branch separates the
    // scratchpad stores above from the loads below, so their addresses are not kept in registers
    if (run) {
        Vector_Scale(ENTITY_SCRATCH_VECTOR, ENTITY_SCRATCH_VECTOR, 2.0f);
    }
    entity->move_delta_x += ENTITY_SCRATCH_VECTOR[0];
    entity->move_delta_y += ENTITY_SCRATCH_VECTOR[1];
    entity->move_delta_z += ENTITY_SCRATCH_VECTOR[2];
}

/* The dodge action (busy bit 8): as EntityAi_Dodge; when it ends it starts
   DarkSuccubus_PlayAnim18Turning one time in two. The think functions start it for the player's swing
   (EntityAi_ShouldDodge). */
static void DarkSuccubus_Dodge(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;

    entity->busy_flags |= ENTITY_BUSY_DODGE;
    entity->flags &= ~ENTITY_FLAG_LOOK_AT_TARGET;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0xA, 0, 5, 1);
        entity->flags = (entity->flags & ~ENTITY_FLAG_MOTION_STEP_MODE_MASK) | ENTITY_FLAG_MOTION_STEP_MODE_1;
        entity->action_step++;
        break;
    case 1:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if ((s32)(entity->flags >> 34) & 1) {
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            if (rand() % 100 < 50) {
                EntityAi_StartAction(entity, DarkSuccubus_PlayAnim18Turning, 1);
            } else if ((s32)(entity->flags >> 5) & 1) {
                ai_state->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, ai_state->move, 1);
            }
        }
        break;
    }
}

/* A second dodge action (busy bit 8): plays animation 29 or 30, by the side of the player's facing
   the entity is on, to its end. The think functions start it when
   DarkSuccubus_ShouldDodgeAnim29Or30 says so, which is not a test of the player's swing. */
static void DarkSuccubus_DodgeAnim29Or30(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;

    entity->busy_flags |= ENTITY_BUSY_DODGE;
    entity->flags &= ~ENTITY_FLAG_LOOK_AT_TARGET;
    switch (entity->action_step) {
    case 0:
        if (EntityMath_AngleDifference(player_state.heading, EntityMath_AngleXZ(player_state.position, (char *)entity->model->position)) > 0.0f) {
            Motion_Start(entity, 0x1D, 0, 5, 1);
        } else {
            Motion_Start(entity, 0x1E, 0, 5, 1);
        }
        entity->flags = (entity->flags & ~ENTITY_FLAG_MOTION_STEP_MODE_MASK) | ENTITY_FLAG_MOTION_STEP_MODE_1;
        entity->action_step++;
        break;
    case 1:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if ((s32)(entity->flags >> 34) & 1) {
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            if ((s32)(entity->flags >> 5) & 1) {
                ai_state->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, ai_state->move, 1);
            }
        }
        break;
    }
}

/* An action (busy bit 0x2000): plays animation 18 to its end with flag bit 9 set and the command of
   EntityAi_TurnCommand. DarkSuccubus_Dodge starts it. */
static void DarkSuccubus_PlayAnim18Turning(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;

    entity->busy_flags |= ENTITY_BUSY_2000;
    entity->flags |= ENTITY_FLAG_LOOK_AT_TARGET;
    entity->move_command |= EntityAi_TurnCommand(entity);
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x12, 0, entity->params->motion_blend_frames >> 2, 0);
        entity->flags &= ~ENTITY_FLAG_MOTION_STEP_MODE_MASK;
        entity->action_step++;
        break;
    case 1:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if ((s32)(entity->flags >> 34) & 1) {
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            if ((s32)(entity->flags >> 5) & 1) {
                ai_state->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, ai_state->move, 1);
            }
        }
        break;
    }
}

/* An action (busy bit 0x4000) that DarkSuccubus_ThinkCombatCircleNear and DarkSuccubus_ThinkCombatKeepAwayWhenFaced start: plays animation 17 to its
   end with the turn command of EntityAi_TurnCommand; up to frame EntityParams.near_distance it takes Entity.model_alpha
   from 0x80 down to 0 (unkD6 is what EntityAi_Death's fade scales), after that it keeps work word
   unk68 at EntityParams.action_frames. */
static void DarkSuccubus_FadeOut(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;

    entity->busy_flags |= ENTITY_BUSY_4000;
    entity->move_command |= EntityAi_TurnCommand(entity);
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x11, 0, entity->params->motion_blend_frames >> 2, 0);
        entity->flags &= ~ENTITY_FLAG_MOTION_STEP_MODE_MASK;
        entity->action_step++;
        break;
    case 1:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (entity->params->near_distance >= entity->model->level_tables->frame) {
            EntityAi_AnimationFrameCount(entity, 0x11);
            entity->model_alpha = ((entity->params->near_distance - entity->model->level_tables->frame) << 7) / entity->params->near_distance;
        } else {
            ai_state->pause_timer = entity->params->action_frames;
        }
        if ((s32)(ENTITY_FLAGS(entity) >> 34) & 1) {
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            if ((s32)(ENTITY_FLAGS(entity) >> 5) & 1) {
                ai_state->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, ai_state->move, 1);
            }
        }
        break;
    }
}

/* Whether to start DarkSuccubus_DodgeAnim29Or30 (the three think functions of state 2 ask): only
   while the counter in work word unk6C runs, the entity is not busy, the player is within reach
   (less the radius and 100) and Entity.heading_to_player within angle_limit of the heading; then
   EntityParams.dodge_chance / 5 times in 100. */
static s32 DarkSuccubus_ShouldDodgeAnim29Or30(Entity *entity, f32 reach, f32 angle_limit) {
    f32 distance;
    f32 turn;

    if (entity->ai_state.mode_timer <= 0) {
        return 0;
    }
    distance = entity->distance_to_player - (entity->radius + 100.0f);
    turn = EntityMath_AngleDifference(entity->model->rotation[1], entity->heading_to_player);
    if (entity->busy_flags != 0) {
        return 0;
    }
    if (reach < distance) {
        return 0;
    }
    if (angle_limit < (0.0f <= turn ? turn : -turn)) {
        return 0;
    }
    return rand() % 100 < (s32)(entity->params->dodge_chance / 5U);
}

/* Helper of DarkSuccubus_Think: counts those of the entities with spawn ids 0x65 to 0x6A whose
   flags' low nibble is 3 and stores (count + 2) / 3 in work byte unk7C, which picks the think
   function of state 2. */
static void DarkSuccubus_CountLiveSpawns(Entity *entity) {
    EntityAiState *sub;
    Entity *object;
    int count;
    int next;
    int i;

    sub = &entity->ai_state;
    count = 0;
    i = 0;
    do {
        object = Entity_GetBySpawnId(i + 0x65);
        if (object != NULL) {
            next = count + 1;
            count = ((object->flags & (ENTITY_FLAG_ACTIVE_LIST | ENTITY_FLAG_LIVE | ENTITY_FLAG_DYING_MASK)) ^ 3) ? count : next;
        }
        i += 1;
    } while (i < 6);
    sub->action_counter = (char)((count + 2) / 3);
}
