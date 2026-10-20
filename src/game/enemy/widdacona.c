/* Widdacona: the behaviour of entity type 30 (EntityParams.entity_type, the index into entity_type_handlers): its set-up
   function, which the table names, and the handlers it installs. Used by: Widdacona. The game names no
   monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by
   hit points and experience, with resistances and drops where two monsters share those. */
#include "entity/entity.h"
#include "common.h"
#include "entity/entity_world.h"
#include "effect/effect.h"

#define ENTITY_WORK_AREA_SIZE 0xA0
void EntityAi_SetUpdate(char *, s32);
int EntityAi_StartAction(Entity *, void (*)(Entity *), int);
void Motion_ClearPartSlots(EntityModel *);
s32 Motion_StepModel(ModelInstance *, s32, s32);
void Vector_Copy(void *, void *);
// match: the definition takes an Actor, a struct local to its file
void EntityAi_Move(Entity *);
extern s32 entity_type_handlers[][2];
int Motion_Step(void *, int);
int Motion_BlendToPose(EntityModel *, f32);
void EntityAi_LowerWeightsAfterHit(char *);
int EntityAi_SetReaction(Entity *, void (*)(Entity *), int);
void EntityAi_SetState(char *, u8);
void EntityAi_ReactionHeavy(Entity *);
extern PlayerData player_state;
int Motion_Start(void *, int, int, u32, int);
char *Model_GetSkinnedBonePoint(char *, u32);
void EntityEvent_OnDeath(Entity *);
int EntityAi_AnimationFrameCount(void *, int);
extern u32 frame_counter;
void EntityAi_SensePlayer(void *, int);
void EntityAi_StepState(void *);
int rand(void);
s32 EntityAi_ShouldGuard(Entity *, f32, f32);
s32 EntityAi_ShouldDodge(Entity *, f32, f32);
s32 EntityAi_PickCombatChoice(Entity *, s32);
void EntityAi_LowerChoiceWeight(Entity *, s32);
int EntityAi_PickAttack(char *);
int EntityAi_TurnCommand(Entity *);
void EntityAi_Dodge(Entity *);
void EntityAi_Guard(Entity *);
void EntityAi_MeleeAttack(Entity *);
s32 EntityAi_TurnToHeading(Entity *, f32 *, f32);
int EntityShot_LaunchAimed(Entity *, EntityParamsUnk3D8 *, s32, s32, f32 *, f32, f32);
void EntityAi_SetAttackDelay(Entity *, s32);

static void Widdacona_Animate(Entity *entity);
static void Widdacona_OnHit(Entity *entity);
static void Widdacona_Death(Entity *entity);
static void Widdacona_Dormant(Entity *entity);
static void Widdacona_ReturnToDormant(Entity *entity);
static void Widdacona_Think(Entity *entity, s32 unknown);
static void Widdacona_ThinkCombat(Entity *entity, s32 unknown);
static void Widdacona_RangedAttack0(Entity *entity);

/* Set-up of a type 30 entity: clears its work area and installs the type's handlers. */
void Widdacona_Init(Entity *entity) {
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
    ai_state->think = Widdacona_Think;
    ai_state->animate = Widdacona_Animate;
    ai_state->on_hit = Widdacona_OnHit;
    ai_state->move = EntityAi_Move;
    ai_state->dormant = Widdacona_Dormant;
    ai_state->death = Widdacona_Death;
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
    Motion_StepModel(ENTITY_MODEL(entity), 0, 0);
}

/* The animate handler: as EntityAi_Animate with flag bits 11 to 13 put to 0 (the motion never moves
   the entity) and without the head turn. */
