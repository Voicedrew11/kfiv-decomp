/* Snake: the behaviour of entity type 21 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers it installs. Used by: Snake. The game names no monsters;
   these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by hit points
   and experience, with resistances and drops where two monsters share those. */
#include "kf_entity.h"
#include "kf_map_object.h"
#include "kf_map.h"
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
int Motion_Start(void *, int, int, u32, int);
void EntityAi_SetState(char *, u8);
extern u8 D_00349690[];
f32 EntityCollision_DistanceToPropFloor(void *, void *);
void EntityMath_FacingVector(Vector4 *, float);
void Vector_Scale(float *, float *, float);
float EntityMath_DistanceXZ(void *, void *);
extern u32 D_00446ED4;
void EntityAi_SensePlayer(void *, int);
void EntityAi_CheckFarFromSpawn(char *);
void EntityAi_StepState(void *);
int EntityAi_ThinkReturn(void *, int);
int EntityAi_ThinkIdle(void *, int);
void EntityAi_ThinkPursue(Entity *, s32);
void EntityAi_ThinkCombat(Entity *, s32);
extern EntitySpawnInfo *D_004467D0[2];
extern MapSlot D_0038CD20[];
extern int D_00446770;
void func_001035A8(u32, int);
int Script_Start(int, int);

static void Snake_DormantHoldAnim2(Entity *entity);
static void Snake_DormantHoldAnim8ThenMove(Entity *entity);
static void Snake_Dormant(Entity *entity);
static void Snake_Think(Entity *entity, s32 unknown);
static void Snake_Death(Entity *entity);

/* Set-up of a type 21 entity: clears its work area and installs the type's handlers. */
void Snake_Init(Entity *entity) {
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
    unkE0->think = Snake_Think;
    unkE0->animate = EntityAi_Animate;
    unkE0->on_hit = (void (*)(void))EntityAi_OnHit;
    unkE0->move = (void (*)(void))EntityAi_Move;
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
        if (entity->spawn_info->unk40 == 0) {
            unkE0->dormant = (void (*)(void))Snake_DormantHoldAnim2;
        } else if (entity->spawn_info->unk40 == 1) {
            unkE0->dormant = (void (*)(void))Snake_DormantHoldAnim8ThenMove;
        } else {
            unkE0->dormant = (void (*)(void))Snake_Dormant;
        }
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

/* The dormant action of a spawn in mode 0 (EntitySpawnInfo.unk40 is 0; busy bit 0x20): holds
   animation 2 at its start until the player is within EntitySpawnInfo.unk54 or the entity is hit,
   then plays it to its end (animation 17 instead when the map object EntitySpawnInfo.variant names
   has bit 16 clear or is at step 15), goes to state 2 and thinks. */
static void Snake_DormantHoldAnim2(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    MapObject *map_object;

    entity->unkC6 |= 0x20;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 2, 0, 0, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        unkE0->unk1C = 0.0f;
        ENTITY_FLAG_BITS(entity)->bit27 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit22 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit23 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit24 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit25 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit28 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
        entity->action_step++;
        break;
    case 1:
        ENTITY_FLAG_BITS(entity)->bit14 = FALSE;
        if (entity->distance_to_player < entity->spawn_info->unk54 || ENTITY_FLAG_BITS(entity)->bits36 != 0) {
            map_object = MapObject_GetById(entity->group_id, entity->spawn_info->variant);
            if (map_object != NULL) {
                if (!(map_object->unk10 & 0x10000) || MAP_OBJECT_BITS(map_object)->step == 0xF) {
                    Motion_Start(entity, 0x11, 0, 0, 1);
                }
            }
            ENTITY_FLAG_BITS(entity)->bit14 = TRUE;
            entity->action_step++;
        }
        break;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            EntityAi_SetState((char *)entity, 2);
            ENTITY_FLAG_BITS(entity)->bit27 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit22 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit23 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit24 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit25 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if (ENTITY_FLAG_BITS(entity)->bit5) {
                unkE0->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            }
        }
        break;
    }
}

/* The dormant action of a spawn in mode 1 (busy bit 0x20): holds animation 8 until the player is
   within EntitySpawnInfo.unk54, plays it past frame 30, then moves the entity along its heading by
   EntityParams.unk436 a frame (unk438 in y) until flag bit 44 is set or EntityCollision_DistanceToPropFloor reads below
   100, plays the rest, goes to state 2 and thinks. While it waits it removes the entity once
   EntitySpawnInfo.variant is set and D_00349690[unk43 + unk42 * 64] is not 0. */
