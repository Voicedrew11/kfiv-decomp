/* DarkOne: the behaviour of entity type 75 (EntityParams.entity_type, the index into entity_type_handlers): its set-up
   function, which the table names, and the handlers it installs. Used by: the Dark One, the last boss,
   chr.prm record 99 (ENTITY_TYPE_LAST_BOSS in Attack_HitEntity), whose one spawn is in 040.map, the end of
   the Dark Reality. The game's own text calls him the Dark One. Attack_HitEntity gives him no damage:
   each weapon hit counts dark_one_hits_left down and throws the player back, magic does nothing, and the hit
   that takes the count to 0 kills him; Prima's official guide (2004) describes the same fight. */
#include "entity/entity.h"
#include "effect/shot.h"
#include "enemy/enemy.h"
#include "entity/entity_ai_choice.h"
#include "entity/entity_ai_handlers.h"
#include "entity/entity_events.h"
#include "math/vu0.h"
#include "player/player.h"
#include "sound/sound_game.h"
#include "entity/entity_ai.h"
#include "model/motion.h"

#define ENTITY_WORK_AREA_SIZE 0xA0

static void DarkOne_Think(Entity *entity, s32 run_now);
static void DarkOne_Death(Entity *entity);
static void DarkOne_DeathByOtherWeapon(Entity *entity);
static void DarkOne_DeathByMoonlightSword(Entity *entity);

/* Set-up of a type 75 entity: clears its work area and installs the type's handlers. */
void DarkOne_Init(Entity *entity) {
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
    ai_state->think = DarkOne_Think;
    ai_state->animate = EntityAi_Animate;
    ai_state->on_hit = EntityAi_OnHit;
    ai_state->move = EntityAi_Move;
    ai_state->dormant = EntityAi_Dormant;
    ai_state->death = DarkOne_Death;
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
    Motion_StepModel(ENTITY_MODEL(entity), 0, 0);
    ENTITY_FLAG_BITS(entity)->no_entity_collision = TRUE;
    ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
    ENTITY_FLAG_BITS(entity)->not_targetable = TRUE;
    ai_state->step_index = dark_one_hits_left;
}

/* The think handler: as EntityAi_Think without the walk back to the spawn point, with
   EntityAi_ThinkCombat for states 1 and 2. Every third frame it calls EntityEvent_StartEffectAtPoint with
   EntityParams.pitch_limit_deg, unk432 or unk434, by dark_one_hits_left (3, 2, other). In a frame with flag bit 43
   set it calls EntityEvent_StartEffectAtPoint with 0x13D and clears the bit; then, with dark_one_hits_left at 0 or less, it
   sets Entity.hit_flash_frames to 6 and starts DarkOne_Death, and else, when dark_one_hits_left has changed (work
   byte unk7A keeps it), calls EntityEvent_StartCue and Sound_PlayMapAtRange. */
