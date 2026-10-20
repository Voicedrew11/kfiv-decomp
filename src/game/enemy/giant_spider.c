/* GiantSpider: the behaviour of entity type 4 (EntityParams.entity_type, the index into entity_type_handlers): its set-up
   function, which the table names, and the handlers it installs. Used by: Giant Spider. The game names no
   monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by
   hit points and experience, with resistances and drops where two monsters share those. */
#include "entity/entity.h"
#include "common.h"

void EntityAi_SetUpdate(char *, s32);
int EntityAi_StartAction(Entity *, void (*)(Entity *), int);
void Motion_ClearPartSlots(EntityModel *);
s32 Motion_StepModel(ModelInstance *, s32, s32);
void Vector_Copy(void *, void *);
void EntityAi_Dormant(Entity *);
void EntityAi_Death(Entity *);
extern s32 entity_type_handlers[][2];
int Motion_Step(void *, int);
int Motion_BlendToPose(EntityModel *, f32);
float EntityMath_AngleDifference(float, float);
float EntityMath_EaseAngleToward(float *, float, float);
void EntityAi_LowerWeightsAfterHit(char *);
void EntityAi_SetState(char *, u8);
int EntityAi_SetReaction(Entity *, void (*)(Entity *), int);
void EntityAi_ReactionHeavy(Entity *);
extern u32 frame_counter;
void EntityAi_SensePlayer(void *, int);
void EntityAi_StepState(void *);
int rand(void);
s32 EntityAi_ShouldGuard(Entity *, f32, f32);
s32 EntityAi_PickCombatChoice(Entity *, s32);
void EntityAi_LowerChoiceWeight(Entity *, s32);
int EntityAi_PickAttack(char *);
void EntityAi_Guard(Entity *);
void EntityAi_MeleeAttack(Entity *);
int Motion_Start(void *, int, int, u32, int);
extern PlayerData player_state;
s32 EntityAi_TurnToHeading(Entity *, f32 *, f32);
int EntityShot_LaunchAimed(Entity *, EntityParamsUnk3D8 *, s32, s32, f32 *, f32, f32);
void EntityAi_SetAttackDelay(Entity *, s32);

static void GiantSpider_Animate(Entity *entity);
static void GiantSpider_OnHit(Entity *entity);
static void GiantSpider_Think(Entity *entity, s32 unknown);
static void GiantSpider_ThinkTurnToPlayer(Entity *entity, s32 unknown);
static void GiantSpider_ThinkCombat(Entity *entity, s32 unknown);
static void GiantSpider_Move(Entity *entity);
static void GiantSpider_PlayTurnAnim6Or11(Entity *entity);
static void GiantSpider_PlayTurnAnim7Or12(Entity *entity);
static void GiantSpider_RangedAttack(Entity *entity);

/* Set-up of a type 4 entity: clears its work area and installs the type's handlers. */
void GiantSpider_Init(Entity *entity) {
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
    ai_state->think = GiantSpider_Think;
    ai_state->animate = GiantSpider_Animate;
    ai_state->on_hit = GiantSpider_OnHit;
    ai_state->move = GiantSpider_Move;
    ai_state->dormant = EntityAi_Dormant;
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
    if (unk138 != NULL) {
        Vector_Copy(ai_state->look_part_angles, unk138 + 0x50);
    }
    ai_state->pose_blend_weight = 0.5f;
    entity->model_alpha = (entity->params->flags & ENTITY_PARAMS_FLAG_HALF_MODEL_ALPHA) ? 0x40 : 0x80;
    entity->model->alpha = entity->model_alpha;
    Motion_ClearPartSlots(entity->model);
    Motion_StepModel(ENTITY_MODEL(entity), 0, 0);
}

/* The animate handler: as EntityAi_Animate, but in place of the head turn it turns the part
   EntityModel.look_at_part itself: towards Entity.heading_to_player, at most 30 degrees to either side, eased in work
   float unk20[1]; in animations 14 to 16 it takes the part's own angle instead. */
