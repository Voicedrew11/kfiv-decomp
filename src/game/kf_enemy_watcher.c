/* Watcher: the behaviour of entity type 73 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers it installs. Used by: Watcher. The game names no
   monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by
   hit points and experience, with resistances and drops where two monsters share those. */
#include "kf_entity.h"

#define ENTITY_WORK_AREA_SIZE 0xA0
void EntityAi_SetUpdate(char *, s32);
void Motion_ClearPartSlots(EntityModel *);
int Motion_StepModel(EntityModel *, s32, s32);
void Vector_Copy(void *, void *);
int EntityAi_StartAction(Entity *, void (*)(Entity *), int);
// match: the definition takes an Actor, a struct local to EntityAi_Animate.c
void EntityAi_Animate(void);
extern s32 D_003E6E40[][2];
extern u32 D_00446ED4;
void EntityAi_SensePlayer(void *, int);
s32 EntityAi_CanUseRanged(Entity *, s32);
void func_001B4EC8(s32);
int Motion_Start(void *, int, int, u32, int);
void EntityEvent_StartCueAlways(void *, int, int);
void EntityEvent_OnDeath(Entity *);

static void Watcher_Think(Entity *entity, s32 unknown);
static void Watcher_Move(Entity *entity);
static void Watcher_Death(Entity *entity);

/* Set-up of a type 73 entity: clears its work area and installs the type's handlers. */
void Watcher_Init(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    u8 *unkBC = entity->unkBC;
    u8 *unkC0 = entity->unkC0;
    char *unk138 = entity->model->unk138;
    s32 i;

    // match: in the three loops the pointer has to be formed inside the loop
    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        u8 *dest = (u8 *)&entity->unkE0;

        dest[i] = 0;
    }
    EntityAi_SetUpdate((char *)entity, D_003E6E40[entity->params->unk02][1]);
    unkE0->think = Watcher_Think;
    unkE0->animate = EntityAi_Animate;
    unkE0->on_hit = NULL;
    unkE0->move = (void (*)(void))Watcher_Move;
    unkE0->dormant = NULL;
    unkE0->death = (void (*)(void))Watcher_Death;
    for (i = 4 - 1; i != -1; i--) {
        entity->unkBC[i] = 0x80;
    }
    for (i = 6 - 1; i != -1; i--) {
        entity->unkC0[i] = 0x80;
    }
    ENTITY_UNK5C(entity)->as_s32 = 0;
    ENTITY_FLAG_BITS(entity)->bit28 = TRUE;
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 0);
    if (unk138 != NULL) {
        Vector_Copy(unkE0->unk20, unk138 + 0x50);
    }
    unkE0->unk40 = 0.5f;
    entity->unkD6 = (entity->params->flags & 0x4000) ? 0x40 : 0x80;
    entity->model->unk15C = entity->unkD6;
    Motion_ClearPartSlots(entity->model);
    Motion_StepModel(entity->model, 0, 0);
}

/* The think handler: no states and no actions. Every tenth frame updates what the entity knows of
   the player; while it is not busy and EntityAi_CanUseRanged finds the player in reach of ranged record 0
   (and flag bit 63 is clear) it calls func_001B4EC8 with EntityParams.unk434, sets work byte unk7A
   and flag bit 9 and plays animation 2, else clears them and plays animation 0. Calls EntityEvent_StartCueAlways
   with 1 when unk7A changes. */
static void Watcher_Think(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    s8 previous;

    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (entity->unkC6 == 0) {
        previous = unkE0->unk7A;
        if (!(s32)(entity->flags >> 63) && EntityAi_CanUseRanged(entity, 0)) {
            func_001B4EC8((s16)entity->params->unk434);
            unkE0->unk7A = 1;
            ENTITY_FLAG_BITS(entity)->bit9 = TRUE;
            Motion_Start(entity, 2, 0, entity->params->unkC4, 0);
        } else {
            unkE0->unk7A = 0;
            ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
            Motion_Start(entity, 0, 0, entity->params->unkC4, 0);
        }
        if (previous != unkE0->unk7A) {
            EntityEvent_StartCueAlways(entity, 1, 0);
        }
    }
}

/* The move handler: this type stays where it is. While work byte unk7A is set it keeps animation 2
   and calls EntityEvent_StartCueAlways with 0 every EntityParams.unk438 frames (counted in work word unk68);
   else it keeps animation 0. */
static void Watcher_Move(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    if (unkE0->unk7A != 0) {
        Motion_Start(entity, 2, 0, entity->params->unkC4, 0);
        if (unkE0->unk68 <= 0) {
            EntityEvent_StartCueAlways(entity, 0, 0);
            unkE0->unk68 = entity->params->unk438;
        }
    } else {
        Motion_Start(entity, 0, 0, entity->params->unkC4, 0);
        unkE0->unk68 = 0;
    }
    unkE0->unk68--;
}

/* The death action (busy bit 0x40): no animation; calls EntityEvent_StartCueAlways with 2, then the fade over 20
   frames and the removal as EntityAi_Death. */
static void Watcher_Death(Entity *entity) {
    entity->unkC6 = 0x40;
    switch (entity->action_step) {
    case 0:
        EntityEvent_StartCueAlways(entity, 2, 0);
        ENTITY_FLAG_BITS(entity)->bit14 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit20 = TRUE;
        ENTITY_FLAG_BITS(entity)->bits2 = 1;
        ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit14 = FALSE;
        ENTITY_FLAG_BITS(entity)->bits2 = 2;
        ENTITY_FLAG_BITS(entity)->bit4 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit27 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit26 = TRUE;
        entity->unkA0 = 0x14;
        EntityEvent_OnDeath(entity);
        entity->action_step++;
        break;
    case 1:
        if (entity->unkA0 != 0) {
            entity->model->unk15C = (f32)entity->unkD6 * (f32)entity->unkA0 / 20.0f;
        } else {
            ENTITY_FLAG_BITS(entity)->bits2 = 3;
            ENTITY_FLAG_BITS(entity)->bit1 = FALSE;
        }
        break;
    }
}
