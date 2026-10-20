/* Fadin: the behaviour of entity type 80 (EntityParams.entity_type, the index into entity_type_handlers): its set-up
   function, which the table names, and the handlers it installs. Used by: Fadin. The game names no monsters;
   these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by hit points
   and experience, with resistances and drops where two monsters share those. */
#include "entity/entity.h"
#include "entity/entity_ai_handlers.h"
#include "entity/entity_events.h"
#include <math.h>
#include "math/vu0.h"
#include "model/model.h"
#include <stdlib.h>
#include "entity/entity_ai.h"
#include "entity/entity_math.h"
#include "model/motion.h"
#include "player/player.h"
#include "save/save_state.h"

static void Fadin_Think(Entity *entity, s32 unknown);

/* Set-up of a type 80 entity: clears its work area and installs the type's handlers. */
void Fadin_Init(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    s32 i;

    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        ENTITY_WORK_BYTES(entity)[i] = 0;
    }
    EntityAi_SetUpdate(entity, entity_type_handlers[entity->params->entity_type][1]);
    ai_state->think = Fadin_Think;
    ai_state->animate = EntityAi_Animate;
    ai_state->on_hit = EntityAi_OnHit;
    ai_state->dormant = EntityAi_Dormant;
    ai_state->death = EntityAi_Death;
    ai_state->move = NULL;
    for (i = 3; i != -1; i--) {
        entity->choice_weights[i] = 0x80;
    }
    for (i = 5; i != -1; i--) {
        entity->attack_weights[i] = 0x80;
    }
    ENTITY_FLAG_BITS(entity)->no_floor_lift = 1;
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

/* The think handler: launches a shot at the player each time the timer Entity.action_timer runs out, as
   type 32's; one launch in three is shot EntityParams.pitch_limit_deg, the others shot 0xCD. Starts the
   death action once master_fadin_killed[0] is set. */
static void Fadin_Think(Entity *entity, s32 unknown) {
    EntityAiState *ai_state = &entity->ai_state;
    f32 angles[4];
    f32 target[4];
    f32 *from;
    f32 *tip;
    f32 delta_x;
    f32 delta_y;
    f32 delta_z;
    f32 pitch;
    f32 yaw;

    if (master_fadin_killed[0] != 0) {
        EntityAi_StartAction(entity, ai_state->death, 0);
        return;
    }
    if (entity->action_timer == 0) {
        from = (f32 *)Model_GetSkinnedBonePoint((char *)entity->model, entity->params->yaw_limit_deg);
        tip = (f32 *)Model_GetSkinnedBonePoint((char *)entity->model, entity->params->yaw_limit_deg + 1);
        if (from == NULL || tip == NULL) {
            return;
        }
        if (entity->params->yaw_limit_deg != 0xFF) {
            delta_x = tip[0] - from[0];
            delta_y = tip[1] - from[1];
            delta_z = tip[2] - from[2];
            angles[0] = -atan2f(delta_y, sqrtf(delta_x * delta_x + delta_z * delta_z));
            angles[1] = EntityMath_WrapAngle(-atan2f(delta_z, delta_x) + HALF_PI);
            angles[2] = angles[3] = 0.0f;
        } else {
            Vector_Copy(angles, entity->model->rotation);
            angles[1] = EntityMath_WrapAngle(angles[1] + PI);
        }
        Vector_Copy(target, &player_state.position);
        target[1] -= 190.0f;
        pitch = EntityMath_WrapAngle(-atan2(target[1] - from[1], EntityMath_DistanceXZ(from, target)));
        yaw = EntityMath_AngleXZ(from, target);
        angles[0] = EntityMath_Approach(angles[0], pitch, entity->params->near_distance);
        angles[1] = EntityMath_Approach(angles[1], yaw, entity->params->far_distance);
        if (ai_state->step_index % 3 == entity->spawn_index % 3U) {
            EntityAi_LaunchShot(entity, entity->params->pitch_limit_deg, from, angles);
        } else {
            EntityAi_LaunchShot(entity, 0xCD, from, angles);
        }
        ai_state->step_index++;
        if (ai_state->step_index < 0) {
            ai_state->step_index = 0;
        }
        EntityEvent_StartCue(entity, entity->params->type_param_0, entity->params->type_param_1, entity->params->type_param_2);
        entity->action_timer = entity->params->action_frames + rand() % 3;
    }
    Motion_Start(entity, 0, 0, entity->params->motion_blend_frames, 0);
    EntityAi_StartAction(entity, ai_state->move, unknown);
}
