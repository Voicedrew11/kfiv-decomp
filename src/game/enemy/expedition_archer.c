/* ExpeditionArcher: the behaviour of entity type 13 (EntityParams.entity_type, the index into entity_type_handlers): its
   set-up function, which the table names, and the handlers it installs. Used by: Expedition Archer. The game
   names no monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm
   records by hit points and experience, with resistances and drops where two monsters share those. */
#include "entity/entity.h"
#include "common.h"
#include "entity/entity_world.h"
#include "enemy/enemy.h"
#include "entity/entity_ai_handlers.h"
#include "entity/entity_math.h"
#include <math.h>
#include "math/vu0.h"
#include <stdlib.h>

#define ENTITY_WORK_AREA_SIZE 0xA0
void EntityAi_SetUpdate(char *, s32);
int EntityAi_StartAction(Entity *, void (*)(Entity *), int);
void Motion_ClearPartSlots(EntityModel *);
int Motion_StepModel(EntityModel *, s32, s32);
extern s32 entity_type_handlers[][2];
int Motion_Step(void *, int);
int Motion_BlendToPose(EntityModel *, f32);
int EntityAi_LookAtPlayer(Entity *, void *, s32, void *, f32);
void EntityAi_SensePlayer(void *, int);
void EntityAi_CheckFarFromSpawn(char *);
void EntityAi_StepState(void *);
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
int Motion_Start(void *, int, int, u32, int);
s32 EntityAi_TurnToHeading(Entity *, f32 *, f32);
int EntityShot_LaunchAimed(Entity *, EntityParamsUnk3D8 *, s32, s32, f32 *, f32, f32);
void EntityAi_SetAttackDelay(Entity *, s32);
void EntityEvent_OnDeath(Entity *);
void EntityEvent_Respawn(char *);

static void ExpeditionArcher_Animate(Entity *entity);
static void ExpeditionArcher_PitchPartToPlayer(Entity *actor);
static void ExpeditionArcher_Think(Entity *entity, s32 unknown);
static void ExpeditionArcher_ThinkCombat(Entity *entity, s32 unknown);
static void ExpeditionArcher_RangedAttack(Entity *entity);
static void ExpeditionArcher_RangedAttack1(Entity *entity);
static void ExpeditionArcher_Death(Entity *entity);

/* Set-up of a type 13 entity: clears its work area and installs the type's handlers. */
void ExpeditionArcher_Init(Entity *entity) {
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
    ai_state->think = ExpeditionArcher_Think;
    ai_state->animate = ExpeditionArcher_Animate;
    ai_state->on_hit = EntityAi_OnHit;
    ai_state->move = EntityAi_Move;
    ai_state->dormant = EntityAi_Dormant;
    ai_state->death = ExpeditionArcher_Death;
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
    if (unk138 != NULL) {
        Vector_Copy(ai_state->look_part_angles, unk138 + 0x50);
    }
    ai_state->pose_blend_weight = 0.5f;
    entity->model_alpha = (entity->params->flags & ENTITY_PARAMS_FLAG_HALF_MODEL_ALPHA) ? 0x40 : 0x80;
    entity->model->alpha = entity->model_alpha;
    Motion_ClearPartSlots(entity->model);
    Motion_StepModel(entity->model, 0, 0);
}

/* The animate handler: as EntityAi_Animate; while flag bit 10 is set ExpeditionArcher_PitchPartToPlayer
   pitches a model part, and otherwise work float unk48 is cleared. */
