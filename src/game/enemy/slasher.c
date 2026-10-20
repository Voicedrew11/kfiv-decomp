/* Slasher: the behaviour of entity type 26 (EntityParams.entity_type, the index into entity_type_handlers): its set-up
   function, which the table names, and the handlers it installs. Used by: Slasher. The game names no
   monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by
   hit points and experience, with resistances and drops where two monsters share those. */
#include "entity/entity.h"
#include "common.h"
#include "effect/effect.h"
#include "enemy/enemy.h"
#include "entity/entity_ai_handlers.h"
#include "entity/entity_math.h"
#include "math/vu0.h"
#include "model/model.h"
#include "model/motion.h"
#include "entity/entity_ai.h"
#include "entity/entity_ai_choice.h"

#define ENTITY_WORK_AREA_SIZE 0xA0

static void Slasher_OnHit(Entity *entity);
static void Slasher_Think(Entity *entity, s32 unknown);
static void Slasher_ThinkStandStill(Entity *entity, s32 unknown);
static void Slasher_WaitForPlayerNear(Entity *entity);
static void Slasher_AlertSameKind(Entity *entity);

/* Set-up of a type 26 entity: clears its work area and installs the type's handlers. */
void Slasher_Init(Entity *entity) {
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
    ai_state->think = Slasher_Think;
    ai_state->animate = EntityAi_Animate;
    ai_state->on_hit = Slasher_OnHit;
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
        EntityAi_StartAction(entity, Slasher_WaitForPlayerNear, 1);
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

/* The on_hit handler: as EntityAi_OnHit, but a light hit starts EntityAi_ReactionHeavy too
   (EntityAi_ReactionRewind while flag bit 19 is set); it then sets work byte unk7A and flag bits 32
   and 33 and calls Slasher_AlertSameKind. */
static void Slasher_OnHit(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;
    u64 flags;
    u64 hit_strength;

    EntityAi_LowerWeightsAfterHit(entity);
    ENTITY_UNK5C(entity)->as_s32 = 0;
    flags = entity->flags & ~ENTITY_FLAG_LOOK_AT_TARGET;
    entity->flags = flags;
    hit_strength = ENTITY_FLAG_BITS(entity)->hit_strength;
    if (hit_strength == 1) {
        if (ENTITY_FLAG_BITS(entity)->attack_window) {
            EntityAi_SetReaction(entity, EntityAi_ReactionRewind, 0);
        } else {
            EntityAi_SetReaction(entity, EntityAi_ReactionHeavy, 0);
        }
    } else if (hit_strength == 2) {
        EntityAi_SetReaction(entity, EntityAi_ReactionHeavy, 0);
    }
    EntityAi_SetState(entity, 2);
    ai_state->step_index = 1;
    ENTITY_FLAG_BITS(entity)->player_in_reach = 1;
    ENTITY_FLAG_BITS(entity)->player_noticed = 1;
    Slasher_AlertSameKind(entity);
}

/* The think handler: as EntityAi_Think, but states 1 and 2 both have EntityAi_ThinkCombat (and
   clear work byte unk7A) and state 0 starts Slasher_WaitForPlayerNear. Every frame it starts effect
   EntityParams.type_param_0 at the points Model_GetSkinnedBonePoint gives for 30 and 31. */
static void Slasher_Think(Entity *entity, s32 unknown) {
    EntityAiState *ai_state = &entity->ai_state;
    char *part;

    if (frame_counter % 10U == entity->spawn_index % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (entity->busy_flags == 0) {
        EntityAi_CheckFarFromSpawn(entity);
        EntityAi_StepState(entity);
        if (ENTITY_FLAG_BITS(entity)->far_from_spawn) {
            EntityAi_ThinkReturn(entity, unknown);
        } else {
            switch (entity->state) {
            case 1:
            case 2:
                EntityAi_ThinkCombat(entity, unknown);
                ai_state->step_index = 0;
                break;
            case 0:
            default:
                EntityAi_StartAction(entity, Slasher_WaitForPlayerNear, unknown);
                break;
            }
        }
    }
    part = Model_GetSkinnedBonePoint((char *)entity->model, 0x1E);
    Effect_StartSpawner(entity->params->type_param_0, part, entity->model->rotation, entity);
    part = Model_GetSkinnedBonePoint((char *)entity->model, 0x1F);
    Effect_StartSpawner(entity->params->type_param_0, part, entity->model->rotation, entity);
}

/* A think function nothing refers to: gives no movement command, lets the head follow the target (flag 9) and
   starts the move action. */
static void Slasher_ThinkStandStill(Entity *entity, s32 unknown) {
    entity->move_command = 0;
    ENTITY_FLAG_BITS(entity)->motion_step_mode = 1;
    ENTITY_FLAG_BITS(entity)->look_at_target = TRUE;
    EntityAi_StartAction(entity, entity->ai_state.move, unknown);
}

/* An action (busy bit 0x1000) that the set-up function and state 0 of Slasher_Think start:
   keeps animation 0 until the player is within EntityParams.near_distance or work byte unk7A is set; then
   it sets flag bits 32 and 33 and unk7A, calls Slasher_AlertSameKind and thinks. */
static void Slasher_WaitForPlayerNear(Entity *entity) {
    EntityAiState *ai_state = &entity->ai_state;

    entity->busy_flags |= ENTITY_BUSY_1000;
    Motion_Start(entity, 0, 0, entity->params->motion_blend_frames, 0);
    ENTITY_FLAG_BITS(entity)->motion_step_mode = 1;
    ENTITY_FLAG_BITS(entity)->look_at_target = TRUE;
    if (entity->distance_to_player < entity->params->near_distance || ai_state->step_index != 0) {
        ENTITY_FLAG_BITS(entity)->player_in_reach = TRUE;
        ENTITY_FLAG_BITS(entity)->player_noticed = TRUE;
        ai_state->step_index = 1;
        Slasher_AlertSameKind(entity);
        entity->busy_flags = 0;
        entity->move_command = 0;
        entity->action_timer = 0;
        if (ENTITY_FLAG_BITS(entity)->own_think) {
            ai_state->think(entity, 0);
        } else {
            EntityAi_StartAction(entity, ai_state->move, 0);
        }
    }
}

/* Sets work byte unk7A of every other live entity with the same params within EntityParams.far_distance (on the
   ground) of this one, which ends its Slasher_WaitForPlayerNear. */
static void Slasher_AlertSameKind(Entity *entity) {
    Entity *other_entity = active_entities_head;
    while (other_entity != NULL) {
        if (entity->params == other_entity->params) {
            if (*(int *)&other_entity->flags & ENTITY_FLAG_ACTIVE_LIST) {
                u64 other_flags = other_entity->flags;
                if (((int)(other_flags >> 1) & 1) && !ENTITY_FLAG_BITS(other_entity)->dying &&
                    other_entity != entity) {
                    EntityAiState *other_unkE0 = &other_entity->ai_state;
                    if (other_unkE0->step_index == 0 &&
                        EntityMath_DistanceXZ(entity->model->position, other_entity->model->position) <
                            (float)entity->params->far_distance) {
                        other_unkE0->step_index = 1;
                    }
                }
            }
        }
        other_entity = other_entity->next;
    }
}
