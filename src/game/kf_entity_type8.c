/* The behaviour of entity type 8 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers that function installs. */
#include "kf_entity.h"
#include "common.h"

#define ENTITY_WORK_AREA_SIZE 0xA0
void func_001958A8(char *, s32);
int EntityAi_StartAction(Entity *, void (*)(Entity *), int);
void func_00191478(EntityModel *);
int func_00193378(EntityModel *, s32, s32);
// match: the definition takes an Actor, a struct local to EntityAi_Animate.c
void EntityAi_Animate(void);
void EntityAi_Move(Entity *);
void EntityAi_Dormant(Entity *);
void EntityAi_Death(Entity *);
extern s32 D_003E6E40[][2];
void func_00190A30(char *);
void EntityAi_RangedAttack(Entity *);
void EntityAi_ReactionRewind(Entity *);
void EntityAi_ReactionLight(Entity *);
void EntityAi_ReactionHeavy(Entity *);
int EntityAi_SetReaction(char *, void (*)(void), int);
s32 func_00197078(Entity *);
int func_0018F030(char *);
void EntityAi_MeleeAttack(Entity *);

static void EntityType8_OnHit(Entity *entity);
static void EntityType8_Think(Entity *entity, s32 unknown);

/* Set-up of a type 8 entity: clears its work area and installs the type's handlers. */
void EntityType8_Init(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    u8 *unkBC = entity->unkBC;
    u8 *unkC0 = entity->unkC0;
    s32 i;

    // match: in the three loops the pointer has to be formed inside the loop
    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        u8 *dest = (u8 *)&entity->unkE0;

        dest[i] = 0;
    }
    func_001958A8((char *)entity, D_003E6E40[entity->params->unk02][1]);
    unkE0->think = EntityType8_Think;
    unkE0->animate = EntityAi_Animate;
    unkE0->on_hit = (void (*)(void))EntityType8_OnHit;
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
    func_00191478(entity->model);
    func_00193378(entity->model, 0, 0);
}

/* The on_hit handler: with flag bits 38-39 not 0 it starts EntityAi_RangedAttack with ranged record
   0; else it starts the reaction as EntityAi_OnHit does (the heavy one for anything but a light
   hit). Unlike that one it keeps the target and does not change the state. */
static void EntityType8_OnHit(Entity *entity) {
    u64 flags;

    func_00190A30((char *)entity);
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
   the attack func_0018F030 picks when func_00197078 says so and the pause Entity.unkA2 is over;
   else it starts the move action. */
static void EntityType8_Think(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0;

    unkE0 = &entity->unkE0;
    if (entity->unkC6 == 0) {
        if (func_00197078(entity) != 0 && entity->unkA2 == 0) {
            if (func_0018F030((char *)entity) != 0) {
                EntityAi_StartAction(entity, EntityAi_RangedAttack, unknown);
            } else {
                EntityAi_StartAction(entity, EntityAi_MeleeAttack, unknown);
            }
        } else {
            EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
        }
    }
}
