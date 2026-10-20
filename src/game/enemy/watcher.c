/* Watcher: the behaviour of entity type 73 (EntityParams.entity_type, the index into entity_type_handlers): its set-up
   function, which the table names, and the handlers it installs. Used by: Watcher. The game names no
   monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by
   hit points and experience, with resistances and drops where two monsters share those. */
#include "entity/entity.h"
#include "enemy/enemy.h"
#include "entity/entity_events.h"
#include "math/vu0.h"
#include "player/player.h"
#include "entity/entity_ai.h"
#include "entity/entity_ai_handlers.h"
#include "model/motion.h"

#define ENTITY_WORK_AREA_SIZE 0xA0
// match: the definition takes an Actor, a struct local to EntityAi_Animate.c

static void Watcher_Think(Entity *entity, s32 unknown);
static void Watcher_Move(Entity *entity);
static void Watcher_Death(Entity *entity);

/* Set-up of a type 73 entity: clears its work area and installs the type's handlers. */
void Watcher_Init(Entity *entity) {
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
    EntityAi_SetUpdate(entity, entity_type_handlers[entity->params->entity_type][1]);
    ai_state->think = Watcher_Think;
    ai_state->animate = EntityAi_Animate;
    ai_state->on_hit = NULL;
    ai_state->move = Watcher_Move;
    ai_state->dormant = NULL;
    ai_state->death = Watcher_Death;
    for (i = 4 - 1; i != -1; i--) {
        entity->choice_weights[i] = 0x80;
    }
    for (i = 6 - 1; i != -1; i--) {
        entity->attack_weights[i] = 0x80;
    }
    ENTITY_UNK5C(entity)->as_s32 = 0;
    ENTITY_FLAG_BITS(entity)->no_floor_lift = TRUE;
    EntityAi_StartAction(entity, ai_state->move, 0);
    if (unk138 != NULL) {
        Vector_Copy(ai_state->look_part_angles, unk138 + 0x50);
    }
    ai_state->pose_blend_weight = 0.5f;
    entity->model_alpha = (entity->params->flags & ENTITY_PARAMS_FLAG_HALF_MODEL_ALPHA) ? 0x40 : 0x80;
    entity->model->alpha = entity->model_alpha;
    Motion_ClearPartSlots(entity->model);
    Motion_StepModel(ENTITY_MODEL(entity), 0, 0);
}

/* The think handler: no states and no actions. Every tenth frame updates what the entity knows of
   the player; while it is not busy and EntityAi_CanUseRanged finds the player in reach of ranged record 0
   (and flag bit 63 is clear) it calls Attack_DrainPlayerMp with EntityParams.action_frames, sets work byte unk7A
   and flag bit 9 and plays animation 2, else clears them and plays animation 0. Calls EntityEvent_StartCueAlways
   with 1 when unk7A changes. */
static void Watcher_Think(Entity *entity, s32 unknown) {
    EntityAiState *ai_state = &entity->ai_state;
    s8 previous;

    if (frame_counter % 10U == entity->spawn_index % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (entity->busy_flags == 0) {
        previous = ai_state->step_index;
        if (!(s32)(entity->flags >> 63) && EntityAi_CanUseRanged(entity, 0)) {
            Attack_DrainPlayerMp((s16)entity->params->action_frames);
            ai_state->step_index = 1;
            ENTITY_FLAG_BITS(entity)->look_at_target = TRUE;
            Motion_Start(entity, 2, 0, entity->params->motion_blend_frames, 0);
        } else {
            ai_state->step_index = 0;
            ENTITY_FLAG_BITS(entity)->look_at_target = FALSE;
            Motion_Start(entity, 0, 0, entity->params->motion_blend_frames, 0);
        }
        if (previous != ai_state->step_index) {
            EntityEvent_StartCueAlways(entity, 1, 0);
        }
    }
}

/* The move handler: this type stays where it is. While work byte unk7A is set it keeps animation 2
   and calls EntityEvent_StartCueAlways with 0 every EntityParams.far_distance frames (counted in work word unk68);
   else it keeps animation 0. */
static void Watcher_Move(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;

    if (ai_state->step_index != 0) {
        Motion_Start(entity, 2, 0, entity->params->motion_blend_frames, 0);
        if (ai_state->pause_timer <= 0) {
            EntityEvent_StartCueAlways(entity, 0, 0);
            ai_state->pause_timer = entity->params->far_distance;
        }
    } else {
        Motion_Start(entity, 0, 0, entity->params->motion_blend_frames, 0);
        ai_state->pause_timer = 0;
    }
    ai_state->pause_timer--;
}

/* The death action (busy bit 0x40): no animation; calls EntityEvent_StartCueAlways with 2, then the fade over 20
   frames and the removal as EntityAi_Death. */
static void Watcher_Death(Entity *entity) {
    entity->busy_flags = ENTITY_BUSY_DEATH;
    switch (entity->action_step) {
    case 0:
        EntityEvent_StartCueAlways(entity, 2, 0);
        ENTITY_FLAG_BITS(entity)->animates = TRUE;
        ENTITY_FLAG_BITS(entity)->bit20 = TRUE;
        ENTITY_FLAG_BITS(entity)->dying = 1;
        ENTITY_FLAG_BITS(entity)->look_at_target = FALSE;
        ENTITY_FLAG_BITS(entity)->animates = FALSE;
        ENTITY_FLAG_BITS(entity)->dying = 2;
        ENTITY_FLAG_BITS(entity)->no_respawn = TRUE;
        ENTITY_FLAG_BITS(entity)->no_player_collision = TRUE;
        ENTITY_FLAG_BITS(entity)->no_entity_collision = TRUE;
        entity->action_timer = 0x14;
        EntityEvent_OnDeath(entity);
        entity->action_step++;
        break;
    case 1:
        if (entity->action_timer != 0) {
            entity->model->alpha = (f32)entity->model_alpha * (f32)entity->action_timer / 20.0f;
        } else {
            ENTITY_FLAG_BITS(entity)->dying = 3;
            ENTITY_FLAG_BITS(entity)->live = FALSE;
        }
        break;
    }
}
