/* DarkKnight: the behaviour of entity type 64 (EntityParams.entity_type, the index into entity_type_handlers): its set-up
   function, which the table names, and the handlers it installs. Used by: Dark Knight. The game names no
   monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by
   hit points and experience, with resistances and drops where two monsters share those. */
#include "entity/entity.h"
#include "common.h"

#define ENTITY_WORK_AREA_SIZE 0xA0
void EntityAi_SetUpdate(char *, s32);
int EntityAi_StartAction(Entity *, void (*)(Entity *), int);
void Motion_ClearPartSlots(EntityModel *);
s32 Motion_StepModel(ModelInstance *, s32, s32);
void Vector_Copy(void *, void *);
// match: the definition takes an Actor, a struct local to EntityAi_Animate.c
void EntityAi_Animate(Entity *);
void EntityAi_Move(Entity *);
void EntityAi_Dormant(Entity *);
void EntityAi_Death(Entity *);
extern s32 entity_type_handlers[][2];
void EntityAi_LowerWeightsAfterHit(char *);
void EntityAi_SetState(char *, u8);
int EntityAi_SetReaction(Entity *, void (*)(Entity *), int);
void EntityAi_ReactionHeavy(Entity *);
extern u32 frame_counter;
extern PlayerData player_state;
int rand(void);
float sqrtf(float);
float atan2f(float, float);
char *Model_GetSkinnedBonePoint(char *, u32);
void EntityEvent_StartEffectAtPoint(char **, int, u32, int);
void EntityAi_SensePlayer(void *, int);
void EntityAi_CheckFarFromSpawn(void *);
void EntityAi_StepState(void *);
int EntityAi_ThinkReturn(void *, int);
int EntityAi_ThinkIdle(void *, int);
void EntityAi_ThinkPursue(Entity *, s32);
s32 EntityAi_ShouldDodge(Entity *, f32, f32);
s32 EntityAi_PickCombatChoice(Entity *, s32);
void EntityAi_LowerChoiceWeight(Entity *, s32);
int EntityAi_PickAttack(char *);
void EntityAi_ClearTurnInPlace(Entity *);
int EntityAi_TurnCommand(Entity *);
int EntityAi_TurnFromPush(void *, void *);
s32 EntityAi_MoveCommand(Entity *, s32, f32);
float EntityMath_AngleDifference(float, float);
float EntityMath_WrapAngle(float);
void EntityAi_Dodge(Entity *);
void EntityAi_MeleeAttack(Entity *);
void EntityAi_RangedAttack(Entity *);
int Motion_Start(void *, int, int, u32, int);
s32 EntityAi_TurnToHeading(Entity *, f32 *, f32);
s32 EntityAi_CheckMeleeHitAt(Entity *, void *, s32, s32, s32, f32);
void EntityAi_SetAttackDelay(Entity *, s32);
int EntityAi_LaunchShots(Entity *, EntityParamsUnk3D8 *, s32, s32);
int EntityShot_Launch(Entity *, EntityParamsUnk3D8 *, s32, s32, f32 *);
s32 EntityAi_CanMelee(Entity *);
s32 EntityAi_CanShoot(Entity *);
s32 EntitySight_EntityInWay(Entity *);
s32 EntityAi_CanUseMelee(Entity *, s32);
s32 EntityAi_CanUseRanged(Entity *, s32);
void EntityAi_LowerAttackWeight(Entity *);

static void DarkKnight_OnHit(Entity *entity);
static void DarkKnight_Think(Entity *entity, s32 unknown);
static void DarkKnight_ThinkCombat(Entity *entity, s32 unknown);
static void DarkKnight_PlayTurnAnim(Entity *entity);
static void DarkKnight_HoldGuardPose(Entity *entity);
static void DarkKnight_GuardThenMeleeAttack(Entity *entity);
static void DarkKnight_MeleeAttack2(Entity *entity);
static void DarkKnight_RangedAttack0(Entity *entity);
static void DarkKnight_RangedAttack1(Entity *entity);
static s32 DarkKnight_ShouldRangedAttack0(Entity *entity);
static s32 DarkKnight_ShouldHoldAnim13(Entity *entity, f32 min_distance, f32 max_angle);
static s32 DarkKnight_PickCombatChoice(Entity *entity, s32 mode);
static s32 DarkKnight_PickAttack(Entity *entity);

