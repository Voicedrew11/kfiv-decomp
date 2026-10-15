/* Larva: the behaviour of entity type 67 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers it installs. Used by: Larva. The game names no monsters;
   these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by hit points
   and experience, with resistances and drops where two monsters share those. */
#include "kf_entity.h"
#include "kf_map.h"
#include "kf_effect.h"
#include "kf_entity_world.h"

#define ENTITY_WORK_AREA_SIZE 0xA0
extern MapSlot D_0038CD20[];
extern s32 D_003E6E40[][2];
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
extern u32 D_00446ED4;
void EntityAi_SensePlayer(void *, int);
void EntityAi_StepState(void *);
int EntityAi_ThinkIdle(void *, int);
void EntityAi_ThinkCombat(Entity *, s32);
void EntityEvent_StartEffectAtPoint(char **, int, u32, int);
int Motion_Start(void *, int, int, u32, int);
void EntityAi_SetState(char *, u8);
void EntityEvent_OnDeath(Entity *);
char *func_0010CAB8(char *, u32);
int rand(void);
float sinf(float);
float cosf(float);
float atan2f(float, float);
s32 EntityAi_CheckMeleeHit(Entity *, void *, s32, s32, s32);
void EntityMath_FacingVector(Vector4 *, float);
void Vector_Scale(float *, float *, float);

static void Larva_Think(Entity *entity, s32 unknown);
static void Larva_Dormant(Entity *entity);
static void Larva_Death(Entity *entity);
static void Larva_DormantHeldAtModelPoint(Entity *entity);
static void Larva_DormantMoveAlongArc(Entity *entity);
static void Larva_DeathFallAndFade(Entity *entity);

/* Set-up of a type 67 entity: clears its work area and installs the type's handlers. */
void Larva_Init(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    u8 *unkBC = entity->unkBC;
    u8 *unkC0 = entity->unkC0;
    char *unk138 = entity->model->unk138;
    MapSlotUnk10 *record;
    s32 i;

    // match: the three fills name their target inside the loop; unkBC and unkC0 above are
    // otherwise unused but retail computes them up front and copies them before each loop
    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        u8 *work_area = (u8 *)&entity->unkE0;

        work_area[i] = 0;
    }
    EntityAi_SetUpdate((char *)entity, D_003E6E40[entity->params->unk02][1]);
    unkE0->think = Larva_Think;
    unkE0->animate = EntityAi_Animate;
    unkE0->on_hit = (void (*)(void))EntityAi_OnHit;
    unkE0->move = (void (*)(void))EntityAi_Move;
    if (entity->spawn_info->unk40 != 0) {
        unkE0->dormant = (void (*)(void))Larva_DormantHeldAtModelPoint;
    } else {
        unkE0->dormant = (void (*)(void))Larva_Dormant;
    }
    unkE0->death = (void (*)(void))EntityAi_Death;
    for (i = 3; i != -1; i--) {
        entity->unkBC[i] = 0x80;
    }
    for (i = 5; i != -1; i--) {
        entity->unkC0[i] = 0x80;
    }
    entity->flags &= ~0x10000000;
    entity->target = NULL;
    if (ENTITY_FLAG_WORD(entity->spawn_info->unk38) & (0x8000L << 43)) {
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
    record = &((MapSlotUnk10 *)D_0038CD20[entity->group_id].unk10)[(s8)((EntityFlagWord *)&entity->spawn_info->unk38)->bytes[3]];
    record->unk74 = 0.5f;
    record->unk70 = 1.0f;
}

/* The think handler: as EntityAi_Think without EntityAi_CheckFarFromSpawn and the return to the spawn point;
   states 1 and 2 both have EntityAi_ThinkCombat. Every 20th frame it starts effect
   EntityParams.unk43A at model point 0xFF. While work word unk6A counts down
   (Larva_DormantMoveAlongArc sets it to 600) it shrinks the model's scale (EntityModel.unkE0) from
   the one kept in work floats unk54 to unk5C towards half of it and lowers unk70 and unk74 of the
   entity's map record; at 0 it starts Larva_Death. */
