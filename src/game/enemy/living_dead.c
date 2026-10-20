/* LivingDead: the behaviour of entity type 11 (EntityParams.entity_type, the index into entity_type_handlers): its set-up
   function, which the table names, and the handlers it installs. Used by: Living Dead. The game names no
   monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by
   hit points and experience, with resistances and drops where two monsters share those. */
#include "entity/entity.h"
#include "common.h"

void EntityAi_SetUpdate(char *, s32);
int EntityAi_StartAction(Entity *, void (*)(Entity *), int);
void Motion_ClearPartSlots(EntityModel *);
s32 Motion_StepModel(ModelInstance *, s32, s32);
// match: the definition takes an Actor, a struct local to EntityAi_Animate.c
void EntityAi_Animate(Entity *);
void EntityAi_Move(Entity *);
void EntityAi_Dormant(Entity *);
void EntityAi_Death(Entity *);
extern s32 entity_type_handlers[][2];
void EntityAi_LowerWeightsAfterHit(char *);
void EntityAi_SetState(char *, u8);
int EntityAi_SetReaction(Entity *, void (*)(Entity *), int);
extern void EntityAi_ReactionRewind(Entity *);
int rand(void);
void EntityEvent_StartFrameCues(Entity *, s32, s32, s32);
int Motion_Start(void *, int, int, u32, int);
void EntityAi_SetAttackDelay(Entity *, s32);
void EntityAi_RangedAttack(Entity *);
extern u32 frame_counter;
int EntityAi_ThinkIdle(void *, int);
int EntityAi_ThinkReturn(void *, int);
void EntityAi_StepState(void *);
void EntityAi_SensePlayer(void *, int);
void EntityAi_CheckFarFromSpawn(void *);
extern PlayerData player_state;
s32 EntityAi_ShouldGuard(Entity *, f32, f32);
s32 EntityAi_ShouldDodge(Entity *, f32, f32);
s32 EntityAi_PickCombatChoice(Entity *, s32);
void EntityAi_LowerChoiceWeight(Entity *, s32);
int EntityAi_PickAttack(char *);
void EntityAi_ClearTurnInPlace(Entity *);
float EntityMath_AngleDifference(float, float);
int EntityAi_TurnCommand(Entity *);
int EntityAi_TurnFromPush(void *, void *);
s32 EntityAi_MoveCommand(Entity *, s32, f32);
void EntityAi_Dodge(Entity *);
void EntityAi_Guard(Entity *);
void EntityAi_MeleeAttack(Entity *);

static void LivingDead_OnHit(Entity *entity);
static void LivingDead_ReactionLight(Entity *entity);
static void LivingDead_ReactionHeavy(Entity *entity);
static void LivingDead_Think(Entity *entity, s32 unknown);
static void LivingDead_ThinkCombat(Entity *entity, s32 unknown);

/* Set-up of a type 11 entity: clears its work area and installs the type's handlers. */
void LivingDead_Init(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    s32 i;

    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        ENTITY_WORK_BYTES(entity)[i] = 0;
    }
    EntityAi_SetUpdate((char *)entity, entity_type_handlers[entity->params->entity_type][1]);
    ai_state->think = LivingDead_Think;
    ai_state->animate = EntityAi_Animate;
    ai_state->on_hit = LivingDead_OnHit;
    ai_state->move = EntityAi_Move;
    ai_state->dormant = EntityAi_Dormant;
    ai_state->death = EntityAi_Death;
    for (i = 3; i != -1; i--) {
        entity->choice_weights[i] = 0x80;
    }
    for (i = 5; i != -1; i--) {
        entity->attack_weights[i] = 0x80;
    }
    ENTITY_FLAG_BITS(entity)->no_floor_lift = 0;
    entity->target = NULL;
    if (ENTITY_SPAWN_FLAGS(entity) & (0x8000L << 43)) {
        EntityAi_StartAction(entity, ai_state->dormant, 1);
    } else {
        EntityAi_StartAction(entity, ai_state->move, 0);
    }
    ai_state->pose_blend_weight = 0.5f;
    entity->model_alpha = (entity->params->flags & ENTITY_PARAMS_FLAG_HALF_MODEL_ALPHA) ? 0x40 : 0x80;
    entity->model->alpha = entity->model_alpha;
    Motion_ClearPartSlots(entity->model);
    Motion_StepModel(ENTITY_MODEL(entity), 0, 0);
}

/* The on_hit handler: as EntityAi_OnHit with the type's own reactions to a light and a heavy hit.
   */
static void LivingDead_OnHit(Entity *entity) {
    u64 hit_strength;

    EntityAi_LowerWeightsAfterHit((char *)entity);
    entity->target = NULL;
    entity->flags &= ~ENTITY_FLAG_ATTACKING;
    entity->flags &= ~ENTITY_FLAG_LOOK_AT_TARGET;
    hit_strength = ENTITY_FLAG_BITS(entity)->hit_strength;
    if (hit_strength == 1) {
        if (ENTITY_FLAG_BITS(entity)->attack_window) {
            EntityAi_SetReaction(entity, EntityAi_ReactionRewind, 0);
        } else {
            EntityAi_SetReaction(entity, LivingDead_ReactionLight, 0);
        }
    } else if (hit_strength == 2) {
        EntityAi_SetReaction(entity, LivingDead_ReactionHeavy, 0);
    }
    EntityAi_SetState((char *)entity, 2);
}

/* The reaction to a light hit (busy bit 2): as EntityAi_ReactionLight, but when it ends and
   EntitySpawnInfo.spawn_mode is 0 it starts EntityAi_RangedAttack with record 0, with a chance of
   EntityParams.near_distance in 100. */