/* Set-up of a type 64 entity: clears its work area and installs the type's handlers. */
void DarkKnight_Init(Entity *entity) {
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
    ai_state->think = DarkKnight_Think;
    ai_state->animate = EntityAi_Animate;
    ai_state->on_hit = DarkKnight_OnHit;
    ai_state->move = EntityAi_Move;
    ai_state->dormant = EntityAi_Dormant;
    ai_state->death = EntityAi_Death;
    for (i = 3; i != -1; i--) {
        entity->choice_weights[i] = 0x80;
    }
    for (i = 5; i != -1; i--) {
        entity->attack_weights[i] = 0x80;
    }
    entity->target = NULL;
    entity->flags &= ~ENTITY_FLAG_NO_FLOOR_LIFT;
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
    Motion_StepModel(ENTITY_MODEL(entity), 0, 0);
}

/* The on_hit handler: as EntityAi_OnHit, but only a heavy hit starts a reaction
   (EntityAi_ReactionHeavy); a light hit only puts the entity in state 2. */
static void DarkKnight_OnHit(Entity *entity) {
    if (ENTITY_FLAG_BITS(entity)->hit_strength == 2) {
        EntityAi_LowerWeightsAfterHit((char *)entity);
        entity->target = NULL;
        ENTITY_FLAG_BITS(entity)->attacking = FALSE;
        ENTITY_FLAG_BITS(entity)->look_at_target = FALSE;
        EntityAi_SetReaction(entity, EntityAi_ReactionHeavy, 0);
    }
    EntityAi_SetState((char *)entity, 2);
}

/* The think handler: as EntityAi_Think, with DarkKnight_ThinkCombat for state 2. Before that,
   every frame: it starts effect 0x3D7 at model points 30 and 31, pointing to points 28 and 29;
   while work word unk68 counts down (DarkKnight_RangedAttack0 sets it), for the first
   EntityParams.near_distance frames it starts one of effects 0x21F to 0x221 at point 4 and writes
   EntityParams.far_distance into unk10[n].unk00.unk00[8] of the three melee records, and 0 once the count
   is over; it counts down work word unk6C, set to EntityParams.type_param_0 while player_state.unk259 is
   not 0; and while work word unk6A counts down it keeps flag bit 35 set, with effect 0x25C every
   third frame. */