static void Larva_Think(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    MapSlotUnk10 *record;
    f32 scale;

    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (entity->unkC6 == 0) {
        EntityAi_StepState(entity);
        switch (entity->state) {
        case 1:
        case 2:
            EntityAi_ThinkCombat(entity, unknown);
            break;
        case 0:
        default:
            EntityAi_ThinkIdle(entity, unknown);
            break;
        }
    }
    if (D_00446ED4 % 20U == 0) {
        EntityEvent_StartEffectAtPoint((char **)entity, entity->params->unk43A, 0xFF, (int)entity->model->unkD0);
    }
    if (unkE0->unk6A != 0) {
        unkE0->unk6A--;
        scale = (f32)unkE0->unk6A / 1200.0f + 0.5f;
        entity->model->unkE0[0] = unkE0->unk54 * scale;
        entity->model->unkE0[1] = unkE0->unk58 * scale;
        entity->model->unkE0[2] = unkE0->unk5C * scale;
        scale = (f32)unkE0->unk6A / 600.0f;
        record = &((MapSlotUnk10 *)D_0038CD20[entity->group_id].unk10)[(s8)((EntityFlagWord *)&entity->spawn_info->unk38)->bytes[3]];
        record->unk70 = scale * 0.75f + 0.25f;
        record->unk74 = scale * 0.375f + 0.125f;
        if (unkE0->unk6A <= 0) {
            EntityAi_StartAction(entity, Larva_Death, 0);
        }
    }
}

/* The dormant action (busy bit 0x20) of a spawn whose EntitySpawnInfo.unk40 is 0: as
   EntityAi_Dormant, with flag bits 11 to 13 at 0 and flag bit 20 set as well while it lasts.
   Larva_DormantMoveAlongArc starts it too. */
static void Larva_Dormant(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 0x20;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 1, 0, entity->params->unkC4, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
        unkE0->unk1C = 0.0f;
        ENTITY_FLAG_BITS(entity)->bit21 = 1;
        ENTITY_FLAG_BITS(entity)->bit20 = 1;
        ENTITY_FLAG_BITS(entity)->bit22 = 1;
        ENTITY_FLAG_BITS(entity)->bit23 = 1;
        ENTITY_FLAG_BITS(entity)->bit24 = 1;
        ENTITY_FLAG_BITS(entity)->bit18 = 1;
        ENTITY_FLAG_BITS(entity)->bit28 = 1;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->action_step++;
        break;
    case 1:
        if (entity->distance_to_player < (f32)entity->spawn_info->unk54 || ENTITY_FLAG_BITS(entity)->bit43) {
            Motion_Start(entity, 2, 0, entity->params->unkC4, 0);
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
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
            ENTITY_FLAG_BITS(entity)->bit18 = 0;
            if (!(((EntityParamsWord *)entity->params)->unk00 & 0x06000000)) {
                ENTITY_FLAG_BITS(entity)->bit28 = 0;
            }
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

/* A death action (busy bit 0x40) that Larva_Think starts when work word unk6A has run out;
   the death slot holds EntityAi_Death. Animation 16, the fade over 20 frames and the removal, as
   EntityAi_Death with other flag settings; it frees the entity's effects when EntityParams.unkC6 is
   neither 0 nor 0xFFFF. */
static void Larva_Death(Entity *entity) {
    entity->unkC6 = 0x40;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x10, 0, 5, 1);
        entity->flags &= ~0x3800;
        entity->flags |= 0x4000;
        entity->flags |= 0x100000;
        entity->flags = (entity->flags & ~0xC) | 4;
        entity->flags |= 0x8000000;
        entity->flags |= 0x4000000;
        entity->flags &= ~0x10000000;
        entity->flags &= ~0x200;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            if (entity->params->unkC6 != 0 && entity->params->unkC6 != 0xFFFF) {
                Effect_FreeForOwner(entity);
            }
            ENTITY_FLAG_BITS(entity)->bit14 = 0;
            ENTITY_FLAG_BITS(entity)->bits2 = 2;
            ENTITY_FLAG_BITS(entity)->bit4 = 1;
            entity->unkA0 = 20;
            EntityEvent_OnDeath(entity);
            entity->action_step++;
        }
        break;
    case 3:
        if (entity->unkA0 != 0) {
            entity->model->unk15C = (f32)entity->unkD6 * (f32)entity->unkA0 / 20.0f;
        } else {
            entity->flags |= 0xC;
            entity->flags &= ~2;
        }
        break;
    }
}

/* The dormant action of a spawn whose EntitySpawnInfo.unk40 is set (busy bit 0x20): holds animation
   17 and keeps the entity at the point func_0010CAB8 gives for 8 on the model of the entity whose
   spawn id is EntitySpawnInfo.variant, until work byte unk7A is not 0: above 0 it starts
   Larva_DormantMoveAlongArc, below 0 Larva_DeathFallAndFade. */
