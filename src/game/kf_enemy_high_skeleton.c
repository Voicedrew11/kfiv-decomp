/* HighSkeleton: the behaviour of entity type 62 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers it installs. Used by: High Skeleton Small, High Skeleton
   Large. The game names no monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to
   the chr.prm records by hit points and experience, with resistances and drops where two monsters share
   those. */
#include "kf_entity.h"
#include "kf_entity_world.h"
#include "kf_effect.h"

void EntityAi_SetUpdate(char *, s32);
int EntityAi_StartAction(Entity *, void (*)(Entity *), int);
void Motion_ClearPartSlots(EntityModel *);
int Motion_StepModel(EntityModel *, s32, s32);
void Vector_Copy(void *, void *);
// match: the definition takes an Actor, a struct local to EntityAi_Animate.c
void EntityAi_Animate(void);
void EntityAi_OnHit(char *);
void EntityAi_Move(Entity *);
void EntityAi_Death(Entity *);
extern s32 D_003E6E40[][2];
extern u32 D_00446ED4;
void EntityAi_SensePlayer(void *, int);
void EntityAi_CheckFarFromSpawn(char *);
void EntityAi_StepState(void *);
int EntityAi_ThinkReturn(void *, int);
int EntityAi_ThinkIdle(void *, int);
void EntityAi_ThinkPursue(Entity *, s32);
void EntityAi_ThinkCombat(Entity *, s32);
int Motion_Start(void *, int, int, u32, int);
void EntityAi_SetState(char *, u8);
void EntityEvent_StartEffectAtPoint(char **, int, u32, int);

static void HighSkeleton_Think(Entity *entity, s32 unknown);
static void HighSkeleton_Dormant(Entity *entity);
static void HighSkeleton_DormantScaleUp(Entity *entity);
static void HighSkeleton_Death(Entity *entity);

/* Set-up of a type 62 entity: clears its work area and installs the type's handlers. */
void HighSkeleton_Init(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    char *unk138 = entity->model->unk138;
    s32 i;

    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        ENTITY_WORK_BYTES(entity)[i] = 0;
    }
    EntityAi_SetUpdate((char *)entity, D_003E6E40[entity->params->unk02][1]);
    unkE0->think = HighSkeleton_Think;
    unkE0->animate = EntityAi_Animate;
    unkE0->on_hit = (void (*)(void))EntityAi_OnHit;
    unkE0->move = (void (*)(void))EntityAi_Move;
    if (entity->spawn_info->unk40 != 0) {
        unkE0->dormant = (void (*)(void))HighSkeleton_DormantScaleUp;
    } else {
        unkE0->dormant = (void (*)(void))HighSkeleton_Dormant;
    }
    unkE0->death = (void (*)(void))EntityAi_Death;
    for (i = 3; i != -1; i--) {
        entity->unkBC[i] = 0x80;
    }
    for (i = 5; i != -1; i--) {
        entity->unkC0[i] = 0x80;
    }
    ENTITY_FLAG_BITS(entity)->bit28 = 0;
    entity->target = NULL;
    if (ENTITY_SPAWN_FLAGS(entity) & (0x8000L << 43)) {
        EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->dormant, 1);
    } else {
        EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 0);
    }
    if (unk138 != NULL) {
        Vector_Copy(unkE0->unk20, unk138 + 0x50);
    }
    unkE0->unk40 = 0.5f;
    entity->unkD6 = (entity->params->flags & 0x4000) ? 0x40 : 0x80;
    entity->model->unk15C = entity->unkD6;
    Motion_ClearPartSlots(entity->model);
    Motion_StepModel(entity->model, 0, 0);
}

/* The think handler: as EntityAi_Think. Before that, with EntitySpawnInfo.unk40 and variant set, it
   looks every Entity.unkD7 frames at the entity with spawn id `variant` and starts
   HighSkeleton_Death once that one has flag bits 2-3 at 2 or more or flag bit 1 clear. */
