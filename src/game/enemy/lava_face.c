/* LavaFace: the behaviour of entity type 32 (EntityParams.entity_type, the index into entity_type_handlers): its set-up
   function, which the table names, and the handlers it installs. Used by: Lava Face. The game names no
   monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by
   hit points and experience, with resistances and drops where two monsters share those. */
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

#define ENTITY_WORK_AREA_SIZE 0xA0
// match: the definition takes an Actor, a struct local to EntityAi_Animate.c

static void LavaFace_Think(Entity *entity, s32 run_now);

/* Set-up of a type 32 entity: clears its work area and installs the type's handlers. */
void LavaFace_Init(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    u8 *unkBC = entity->choice_weights;
    u8 *unkC0 = entity->attack_weights;
    s32 i;

    // match: in the three loops the pointer has to be formed inside the loop
    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        u8 *dest = (u8 *)&entity->ai_state;

        dest[i] = 0;
    }
    EntityAi_SetUpdate(entity, entity_type_handlers[entity->params->entity_type][1]);
    ai_state->think = LavaFace_Think;
    ai_state->animate = EntityAi_Animate;
    ai_state->on_hit = EntityAi_OnHit;
    ai_state->dormant = EntityAi_Dormant;
    ai_state->death = EntityAi_Death;
    ai_state->move = NULL;
    for (i = 4 - 1; i != -1; i--) {
        entity->choice_weights[i] = 0x80;
    }
    for (i = 6 - 1; i != -1; i--) {
        entity->attack_weights[i] = 0x80;
    }
    ENTITY_UNK5C(entity)->as_s32 = 0;
    ENTITY_FLAG_BITS(entity)->no_floor_lift = TRUE;
    if (entity->spawn_info->spawn_flags & (0x8000L << 43)) {
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

/* The think handler: each time the timer Entity.action_timer runs out (and EntityParams.action_frames is not 0)
   launches shot EntityParams.pitch_limit_deg from model point unk432, aimed at the player, and sets the
   timer to unk434 plus 0 to 2 frames; keeps animation 0. */
static void LavaFace_Think(Entity *entity, s32 run_now) {
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

    if (entity->action_timer == 0 && (s16)entity->params->action_frames != 0) {
        from = Model_GetSkinnedBonePoint(ENTITY_MODEL(entity), entity->params->yaw_limit_deg);
        tip = Model_GetSkinnedBonePoint(ENTITY_MODEL(entity), entity->params->yaw_limit_deg + 1);
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
        EntityAi_LaunchShot(entity, entity->params->pitch_limit_deg, from, angles);
        EntityEvent_StartCue(entity, entity->params->type_param_0, entity->params->type_param_1, entity->params->type_param_2);
        entity->action_timer = entity->params->action_frames + rand() % 3;
    }
    Motion_Start(entity, 0, 0, entity->params->motion_blend_frames, 0);
    EntityAi_StartAction(entity, ai_state->move, run_now);
}