static void ExpeditionArcher_Animate(Entity *entity) {
    f32 limits[4];
    u32 blend_frames; // match: converted to float as unsigned
    s64 flags;
    s64 flags_now;
    EntityAiState *ai_state;
    s32 look_at_target;
    EntityParams *params;

    flags = entity->flags;
    ai_state = &entity->ai_state;
    if ((s32)((s64)(flags << 18) >> 32) & 1) {
        blend_frames = entity->model->level_tables->blend_left;
        if (blend_frames != 0) {
            ENTITY_FLAG_BITS(entity)->motion_ended = Motion_BlendToPose(entity->model, (entity->busy_flags & ENTITY_BUSY_REACTION_LIGHT) ? ai_state->pose_blend_weight : 1.0f / (f32)blend_frames);
            flags = entity->flags;
        } else {
            ENTITY_FLAG_BITS(entity)->motion_ended = Motion_Step(entity, ENTITY_FLAG_BITS(entity)->motion_step_mode);
            flags = entity->flags;
        }
    }
    if ((s32)((s64)(flags << 22) >> 32) & 1) {
        ExpeditionArcher_PitchPartToPlayer(entity);
    } else {
        ai_state->part_angle_x = 0.0f;
    }
    flags_now = entity->flags;
    if ((s32)((s64)(flags_now << 18) >> 32) & 1) {
        params = entity->params;
        look_at_target = (s32)(flags_now >> 9) & 1;
        limits[1] = DEG_TO_RAD(params->pitch_limit_deg);
        limits[0] = DEG_TO_RAD(-params->pitch_limit_deg);
        limits[3] = DEG_TO_RAD(params->yaw_limit_deg);
        limits[2] = DEG_TO_RAD(-params->yaw_limit_deg);
        if (look_at_target) {
            EntityAi_LookAtPlayer(entity, ai_state->look_part_angles, look_at_target, limits, 12.0f);
        } else {
            EntityAi_LookAtPlayer(entity, ai_state->look_part_angles, 0, NULL, 8.0f);
        }
    }
}

/* Helper of ExpeditionArcher_Animate: pitches the model part EntityParams.action_frames names towards the
   player, at most 50 degrees either way (work float unk48, rate 10). */
static void ExpeditionArcher_PitchPartToPlayer(Entity *actor) {
    ModelInstance *model = ENTITY_MODEL(actor);
    ModelPart *part = &model->parts[ENTITY_CONTROLLER(actor)->action_frames];
    EntityAiState *sub = &actor->ai_state;
    float angle;

    if (part != NULL) {
        angle = EntityMath_WrapAngle(atan2(player_state.position[1] - model->position.y, actor->distance_to_player));
        if (DEG_TO_RAD(50) < angle) {
            angle = DEG_TO_RAD(50);
        } else if (angle < -DEG_TO_RAD(50)) {
            angle = -DEG_TO_RAD(50);
        }
        EntityMath_EaseAngleToward(&sub->part_angle_x, angle, 10.0f);
        part->rotation_x = sub->part_angle_x;
    }
}

/* The think handler: as EntityAi_Think, with ExpeditionArcher_ThinkCombat for state 2. Before that it
   clears bit 40 of model part 5's unk68 during frames 9 to 46 of animations 23 and 24 and sets it
   otherwise. */
static void ExpeditionArcher_Think(Entity *entity, s32 unknown) {
    if (((u8)entity->animation_index == 0x17 || (u8)entity->animation_index == 0x18) && entity->model->level_tables->blend_left == 0 &&
        entity->model->level_tables->frame >= 9 && entity->model->level_tables->frame < 0x2F) {
        entity->model->parts[5].part_flags &= ~(0x8000L << 25);
    } else {
        entity->model->parts[5].part_flags |= 0x8000L << 25;
    }
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
        EntityAi_ThinkIdle(entity, unknown);
        return;
    case 1:
        EntityAi_ThinkPursue(entity, unknown);
        return;
    case 2:
        ExpeditionArcher_ThinkCombat(entity, unknown);
        return;
    }
    EntityAi_ThinkIdle(entity, unknown);
}

/* The think function of state 2: as EntityAi_ThinkCombat, but ranged record 1 is
   ExpeditionArcher_RangedAttack1 and the other ranged records ExpeditionArcher_RangedAttack; closing in
   never runs. */
