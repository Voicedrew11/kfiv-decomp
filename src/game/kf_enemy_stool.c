/* Stool: the behaviour of entity type 8 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers it installs. Used by: Stool. The game names no monsters;
   these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by hit points
   and experience, with resistances and drops where two monsters share those. */
#include "kf_entity.h"
#include "common.h"

#define ENTITY_WORK_AREA_SIZE 0xA0
void EntityAi_SetUpdate(char *, s32);
int EntityAi_StartAction(Entity *, void (*)(Entity *), int);
void Motion_ClearPartSlots(EntityModel *);
int Motion_StepModel(EntityModel *, s32, s32);
// match: the definition takes an Actor, a struct local to EntityAi_Animate.c
void EntityAi_Animate(void);
void EntityAi_Move(Entity *);
void EntityAi_Dormant(Entity *);
void EntityAi_Death(Entity *);
extern s32 D_003E6E40[][2];
void EntityAi_LowerWeightsAfterHit(char *);
void EntityAi_RangedAttack(Entity *);
void EntityAi_ReactionRewind(Entity *);
void EntityAi_ReactionLight(Entity *);
void EntityAi_ReactionHeavy(Entity *);
int EntityAi_SetReaction(char *, void (*)(void), int);
s32 EntityAi_CanAttack(Entity *);
int EntityAi_PickAttack(char *);
void EntityAi_MeleeAttack(Entity *);

static void Stool_OnHit(Entity *entity);
static void Stool_Think(Entity *entity, s32 unknown);

/* Set-up of a type 8 entity: clears its work area and installs the type's handlers. */
void Stool_Init(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    u8 *unkBC = entity->unkBC;
    u8 *unkC0 = entity->unkC0;
    s32 i;

    // match: in the three loops the pointer has to be formed inside the loop
    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        u8 *dest = (u8 *)&entity->unkE0;

        dest[i] = 0;
    }
    EntityAi_SetUpdate((char *)entity, D_003E6E40[entity->params->unk02][1]);
    unkE0->think = Stool_Think;
    unkE0->animate = EntityAi_Animate;
    unkE0->on_hit = (void (*)(void))Stool_OnHit;
    unkE0->move = (void (*)(void))EntityAi_Move;
    unkE0->dormant = (void (*)(void))EntityAi_Dormant;
    unkE0->death = (void (*)(void))EntityAi_Death;
    for (i = 4 - 1; i != -1; i--) {
        entity->unkBC[i] = 0x80;
    }
    for (i = 6 - 1; i != -1; i--) {
        entity->unkC0[i] = 0x80;
    }
    ENTITY_UNK5C(entity)->as_s32 = 0;
    if (entity->spawn_info->unk38 & (0x8000L << 43)) {
        EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->dormant, 1);
    } else {
        EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 0);
    }
    ENTITY_FLAG_BITS(entity)->bit28 = TRUE;
    unkE0->unk40 = 0.5f;
    entity->unkD6 = (entity->params->flags & 0x4000) ? 0x40 : 0x80;
    entity->model->unk15C = entity->unkD6;
    Motion_ClearPartSlots(entity->model);
    Motion_StepModel(entity->model, 0, 0);
}

/* The on_hit handler: with flag bits 38-39 not 0 it starts EntityAi_RangedAttack with ranged record
   0; else it starts the reaction as EntityAi_OnHit does (the heavy one for anything but a light
   hit). Unlike that one it keeps the target and does not change the state. */
static void Stool_OnHit(Entity *entity) {
    u64 flags;

    EntityAi_LowerWeightsAfterHit((char *)entity);
    flags = entity->flags;
    if ((s32)(flags >> 38) & 3) {
        entity->flags = (flags & ~(3UL << 58)) | (1UL << 57);
        EntityAi_StartAction(entity, EntityAi_RangedAttack, 1);
        return;
    }
    if (((s32)(flags >> 36) & 3) == 1) {
        if ((s32)((s64)(flags << 13) >> 32) & 1) {
            EntityAi_SetReaction((char *)entity, (void (*)(void))EntityAi_ReactionRewind, 0);
            return;
        }
        EntityAi_SetReaction((char *)entity, (void (*)(void))EntityAi_ReactionLight, 0);
        return;
    }
    EntityAi_SetReaction((char *)entity, (void (*)(void))EntityAi_ReactionHeavy, 0);
}

/* The think handler: no states and no look at the player. While the entity is not busy it starts
   the attack EntityAi_PickAttack picks when EntityAi_CanAttack says so and the pause Entity.unkA2 is over;
   else it starts the move action. */
static void Stool_Think(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0;

    unkE0 = &entity->unkE0;
    if (entity->unkC6 == 0) {
        if (EntityAi_CanAttack(entity) != 0 && entity->unkA2 == 0) {
            if (EntityAi_PickAttack((char *)entity) != 0) {
                EntityAi_StartAction(entity, EntityAi_RangedAttack, unknown);
            } else {
                EntityAi_StartAction(entity, EntityAi_MeleeAttack, unknown);
            }
        } else {
            EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
        }
    }
}