static void Larva_DormantHeldAtModelPoint(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    Entity *other;
    f32 *point;

    entity->unkC6 |= 0x20;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x11, 0, 0, 1);
        entity->flags &= ~0x3800;
        entity->flags |= 0x200000;
        entity->flags |= 0x100000;
        entity->flags |= 0x40000;
        entity->flags |= 0x10000000;
        entity->flags |= 0x400000;
        entity->flags |= 0x800000;
        entity->flags |= 0x1000000;
        entity->flags |= 0x8000000;
        entity->action_step++;
    case 1:
        if (unkE0->unk7A > 0) {
            EntityAi_StartAction(entity, Larva_DormantMoveAlongArc, 1);
            return;
        }
        if (unkE0->unk7A < 0) {
            EntityAi_StartAction(entity, Larva_DeathFallAndFade, 1);
            return;
        }
        other = Entity_GetBySpawnId(entity->spawn_info->variant);
        if (other != NULL && (other->flags & 3) == 3) {
            point = (f32 *)func_0010CAB8((char *)other->model, 8);
            if (point != NULL) {
                entity->model->position[0] = point[0];
                entity->model->position[1] = point[1];
                entity->model->position[2] = point[2];
            }
        }
        break;
    }
}

/* A second dormant action (busy bit 0x20), which Larva_DormantHeldAtModelPoint starts: animation 18. It
   turns the entity to Entity.unk68, give or take half of EntityParams.unk434 degrees, and sends it
   off at 40 degrees with a speed of 24 to 48 by the distance to the player, the y speed growing by
   EntityParams.unk430 (limit unk432); it checks part 0 of melee record 0 for a hit (EntityAi_CheckMeleeHit)
   and starts effect unk43C every third frame. When flag bit 44 is set it puts the model's scale
   back, sets work word unk6A to 600, goes to state 2 and starts Larva_Dormant. */