static void ExpeditionArcher_ThinkCombat(Entity *entity, s32 unknown) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 direction[4];
    f32 angle;
    s32 side;
    s32 turn;

    ENTITY_FLAG_BITS(entity)->motion_step_mode = 1;
    if (EntityAi_ShouldGuard(entity, 300.0f, DEG_TO_RAD(30))) {
        EntityAi_StartAction(entity, EntityAi_Guard, 1);
        return;
    }
    if (EntityAi_ShouldDodge(entity, 300.0f, DEG_TO_RAD(30))) {
        EntityAi_StartAction(entity, EntityAi_Dodge, 1);
        return;
    }
    side = EntityAi_PickCombatChoice(entity, 0);
    ENTITY_FLAG_BITS(entity)->attacking = FALSE;
    ENTITY_FLAG_BITS(entity)->look_at_target = TRUE;
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
                if (ENTITY_FLAG_BITS(entity)->attack_index == 1) {
                    EntityAi_StartAction(entity, ExpeditionArcher_RangedAttack1, unknown);
                } else {
                    EntityAi_StartAction(entity, ExpeditionArcher_RangedAttack, unknown);
                }
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
            entity->action_timer = rand() % 30 + 0x1E;
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
                ai_state->turn_in_place = ABS(angle) > DEG_TO_RAD(30);
            }
            if (ai_state->turn_in_place != 0) {
                entity->move_command = EntityAi_TurnCommand(entity);
                if (entity->move_command == 0) {
                    ai_state->turn_in_place = 0;
                }
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
    EntityAi_StartAction(entity, ai_state->move, unknown);
}

/* The ranged attack (busy bit 1): as EntityAi_RangedAttack, but it sets flag bit 10 and aims its
   shots at a point 170 below the player's position, within 10 degrees (EntityShot_LaunchAimed).
   ExpeditionArcher_ThinkCombat starts it for records 0 and 2. */
static void ExpeditionArcher_RangedAttack(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    s32 attack_index = ENTITY_FLAG_BITS(entity)->attack_index;
    EntityParamsUnk3D8 *unk3D8 = ENTITY_PARAMS_UNK3D8(entity->params, attack_index);
    f32 target[4];
    s32 elapsed;

    entity->busy_flags |= ENTITY_BUSY_ATTACK;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, attack_index + 0x17, 0, entity->params->motion_blend_frames >> 1, 1);
        if (entity->params->flags & ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE) {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = 0;
        } else {
            s32 kind = 2;

            if (!(ENTITY_PARAMS_BITS(entity)->bit25 && !ENTITY_PARAMS_BITS(entity)->bit26)) {
                kind = 1;
            }
            ENTITY_FLAG_BITS(entity)->motion_step_mode = kind;
        }
        ENTITY_FLAG_BITS(entity)->look_at_target = FALSE;
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
        ENTITY_FLAG_BITS(entity)->attacking = TRUE;
        if (unk3D8->turn_frames >= entity->model->level_tables->frame) {
            EntityAi_TurnToHeading(entity, &entity->ai_state.turn_rate, entity->heading_to_player);
        } else {
            ai_state->turn_rate = 0.0f;
        }
        elapsed = (entity->model->level_tables->frame_fixed - entity->params->motion_speeds[entity->animation_index]) >> 8;
        Vector_Copy(target, player_state.position);
        target[1] -= 170.0f;
        EntityShot_LaunchAimed(entity, unk3D8, elapsed, entity->model->level_tables->frame, target, AIMED_SHOT_TURN_RATE, AIMED_SHOT_TURN_RATE);
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

/* The ranged attack with record 1 (busy bit 1): it takes the record from flag bits 58-59, but
   ExpeditionArcher_ThinkCombat starts it for record 1 only. It turns to the player with flag bit 10
   set; at frame 42 it stops the motion until Entity.heading_to_player no longer changes, the player is nearer
   than 200 or 90 frames have gone by, then plays on and launches the shots at a point 170 below the
   player's position, within 10 degrees (EntityShot_LaunchAimed). */