static void Snake_DormantHoldAnim8ThenMove(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    f32 distance;

    entity->unkC6 |= 0x20;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 8, 0, 0, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        unkE0->unk1C = 0.0f;
        ENTITY_FLAG_BITS(entity)->bit27 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit23 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit24 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit25 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit28 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
        entity->action_step++;
        break;
    case 1:
        if (entity->spawn_info->variant != 0 &&
            D_00349690[entity->spawn_info->unk43 + entity->spawn_info->unk42 * 64] != 0) {
            ENTITY_FLAG_BITS(entity)->bit1 = FALSE;
            ENTITY_FLAG_BITS(entity)->bits2 = 3;
            break;
        }
        ENTITY_FLAG_BITS(entity)->bit14 = FALSE;
        if (entity->distance_to_player < entity->spawn_info->unk54) {
            ENTITY_FLAG_BITS(entity)->bit14 = TRUE;
            entity->action_step++;
        }
        break;
    case 2:
        if (entity->model->unkFC->unk82 > 30) {
            ENTITY_FLAG_BITS(entity)->bit14 = FALSE;
            entity->action_step++;
        }
        break;
    case 3:
        distance = EntityCollision_DistanceToPropFloor(entity->unk14, entity->model->position);
        if (ENTITY_FLAG_BITS(entity)->bit44 || distance < 100.0f) {
            ENTITY_FLAG_BITS(entity)->bit14 = TRUE;
            entity->action_step++;
            break;
        }
        EntityMath_FacingVector((Vector4 *)direction, entity->model->unkD0[1]);
        Vector_Scale(direction, direction, entity->params->unk436);
        direction[1] += entity->params->unk438;
        entity->unk84 += direction[0];
        entity->unk88 += direction[1];
        entity->unk8C += direction[2];
        break;
    case 4:
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            EntityAi_SetState((char *)entity, 2);
            ENTITY_FLAG_BITS(entity)->bit27 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit23 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit24 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit25 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if (ENTITY_FLAG_BITS(entity)->bit5) {
                unkE0->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            }
        }
        break;
    }
}

/* The dormant action of the other modes (busy bit 0x20): holds animation 3 until the player is
   within EntitySpawnInfo.unk54, then plays it until EntityMath_DistanceXZ between the entity and its spawn
   point reaches EntityParams.unk43A, goes to state 2 and thinks. */
static void Snake_Dormant(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 0x20;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 3, 0, 0, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        unkE0->unk1C = 0.0f;
        ENTITY_FLAG_BITS(entity)->bit27 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit22 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit23 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit24 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit28 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
        entity->action_step++;
        break;
    case 1:
        ENTITY_FLAG_BITS(entity)->bit14 = FALSE;
        if (entity->distance_to_player < (f32)entity->spawn_info->unk54) {
            ENTITY_FLAG_BITS(entity)->bit14 = TRUE;
            entity->action_step++;
        }
        break;
    case 2:
        if (EntityMath_DistanceXZ(entity->model->position, entity->spawn_info) >= (f32)entity->params->unk43A) {
            EntityAi_SetState((char *)entity, 2);
            ENTITY_FLAG_BITS(entity)->bit27 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit22 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit23 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit24 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if (ENTITY_FLAG_BITS(entity)->bit5) {
                unkE0->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            }
        }
        break;
    }
}

/* The think handler: as EntityAi_Think, but first, for a spawn in mode 1 whose
   EntitySpawnInfo.variant is 1, it starts Snake_Death once D_00349690[unk43 + unk42 * 64] is
   not 0. */
static void Snake_Think(Entity *entity, s32 unknown) {
    s32 index;

    if (entity->spawn_info->unk40 == 1 && entity->spawn_info->variant == 1) {
        index = entity->spawn_info->unk43;
        index += entity->spawn_info->unk42 * 64;
        if (D_00349690[index] != 0) {
            EntityAi_StartAction(entity, Snake_Death, 1);
            return;
        }
    }
    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (entity->unkC6 == 0) {
        if ((((EntityParamsWord *)entity->params)->unk00 & 0x6000000) != 0x6000000) {
            EntityAi_CheckFarFromSpawn((char *)entity);
        }
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

/* A death action (busy bit 0x40) that Snake_Think starts; the death slot holds
   EntityAi_Death. As EntityAi_Death, but when the motion has ended it calls func_001035A8, frees
   the entity's effects and starts script EntitySpawnInfo.unk36 where that one calls EntityEvent_OnDeath.
   */
static void Snake_Death(Entity *entity) {
    entity->unkC6 = 0x40;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x10, 0, 5, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        ENTITY_FLAG_BITS(entity)->bit14 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit20 = TRUE;
        ENTITY_FLAG_BITS(entity)->bits2 = 1;
        ENTITY_FLAG_BITS(entity)->bit27 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit26 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
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
            Effect_FreeForOwner(entity);
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