static void GiantSpider_Animate(Entity *entity) {
    u32 blend_frames; // match: converted to float as unsigned
    s64 flags;
    EntityAiState *ai_state;
    EntityModelPart80 *part;
    f32 angle;

    ai_state = &entity->ai_state;
    flags = entity->flags;
    part = entity->model->look_at_part;
    if (ENTITY_FLAG_BITS(entity)->animates) {
        blend_frames = entity->model->level_tables->blend_left;
        if (blend_frames != 0) {
            ENTITY_FLAG_BITS(entity)->motion_ended = Motion_BlendToPose(entity->model, (entity->busy_flags & ENTITY_BUSY_REACTION_LIGHT) ? ai_state->pose_blend_weight : 1.0f / (f32)blend_frames);
        } else {
            ENTITY_FLAG_BITS(entity)->motion_ended = Motion_Step(entity, (s32)(flags >> 11) & 7);
        }
        if (part != NULL) {
            if ((u8)(entity->animation_index - 0xE) < 3) {
                ai_state->look_part_angles[1] = -part->part_angles[1];
            } else {
                angle = EntityMath_AngleDifference(entity->model->rotation[1], entity->heading_to_player);
                if (angle < -0.5235988f) {
                    angle = -0.5235988f;
                }
                if (0.5235988f < angle) {
                    angle = 0.5235988f;
                }
                EntityMath_EaseAngleToward(&ai_state->look_part_angles[1], angle, 10.0f);
                part->part_angles[1] = -ai_state->look_part_angles[1];
            }
        }
    }
}

/* The on_hit handler: every hit starts EntityAi_ReactionHeavy, whatever its strength; sets work
   byte unk7A. */
static void GiantSpider_OnHit(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;

    EntityAi_LowerWeightsAfterHit((char *)entity);
    entity->flags &= ~ENTITY_FLAG_LOOK_AT_TARGET;
    EntityAi_SetReaction(entity, EntityAi_ReactionHeavy, 0);
    EntityAi_SetState((char *)entity, 2);
    ai_state->step_index = 1;
}

/* The think handler: as EntityAi_Think without EntityAi_CheckFarFromSpawn and the return to the spawn point,
   flag bit 9 kept set, with GiantSpider_ThinkCombat for state 2 and GiantSpider_ThinkTurnToPlayer for the other
   states. */
static void GiantSpider_Think(Entity *entity, s32 unknown) {
    if (frame_counter % 10U == entity->spawn_index % 10U) {
        EntityAi_SensePlayer(entity, 0);
    }
    if (entity->busy_flags != 0) {
        return;
    }
    entity->flags |= ENTITY_FLAG_LOOK_AT_TARGET;
    EntityAi_StepState(entity);
    switch (entity->state) {
    case 0:
    case 1:
        break;
    case 2:
        GiantSpider_ThinkCombat(entity, unknown);
        return;
    }
    GiantSpider_ThinkTurnToPlayer(entity, unknown);
}

/* The think function of every state but 2 (GiantSpider_Think calls it): turns the entity to
   Entity.heading_to_player, the bearing the shared code turns to when it turns to the player. When unk68 is
   more than 25 degrees off the heading it starts GiantSpider_PlayTurnAnim7Or12 (positive side) or
   GiantSpider_PlayTurnAnim6Or11; else it starts the move action with no command. */
static void GiantSpider_ThinkTurnToPlayer(Entity *entity, s32 unknown) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 angle;

    ENTITY_FLAG_BITS(entity)->motion_step_mode = 1;
    angle = EntityMath_AngleDifference(entity->model->rotation[1], entity->heading_to_player);
    if (ABS(angle) > 0.43633232f) {
        if (angle >= 0.0f) {
            EntityAi_StartAction(entity, GiantSpider_PlayTurnAnim7Or12, unknown);
        } else {
            EntityAi_StartAction(entity, GiantSpider_PlayTurnAnim6Or11, unknown);
        }
    } else {
        EntityAi_StartAction(entity, ai_state->move, unknown);
    }
}

/* The think function of state 2: guards the player's swing and picks among attacking, closing in
   and waiting as EntityAi_ThinkCombat does; it has no dodge, and its ranged attack is
   GiantSpider_RangedAttack. It gives no command: where that one closes in, this one starts
   GiantSpider_PlayTurnAnim7Or12 or GiantSpider_PlayTurnAnim6Or11 (by the side) when Entity.heading_to_player is more than
   25 degrees off the heading. An attack clears work byte unk7A. */
static void GiantSpider_ThinkCombat(Entity *entity, s32 unknown) {
    EntityAiState *ai_state = &entity->ai_state;
    s32 side;
    f32 angle;

    ENTITY_FLAG_BITS(entity)->motion_step_mode = 1;
    if (EntityAi_ShouldGuard(entity, 300.0f, 0.5235988f)) {
        EntityAi_StartAction(entity, EntityAi_Guard, 1);
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
                EntityAi_StartAction(entity, GiantSpider_RangedAttack, unknown);
            } else {
                EntityAi_StartAction(entity, EntityAi_MeleeAttack, unknown);
            }
            ai_state->step_index = 0;
            return;
        case 1:
            angle = EntityMath_AngleDifference(entity->model->rotation[1], entity->heading_to_player);
            if (ABS(angle) > 0.43633232f) {
                if (angle >= 0.0f) {
                    EntityAi_StartAction(entity, GiantSpider_PlayTurnAnim7Or12, unknown);
                    return;
                }
                EntityAi_StartAction(entity, GiantSpider_PlayTurnAnim6Or11, unknown);
                return;
            }
            ai_state->step_index = 0;
            ai_state->move_choice = -1;
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
    EntityAi_StartAction(entity, ai_state->move, unknown);
}

