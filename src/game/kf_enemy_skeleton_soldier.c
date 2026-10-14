/* SkeletonSoldier: the behaviour of entity type 6 (Entity.params->unk02, the index into D_003E6E40): its
   set-up function, which the table names, and the handlers it installs. Used by: Skeleton Soldier
   Roundshield, Skeleton Soldier Squareshield, Skeleton Soldier Axe, High Skeleton Soldier. The game names no
   monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by
   hit points and experience, with resistances and drops where two monsters share those. */
#include "kf_entity.h"
#include "common.h"

void func_001958A8(char *, s32);
int EntityAi_StartAction(Entity *, void (*)(Entity *), int);
void func_00191478(EntityModel *);
int func_00193378(EntityModel *, s32, s32);
void func_00216778(void *, void *);
// match: the definition takes an Actor, a struct local to EntityAi_Animate.c
void EntityAi_Animate(void);
void EntityAi_OnHit(char *);
void EntityAi_Move(Entity *);
void EntityAi_Death(Entity *);
extern s32 D_003E6E40[][2];
extern u32 D_00446ED4;
void func_00196870(void *, int);
void func_00196CC0(char *);
void func_0018E9C0(void *);
int EntityAi_ThinkReturn(void *, int);
int EntityAi_ThinkIdle(void *, int);
void EntityAi_ThinkPursue(Entity *, s32);
void EntityAi_ThinkCombat(Entity *, s32);
int func_00192C98(void *, int, int, u32, int);
void func_00195898(char *, u8);
float func_00194F80(void *, void *);
int rand(void);
s32 func_0018ED98(Entity *, f32, f32);
s32 func_0018EEE0(Entity *, f32, f32);
s32 func_0018FD50(Entity *, s32);
void func_00190B88(Entity *, s32);
int func_0018F030(char *);
void EntityAi_Dodge(Entity *);
void EntityAi_Guard(Entity *);
void EntityAi_MeleeAttack(Entity *);
void EntityAi_RangedAttack(Entity *);

static void SkeletonSoldier_Think(Entity *entity, s32 unknown);
static void SkeletonSoldier_Dormant(Entity *entity);
static void SkeletonSoldier_PlayAnim17(Entity *entity);
static void SkeletonSoldier_ThinkClearFlag25Away(Entity *entity, s32 unknown);
static void SkeletonSoldier_DormantAnim18Then19(Entity *entity);
static void SkeletonSoldier_ThinkNoStates(Entity *entity, s32 unknown);
static void SkeletonSoldier_ThinkDecideAttackOrWait(Entity *entity, s32 unknown);

/* Set-up of a type 6 entity: clears its work area and installs the type's handlers. */
void SkeletonSoldier_Init(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    char *unk138 = entity->model->unk138;
    s32 i;

    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        ENTITY_WORK_BYTES(entity)[i] = 0;
    }
    func_001958A8((char *)entity, D_003E6E40[entity->params->unk02][1]);
    unkE0->animate = EntityAi_Animate;
    unkE0->on_hit = (void (*)(void))EntityAi_OnHit;
    unkE0->move = (void (*)(void))EntityAi_Move;
    unkE0->dormant = (void (*)(void))SkeletonSoldier_Dormant;
    unkE0->death = (void (*)(void))EntityAi_Death;
    if (entity->spawn_info->unk40 == 1) {
        unkE0->think = (void (*)(Entity *, s32))SkeletonSoldier_ThinkNoStates;
    } else if (entity->spawn_info->unk40 >= 2 && entity->spawn_info->unk40 <= 3) {
        unkE0->think = SkeletonSoldier_ThinkClearFlag25Away;
        unkE0->dormant = (void (*)(void))SkeletonSoldier_DormantAnim18Then19;
    } else {
        unkE0->think = SkeletonSoldier_Think;
    }
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
        func_00216778(unkE0->unk20, unk138 + 0x50);
    }
    unkE0->unk40 = 0.5f;
    entity->unkD6 = (entity->params->flags & 0x4000) ? 0x40 : 0x80;
    entity->model->unk15C = entity->unkD6;
    func_00191478(entity->model);
    func_00193378(entity->model, 0, 0);
}

/* The think handler: as EntityAi_Think; in state 2 with flag bit 32 set it first plays
   SkeletonSoldier_PlayAnim17 once (work word unk6C says whether it has; state 0 clears it). */