static void HighSkeleton_Think(Entity *entity, s32 unknown) {
    Entity *other;

    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (entity->spawn_info->unk40 != 0 && entity->spawn_info->variant != 0) {
        other = Entity_GetBySpawnId(entity->spawn_info->variant);
        if (other != NULL && D_00446ED4 % entity->unkD7 == 0) {
            if (ENTITY_FLAG_BITS(other)->bits2 >= 2 || !ENTITY_FLAG_BITS(other)->bit1) {
                EntityAi_StartAction(entity, HighSkeleton_Death, 1);
                return;
            }
        }
    }
    if (entity->unkC6 == 0) {
        EntityAi_CheckFarFromSpawn((char *)entity);
        EntityAi_StepState(entity);
        if (ENTITY_FLAG_BITS(entity)->bit53) {
            EntityAi_ThinkReturn(entity, unknown);
        } else {
            switch (entity->state) {
            case 0:
                EntityAi_ThinkIdle(entity, unknown);
                break;
            case 1:
                EntityAi_ThinkPursue(entity, unknown);
                break;
            case 2:
                EntityAi_ThinkCombat(entity, unknown);
                break;
            default:
                EntityAi_ThinkIdle(entity, unknown);
                break;
            }
        }
    }
}

/* The dormant action (busy bit 0x20): as EntityAi_Dormant, and flag bits 20 and 25 are set with the
   others while it lasts. */
static void HighSkeleton_Dormant(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 0x20;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 1, 0, 0, 1);
        if (entity->params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->bits11 = (((EntityParamsWord *)entity->params)->unk00 & 0x6000000) == 0x2000000 ? 2 : 1;
        }
        ENTITY_FLAG_BITS(entity)->bit21 = 1;
        ENTITY_FLAG_BITS(entity)->bit20 = 1;
        unkE0->unk1C = 0.0f;
        ENTITY_FLAG_BITS(entity)->bit22 = 1;
        ENTITY_FLAG_BITS(entity)->bit23 = 1;
        ENTITY_FLAG_BITS(entity)->bit24 = 1;
        ENTITY_FLAG_BITS(entity)->bit25 = 1;
        ENTITY_FLAG_BITS(entity)->bit18 = 1;
        ENTITY_FLAG_BITS(entity)->bit28 = 1;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->action_step++;
        break;
    case 1:
        if (entity->distance_to_player < entity->spawn_info->unk54 || ENTITY_FLAG_BITS(entity)->bit43) {
            Motion_Start(entity, 2, 0, entity->params->unkC4, 0);
            if (entity->params->flags & 0x100) {
                ENTITY_FLAG_BITS(entity)->bits11 = 0;
            } else {
                ENTITY_FLAG_BITS(entity)->bits11 = (((EntityParamsWord *)entity->params)->unk00 & 0x6000000) == 0x2000000 ? 2 : 1;
            }
            ENTITY_FLAG_BITS(entity)->bits36 = 0;
            entity->action_step++;
        }
        break;
    case 2:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 3:
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            EntityAi_SetState((char *)entity, 2);
            ENTITY_FLAG_BITS(entity)->bit21 = 0;
            ENTITY_FLAG_BITS(entity)->bit20 = 0;
            ENTITY_FLAG_BITS(entity)->bit22 = 0;
            ENTITY_FLAG_BITS(entity)->bit23 = 0;
            ENTITY_FLAG_BITS(entity)->bit24 = 0;
            ENTITY_FLAG_BITS(entity)->bit25 = 0;
            ENTITY_FLAG_BITS(entity)->bit18 = 0;
            if (!(((EntityParamsWord *)entity->params)->unk00 & 0x6000000)) {
                ENTITY_FLAG_BITS(entity)->bit28 = 0;
            }
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if (ENTITY_FLAG_BITS(entity)->bit5) {
                entity->unkE0.think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            }
        }
        break;
    }
}

