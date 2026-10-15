/* HammerSkeleton: the behaviour of entity type 37 (Entity.params->unk02, the index into D_003E6E40): its
   set-up function, which the table names, and the handlers it installs. Used by: chr.prm record 154,
   model c0154 (a skeleton holding a small hammer), whose one spawn in 007.map stands beside the bell
   (kf_enemy_bell.c). It gives no experience and dies by itself when the player enters one of two
   boxes, then starts its spawn's script. A descriptive name: the game and the guides name it nowhere. */
#include "kf_entity.h"
#include "kf_map.h"
#include "kf_effect.h"

#define ENTITY_WORK_AREA_SIZE 0xA0
void EntityAi_SetUpdate(char *, s32);
int EntityAi_StartAction(Entity *, void (*)(Entity *), int);
void Motion_ClearPartSlots(EntityModel *);
int Motion_StepModel(EntityModel *, s32, s32);
// match: the definition takes an Actor, a struct local to EntityAi_Animate.c
void EntityAi_Animate(void);
void EntityAi_OnHit(char *);
void EntityAi_Move(Entity *);
void EntityAi_Dormant(Entity *);
void EntityAi_Death(Entity *);
extern s32 D_003E6E40[][2];
extern float D_00413F80[];
extern EntitySpawnInfo *D_004467D0[2];
extern MapSlot D_0038CD20[];
extern int D_00446770;
int Motion_Start(void *, int, int, u32, int);
void func_001035A8(u32, int);
int Script_Start(int, int);

static void HammerSkeleton_Think(Entity *entity, s32 unknown);
static void HammerSkeleton_Death(Entity *entity);

/* Set-up of a type 37 entity: clears its work area and installs the type's handlers. */
void HammerSkeleton_Init(Entity *entity) {
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
    unkE0->think = HammerSkeleton_Think;
    unkE0->animate = EntityAi_Animate;
    unkE0->on_hit = (void (*)(void))EntityAi_OnHit;
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
    ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
    if (entity->spawn_info->unk38 & (0x8000L << 43)) {
        EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->dormant, 1);
    } else {
        EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 0);
    }
    unkE0->unk40 = 0.5f;
    entity->unkD6 = (entity->params->flags & 0x4000) ? 0x40 : 0x80;
    entity->model->unk15C = entity->unkD6;
    Motion_ClearPartSlots(entity->model);
    Motion_StepModel(entity->model, 0, 0);
}

/* The think handler: no states. While the entity is not busy it starts HammerSkeleton_Death when the
   player is inside either of two fixed boxes, else the move action. */
static void HammerSkeleton_Think(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    if (entity->unkC6 == 0) {
        if ((-6200.0f <= D_00413F80[0] && D_00413F80[0] <= -1400.0f &&
             -50.0f <= D_00413F80[1] && D_00413F80[1] <= 450.0f &&
             -4200.0f <= D_00413F80[2] && D_00413F80[2] <= -600.0f) ||
            (-6200.0f <= D_00413F80[0] && D_00413F80[0] <= -2600.0f &&
             -50.0f <= D_00413F80[1] && D_00413F80[1] <= 450.0f &&
             -600.0f <= D_00413F80[2] && D_00413F80[2] <= 1000.0f)) {
            EntityAi_StartAction(entity, HammerSkeleton_Death, unknown);
        } else {
            EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
        }
    }
}

/* A death action (busy bit 0x40) that HammerSkeleton_Think starts; the death slot holds
   EntityAi_Death. Animation 16, the fade over 20 frames and the removal, as EntityAi_Death with
   other flag settings; it frees the entity's effects at the start, and when the motion has ended
   calls func_001035A8 and starts script EntitySpawnInfo.unk36 where that one calls EntityEvent_OnDeath.
   */
static void HammerSkeleton_Death(Entity *entity) {
    entity->unkC6 = 0x40;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x10, 0, 5, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        ENTITY_FLAG_BITS(entity)->bit14 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit20 = TRUE;
        ENTITY_FLAG_BITS(entity)->bits2 = 1;
        Effect_FreeForOwner(entity);
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            ENTITY_FLAG_BITS(entity)->bit14 = FALSE;
            ENTITY_FLAG_BITS(entity)->bits2 = 2;
            ENTITY_FLAG_BITS(entity)->bit4 = TRUE;
            entity->unkA0 = 20;
            func_001035A8(D_0038CD20[entity->group_id].tag, entity->spawn_info - D_004467D0[entity->group_id]);
            if (entity->spawn_info->unk36 != 0 && entity->group_id == D_00446770) {
                Script_Start(entity->spawn_info->unk36, 0);
            }
            entity->action_step++;
        }
        break;
    case 3:
        if (entity->unkA0 != 0) {
            entity->model->unk15C = (f32)entity->unkD6 * (f32)entity->unkA0 / 20.0f;
        } else {
            ENTITY_FLAG_BITS(entity)->bits2 = 3;
            ENTITY_FLAG_BITS(entity)->bit1 = FALSE;
        }
        break;
    }
}