static void ExpeditionArcher_RangedAttack1(Entity *entity) {
    EntityAiStateKind1740A8 *ai_state = (EntityAiStateKind1740A8 *)&entity->ai_state;
    EntityParamsUnk3D8 *unk3D8 = ENTITY_PARAMS_UNK3D8(entity->params, ENTITY_FLAG_BITS(entity)->attack_index);
    f32 target[4];
    f32 angle;
    s32 elapsed;

    entity->busy_flags |= ENTITY_BUSY_ATTACK;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, ENTITY_FLAG_BITS(entity)->attack_index + 0x17, 0, entity->params->motion_blend_frames >> 1, 1);
        if (entity->params->flags & ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE) {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = (ENTITY_PARAMS_MOVEMENT_KIND(entity->params) == ENTITY_PARAMS_MOVEMENT_1) ? 2 : 1;
        }
        ENTITY_FLAG_BITS(entity)->look_at_target = FALSE;
        entity->action_step++;
        break;
    case 1:
        if (entity->model->level_tables->blend_left == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        ENTITY_FLAG_BITS(entity)->attacking = TRUE;
        EntityAi_TurnToHeading(entity, (f32 *)&ai_state->turn_rate, entity->heading_to_player);
        if (entity->model->level_tables->frame > 0 && unk3D8->attack_end_frame >= entity->model->level_tables->frame) {
            ENTITY_FLAGS(entity) |= ENTITY_FLAG_ATTACK_WINDOW;
        } else {
            ENTITY_FLAGS(entity) &= ~ENTITY_FLAG_ATTACK_WINDOW;
        }
        if (entity->model->level_tables->frame >= 0x2A) {
            entity->action_timer = 0x5A;
            entity->action_step++;
        }
        break;
    case 3:
        EntityAi_TurnToHeading(entity, (f32 *)&ai_state->turn_rate, entity->heading_to_player);
        ENTITY_FLAG_BITS(entity)->attacking = TRUE;
        ENTITY_FLAG_BITS(entity)->animates = FALSE;
        EntityMath_AngleDifference(entity->model->rotation[1], entity->heading_to_player);
        angle = EntityMath_AngleDifference(ai_state->point_y, entity->heading_to_player);
        if (entity->action_timer == 0 || entity->distance_to_player < 200.0f || ABS(angle) <= DEG_TO_RAD(0.6f)) {
            entity->action_step++;
        }
        break;
    case 4:
        ENTITY_FLAG_BITS(entity)->animates = TRUE;
        elapsed = (entity->model->level_tables->frame_fixed - entity->params->motion_speeds[entity->animation_index]) >> 8;
        Vector_Copy(target, player_state.position);
        target[1] -= 170.0f;
        EntityShot_LaunchAimed(entity, unk3D8, elapsed, entity->model->level_tables->frame, target, AIMED_SHOT_TURN_RATE, AIMED_SHOT_TURN_RATE);
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
    ai_state->point_y = entity->heading_to_player;
}

/* The death action (busy bit 0x40): as EntityAi_Death; when the fade starts and
   EntitySpawnInfo.spawn_mode is set, it calls EntityEvent_Respawn for the entity with that spawn id (if its
   flag bit 0 is set) and puts it at this entity's position. */
static void ExpeditionArcher_Death(Entity *entity) {
    entity->busy_flags = ENTITY_BUSY_DEATH;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x10, 0, 5, 1);
        if (entity->params->flags & ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE) {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = 0;
        } else {
            s32 kind = 2;

            if (!(ENTITY_PARAMS_BITS(entity)->bit25 && !ENTITY_PARAMS_BITS(entity)->bit26)) {
                kind = 1;
            }
            ENTITY_FLAG_BITS(entity)->motion_step_mode = kind;
        }
        ENTITY_FLAG_BITS(entity)->animates = TRUE;
        ENTITY_FLAG_BITS(entity)->bit20 = TRUE;
        ENTITY_FLAG_BITS(entity)->dying = 1;
        ENTITY_FLAG_BITS(entity)->no_player_collision = TRUE;
        ENTITY_FLAG_BITS(entity)->no_entity_collision = TRUE;
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
            ENTITY_FLAG_BITS(entity)->animates = FALSE;
            ENTITY_FLAG_BITS(entity)->dying = 2;
            ENTITY_FLAG_BITS(entity)->no_respawn = TRUE;
            entity->action_timer = 20;
            EntityEvent_OnDeath(entity);
            if (entity->spawn_info->spawn_mode != 0) {
                Entity *partner = Entity_GetBySpawnId(entity->spawn_info->spawn_mode);

                if (partner != NULL && ((s32)partner->flags & ENTITY_FLAG_ACTIVE_LIST)) {
                    EntityEvent_Respawn((char *)partner);
                    Vector_Copy(partner->model->position, entity->model->position);
                }
            }
            entity->action_step++;
        }
        break;
    case 3:
        if (entity->action_timer != 0) {
            entity->model->alpha = (f32)entity->model_alpha * (f32)entity->action_timer / 20.0f;
        } else {
            ENTITY_FLAG_BITS(entity)->dying = 3;
            ENTITY_FLAG_BITS(entity)->live = FALSE;
        }
        break;
    }
}