static void Widdacona_Animate(Entity *entity) {
    u32 blend_frames; // match: converted to float as unsigned
    EntityAiState *ai_state;

    ai_state = &entity->ai_state;
    if (ENTITY_FLAG_BITS(entity)->animates) {
        ENTITY_FLAG_BITS(entity)->motion_step_mode = 0;
        blend_frames = entity->model->level_tables->blend_left;
        if (blend_frames != 0) {
            ENTITY_FLAG_BITS(entity)->motion_ended = Motion_BlendToPose(entity->model, (entity->busy_flags & (ENTITY_BUSY_GUARD | ENTITY_BUSY_REACTION_LIGHT)) ? ai_state->pose_blend_weight : 1.0f / (f32)blend_frames);
        } else {
            ENTITY_FLAG_BITS(entity)->motion_ended = Motion_Step(entity, ENTITY_FLAG_BITS(entity)->motion_step_mode);
        }
    }
}

/* The on_hit handler: as EntityAi_OnHit, but only a heavy hit starts a reaction
   (EntityAi_ReactionHeavy). */
static void Widdacona_OnHit(Entity *entity) {
    EntityAi_LowerWeightsAfterHit((char *)entity);
    entity->target = NULL;
    entity->flags &= ~ENTITY_FLAG_LOOK_AT_TARGET;
    if (ENTITY_FLAG_BITS(entity)->hit_strength == 2) {
        EntityAi_SetReaction(entity, EntityAi_ReactionHeavy, 0);
    }
    EntityAi_SetState((char *)entity, 2);
}

/* The death action (busy bit 0x40): animation 16; when it has ended it starts effect 0x3BC at the
   points Model_GetSkinnedBonePoint gives for 20 to 22, sets work byte unk7A and waits until the entity whose
   spawn id is EntitySpawnInfo.spawn_mode has its unk7A set too; then the fade over 20 frames and the
   removal, as EntityAi_Death. */
static void Widdacona_Death(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    Entity *other;
    char *part;

    entity->busy_flags = ENTITY_BUSY_DEATH;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x10, 0, 5, 1);
        ENTITY_FLAG_BITS(entity)->motion_step_mode = 0;
        if (ENTITY_PARAMS_MOVEMENT_KIND(entity->params) == ENTITY_PARAMS_MOVEMENT_FLY) {
            entity->collision_radius = 20.0f;
        }
        ENTITY_FLAG_BITS(entity)->animates = TRUE;
        ENTITY_FLAG_BITS(entity)->bit21 = TRUE;
        ENTITY_FLAG_BITS(entity)->dying = 1;
        ENTITY_FLAG_BITS(entity)->no_floor_lift = FALSE;
        ENTITY_FLAG_BITS(entity)->look_at_target = FALSE;
        entity->action_step++;
    case 1:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            part = Model_GetSkinnedBonePoint((char *)entity->model, 0x14);
            if (part != NULL) {
                Effect_StartSpawner(0x3BC, part, entity->model->rotation, player_state.position);
            }
            part = Model_GetSkinnedBonePoint((char *)entity->model, 0x15);
            if (part != NULL) {
                Effect_StartSpawner(0x3BC, part, entity->model->rotation, player_state.position);
            }
            part = Model_GetSkinnedBonePoint((char *)entity->model, 0x16);
            if (part != NULL) {
                Effect_StartSpawner(0x3BC, part, entity->model->rotation, player_state.position);
            }
            ENTITY_FLAG_BITS(entity)->animates = FALSE;
            ai_state->step_index = 1;
            entity->action_step++;
        }
        break;
    case 3:
        other = Entity_GetBySpawnIdForMap(entity->group_id, entity->spawn_info->spawn_mode);
        if (other != NULL && ((s32)other->flags & ENTITY_FLAG_ACTIVE_LIST) && other->ai_state.step_index != 0) {
            ENTITY_FLAG_BITS(entity)->dying = 2;
            ENTITY_FLAG_BITS(entity)->no_respawn = TRUE;
            ENTITY_FLAG_BITS(entity)->bit20 = TRUE;
            ENTITY_FLAG_BITS(entity)->no_player_collision = TRUE;
            ENTITY_FLAG_BITS(entity)->no_entity_collision = TRUE;
            entity->action_timer = 20;
            EntityEvent_OnDeath(entity);
            entity->action_step++;
        }
        break;
    case 4:
        if (entity->action_timer != 0) {
            entity->model->alpha = (f32)entity->model_alpha * (f32)entity->action_timer / 20.0f;
        } else {
            ENTITY_FLAG_BITS(entity)->dying = 3;
            ENTITY_FLAG_BITS(entity)->live = FALSE;
        }
        break;
    }
}