static void Larva_DormantMoveAlongArc(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    f32 max_angle;
    f32 min_angle;
    f32 half;
    f32 degree;
    f32 launch_angle;
    f32 angle;

    entity->unkC6 |= 0x20;
    switch (entity->action_step) {
    case 0:
        half = 0.5f;
        Motion_Start(entity, 0x12, 0, entity->params->unkC4, 1);
        degree = 0.017453292f;
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
        ENTITY_FLAG_BITS(entity)->bit21 = 1;
        ENTITY_FLAG_BITS(entity)->bit20 = 1;
        ENTITY_FLAG_BITS(entity)->bit18 = 1;
        ENTITY_FLAG_BITS(entity)->bit28 = 1;
        ENTITY_FLAG_BITS(entity)->bit22 = 0;
        ENTITY_FLAG_BITS(entity)->bit23 = 0;
        ENTITY_FLAG_BITS(entity)->bit24 = 0;
        unkE0->unk54 = entity->model->unkE0[0];
        unkE0->unk58 = entity->model->unkE0[1];
        unkE0->unk5C = entity->model->unkE0[2];
        entity->model->unkE0[0] = 0.75f;
        entity->model->unkE0[1] = 0.75f;
        entity->model->unkE0[2] = half;
        min_angle = 105.0f - (f32)(s16)entity->params->unk434 * half;
        max_angle = (f32)(s16)entity->params->unk434 * half + -130.0f;
        angle =
            entity->unk68 + ((f32)(rand() % ((s16)entity->params->unk434 + 1)) - (f32)(s16)entity->params->unk434 * half) * degree;
        entity->model->unkD0[1] = angle;
        max_angle *= degree;
        min_angle *= degree;
        if (entity->model->unkD0[1] < min_angle && 0.0f <= entity->model->unkD0[1]) {
            entity->model->unkD0[1] = min_angle;
        }
        if (max_angle < entity->model->unkD0[1] && entity->model->unkD0[1] < 0.0f) {
            entity->model->unkD0[1] = max_angle;
        }
        unkE0->unk50 =
            (entity->distance_to_player + (f32)(rand() % (entity->params->unk436 + 1)) - (f32)entity->params->unk436 * 0.5f - 500.0f) /
                1300.0f * 24.0f + 24.0f;
        if (unkE0->unk50 < 24.0f) {
            unkE0->unk50 = 24.0f;
        }
        if (48.0f < unkE0->unk50) {
            unkE0->unk50 = 48.0f;
        }
        launch_angle = 0.6981317f;
        unkE0->unk48 = -unkE0->unk50 * sinf(launch_angle);
        unkE0->unk4C = unkE0->unk50 * cosf(launch_angle);
        unkE0->unk68 = 0;
        unkE0->unk7C = 0;
        entity->action_step++;
        break;
    case 1:
        unkE0->unk48 += (f32)entity->params->unk430 * ((f32)unkE0->unk68 / 30.0f);
        if ((f32)entity->params->unk432 < unkE0->unk48) {
            unkE0->unk48 = (f32)entity->params->unk432;
        }
        if (D_00446ED4 % 3U == 0) {
            EntityEvent_StartEffectAtPoint((char **)entity, entity->params->unk43C, 0xFF, (int)entity->model->unkD0);
        }
        if (unkE0->unk7C == 0) {
            unkE0->unk7C = EntityAi_CheckMeleeHit(entity, entity->params->unk240, 0, 1, 2);
        }
        if ((s32)(entity->flags >> 44) & 1) {
            ENTITY_FLAG_BITS(entity)->bit27 = 0;
            entity->model->unkD0[0] = 0.0f;
            unkE0->unk6A = 600;
            entity->model->unkE0[0] = unkE0->unk54;
            entity->model->unkE0[1] = unkE0->unk58;
            entity->model->unkE0[2] = unkE0->unk5C;
            unkE0->unk7B = 1;
            EntityEvent_StartEffectAtPoint((char **)entity, entity->params->unk43E, 0xFF, (int)entity->model->unkD0);
            EntityAi_SetState((char *)entity, 2);
            entity->flags &= ~0x200000;
            entity->flags &= ~0x100000;
            entity->flags &= ~0x40000;
            entity->flags &= ~0x10000000;
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            EntityAi_StartAction(entity, Larva_Dormant, 1);
            return;
        }
        break;
    }
    unkE0->unk68++;
    EntityMath_FacingVector((Vector4 *)direction, entity->model->unkD0[1]);
    Vector_Scale(direction, direction, unkE0->unk4C);
    entity->unk84 += direction[0];
    entity->unk88 += unkE0->unk48;
    entity->unk8C += direction[2];
    entity->model->unkD0[0] = atan2f(unkE0->unk48, unkE0->unk4C);
}

/* A second death action (busy bit 0x40), which Larva_DormantHeldAtModelPoint starts when
   work byte unk7A is below 0: animation 17; fades the model out over 20 frames while the entity
   moves down (y grows downward, EntityCollision_DistanceToPropFloor) with a speed that grows by EntityParams.unk430
   (limit unk432), then frees its effects and removes it. */
static void Larva_DeathFallAndFade(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 = 0x40;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x11, 0, entity->params->unkC4, 1);
        ENTITY_FLAG_BITS(entity)->bit22 = 0;
        ENTITY_FLAG_BITS(entity)->bit20 = 1;
        ENTITY_FLAG_BITS(entity)->bit27 = 1;
        ENTITY_FLAG_BITS(entity)->bit26 = 1;
        ENTITY_FLAG_BITS(entity)->bits2 = 1;
        ENTITY_FLAG_BITS(entity)->bits2 = 2;
        ENTITY_FLAG_BITS(entity)->bit4 = 1;
        entity->unkA0 = 20;
        unkE0->unk68 = 0;
        entity->action_step++;
    case 1:
        unkE0->unk48 += (f32)entity->params->unk430 * ((f32)unkE0->unk68 / 30.0f);
        if ((f32)entity->params->unk432 < unkE0->unk48) {
            unkE0->unk48 = (f32)entity->params->unk432;
        }
        if (entity->unkA0 != 0) {
            entity->model->unk15C = (f32)entity->unkD6 * (f32)entity->unkA0 / 20.0f;
        } else {
            if (entity->params->unkC6 != 0 && entity->params->unkC6 != 0xFFFF) {
                Effect_FreeForOwner(entity);
            }
            ENTITY_FLAG_BITS(entity)->bits2 = 3;
            ENTITY_FLAG_BITS(entity)->bit1 = 0;
        }
        break;
    }
    entity->unk88 += unkE0->unk48 + unkE0->unk48;
    unkE0->unk68++;
}