static void DarkKnight_Think(Entity *entity, s32 unknown) {
    EntityAiState *ai_state = &entity->ai_state;
    EntityAiStateKind162BC8 *aims = (EntityAiStateKind162BC8 *)ai_state;
    f32 delta[4];
    f32 *from;
    f32 *tip;
    f32 *first_aim;
    f32 *second_aim;
    u16 frames;

    if (frame_counter % 10U == entity->spawn_index % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    ai_state->pose_blend_weight = 0.5f;
    first_aim = aims->first_aim;
    second_aim = aims->second_aim;
    from = (f32 *)Model_GetSkinnedBonePoint((char *)entity->model, 0x1E);
    tip = (f32 *)Model_GetSkinnedBonePoint((char *)entity->model, 0x1C);
    delta[0] = tip[0] - from[0];
    delta[1] = tip[1] - from[1];
    delta[2] = tip[2] - from[2];
    first_aim[0] = atan2f(-delta[1], sqrtf(delta[0] * delta[0] + delta[2] * delta[2]));
    first_aim[1] = atan2f(delta[0], delta[2]);
    first_aim[2] = 0.0f;
    from = (f32 *)Model_GetSkinnedBonePoint((char *)entity->model, 0x1F);
    tip = (f32 *)Model_GetSkinnedBonePoint((char *)entity->model, 0x1D);
    delta[0] = tip[0] - from[0];
    delta[1] = tip[1] - from[1];
    delta[2] = tip[2] - from[2];
    second_aim[0] = atan2f(-delta[1], sqrtf(delta[0] * delta[0] + delta[2] * delta[2]));
    second_aim[1] = atan2f(delta[0], delta[2]);
    second_aim[2] = 0.0f;
    EntityEvent_StartEffectAtPoint((char **)entity, 0x3D7, 0x1E, (int)first_aim);
    EntityEvent_StartEffectAtPoint((char **)entity, 0x3D7, 0x1F, (int)second_aim);
    if (ai_state->pause_timer > 0) {
        if (ai_state->pause_timer > (s16)entity->params->action_frames - entity->params->near_distance) {
            from = (f32 *)Model_GetSkinnedBonePoint((char *)entity->model, 4);
            tip = (f32 *)Model_GetSkinnedBonePoint((char *)entity->model, 5);
            delta[0] = tip[0] - from[0];
            delta[1] = tip[1] - from[1];
            delta[2] = tip[2] - from[2];
            aims->effect_angles[0] = atan2f(-delta[1], sqrtf(delta[0] * delta[0] + delta[2] * delta[2]));
            aims->effect_angles[1] = atan2f(delta[0], delta[2]);
            aims->effect_angles[2] = 0.0f;
            EntityEvent_StartEffectAtPoint((char **)entity, rand() % 3 + 0x21F, 4, (int)aims->effect_angles);
            frames = entity->params->far_distance;
            entity->params->melee_attacks[0].hit_parts[0].strengths.element_strengths[8] = frames;
            entity->params->melee_attacks[0].hit_parts[1].strengths.element_strengths[8] = frames;
            entity->params->melee_attacks[0].hit_parts[2].strengths.element_strengths[8] = frames;
            entity->params->melee_attacks[1].hit_parts[0].strengths.element_strengths[8] = frames;
            entity->params->melee_attacks[1].hit_parts[1].strengths.element_strengths[8] = frames;
            entity->params->melee_attacks[1].hit_parts[2].strengths.element_strengths[8] = frames;
            ((EntityParamsUnk240 *)entity->params->melee_attack_third)->hit_parts[0].strengths.element_strengths[8] = frames;
            ((EntityParamsUnk240 *)entity->params->melee_attack_third)->hit_parts[1].strengths.element_strengths[8] = frames;
            ((EntityParamsUnk240 *)entity->params->melee_attack_third)->hit_parts[2].strengths.element_strengths[8] = frames;
        }
        ai_state->pause_timer--;
    } else {
        entity->params->melee_attacks[0].hit_parts[0].strengths.element_strengths[8] = 0;
        entity->params->melee_attacks[0].hit_parts[1].strengths.element_strengths[8] = 0;
        entity->params->melee_attacks[0].hit_parts[2].strengths.element_strengths[8] = 0;
        entity->params->melee_attacks[1].hit_parts[0].strengths.element_strengths[8] = 0;
        entity->params->melee_attacks[1].hit_parts[1].strengths.element_strengths[8] = 0;
        entity->params->melee_attacks[1].hit_parts[2].strengths.element_strengths[8] = 0;
        ((EntityParamsUnk240 *)entity->params->melee_attack_third)->hit_parts[0].strengths.element_strengths[8] = 0;
        ((EntityParamsUnk240 *)entity->params->melee_attack_third)->hit_parts[1].strengths.element_strengths[8] = 0;
        ((EntityParamsUnk240 *)entity->params->melee_attack_third)->hit_parts[2].strengths.element_strengths[8] = 0;
    }
    if (ai_state->mode_timer > 0) {
        ai_state->mode_timer--;
    }
    if (player_state.magic_shot_launched != 0) {
        ai_state->mode_timer = entity->params->type_param_0;
    }
    if (ai_state->sub_timer > 0) {
        if (frame_counter % 3U == 0) {
            EntityEvent_StartEffectAtPoint((char **)entity, 0x25C, 0, (int)entity->model->rotation);
        }
        ENTITY_FLAGS(entity) |= 0x8000L << 20;
        ai_state->sub_timer--;
    } else {
        ENTITY_FLAGS(entity) &= ~(0x8000L << 20);
    }
    if (entity->busy_flags == 0) {
        EntityAi_CheckFarFromSpawn(entity);
        EntityAi_StepState(entity);
        if (ENTITY_FLAG_BITS(entity)->far_from_spawn) {
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
                DarkKnight_ThinkCombat(entity, unknown);
                break;
            default:
                EntityAi_ThinkIdle(entity, unknown);
                break;
            }
        }
    }
}