/* The action the set-up function puts in the dormant slot for a spawn whose EntitySpawnInfo.unk40
   is set; its busy bit is 0x1000, not 0x20. It holds animation 0 for EntityParams.unk436 frames
   with flag bits 18 and 21 set, calls EntityEvent_StartEffectAtPoint with 0x22F once, and meanwhile scales the model
   in x and z and the radius from 0 up to their full size; then goes to state 2 and thinks. */
static void HighSkeleton_DormantScaleUp(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    s32 duration;

    entity->unkC6 |= 0x1000;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0, 0, 0, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
        entity->unkA0 = (u16)entity->params->unk436;
        unkE0->unk48 = entity->model->unkE0[0];
        unkE0->unk4C = entity->model->unkE0[1];
        unkE0->unk50 = entity->model->unkE0[2];
        unkE0->unk54 = entity->radius;
        entity->model->unkE0[0] = 0.0f;
        entity->model->unkE0[2] = 0.0f;
        unkE0->unk1C = 0.0f;
        ENTITY_FLAG_BITS(entity)->bit21 = 1;
        ENTITY_FLAG_BITS(entity)->bit18 = 1;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->action_step++;
        break;
    case 1:
        EntityEvent_StartEffectAtPoint((char **)entity, 0x22F, 0xFF, (int)entity->model->unkD0);
        entity->action_step++;
        break;
    case 2:
        duration = entity->params->unk436;
        entity->model->unkE0[0] = (f32)(duration - entity->unkA0) / (f32)duration;
        entity->model->unkE0[2] = (f32)(duration - entity->unkA0) / (f32)duration;
        entity->radius = unkE0->unk54 * ((f32)(duration - entity->unkA0) / (f32)duration);
        if (entity->unkA0 == 0) {
            entity->model->unkE0[0] = unkE0->unk48;
            entity->model->unkE0[1] = unkE0->unk4C;
            entity->model->unkE0[2] = unkE0->unk50;
            entity->radius = unkE0->unk54;
            EntityAi_SetState((char *)entity, 2);
            ENTITY_FLAG_BITS(entity)->bit21 = 0;
            ENTITY_FLAG_BITS(entity)->bit18 = 0;
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if (ENTITY_FLAG_BITS(entity)->bit5) {
                entity->unkE0.think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            }
        }
        break;
    }
}

/* A death action (busy bit 0x40) that HighSkeleton_Think starts; the death slot has EntityAi_Death.
   As EntityAi_Death, but it frees the entity's effects (Effect_FreeForOwner) when
   EntityParams.unkC6 is neither 0 nor 0xFFFF and does not call EntityEvent_OnDeath. */
static void HighSkeleton_Death(Entity *entity) {
    entity->unkC6 = 0x40;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x10, 0, 5, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        ENTITY_FLAG_BITS(entity)->bit14 = 1;
        ENTITY_FLAG_BITS(entity)->bit20 = 1;
        ENTITY_FLAG_BITS(entity)->bits2 = 1;
        ENTITY_FLAG_BITS(entity)->bit27 = 1;
        ENTITY_FLAG_BITS(entity)->bit26 = 1;
        ENTITY_FLAG_BITS(entity)->bit28 = 0;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        if (entity->params->unkC6 != 0 && entity->params->unkC6 != 0xFFFF) {
            Effect_FreeForOwner(entity);
        }
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            entity->unkA0 = 20;
            ENTITY_FLAG_BITS(entity)->bit14 = 0;
            ENTITY_FLAG_BITS(entity)->bits2 = 2;
            ENTITY_FLAG_BITS(entity)->bit4 = 1;
            entity->action_step++;
        }
        break;
    case 3:
        if (entity->unkA0 != 0) {
            entity->model->unk15C = (f32)entity->unkD6 * (f32)entity->unkA0 / 20.0f;
        } else {
            ENTITY_FLAG_BITS(entity)->bits2 = 3;
            ENTITY_FLAG_BITS(entity)->bit1 = 0;
        }
        break;
    }
}