static void SkeletonSoldier_Think(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        func_00196870(entity, 1);
    }
    if (entity->unkC6 == 0) {
        func_00196CC0((char *)entity);
        func_0018E9C0(entity);
        if (ENTITY_FLAG_BITS(entity)->bit53) {
            EntityAi_ThinkReturn(entity, unknown);
        } else {
            switch (entity->state) {
            case 0:
                EntityAi_ThinkIdle(entity, unknown);
                unkE0->unk6C = 0;
                break;
            case 1:
                EntityAi_ThinkPursue(entity, unknown);
                break;
            case 2:
                if (ENTITY_FLAG_BITS(entity)->bit32 && unkE0->unk6C == 0) {
                    EntityAi_StartAction(entity, SkeletonSoldier_PlayAnim17, unknown);
                    unkE0->unk6C = 1;
                } else {
                    EntityAi_ThinkCombat(entity, unknown);
                }
                break;
            default:
                EntityAi_ThinkIdle(entity, unknown);
                break;
            }
        }
    }
}

/* The dormant action (busy bit 0x20): as EntityAi_Dormant, but with EntitySpawnInfo.unk40 set the
   animations are 18 and 19 instead of 1 and 2; flag bit 20 is set and flag bit 17 clear while it
   lasts. */
static void SkeletonSoldier_Dormant(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 0x20;
    switch (entity->action_step) {
    case 0:
        if (entity->spawn_info->unk40 != 0) {
            func_00192C98(entity, 0x12, 0, 0, 1);
        } else {
            func_00192C98(entity, 1, 0, 0, 1);
        }
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
        ENTITY_FLAG_BITS(entity)->bit18 = 1;
        ENTITY_FLAG_BITS(entity)->bit17 = 0;
        ENTITY_FLAG_BITS(entity)->bit28 = 1;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->action_step++;
        break;
    case 1:
        if (entity->distance_to_player < entity->spawn_info->unk54 || ENTITY_FLAG_BITS(entity)->bit43) {
            if (entity->spawn_info->unk40 != 0) {
                func_00192C98(entity, 0x13, 0, entity->params->unkC4, 0);
            } else {
                func_00192C98(entity, 2, 0, entity->params->unkC4, 0);
            }
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
            func_00195898((char *)entity, 2);
            ENTITY_FLAG_BITS(entity)->bit21 = 0;
            ENTITY_FLAG_BITS(entity)->bit20 = 0;
            ENTITY_FLAG_BITS(entity)->bit22 = 0;
            ENTITY_FLAG_BITS(entity)->bit23 = 0;
            ENTITY_FLAG_BITS(entity)->bit24 = 0;
            ENTITY_FLAG_BITS(entity)->bit18 = 0;
            ENTITY_FLAG_BITS(entity)->bit17 = 1;
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

/* An action (busy bit 0x1000): plays animation 17 to its end with flag bit 9 clear. The think
   handlers start it once on entering state 2. */
static void SkeletonSoldier_PlayAnim17(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 0x1000;
    ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 0x11, 0, entity->params->unkC4, 0);
        if (entity->params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->bits11 = (((EntityParamsWord *)entity->params)->unk00 & 0x6000000) == 0x2000000 ? 2 : 1;
        }
        unkE0->unk1C = 0.0f;
        entity->action_step++;
        break;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->bit34) {
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

/* The think handler the set-up function installs for EntitySpawnInfo.unk40 2 and 3: as
   SkeletonSoldier_Think; with unk40 3 it also clears flag bit 25 once the entity is more than 180 from
   its spawn point (SkeletonSoldier_DormantAnim18Then19 leaves the bit set in that mode). What flag bit
   25 does is not known; scripts read and set it (func_00216088, func_00216408). */
static void SkeletonSoldier_ThinkClearFlag25Away(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        func_00196870(entity, 1);
    }
    if (entity->spawn_info->unk40 == 3 && func_00194F80(entity->model->position, entity->spawn_info) > 180.0f) {
        ENTITY_FLAG_BITS(entity)->bit25 = 0;
    }
    if (entity->unkC6 == 0) {
        func_00196CC0((char *)entity);
        func_0018E9C0(entity);
        if (ENTITY_FLAG_BITS(entity)->bit53) {
            EntityAi_ThinkReturn(entity, unknown);
        } else {
            switch (entity->state) {
            case 0:
                EntityAi_ThinkIdle(entity, unknown);
                unkE0->unk6C = 0;
                break;
            case 1:
                EntityAi_ThinkPursue(entity, unknown);
                break;
            case 2:
                if (ENTITY_FLAG_BITS(entity)->bit32 && unkE0->unk6C == 0) {
                    EntityAi_StartAction(entity, SkeletonSoldier_PlayAnim17, unknown);
                    unkE0->unk6C = 1;
                } else {
                    EntityAi_ThinkCombat(entity, unknown);
                }
                break;
            default:
                EntityAi_ThinkIdle(entity, unknown);
                break;
            }
        }
    }
}

/* The dormant action (busy bit 0x20) the set-up function installs for EntitySpawnInfo.unk40 2 and
   3: as SkeletonSoldier_Dormant with animations 18 and 19; it also sets flag bit 25, and clears it at
   the end only when unk40 is 2. */
static void SkeletonSoldier_DormantAnim18Then19(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 0x20;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 0x12, 0, 0, 1);
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
        ENTITY_FLAG_BITS(entity)->bit17 = 0;
        ENTITY_FLAG_BITS(entity)->bit28 = 1;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->action_step++;
        break;
    case 1:
        if (entity->distance_to_player < entity->spawn_info->unk54 || ENTITY_FLAG_BITS(entity)->bit43) {
            func_00192C98(entity, 0x13, 0, entity->params->unkC4, 0);
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
            func_00195898((char *)entity, 2);
            ENTITY_FLAG_BITS(entity)->bit21 = 0;
            ENTITY_FLAG_BITS(entity)->bit20 = 0;
            ENTITY_FLAG_BITS(entity)->bit22 = 0;
            ENTITY_FLAG_BITS(entity)->bit23 = 0;
            ENTITY_FLAG_BITS(entity)->bit24 = 0;
            if (entity->spawn_info->unk40 == 2) {
                ENTITY_FLAG_BITS(entity)->bit25 = 0;
            }
            ENTITY_FLAG_BITS(entity)->bit18 = 0;
            ENTITY_FLAG_BITS(entity)->bit17 = 1;
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

/* The think handler of mode 1 (EntitySpawnInfo.unk40 is 1): as EntityAi_Think without the states;
   while flag bit 53 is clear it calls SkeletonSoldier_ThinkDecideAttackOrWait. */
static void SkeletonSoldier_ThinkNoStates(Entity *entity, s32 unknown) {
    u32 phase;

    phase = D_00446ED4 % 10U;
    if (phase == entity->unkD7 % 10U) {
        func_00196870(entity, 1);
    }
    if (entity->unkC6 != 0) {
        return;
    }
    if ((((EntityParamsWord *)entity->params)->unk00 & 0x06000000) != 0x06000000) {
        func_00196CC0((char *)entity);
    }
    func_0018E9C0(entity);
    if ((s32)(entity->flags >> 53) & 1) {
        EntityAi_ThinkReturn(entity, unknown);
        return;
    }
    SkeletonSoldier_ThinkDecideAttackOrWait(entity, unknown);
}

/* Decides for SkeletonSoldier_ThinkNoStates: as EntityAi_ThinkCombat, but it never closes in: when
   func_0018FD50 does not choose the attack it waits 30 to 59 frames. The motion never moves the
   entity. */
static void SkeletonSoldier_ThinkDecideAttackOrWait(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    s32 side;

    ENTITY_FLAG_BITS(entity)->bits11 = 0;
    if (func_0018ED98(entity, 300.0f, 0.5235988f)) {
        EntityAi_StartAction(entity, EntityAi_Guard, 1);
        return;
    }
    if (func_0018EEE0(entity, 300.0f, 0.5235988f)) {
        EntityAi_StartAction(entity, EntityAi_Dodge, 1);
        return;
    }
    side = func_0018FD50(entity, 0);
    ENTITY_FLAG_BITS(entity)->bit10 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit9 = TRUE;
    if (entity->unkA0 == 0 || side == 0) {
        entity->unkCF = entity->unkCD;
        entity->unkCD = side;
        if (entity->unkCD != entity->unkCF) {
            entity->unk9E = D_00446ED4;
        }
        func_00190B88(entity, side);
        if (entity->unkCD == 0) {
            if (func_0018F030((char *)entity)) {
                EntityAi_StartAction(entity, EntityAi_RangedAttack, unknown);
            } else {
                EntityAi_StartAction(entity, EntityAi_MeleeAttack, unknown);
            }
            return;
        }
        entity->unkA0 = rand() % 30 + 30;
        unkE0->unk78 = -1;
    }
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}
