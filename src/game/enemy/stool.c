/* Stool: the behaviour of entity type 8 (EntityParams.entity_type, the index into entity_type_handlers): its set-up
   function, which the table names, and the handlers it installs. Used by: Stool. The game names no monsters;
   these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by hit points
   and experience, with resistances and drops where two monsters share those. */
#include "entity/entity.h"
#include "common.h"

#define ENTITY_WORK_AREA_SIZE 0xA0
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
void EntityAi_RangedAttack(Entity *);
void EntityAi_ReactionRewind(Entity *);
void EntityAi_ReactionLight(Entity *);
void EntityAi_ReactionHeavy(Entity *);
int EntityAi_SetReaction(Entity *, void (*)(Entity *), int);
s32 EntityAi_CanAttack(Entity *);
int EntityAi_PickAttack(char *);
void EntityAi_MeleeAttack(Entity *);

static void Stool_OnHit(Entity *entity);
static void Stool_Think(Entity *entity, s32 unknown);

/* Set-up of a type 8 entity: clears its work area and installs the type's handlers. */
void Stool_Init(Entity *entity) {
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
    ai_state->think = Stool_Think;
    ai_state->animate = EntityAi_Animate;
    ai_state->on_hit = Stool_OnHit;
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
    if (entity->spawn_info->spawn_flags & (0x8000L << 43)) {
        EntityAi_StartAction(entity, ai_state->dormant, 1);
    } else {
        EntityAi_StartAction(entity, ai_state->move, 0);
    }
    ENTITY_FLAG_BITS(entity)->no_floor_lift = TRUE;
    ai_state->pose_blend_weight = 0.5f;
    entity->model_alpha = (entity->params->flags & ENTITY_PARAMS_FLAG_HALF_MODEL_ALPHA) ? 0x40 : 0x80;
    entity->model->alpha = entity->model_alpha;
    Motion_ClearPartSlots(entity->model);
    Motion_StepModel(ENTITY_MODEL(entity), 0, 0);
}

/* The on_hit handler: with flag bits 38-39 not 0 it starts EntityAi_RangedAttack with ranged record
   0; else it starts the reaction as EntityAi_OnHit does (the heavy one for anything but a light
   hit). Unlike that one it keeps the target and does not change the state. */
static void Stool_OnHit(Entity *entity) {
    u64 flags;

    EntityAi_LowerWeightsAfterHit((char *)entity);
    flags = entity->flags;
    if ((s32)(flags >> 38) & 3) {
        entity->flags = (flags & ~ENTITY_FLAG_ATTACK_INDEX_MASK) | ENTITY_FLAG_RANGED_ATTACK;
        EntityAi_StartAction(entity, EntityAi_RangedAttack, 1);
        return;
    }
    if (((s32)(flags >> 36) & 3) == 1) {
        if ((s32)((s64)(flags << 13) >> 32) & 1) {
            EntityAi_SetReaction(entity, EntityAi_ReactionRewind, 0);
            return;
        }
        EntityAi_SetReaction(entity, EntityAi_ReactionLight, 0);
        return;
    }
    EntityAi_SetReaction(entity, EntityAi_ReactionHeavy, 0);
}

/* The think handler: no states and no look at the player. While the entity is not busy it starts
   the attack EntityAi_PickAttack picks when EntityAi_CanAttack says so and the pause Entity.attack_delay_frames is over;
   else it starts the move action. */
static void Stool_Think(Entity *entity, s32 unknown) {
    EntityAiState *ai_state;

    ai_state = &entity->ai_state;
    if (entity->busy_flags == 0) {
        if (EntityAi_CanAttack(entity) != 0 && entity->attack_delay_frames == 0) {
            if (EntityAi_PickAttack((char *)entity) != 0) {
                EntityAi_StartAction(entity, EntityAi_RangedAttack, unknown);
            } else {
                EntityAi_StartAction(entity, EntityAi_MeleeAttack, unknown);
            }
        } else {
            EntityAi_StartAction(entity, ai_state->move, unknown);
        }
    }
}