/* The move handler: only picks the animation: 12 or 11 for command bit 0x100 or 0x200, 7 or 6 when
   bit 0x400 is set as well, else 0. */
static void GiantSpider_Move(Entity *entity) {
    u16 command;

    command = entity->move_command;
    if (command & MOVE_COMMAND_TURN_LEFT) {
        if (command & MOVE_COMMAND_TURN_FAST) {
            Motion_Start(entity, 7, 0, entity->params->motion_blend_frames >> 1, 0);
            return;
        }
        Motion_Start(entity, 0xC, 0, entity->params->motion_blend_frames, 0);
        return;
    }
    if (command & MOVE_COMMAND_TURN_RIGHT) {
        if (command & MOVE_COMMAND_TURN_FAST) {
            Motion_Start(entity, 6, 0, entity->params->motion_blend_frames >> 1, 0);
            return;
        }
        Motion_Start(entity, 0xB, 0, entity->params->motion_blend_frames, 0);
        return;
    }
    Motion_Start(entity, 0, 0, entity->params->motion_blend_frames, 0);
}

/* An action (busy bit 0x1000), which GiantSpider_ThinkCombat and GiantSpider_ThinkTurnToPlayer start when
   Entity.heading_to_player is more than 25 degrees off the heading on the negative side: plays animation 6 when
   work byte unk7A is set (GiantSpider_OnHit sets it), else animation 11, to its end, with flag bits
   11 to 13 at 3. */
static void GiantSpider_PlayTurnAnim6Or11(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;

    entity->busy_flags |= ENTITY_BUSY_1000;
    switch (entity->action_step) {
    case 0:
        if (ai_state->step_index != 0) {
            Motion_Start(entity, 6, 0, entity->params->motion_blend_frames >> 1, 1);
        } else {
            Motion_Start(entity, 0xB, 0, entity->params->motion_blend_frames, 1);
        }
        ENTITY_FLAG_BITS(entity)->motion_step_mode = 3;
        ENTITY_UNK5C(entity)->as_s32 = 0;
        entity->action_step++;
        break;
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

/* An action (busy bit 0x2000), the same as GiantSpider_PlayTurnAnim6Or11 for the positive side: plays
   animation 7 when work byte unk7A is set, else animation 12, to its end. */
static void GiantSpider_PlayTurnAnim7Or12(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;

    entity->busy_flags |= ENTITY_BUSY_2000;
    switch (entity->action_step) {
    case 0:
        if (ai_state->step_index != 0) {
            Motion_Start(entity, 7, 0, entity->params->motion_blend_frames >> 1, 1);
        } else {
            Motion_Start(entity, 0xC, 0, entity->params->motion_blend_frames, 1);
        }
        ENTITY_FLAG_BITS(entity)->motion_step_mode = 3;
        ENTITY_UNK5C(entity)->as_s32 = 0;
        entity->action_step++;
        break;
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

/* The ranged attack (busy bit 1): as EntityAi_RangedAttack, the shots turned towards a point 160
   below the player by at most 15 degrees (EntityShot_LaunchAimed). */
static void GiantSpider_RangedAttack(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    s32 kind = (s32)(entity->flags >> 58) & 3;
    EntityParamsUnk3D8 *unk3D8 = ENTITY_PARAMS_UNK3D8(entity->params, kind);
    f32 direction[4];
    s32 elapsed;

    entity->busy_flags |= ENTITY_BUSY_ATTACK;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, kind + 0x17, 0, entity->params->motion_blend_frames >> 1, 1);
        entity->flags &= ~ENTITY_FLAG_MOTION_STEP_MODE_MASK;
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
        Vector_Copy(direction, &player_state);
        direction[1] -= 160.0f;
        EntityShot_LaunchAimed(entity, unk3D8, elapsed, entity->model->level_tables->frame, direction, 0.2617994f, 0.2617994f);
        if (entity->model->level_tables->frame > 0 && unk3D8->attack_end_frame >= entity->model->level_tables->frame) {
            entity->flags |= ENTITY_FLAG_ATTACK_WINDOW;
        } else {
            entity->flags &= ~ENTITY_FLAG_ATTACK_WINDOW;
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
