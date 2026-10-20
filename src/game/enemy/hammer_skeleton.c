/* HammerSkeleton: the behaviour of entity type 37 (EntityParams.entity_type, the index into entity_type_handlers): its
   set-up function, which the table names, and the handlers it installs. Used by: chr.prm record 154,
   model c0154 (a skeleton holding a small hammer), whose one spawn in 007.map stands beside the bell
   (enemy/bell.c). It gives no experience and dies by itself when the player enters one of two
   boxes, then starts its spawn's script. A descriptive name: the game and the guides name it nowhere. */
#include "entity/entity.h"
#include "map/map.h"
#include "effect/effect.h"
#include "core/game.h"
#include "enemy/enemy.h"
#include "entity/entity_ai_handlers.h"
#include "script/script.h"

#define ENTITY_WORK_AREA_SIZE 0xA0
void EntityAi_SetUpdate(char *, s32);
int EntityAi_StartAction(Entity *, void (*)(Entity *), int);
void Motion_ClearPartSlots(EntityModel *);
int Motion_StepModel(EntityModel *, s32, s32);
// match: the definition takes an Actor, a struct local to EntityAi_Animate.c
void EntityAi_Animate(Entity *);
extern s32 entity_type_handlers[][2];
extern float player_state[];
int Motion_Start(void *, int, int, u32, int);

static void HammerSkeleton_Think(Entity *entity, s32 unknown);
static void HammerSkeleton_Death(Entity *entity);

/* Set-up of a type 37 entity: clears its work area and installs the type's handlers. */
void HammerSkeleton_Init(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    u8 *unkBC = entity->choice_weights;
    u8 *unkC0 = entity->attack_weights;
    s32 i;

    // match: in the three loops the pointer has to be formed inside the loop
    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        u8 *dest = (u8 *)&entity->ai_state;

        dest[i] = 0;
    }
    EntityAi_SetUpdate((char *)entity, entity_type_handlers[entity->params->entity_type][1]);
    ai_state->think = HammerSkeleton_Think;
    ai_state->animate = EntityAi_Animate;
    ai_state->on_hit = EntityAi_OnHit;
    ai_state->move = EntityAi_Move;
    ai_state->dormant = EntityAi_Dormant;
    ai_state->death = EntityAi_Death;
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
    ai_state->pose_blend_weight = 0.5f;
    entity->model_alpha = (entity->params->flags & ENTITY_PARAMS_FLAG_HALF_MODEL_ALPHA) ? 0x40 : 0x80;
    entity->model->alpha = entity->model_alpha;
    Motion_ClearPartSlots(entity->model);
    Motion_StepModel(entity->model, 0, 0);
}

/* The think handler: no states. While the entity is not busy it starts HammerSkeleton_Death when the
   player is inside either of two fixed boxes, else the move action. */
static void HammerSkeleton_Think(Entity *entity, s32 unknown) {
    EntityAiState *ai_state = &entity->ai_state;

    if (entity->busy_flags == 0) {
        if ((-6200.0f <= player_state[0] && player_state[0] <= -1400.0f &&
             -50.0f <= player_state[1] && player_state[1] <= 450.0f &&
             -4200.0f <= player_state[2] && player_state[2] <= -600.0f) ||
            (-6200.0f <= player_state[0] && player_state[0] <= -2600.0f &&
             -50.0f <= player_state[1] && player_state[1] <= 450.0f &&
             -600.0f <= player_state[2] && player_state[2] <= 1000.0f)) {
            EntityAi_StartAction(entity, HammerSkeleton_Death, unknown);
        } else {
            EntityAi_StartAction(entity, ai_state->move, unknown);
        }
    }
}

/* A death action (busy bit 0x40) that HammerSkeleton_Think starts; the death slot holds
   EntityAi_Death. Animation 16, the fade over 20 frames and the removal, as EntityAi_Death with
   other flag settings; it frees the entity's effects at the start, and when the motion has ended
   calls Game_CountSpawnDeath and starts script EntitySpawnInfo.death_script_id where that one calls EntityEvent_OnDeath.
   */
static void HammerSkeleton_Death(Entity *entity) {
    entity->busy_flags = ENTITY_BUSY_DEATH;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x10, 0, 5, 1);
        ENTITY_FLAG_BITS(entity)->motion_step_mode = 1;
        ENTITY_FLAG_BITS(entity)->animates = TRUE;
        ENTITY_FLAG_BITS(entity)->bit20 = TRUE;
        ENTITY_FLAG_BITS(entity)->dying = 1;
        Effect_FreeForOwner(entity);
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
            Game_CountSpawnDeath(map_slots[entity->group_id].tag, entity->spawn_info - map_spawn_infos[entity->group_id]);
            if (entity->spawn_info->death_script_id != 0 && entity->group_id == current_map_slot) {
                Script_Start(entity->spawn_info->death_script_id, 0);
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