/* The think function of state 2: as EntityAi_ThinkCombat, with these changes. In place of the guard
   it starts DarkKnight_HoldGuardPose when DarkKnight_ShouldHoldAnim13 says so; it starts
   DarkKnight_RangedAttack0 whenever DarkKnight_ShouldRangedAttack0 says so; ranged index 1 has
   DarkKnight_RangedAttack1 and melee index 3 DarkKnight_MeleeAttack2; closing in from more than
   500 away adds command bits 0x40 and 0x400; and with Entity.heading_to_player more than 90 degrees off the
   heading it may start DarkKnight_PlayTurnAnim (work byte unk7B: the side). */
static void DarkKnight_ThinkCombat(Entity *entity, s32 unknown) {
    EntityAiState *ai_state = &entity->ai_state;
    EntityAiStateKind162BC8 *work = (EntityAiStateKind162BC8 *)ai_state;
    f32 direction[4];
    f32 angle;
    s32 mode;
    s32 side;

    entity->flags = (entity->flags & ~ENTITY_FLAG_MOTION_STEP_MODE_MASK) | ENTITY_FLAG_MOTION_STEP_MODE_1;
    if (DarkKnight_ShouldHoldAnim13(entity, 200.0f, 2.443461f)) {
        EntityAi_StartAction(entity, DarkKnight_HoldGuardPose, 1);
        return;
    }
    if (EntityAi_ShouldDodge(entity, 300.0f, 0.5235988f)) {
        EntityAi_StartAction(entity, EntityAi_Dodge, 1);
        return;
    }
    if (DarkKnight_ShouldRangedAttack0(entity)) {
        EntityAi_StartAction(entity, DarkKnight_RangedAttack0, unknown);
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
            if (EntityAi_PickAttack((char *)entity)) {
                if (ENTITY_FLAG_BITS(entity)->attack_index == 1) {
                    EntityAi_StartAction(entity, DarkKnight_RangedAttack1, unknown);
                } else {
                    EntityAi_StartAction(entity, EntityAi_RangedAttack, unknown);
                }
            } else {
                if (ENTITY_FLAG_BITS(entity)->attack_index == 3) {
                    EntityAi_StartAction(entity, DarkKnight_MeleeAttack2, unknown);
                } else {
                    EntityAi_StartAction(entity, EntityAi_MeleeAttack, unknown);
                }
            }
            return;
        case 1:
            entity->target = &player_state;
            ai_state->move_choice = 0;
            if (entity->distance_to_player > 500.0f) {
                entity->action_timer = rand() % 30 + 60;
                work->should_run = 1;
            } else {
                entity->action_timer = rand() % 30 + 60;
                work->should_run = 0;
            }
            break;
        case 2:
        default:
            ai_state->move_choice = -1;
            break;
        }
    }
    if (ai_state->move_choice == 0) {
        side = EntityAi_TurnFromPush(entity, direction);
        EntityAi_ClearTurnInPlace(entity);
        if (ENTITY_FLAG_BITS(entity)->turn_in_place) {
            angle = EntityMath_AngleDifference(entity->model->rotation[1], entity->heading_to_player);
            if (ai_state->turn_in_place == 0) {
                ai_state->turn_in_place = ABS(angle) > 0.5235988f;
            }
            if (ai_state->turn_in_place != 0) {
                entity->move_command = EntityAi_TurnCommand(entity);
                if (entity->move_command == 0) {
                    ai_state->turn_in_place = 0;
                }
            }
        } else if (work->should_run != 0) {
            if (side > 0) {
                entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_RUN | MOVE_COMMAND_TURN_RIGHT);
            } else if (side < 0) {
                entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_RUN | MOVE_COMMAND_TURN_LEFT);
            } else {
                if (ABS(EntityMath_WrapAngle(entity->heading_to_player - entity->model->rotation[1])) > 1.5707964f
                    && frame_counter % 15U == 0 && !(rand() & 1)) {
                    ai_state->action_flags = (EntityMath_WrapAngle(entity->heading_to_player - entity->model->rotation[1]) > 0.0f) ? 1 : -1;
                    EntityAi_StartAction(entity, DarkKnight_PlayTurnAnim, unknown);
                    return;
                }
                entity->move_command = EntityAi_MoveCommand(entity, 1, entity->radius + 100.0f + 30.0f);
            }
            entity->move_command |= MOVE_COMMAND_TURN_FAST;
        } else {
            if (side > 0) {
                entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_RIGHT);
            } else if (side < 0) {
                entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_LEFT);
            } else {
                if (ABS(EntityMath_WrapAngle(entity->heading_to_player - entity->model->rotation[1])) > 1.5707964f
                    && frame_counter % 15U == 0 && !(rand() & 1)) {
                    ai_state->action_flags = (EntityMath_WrapAngle(entity->heading_to_player - entity->model->rotation[1]) > 0.0f) ? 1 : -1;
                    EntityAi_StartAction(entity, DarkKnight_PlayTurnAnim, unknown);
                    return;
                }
                entity->move_command = EntityAi_MoveCommand(entity, 0, entity->radius + 100.0f + 30.0f);
            }
        }
    }
    EntityAi_StartAction(entity, ai_state->move, unknown);
}