/* The dormant action (busy bit 0x20): as EntityAi_Dormant, but it gets up only while the player is
   inside a fixed rectangle (x -4900 to -2500, z -5350 to -3350), and it leaves flag bit 28 alone.
   */
static void Widdacona_Dormant(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;

    entity->busy_flags |= ENTITY_BUSY_DORMANT;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 1, 0, 0, 1);
        if (entity->params->flags & ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE) {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = ENTITY_PARAMS_MOVEMENT_KIND(entity->params) == ENTITY_PARAMS_MOVEMENT_1 ? 2 : 1;
        }
        ai_state->turn_rate = 0.0f;
        ENTITY_FLAG_BITS(entity)->bit21 = 1;
        ENTITY_FLAG_BITS(entity)->bit22 = 1;
        ENTITY_FLAG_BITS(entity)->no_wall_collision = 1;
        ENTITY_FLAG_BITS(entity)->no_second_wall_collision = 1;
        ENTITY_FLAG_BITS(entity)->bit18 = 1;
        ENTITY_FLAG_BITS(entity)->look_at_target = 0;
        entity->action_step++;
        break;
    case 1:
        if (player_state.position[0] > -4900.0f && player_state.position[0] < -2500.0f &&
            player_state.position[2] > -5350.0f && player_state.position[2] < -3350.0f) {
            if (entity->distance_to_player < entity->spawn_info->wake_distance || ENTITY_FLAG_BITS(entity)->hit_by_player) {
                Motion_Start(entity, 2, 0, entity->params->motion_blend_frames, 0);
                if (entity->params->flags & ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE) {
                    ENTITY_FLAG_BITS(entity)->motion_step_mode = 0;
                } else {
                    ENTITY_FLAG_BITS(entity)->motion_step_mode = ENTITY_PARAMS_MOVEMENT_KIND(entity->params) == ENTITY_PARAMS_MOVEMENT_1 ? 2 : 1;
                }
                ENTITY_FLAG_BITS(entity)->hit_strength = 0;
                entity->action_step++;
            }
        }
        break;
    case 2:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        break;
    case 3:
        if (ENTITY_FLAG_BITS(entity)->motion_ended) {
            EntityAi_SetState((char *)entity, 2);
            ENTITY_FLAG_BITS(entity)->bit21 = 0;
            ENTITY_FLAG_BITS(entity)->bit22 = 0;
            ENTITY_FLAG_BITS(entity)->no_wall_collision = 0;
            ENTITY_FLAG_BITS(entity)->no_second_wall_collision = 0;
            ENTITY_FLAG_BITS(entity)->bit18 = 0;
            entity->busy_flags = 0;
            entity->move_command = 0;
            entity->action_timer = 0;
            if (ENTITY_FLAG_BITS(entity)->own_think) {
                entity->ai_state.think(entity, 1);
            } else {
                EntityAi_StartAction(entity, ai_state->move, 1);
            }
        }
        break;
    }
}

/* An action (busy bit 0x1000) that Widdacona_Think starts while the player is outside the
   rectangle: starts animation 2 at frame EntityAi_AnimationFrameCount(entity, 2) - 1 with flag bits 15 and 18 set;
   when the motion has ended it copies EntitySpawnInfo.spawn_rotation_x to the model's angles and starts the
   dormant action. */
