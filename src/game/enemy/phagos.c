/* Phagos: the behaviour of entity type 14 (EntityParams.entity_type, the index into entity_type_handlers): its set-up
   function, which the table names, and the handlers it installs. Used by: Phagos. The game names no monsters;
   these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by hit points
   and experience, with resistances and drops where two monsters share those. */
#include "entity/entity.h"
#include "common.h"
#include "enemy/enemy.h"
#include "entity/entity_ai_handlers.h"
#include "entity/entity_collision.h"
#include "entity/entity_events.h"
#include "entity/entity_math.h"
#include "math/vu0.h"
#include <stdlib.h>
#include "entity/entity_ai.h"
#include "entity/entity_ai_choice.h"
#include "model/motion.h"

/* EntityAiState action_step of Phagos_RunAwayAndRemove */
#define PHAGOS_ACTION_STEP_RUN_AWAY     1
#define PHAGOS_ACTION_STEP_RUN_TO_POINT 2
#define PHAGOS_ACTION_STEP_FADE_OUT     3

static void Phagos_Animate(Entity *entity);
static void Phagos_OnHit(Entity *entity);
static void Phagos_Think(Entity *entity, s32 run_now);
static void Phagos_RunAwayAndRemove(Entity *entity);

/* Set-up of a type 14 entity: clears its work area and installs the type's handlers. */
void Phagos_Init(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    s32 i;

    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        ENTITY_WORK_BYTES(entity)[i] = 0;
    }
    EntityAi_SetUpdate(entity, entity_type_handlers[entity->params->entity_type][1]);
    ai_state->think = Phagos_Think;
    ai_state->animate = Phagos_Animate;
    ai_state->on_hit = Phagos_OnHit;
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

/* The animate handler: as EntityAi_Animate without the test of the EntityParams kind bits and
   without the head turn. */
static void Phagos_Animate(Entity *entity) {
    u32 blend_frames; // match: converted to float as unsigned
    EntityAiState *ai_state;

    ai_state = &entity->ai_state;
    if (ENTITY_FLAG_BITS(entity)->animates) {
        blend_frames = entity->model->level_tables->blend_left;
        if (blend_frames != 0) {
            ENTITY_FLAG_BITS(entity)->motion_ended = Motion_BlendToPose(ENTITY_MODEL(entity), (entity->busy_flags & (ENTITY_BUSY_GUARD | ENTITY_BUSY_REACTION_LIGHT)) ? ai_state->pose_blend_weight : 1.0f / (f32)blend_frames);
        } else {
            ENTITY_FLAG_BITS(entity)->motion_ended = Motion_Step(entity, ENTITY_FLAG_BITS(entity)->motion_step_mode);
        }
    }
}

/* The on_hit handler: every hit starts EntityAi_ReactionLight, whatever its strength, and puts the
   entity in state 3; the target and flag bits 9 and 10 are left as they are. */
static void Phagos_OnHit(Entity *entity) {
    EntityAi_LowerWeightsAfterHit(entity);
    EntityAi_SetReaction(entity, EntityAi_ReactionLight, 0);
    EntityAi_SetState(entity, 3);
}

/* The think handler: no states. While the entity is not busy it starts Phagos_RunAwayAndRemove; it
   tests for state 3, but both branches start the same action. */
static void Phagos_Think(Entity *entity, s32 run_now) {
    if (frame_counter % 10 == entity->spawn_index % 10) {
        EntityAi_SensePlayer(entity, 2);
    }
    if (entity->busy_flags == 0) {
        if (entity->state == ENTITY_STATE_HELD) {
            EntityAi_StartAction(entity, Phagos_RunAwayAndRemove, run_now);
        } else {
            EntityAi_StartAction(entity, Phagos_RunAwayAndRemove, run_now);
        }
    }
}

/* An action (busy bit 0x1000), the only one Phagos_Think starts: plays animation 4 and turns
   the entity to a heading drawn between 90 and 269 degrees from Entity.heading_to_player (work float unk48).
   When flag bit 52 is set it starts effect EntityParams.pitch_limit_deg at model point 0xFF, fades the model
   out over 20 frames and removes the entity. When EntityCollision_RayHitsPropWall finds a wall ahead within the
   radius it starts the same effect, sets flag bit 28, and removes the entity once it is more than
   four radii from that place. */
static void Phagos_RunAwayAndRemove(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 bounds[4];
    f32 direction[4];
    f32 heading;

    entity->busy_flags |= ENTITY_BUSY_1000;
    switch (entity->action_step) {
    case ACTION_STEP_START:
        Motion_Start(entity, 4, 0, entity->params->motion_blend_frames, 1);
        if (entity->params->flags & ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE) {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_IN_PLACE;
        } else {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_MOVE;
        }
        ai_state->part_angle_x = entity->heading_to_player + HALF_PI + DEG_TO_RAD((f32)(rand() % 180));
        ai_state->part_angle_x = EntityMath_WrapAngle(ai_state->part_angle_x);
        ENTITY_FLAG_BITS(entity)->no_wall_collision = TRUE;
        ENTITY_FLAG_BITS(entity)->no_entity_collision = TRUE;
        entity->action_step++;
        break;
    case PHAGOS_ACTION_STEP_RUN_AWAY:
        heading = entity->model->rotation[1];
        EntityMath_EaseAngleToward(&heading, ai_state->part_angle_x, 10.0f);
        entity->turn_delta_yaw += heading - entity->model->rotation[1];
        bounds[0] = entity->model->position[0] - entity->radius;
        bounds[1] = entity->model->position[0] + entity->radius;
        bounds[2] = entity->model->position[2] - entity->radius;
        bounds[3] = entity->model->position[2] + entity->radius;
        EntityMath_FacingVector((Vector4 *)direction, entity->model->rotation[1]);
        if (ENTITY_FLAG_BITS(entity)->ledge_stop) {
            ENTITY_FLAG_BITS(entity)->animates = FALSE;
            ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
            entity->action_timer = 20;
            EntityEvent_StartEffectAtPoint((char **)entity, entity->params->pitch_limit_deg, 0xFF, (int)entity->model->rotation);
            entity->action_step = PHAGOS_ACTION_STEP_FADE_OUT;
        } else if (EntityCollision_RayHitsPropWall(entity->model->position, direction, entity->radius, bounds)) {
            Vector_Copy(ai_state->target_point, entity->model->position);
            ENTITY_FLAG_BITS(entity)->no_floor_lift = TRUE;
            EntityEvent_StartEffectAtPoint((char **)entity, entity->params->pitch_limit_deg, 0xFF, (int)entity->model->rotation);
            entity->action_step++;
        }
        break;
    case PHAGOS_ACTION_STEP_RUN_TO_POINT:
        if (EntityMath_DistanceXZ(entity->model->position, ai_state->target_point) > entity->radius * 4.0f) {
            ENTITY_FLAG_BITS(entity)->live = FALSE;
        }
        break;
    case PHAGOS_ACTION_STEP_FADE_OUT:
        if (entity->action_timer != 0) {
            entity->model->alpha = (f32)entity->model_alpha * (f32)entity->action_timer / 20.0f;
        } else {
            ENTITY_FLAG_BITS(entity)->live = FALSE;
        }
        break;
    }
}