/* An action (busy bit 0x1000) that DarkKnight_ThinkCombat starts: plays animation 12 or 11, by
   the sign of work byte unk7B, to its end. */
static void DarkKnight_PlayTurnAnim(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;

    entity->busy_flags |= ENTITY_BUSY_1000;
    switch (entity->action_step) {
    case 0:
        if (ai_state->action_flags > 0) {
            Motion_Start(entity, 0xC, 0, entity->params->motion_blend_frames, 1);
        } else {
            Motion_Start(entity, 0xB, 0, entity->params->motion_blend_frames, 1);
        }
        entity->flags = (entity->flags & ~ENTITY_FLAG_MOTION_STEP_MODE_MASK) | ENTITY_FLAG_MOTION_STEP_MODE_1;
        entity->flags &= ~ENTITY_FLAG_LOOK_AT_TARGET;
        entity->action_step++;
    case 1:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
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

/* An action (busy bit 0x10) that DarkKnight_ThinkCombat starts when DarkKnight_ShouldHoldAnim13 says so: as
   EntityAi_Guard (animation 13 held for EntityParams.guard_frames frames with the motion stopped), but without flag
   35, so it adds no defence and a hit still starts a reaction; once the motion has blended in it puts work word
   unk6A to EntityParams.type_param_1, the pause before the next. */
static void DarkKnight_HoldGuardPose(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;

    entity->busy_flags |= ENTITY_BUSY_GUARD;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0xD, 0, 0xA, 1);
        ENTITY_FLAGS(entity) = (ENTITY_FLAGS(entity) & ~ENTITY_FLAG_MOTION_STEP_MODE_MASK) | ENTITY_FLAG_MOTION_STEP_MODE_1;
        ai_state->turn_rate = 0.0f;
        ENTITY_FLAGS(entity) &= ~ENTITY_FLAG_LOOK_AT_TARGET;
        entity->action_step++;
        break;
    case 1:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_timer = entity->params->guard_frames;
            ENTITY_FLAGS(entity) &= ~ENTITY_FLAG_ANIMATES;
            ai_state->sub_timer = entity->params->type_param_1;
            entity->action_step++;
        }
        break;
    case 2:
        if (entity->action_timer == 0) {
            ENTITY_FLAGS(entity) |= ENTITY_FLAG_ANIMATES;
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

/* A melee attack (busy bit 1) that nothing starts. With Entity.heading_to_player within 30 degrees of the heading it plays
   animation 17 (record 0), 30 to 90 degrees to one side 18 or 19 (record 2), guarding (flag 35) until the
   strike has blended in; then animation 20 (record 0) or 22 (record 2), checking part 0 of the record ahead
   and, if that misses, behind, and part 2. */
static void DarkKnight_GuardThenMeleeAttack(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    EntityParamsUnk240 *unk240 = &entity->params->melee_attacks[ai_state->step_index];
    s32 frame;

    entity->busy_flags |= ENTITY_BUSY_ATTACK;
    switch (entity->action_step) {
    case 0:
        if (ABS(entity->heading_to_player - entity->model->rotation[1]) < 0.5235988f) {
            Motion_Start(entity, 0x11, 0, 5, 1);
            ai_state->step_index = 0;
        } else if (entity->heading_to_player - entity->model->rotation[1] > 0.5235988f && entity->heading_to_player - entity->model->rotation[1] < 1.5707964f) {
            Motion_Start(entity, 0x12, 0, 5, 1);
            ai_state->step_index = 2;
        } else if (entity->heading_to_player - entity->model->rotation[1] < -0.5235988f && entity->heading_to_player - entity->model->rotation[1] > -1.5707964f) {
            Motion_Start(entity, 0x13, 0, 5, 1);
            ai_state->step_index = 2;
        }
        ENTITY_FLAG_BITS(entity)->motion_step_mode = 1;
        ai_state->turn_rate = 0.0f;
        ENTITY_FLAG_BITS(entity)->melee_0_hit = 0;
        ENTITY_FLAG_BITS(entity)->melee_1_hit = 0;
        ENTITY_FLAG_BITS(entity)->melee_2_hit = 0;
        ENTITY_FLAG_BITS(entity)->look_at_target = 0;
        ENTITY_FLAG_BITS(entity)->guarding = 1;
        entity->action_step++;
        break;
    case 1:
        ai_state->pose_blend_weight = 0.5f;
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            if (ai_state->step_index == 0) {
                Motion_Start(entity, 0x14, 0xE, 0, 1);
            } else if (ai_state->step_index == 2) {
                Motion_Start(entity, 0x16, 0xE, 0, 1);
            }
            entity->action_step++;
        }
        break;
    case 3:
        ai_state->pose_blend_weight = 0.5f;
        if (entity->model->level_tables->blend_left == 0) {
            ENTITY_FLAG_BITS(entity)->guarding = 0;
            entity->action_step++;
        }
        if (unk240->hit_start_frame != 0) {
            EntityAi_TurnToHeading(entity, &ai_state->turn_rate, entity->heading_to_player);
        }
        break;
    case 4:
        if (unk240->hit_start_frame >= entity->model->level_tables->frame) {
            EntityAi_TurnToHeading(entity, &ai_state->turn_rate, entity->heading_to_player);
        } else {
            ai_state->turn_rate = 0.0f;
        }
        frame = (entity->model->level_tables->frame_fixed - entity->params->motion_speeds[entity->animation_index]) >> 8;
        if (!ENTITY_FLAG_BITS(entity)->melee_0_hit) {
            ENTITY_FLAG_BITS(entity)->melee_0_hit = EntityAi_CheckMeleeHitAt(entity, unk240, 0, frame, entity->model->level_tables->frame, entity->model->rotation[1]);
            if (!ENTITY_FLAG_BITS(entity)->melee_0_hit) {
                ENTITY_FLAG_BITS(entity)->melee_0_hit = EntityAi_CheckMeleeHitAt(entity, unk240, 1, frame, entity->model->level_tables->frame, EntityMath_WrapAngle(entity->model->rotation[1] + 3.1415927f));
            }
        }
        if (!ENTITY_FLAG_BITS(entity)->melee_1_hit) {
            ENTITY_FLAG_BITS(entity)->melee_1_hit = EntityAi_CheckMeleeHitAt(entity, unk240, 2, frame, entity->model->level_tables->frame, entity->model->rotation[1]);
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

/* The melee attack with record 2 (EntityParams.melee_attack_third; busy bit 1), which DarkKnight_ThinkCombat
   starts for attack index 3: animation 22; part 0 is checked along the heading and, when it has not
   hit, part 1 along the reverse of the heading; part 2 along the heading (EntityAi_CheckMeleeHitAt). */
static void DarkKnight_MeleeAttack2(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    EntityParamsUnk240 *unk240 = (EntityParamsUnk240 *)entity->params->melee_attack_third;
    s32 frame;

    entity->busy_flags |= ENTITY_BUSY_ATTACK;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x16, 0, entity->params->motion_blend_frames >> 1, 1);
        ENTITY_FLAG_BITS(entity)->motion_step_mode = 1;
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
            ENTITY_FLAG_BITS(entity)->melee_0_hit = EntityAi_CheckMeleeHitAt(entity, unk240, 0, frame, entity->model->level_tables->frame, entity->model->rotation[1]);
            if (!ENTITY_FLAG_BITS(entity)->melee_0_hit) {
                ENTITY_FLAG_BITS(entity)->melee_0_hit = EntityAi_CheckMeleeHitAt(entity, unk240, 1, frame, entity->model->level_tables->frame, EntityMath_WrapAngle(entity->model->rotation[1] + 3.1415927f));
            }
        }
        if (!ENTITY_FLAG_BITS(entity)->melee_1_hit) {
            ENTITY_FLAG_BITS(entity)->melee_1_hit = EntityAi_CheckMeleeHitAt(entity, unk240, 2, frame, entity->model->level_tables->frame, entity->model->rotation[1]);
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

/* The ranged attack with record 0 (busy bit 1): as EntityAi_RangedAttack with animation 23; at its
   end it also sets work word unk68 to EntityParams.action_frames. */
static void DarkKnight_RangedAttack0(Entity *entity) {
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
            ai_state->pause_timer = entity->params->action_frames;
            if (ENTITY_FLAG_BITS(entity)->own_think) {
                ai_state->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, ai_state->move, 1);
            }
        }
        break;
    }
}

/* The ranged attack with record 1 (busy bit 1): as EntityAi_RangedAttack with animation 24, but the
   shots are launched along the reverse of the heading (EntityShot_Launch). */
static void DarkKnight_RangedAttack1(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    EntityParamsUnk3D8 *unk3D8 = ENTITY_PARAMS_UNK3D8(entity->params, 1);
    f32 direction[4];
    s32 elapsed;

    entity->busy_flags |= ENTITY_BUSY_ATTACK;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x18, 0, entity->params->motion_blend_frames >> 1, 1);
        if (entity->params->flags & ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE) {
            ENTITY_FLAGS(entity) &= ~ENTITY_FLAG_MOTION_STEP_MODE_MASK;
        } else {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = (ENTITY_PARAMS_MOVEMENT_KIND(entity->params) == ENTITY_PARAMS_MOVEMENT_1) ? 2 : 1;
        }
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
        elapsed = (entity->model->level_tables->frame_fixed - entity->params->motion_speeds[entity->animation_index]) >> 8;
        Vector_Copy(direction, entity->model->rotation);
        direction[1] -= 3.1415927f;
        EntityShot_Launch(entity, unk3D8, elapsed, entity->model->level_tables->frame, direction);
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

/* Helper of DarkKnight_ThinkCombat: whether to start DarkKnight_RangedAttack0, which is
   whenever the count in work word unk68 has run out. */
static s32 DarkKnight_ShouldRangedAttack0(Entity *entity) {
    return entity->ai_state.pause_timer < 1;
}

/* Helper of DarkKnight_ThinkCombat: whether to start DarkKnight_HoldGuardPose, the action (busy bit 0x10)
   that starts animation 13 and stops the motion for EntityParams.guard_frames frames. Only while work word
   unk6C runs and unk6A does not, the entity is not busy, the player is at least min_distance away
   (less the radius and 100) and Entity.heading_to_player is within max_angle of the heading; then
   EntityParams.guard_chance / 5 times in 100. */
static s32 DarkKnight_ShouldHoldAnim13(Entity *entity, f32 min_distance, f32 max_angle) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 distance;
    f32 angle;

    if (ai_state->mode_timer <= 0) {
        return FALSE;
    }
    if (ai_state->sub_timer > 0) {
        return FALSE;
    }
    distance = entity->distance_to_player - (entity->radius + 100.0f);
    angle = EntityMath_AngleDifference(entity->model->rotation[1], entity->heading_to_player);
    if (entity->busy_flags != 0) {
        return FALSE;
    }
    if (distance < min_distance) {
        return FALSE;
    }
    if (ABS(angle) > max_angle) {
        return FALSE;
    }
    return rand() % 100 < entity->params->guard_chance / 5;
}