static void Widdacona_ReturnToDormant(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;

    entity->busy_flags |= ENTITY_BUSY_1000;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 2, EntityAi_AnimationFrameCount(entity, 2) - 1, entity->params->motion_blend_frames, 1);
        ENTITY_FLAG_BITS(entity)->motion_step_mode = 0;
        ENTITY_FLAG_BITS(entity)->bit15 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
        ENTITY_FLAG_BITS(entity)->look_at_target = FALSE;
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
            ENTITY_FLAG_BITS(entity)->bit15 = FALSE;
            Vector_Copy(entity->model->rotation, &entity->spawn_info->spawn_rotation_x);
            EntityAi_StartAction(entity, ai_state->dormant, 1);
        }
        break;
    }
}

/* The think handler: has no switch on the state. While the entity is not busy: with the player
   outside a fixed rectangle (x -4900 to -2500, z -5350 to -3350) it starts
   Widdacona_ReturnToDormant; else in state 2 it calls Widdacona_ThinkCombat and in the other
   states starts the move action. */
static void Widdacona_Think(Entity *entity, s32 unknown) {
    EntityAiState *ai_state = &entity->ai_state;

    if (frame_counter % 10U == entity->spawn_index % 10U) {
        EntityAi_SensePlayer(entity, 2);
    }
    if (entity->busy_flags != 0) {
        return;
    }
    EntityAi_StepState(entity);
    if (player_state.position[0] < -4900.0f || player_state.position[0] > -2500.0f ||
        player_state.position[2] < -5350.0f || player_state.position[2] > -3350.0f) {
        EntityAi_StartAction(entity, Widdacona_ReturnToDormant, unknown);
        return;
    }
    if (entity->state == 2) {
        Widdacona_ThinkCombat(entity, unknown);
        return;
    }
    EntityAi_StartAction(entity, ai_state->move, unknown);
}

/* The think function of state 2 (called under `entity->state == 2`, not from a switch): as
   EntityAi_ThinkCombat with EntityAi_PickCombatChoice in mode 3, the root movement and the head turn off, and
   Widdacona_RangedAttack0 as the ranged attack; where that one closes in, this one only turns on
   the spot (EntityAi_TurnCommand). */
static void Widdacona_ThinkCombat(Entity *entity, s32 unknown) {
    EntityAiState *ai_state = &entity->ai_state;
    s32 side;

    ENTITY_FLAG_BITS(entity)->motion_step_mode = 0;
    if (EntityAi_ShouldGuard(entity, 300.0f, DEG_TO_RAD(30))) {
        EntityAi_StartAction(entity, EntityAi_Guard, 1);
        return;
    }
    if (EntityAi_ShouldDodge(entity, 300.0f, DEG_TO_RAD(30))) {
        EntityAi_StartAction(entity, EntityAi_Dodge, 1);
        return;
    }
    side = EntityAi_PickCombatChoice(entity, 3);
    ENTITY_FLAG_BITS(entity)->look_at_target = 0;
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
                EntityAi_StartAction(entity, Widdacona_RangedAttack0, unknown);
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
        entity->move_command = EntityAi_TurnCommand(entity);
    }
    EntityAi_StartAction(entity, ai_state->move, unknown);
}

/* The ranged attack with record 0 (busy bit 1): animation 23; from frame 12 to 76, on two frames in
   three, launches the record's shots at a point 100 below the player (EntityShot_LaunchAimed). */
static void Widdacona_RangedAttack0(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    EntityParamsUnk3D8 *unk3D8 = &entity->params->ranged_attacks[0].record;
    f32 direction[4];

    entity->busy_flags |= ENTITY_BUSY_ATTACK;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x17, 0, entity->params->motion_blend_frames >> 1, 1);
        ENTITY_FLAGS(entity) &= ~ENTITY_FLAG_MOTION_STEP_MODE_MASK;
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
        if (entity->model->level_tables->frame >= 12 && entity->model->level_tables->frame <= 76 && frame_counter % 3 != 0) {
            Vector_Copy(direction, &player_state);
            direction[1] -= 100.0f;
            EntityShot_LaunchAimed(entity, unk3D8, 12, 76, direction, DEG_TO_RAD(22.5f), AIMED_SHOT_TURN_RATE);
        }
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