static void DarkOne_Think(Entity *entity, s32 run_now) {
    EntityAiState *ai_state = &entity->ai_state;

    if (frame_counter % 3U == 0) {
        switch (dark_one_hits_left) {
        case 3:
            EntityEvent_StartEffectAtPoint((char **)entity, entity->params->pitch_limit_deg, 0xFF, (int)entity->model->rotation);
            break;
        case 2:
            EntityEvent_StartEffectAtPoint((char **)entity, entity->params->yaw_limit_deg, 0xFF, (int)entity->model->rotation);
            break;
        default:
            EntityEvent_StartEffectAtPoint((char **)entity, (s16)entity->params->action_frames, 0xFF, (int)entity->model->rotation);
            break;
        }
    }
    if (ENTITY_FLAG_BITS(entity)->hit_by_player) {
        EntityEvent_StartEffectAtPoint((char **)entity, 0x13D, 0xFF, (int)entity->model->rotation);
        ENTITY_FLAG_BITS(entity)->hit_by_player = FALSE;
        if (dark_one_hits_left <= 0) {
            entity->hit_flash_frames = 6;
            EntityAi_StartAction(entity, DarkOne_Death, run_now);
            return;
        }
        if (ai_state->step_index != dark_one_hits_left) {
            EntityEvent_StartCue(entity, entity->params->near_distance, entity->params->far_distance, 0);
            Sound_PlayMapAtRange((u8)entity->params->type_param_0, (u8)entity->params->type_param_1, entity->model->position,
                                 entity->params->sound_id);
            ai_state->step_index = dark_one_hits_left;
        }
    }
    if (frame_counter % 10U == entity->spawn_index % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (entity->busy_flags == 0) {
        EntityAi_StepState(entity);
        switch (entity->state) {
        case 1:
        case 2:
            EntityAi_ThinkCombat(entity, run_now);
            break;
        case 0:
        default:
            EntityAi_ThinkIdle(entity, run_now);
            break;
        }
    }
}

/* The death action in the slot: it only starts one of the two others, DarkOne_DeathByMoonlightSword with
   flag bit 29 set when the weapon in hand (PlayerState.weapon, player_weapon[0]) is 0x2E, the Moonlight Sword
   (item 196), else DarkOne_DeathByOtherWeapon with flag bit 29 clear.
   DarkOne_Think starts it too. */
static void DarkOne_Death(Entity *entity) {
    if (player_weapon[0] == 0x2E) {
        ENTITY_FLAG_BITS(entity)->bit29 = TRUE;
        EntityAi_StartAction(entity, DarkOne_DeathByMoonlightSword, 1);
    } else {
        ENTITY_FLAG_BITS(entity)->bit29 = FALSE;
        EntityAi_StartAction(entity, DarkOne_DeathByOtherWeapon, 1);
    }
}

/* A death action (busy bit 0x40), started by DarkOne_Death: plays animation 16 as
   EntityAi_Death does, calling EntityEvent_OnDeath at the start, and when the animation has ended sets
   the flags of the fade and stops there: no fade and no removal. */
static void DarkOne_DeathByOtherWeapon(Entity *entity) {
    entity->busy_flags = ENTITY_BUSY_DEATH;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x10, 0, 5, 1);
        if (entity->params->flags & ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE) {
            ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_IN_PLACE;
        } else {
            s32 kind = 2;

            if (!(ENTITY_PARAMS_BITS(entity)->bit25 && !ENTITY_PARAMS_BITS(entity)->bit26)) {
                kind = 1;
            }
            ENTITY_FLAG_BITS(entity)->motion_step_mode = kind;
        }
        if (!ENTITY_PARAMS_BITS(entity)->bit25 && ENTITY_PARAMS_BITS(entity)->bit26) {
            entity->collision_radius = 20.0f;
        }
        ENTITY_FLAG_BITS(entity)->animates = TRUE;
        ENTITY_FLAG_BITS(entity)->bit20 = TRUE;
        ENTITY_FLAG_BITS(entity)->dying = 1;
        ENTITY_FLAG_BITS(entity)->no_floor_lift = FALSE;
        ENTITY_FLAG_BITS(entity)->look_at_target = FALSE;
        EntityEvent_OnDeath(entity);
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
            ENTITY_FLAG_BITS(entity)->no_player_collision = TRUE;
            ENTITY_FLAG_BITS(entity)->no_entity_collision = TRUE;
            entity->action_step++;
        }
        break;
    }
}

/* A death action (busy bit 0x40), started by DarkOne_Death: as DarkOne_DeathByOtherWeapon with
   animation 0 in place of 16. */
static void DarkOne_DeathByMoonlightSword(Entity *entity) {
    entity->busy_flags = ENTITY_BUSY_DEATH;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0, 0, 5, 1);
        ENTITY_FLAG_BITS(entity)->motion_step_mode = MOTION_STEP_IN_PLACE;
        ENTITY_FLAG_BITS(entity)->animates = TRUE;
        ENTITY_FLAG_BITS(entity)->bit20 = TRUE;
        ENTITY_FLAG_BITS(entity)->dying = 1;
        ENTITY_FLAG_BITS(entity)->no_floor_lift = FALSE;
        ENTITY_FLAG_BITS(entity)->look_at_target = FALSE;
        EntityEvent_OnDeath(entity);
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
            ENTITY_FLAG_BITS(entity)->no_player_collision = TRUE;
            ENTITY_FLAG_BITS(entity)->no_entity_collision = TRUE;
            entity->action_step++;
        }
        break;
    }
}