/* As EntityAi_PickCombatChoice, but the attack is left out only when EntityAi_CanMelee finds no melee record in
   reach and either unkA2 runs or EntityAi_CanShoot finds no ranged one. Returns the choice, -1 when none can be
   chosen. Nothing calls it. */
static s32 DarkKnight_PickCombatChoice(Entity *entity, s32 mode) {
    s32 chances[4];
    s32 weights[4];
    s32 choice = -1;
    s32 i;
    s32 total;
    s32 fade;
    s32 elapsed;
    s32 roll;
    s32 sum;

    for (i = 0; i < 4; i++) {
        weights[i] = entity->params->combat_choice_weights[i];
    }
    if (!EntityAi_CanMelee(entity) && (entity->attack_delay_frames != 0 || !EntityAi_CanShoot(entity))) {
        weights[0] = 0;
    }
    if (mode == 0) {
        if ((u32)(entity->flags >> 63) || EntitySight_EntityInWay(entity)) {
            weights[0] = 0;
        }
    } else if (mode == 1) {
        if (EntitySight_EntityInWay(entity)) {
            weights[0] = 0;
        }
    } else if (mode == 2) {
        if ((u32)(entity->flags >> 63)) {
            weights[0] = 0;
        }
    }
    total = 0;
    if (weights[0] + weights[1] + weights[2] + weights[3] == 0) {
        return -1;
    }
    for (i = 0; i < 4; i++) {
        if (i == entity->combat_choice) {
            elapsed = (u16)frame_counter - entity->choice_change_frame;
            if (elapsed < 240) {
                fade = 0x80 - (elapsed * (elapsed << 7)) / 57600;
                fade += 0x80;
            } else {
                fade = 0x80;
            }
        } else {
            fade = 0x80;
        }
        chances[i] = (fade * ((entity->choice_weights[i] * weights[i]) >> 6)) >> 7;
        total += chances[i];
    }
    if (total == 0) {
        return -1;
    }
    roll = rand() % total;
    sum = 0;
    for (i = 0; i < 4; i++) {
        sum += chances[i];
        if (roll < sum) {
            choice = i;
            break;
        }
    }
    return choice;
}