static void LivingDead_ReactionLight(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;

    entity->busy_flags = ENTITY_BUSY_REACTION_LIGHT;
    switch (entity->reaction_step) {
    case 0:
        EntityEvent_StartFrameCues(entity, 0xE, 0, 0);
        Motion_Start(entity, 0xE, 0, 5, 1);
        if (entity->params->flags & ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE) {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = ENTITY_PARAMS_MOVEMENT_KIND(entity->params) == ENTITY_PARAMS_MOVEMENT_1 ? 2 : 1;
        }
        ai_state->turn_rate = 0.0f;
        ENTITY_FLAG_BITS(entity)->animates = 1;
        ENTITY_FLAG_BITS(entity)->attack_window = 0;
        ENTITY_FLAG_BITS(entity)->guarding = 0;
        ENTITY_FLAG_BITS(entity)->attacking = 0;
        ENTITY_FLAG_BITS(entity)->look_at_target = 0;
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
        if (entity->spawn_info->spawn_mode == 0 && rand() % 100 < entity->params->near_distance) {
            ENTITY_FLAG_BITS(entity)->ranged_attack = 1;
            ENTITY_FLAG_BITS(entity)->attack_index = 0;
            EntityAi_StartAction(entity, EntityAi_RangedAttack, 1);
        } else if (ENTITY_FLAG_BITS(entity)->own_think) {
            ai_state->think(entity, 1);
        } else {
            EntityAi_StartAction(entity, ai_state->move, 1);
        }
        break;
    }
}

/* The reaction to a heavy hit (busy bit 4): as EntityAi_ReactionHeavy, but when it ends and
   EntitySpawnInfo.spawn_mode is 0 it starts EntityAi_RangedAttack with record 0, with a chance of
   EntityParams.near_distance in 100. */
static void LivingDead_ReactionHeavy(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;

    entity->busy_flags = ENTITY_BUSY_REACTION_HEAVY;
    switch (entity->reaction_step) {
    case 0:
        Motion_Start(entity, 0xF, 0, 5, 1);
        if (entity->params->flags & ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE) {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = ENTITY_PARAMS_MOVEMENT_KIND(entity->params) == ENTITY_PARAMS_MOVEMENT_1 ? 2 : 1;
        }
        ai_state->turn_rate = 0.0f;
        ENTITY_FLAG_BITS(entity)->animates = 1;
        ENTITY_FLAG_BITS(entity)->attack_window = 0;
        ENTITY_FLAG_BITS(entity)->guarding = 0;
        if (ENTITY_PARAMS_MOVEMENT_KIND(entity->params) == ENTITY_PARAMS_MOVEMENT_WALK) {
            ENTITY_FLAG_BITS(entity)->no_floor_lift = 0;
        }
        ENTITY_FLAG_BITS(entity)->attacking = 0;
        ENTITY_FLAG_BITS(entity)->look_at_target = 0;
        entity->reaction_step++;
        break;
    case 1:
        if (entity->model->level_tables->blend_left == 0) {
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
            if (entity->spawn_info->spawn_mode == 0 && rand() % 100 < entity->params->near_distance) {
                ENTITY_FLAG_BITS(entity)->ranged_attack = 1;
                ENTITY_FLAG_BITS(entity)->attack_index = 0;
                EntityAi_StartAction(entity, EntityAi_RangedAttack, 1);
            } else if (ENTITY_FLAG_BITS(entity)->own_think) {
                ai_state->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, ai_state->move, 1);
            }
        }
        break;
    }
}

/* The think handler: as EntityAi_Think, with EntityAi_ThinkIdle for state 1 too and
   LivingDead_ThinkCombat for state 2. */
static void LivingDead_Think(Entity *entity, s32 unknown) {
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
    if (ENTITY_FLAG_BITS(entity)->far_from_spawn) {
        EntityAi_ThinkReturn(entity, unknown);
        return;
    }
    switch (entity->state) {
    case 0:
    case 1:
        break;
    case 2:
        LivingDead_ThinkCombat(entity, unknown);
        return;
    }
    EntityAi_ThinkIdle(entity, unknown);
}

/* The think function of state 2: as EntityAi_ThinkCombat, but with EntitySpawnInfo.spawn_mode at 0, on
   each frame whose count divides by 120 plus work byte unk7A, it draws unk7A anew (0 to 59) and
   starts EntityAi_RangedAttack with record 0 with a chance of EntityParams.action_frames in 100. Closing
   in never runs; waiting lasts 60 to 89 frames. */
static void LivingDead_ThinkCombat(Entity *entity, s32 unknown) {
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
    if (entity->spawn_info->spawn_mode == 0 && frame_counter % (u32)(ai_state->step_index + 0x78) == 0) {
        ai_state->step_index = rand() % 60;
        if (rand() % 100 < (s16)entity->params->action_frames) {
            ENTITY_FLAG_BITS(entity)->ranged_attack = 1;
            ENTITY_FLAG_BITS(entity)->attack_index = 0;
            EntityAi_StartAction(entity, EntityAi_RangedAttack, unknown);
            return;
        }
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
            entity->target = &player_state;
            entity->action_timer = rand() % 60 + 0x78;
            ai_state->move_choice = 0;
            break;
        case 2:
            entity->action_timer = rand() % 30 + 0x3C;
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
                entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_RIGHT);
            } else if (turn < 0) {
                entity->move_command = (MOVE_COMMAND_FORWARD | MOVE_COMMAND_TURN_LEFT);
            } else {
                entity->move_command = EntityAi_MoveCommand(entity, 0, entity->radius + 100.0f + 30.0f);
            }
        }
    }
done:
    EntityAi_StartAction(entity, ai_state->move, unknown);
}