/* As EntityAi_PickAttack, with the reach tests of EntityAi_CanUseMelee and EntityAi_CanUseRanged, and no ranged
   attack while unkA2 runs: a weighted draw among the six attacks, the choice in flag bits 57-59; returns bit
   57. Nothing calls it. */
static s32 DarkKnight_PickAttack(Entity *entity) {
    s32 chances[6];
    s32 choice = 0;
    s32 total = 0;
    s32 i;
    s32 roll;
    s32 sum;
    s32 chance;
    s32 slot;

    for (i = 0; i < 6; i++) {
        if (i < 3) {
            slot = i;
            if (EntityAi_CanUseMelee(entity, slot)) {
                chance = (entity->attack_weights[i] * entity->params->melee_attacks[slot].attack_weight) >> 6;
            } else {
                chance = 0;
            }
        } else if (entity->attack_delay_frames != 0) {
            chance = 0;
        } else {
            slot = i - 3;
            if (!EntityAi_CanUseRanged(entity, slot)) {
                chance = 0;
            } else {
                chance = (entity->attack_weights[i] * ENTITY_PARAMS_RANGED_VIEW(entity->params)->ranged[slot].record.ranged_weight) >> 6;
            }
        }
        chances[i] = chance;
        total += chance;
    }
    if (total == 0) {
        total++;
    }
    roll = rand() % total;
    sum = 0;
    for (i = 0; i < 6; i++) {
        sum += chances[i];
        if (roll < sum) {
            choice = i;
            break;
        }
    }
    if (choice < 3) {
        ENTITY_FLAG_BITS(entity)->attack_index = choice;
        ENTITY_FLAG_BITS(entity)->ranged_attack = 0;
    } else {
        ENTITY_FLAG_BITS(entity)->attack_index = choice - 3;
        ENTITY_FLAG_BITS(entity)->ranged_attack = 1;
    }
    EntityAi_LowerAttackWeight(entity);
    return (s32)(entity->flags >> 57) & ENTITY_FLAG_ACTIVE_LIST;
}
