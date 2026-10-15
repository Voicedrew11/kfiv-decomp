/* FireGenie: the behaviour of entity type 66 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers it installs. Used by: Fire Genie. The game names no
   monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by
   hit points and experience, with resistances and drops where two monsters share those. */
#include "kf_entity.h"
#include "common.h"
#include "kf_entity_world.h"

#define ENTITY_WORK_AREA_SIZE 0xA0
void EntityAi_SetUpdate(char *, s32);
int EntityAi_StartAction(Entity *, void (*)(Entity *), int);
void Motion_ClearPartSlots(EntityModel *);
int Motion_StepModel(EntityModel *, s32, s32);
void Vector_Copy(void *, void *);
void EntityAi_Dormant(Entity *);
extern s32 D_003E6E40[][2];
int Motion_Step(void *, int);
// match: the callee is void, but retail keeps what it leaves in $2
int Motion_BlendToPose(EntityModel *, f32);
int EntityAi_LookAtPlayer(Entity *, void *, s32, void *, f32);
extern f32 D_00431AF0[16];
float EntityMath_AngleDifference(float, float);
void sceVu0CopyMatrix(void *, void *);
void sceVu0TransposeMatrix(void *, void *);
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0RotMatrixZ(float *, float *, float);
s32 Bone_MultiplyPathTo(void *, void *, void *);
void Bone_MatrixToAngles(void *, void *);
void Matrix_Multiply(void *, void *, void *);
float EntityMath_EaseAngleToward(float *, float, float);
void EntityAi_LowerWeightsAfterHit(char *);
void EntityAi_SetState(char *, u8);
int EntityAi_SetReaction(char *, void (*)(void), int);
void EntityAi_ReactionHeavy(Entity *);
extern u32 D_00446ED4;
void EntityAi_StepState(void *);
void EntityAi_SensePlayer(void *, int);
s32 EntityAi_ShouldGuard(Entity *, f32, f32);
void EntityAi_Guard(Entity *);
void EntityAi_MeleeAttack(Entity *);
void EntityAi_RangedAttack(Entity *);
int Motion_Start(void *, int, int, u32, int);
int EntityAi_LaunchShots(Entity *, EntityParamsUnk3D8 *, s32, s32);
void EntityAi_SetAttackDelay(Entity *, s32);
void EntityEvent_OnDeath(Entity *);
float EntityMath_WrapAngle(float);
// match: the definitions take the float last, but retail sets it up before the index
int EntityAi_CanUseMeleeFacing(Entity *, f32, s32);
int EntityAi_CanUseRangedFacing(Entity *, f32, s32); // match: as EntityAi_CanUseMeleeFacing
int rand(void);
void EntityAi_LowerAttackWeight(Entity *);
void EntityEvent_Respawn(char *);

static void FireGenie_Animate(Entity *entity);
static void FireGenie_TurnPart3ToPlayer(Entity *entity, s32 mode);
static void FireGenie_OnHit(Entity *entity);
static void FireGenie_Think(Entity *entity, s32 unknown);
static void FireGenie_ThinkDecide(Entity *entity, s32 unknown);
static void FireGenie_RangedAttack0(Entity *entity);
static void FireGenie_RespawnOther(Entity *entity);
static void FireGenie_Death(Entity *entity);
static void FireGenie_Move(Entity *entity);
static s32 FireGenie_CanAttack(Entity *entity);
static s32 FireGenie_PickAttack(Entity *entity);
static s32 FireGenie_ShouldRespawnOther(Entity *entity);
static void FireGenie_RespawnOneOther(Entity *entity);
static void FireGenie_SignalRespawned(Entity *entity);

/* Set-up of a type 66 entity: clears its work area and installs the type's handlers. */
void FireGenie_Init(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    u8 *unkBC = entity->unkBC;
    u8 *unkC0 = entity->unkC0;
    char *unk138 = entity->model->unk138;
    s32 i;

    // match: the three fills name their target inside the loop; unkBC and unkC0 above are
    // otherwise unused but retail computes them up front and copies them before each loop
    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        u8 *work_area = (u8 *)&entity->unkE0;

        work_area[i] = 0;
    }
    EntityAi_SetUpdate((char *)entity, D_003E6E40[entity->params->unk02][1]);
    unkE0->think = FireGenie_Think;
    unkE0->animate = (void (*)(void))FireGenie_Animate;
    unkE0->on_hit = (void (*)(void))FireGenie_OnHit;
    unkE0->move = (void (*)(void))FireGenie_Move;
    unkE0->dormant = (void (*)(void))EntityAi_Dormant;
    unkE0->death = (void (*)(void))FireGenie_Death;
    for (i = 3; i != -1; i--) {
        entity->unkBC[i] = 0x80;
    }
    for (i = 5; i != -1; i--) {
        entity->unkC0[i] = 0x80;
    }
    entity->target = NULL;
    entity->flags |= 0x10000000;
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
}

/* The animate handler: as EntityAi_Animate, but the motion never moves the entity, and
   FireGenie_TurnPart3ToPlayer turns model part 3 (mode 0 while a blend is on, mode 1 otherwise once
   the motion is past frame 0). */
static void FireGenie_Animate(Entity *entity) {
    f32 limits[4];
    u32 blend_frames; // match: converted to float as unsigned
    s64 flags;
    EntityUnkE0 *unkE0;
    s32 look_at_target;

    flags = entity->flags;
    unkE0 = &entity->unkE0;
    if (ENTITY_FLAG_BITS(entity)->bit14) {
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
        blend_frames = entity->model->unkFC->unk88;
        if (blend_frames != 0) {
            ENTITY_FLAG_BITS(entity)->bit34 = Motion_BlendToPose(entity->model, (entity->unkC6 & 0x12) ? unkE0->unk40 : 1.0f / (f32)blend_frames);
            FireGenie_TurnPart3ToPlayer(entity, 0);
        } else {
            ENTITY_FLAG_BITS(entity)->bit34 = Motion_Step(entity, ENTITY_FLAG_BITS(entity)->bits11);
            if (entity->model->unkFC->unk82) {
                FireGenie_TurnPart3ToPlayer(entity, 1);
            }
        }
        limits[1] = entity->params->unk430 * 0.017453292f;
        limits[0] = -entity->params->unk430 * 0.017453292f;
        limits[3] = entity->params->unk432 * 0.017453292f;
        limits[2] = -entity->params->unk432 * 0.017453292f;
        look_at_target = (s32)(entity->flags >> 9) & 1;
        if (look_at_target) {
            EntityAi_LookAtPlayer(entity, unkE0->unk20, look_at_target, limits, 12.0f);
        } else {
            EntityAi_LookAtPlayer(entity, unkE0->unk20, 0, NULL, 8.0f);
        }
    }
}

/* Helper of FireGenie_Animate: in mode 1 turns model part 3 by the angle of Entity.unk68 off the
   heading, kept between 0 and 40 degrees (work float unk48), worked out through the part's parent
   matrix; the part's three angles are work floats unk4C to unk54. In mode 0 it only puts those back
   on the part. */
static void FireGenie_TurnPart3ToPlayer(Entity *entity, s32 mode) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityModelPart80 *bone = &((EntityModelPart80 *)entity->model->unkF8)[3];
    f32 parent[16];
    f32 rotation[16];
    f32 twist[16];
    f32 angles[4];
    f32 yaw;
    f32 rate;

    if (bone != NULL) {
        if (mode != 0) {
            yaw = EntityMath_AngleDifference(entity->model->unkD0[1], entity->unk68);
            if (yaw > 0.6981317f) {
                yaw = 0.6981317f;
            }
            if (yaw < 0.0f) {
                yaw = 0.0f;
            }
            rate = 10.0f;
            EntityMath_EaseAngleToward(&unkE0->unk48, yaw, rate);
            sceVu0CopyMatrix(parent, D_00431AF0);
            Bone_MultiplyPathTo(entity->model->unkF8, bone, parent);
            parent[12] = 0.0f;
            parent[13] = 0.0f;
            parent[14] = 0.0f;
            parent[15] = 1.0f;
            sceVu0RotMatrixX(rotation, D_00431AF0, bone->unk50[0]);
            sceVu0RotMatrixZ(rotation, rotation, bone->unk50[2]);
            sceVu0RotMatrixY(rotation, rotation, bone->unk50[1]);
            Matrix_Multiply(rotation, parent, rotation);
            sceVu0RotMatrixY(twist, D_00431AF0, unkE0->unk48);
            Matrix_Multiply(rotation, twist, rotation);
            sceVu0TransposeMatrix(parent, parent);
            Matrix_Multiply(rotation, parent, rotation);
            Bone_MatrixToAngles(angles, rotation);
            EntityMath_EaseAngleToward(&unkE0->unk4C, angles[0], rate);
            EntityMath_EaseAngleToward(&unkE0->unk50, angles[1], rate);
            EntityMath_EaseAngleToward(&unkE0->unk54, angles[2], rate);
            bone->unk50[0] = unkE0->unk4C;
            bone->unk50[1] = unkE0->unk50;
            bone->unk50[2] = unkE0->unk54;
        } else {
            bone->unk50[0] = unkE0->unk4C;
            bone->unk50[1] = unkE0->unk50;
            bone->unk50[2] = unkE0->unk54;
        }
    }
}

/* The on_hit handler: as EntityAi_OnHit, but only a heavy hit starts a reaction
   (EntityAi_ReactionHeavy); with work byte unk7C set that hit also puts -1 in work byte unk7A of
   the entity with spawn id work byte unk7B, and clears unk7C. */
static void FireGenie_OnHit(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    u64 flags;
    Entity *other;

    EntityAi_LowerWeightsAfterHit((char *)entity);
    flags = entity->flags;
    if (((flags >> 36) & 3) == 2) {
        ENTITY_UNK5C(entity)->as_s32 = 0;
        entity->flags &= ~0x200;
        EntityAi_SetReaction((char *)entity, (void (*)(void))EntityAi_ReactionHeavy, 0);
        if (unkE0->unk7C) {
            other = Entity_GetBySpawnId(unkE0->unk7B);
            if (other != NULL) {
                other->unkE0.unk7A = -1;
            }
        }
        unkE0->unk7C = 0;
    }
    EntityAi_SetState((char *)entity, 2);
}

/* The think handler: no states; every tenth frame updates what the entity knows of the player, and
   while it is not busy calls FireGenie_ThinkDecide. Counts down work word unk68. */
static void FireGenie_Think(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (entity->unkC6 == 0) {
        EntityAi_StepState(entity);
        FireGenie_ThinkDecide(entity, unknown);
    }
    if (unkE0->unk68 != 0) {
        unkE0->unk68--;
    }
}

/* Decides for FireGenie_Think: guards the player's swing; starts FireGenie_RespawnOther when
   FireGenie_ShouldRespawnOther says so; when an attack reaches (FireGenie_CanAttack) starts
   the one FireGenie_PickAttack draws, FireGenie_RangedAttack0 for ranged record 0; else
   starts the move action. */
static void FireGenie_ThinkDecide(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    if (EntityAi_ShouldGuard(entity, 300.0f, 0.5235988f)) {
        EntityAi_StartAction(entity, EntityAi_Guard, 1);
    } else {
        entity->flags |= 0x400;
        entity->flags |= 0x200;
        if (FireGenie_ShouldRespawnOther(entity)) {
            EntityAi_StartAction(entity, FireGenie_RespawnOther, unknown);
        } else if (entity->unkA2 == 0 && FireGenie_CanAttack(entity)) {
            if (FireGenie_PickAttack(entity)) {
                if (((s32)(entity->flags >> 58) & 3) == 0) {
                    EntityAi_StartAction(entity, FireGenie_RangedAttack0, unknown);
                } else {
                    EntityAi_StartAction(entity, EntityAi_RangedAttack, unknown);
                }
            } else {
                EntityAi_StartAction(entity, EntityAi_MeleeAttack, unknown);
            }
        } else {
            EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
        }
    }
}

/* The ranged attack with record 0 (busy bit 1): animation 23, no turning; from frame
   EntityParams.unk434 to unk436 it sets flag bit 9 and launches the record's shots on two frames in
   three. */
static void FireGenie_RangedAttack0(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityParamsUnk3D8 *unk3D8 = &entity->params->unk3D8[0].record;

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x17, 0, entity->params->unkC4 >> 1, 1);
        ENTITY_FLAGS(entity) &= ~0x200;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (entity->model->unkFC->unk82 >= (s16)entity->params->unk434
            && entity->model->unkFC->unk82 <= entity->params->unk436) {
            ENTITY_FLAGS(entity) |= 0x200;
            if (D_00446ED4 % 3U != 0) {
                EntityAi_LaunchShots(entity, unk3D8, (s16)entity->params->unk434, entity->params->unk436);
            }
        }
        if (entity->model->unkFC->unk82 > 0 && unk3D8->unk03 >= entity->model->unkFC->unk82) {
            ENTITY_FLAGS(entity) |= 0x80000;
        } else {
            ENTITY_FLAGS(entity) &= ~0x80000;
        }
        if ((s32)(ENTITY_FLAGS(entity) >> 34) & 1) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            EntityAi_SetAttackDelay(entity, unk3D8->unk02);
            if ((s32)(ENTITY_FLAGS(entity) >> 5) & 1) {
                unkE0->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            }
        }
        break;
    }
}

/* An action (busy bit 0x1000) that FireGenie_ThinkDecide starts when
   FireGenie_ShouldRespawnOther says so: plays animation 17 to its end. Past frame
   EntityParams.unk438 it calls FireGenie_RespawnOneOther and sets work byte unk7C; past frame
   unk43A it calls FireGenie_SignalRespawned and clears unk7C. */
static void FireGenie_RespawnOther(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 0x1000;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x11, 0, entity->params->unkC4, 1);
        ENTITY_FLAGS(entity) = (ENTITY_FLAGS(entity) & ~0x3800) | 0x800;
        ENTITY_FLAGS(entity) &= ~0x200;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (entity->params->unk438 < entity->model->unkFC->unk82) {
            FireGenie_RespawnOneOther(entity);
            ENTITY_FLAGS(entity) |= 0x200;
            unkE0->unk7C = 1;
            entity->action_step++;
        }
        break;
    case 3:
        if (entity->params->unk43A < entity->model->unkFC->unk82) {
            FireGenie_SignalRespawned(entity);
            unkE0->unk7C = 0;
            entity->action_step++;
        }
        break;
    case 4:
        if ((s32)(ENTITY_FLAGS(entity) >> 34) & 1) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if ((s32)(ENTITY_FLAGS(entity) >> 5) & 1) {
                unkE0->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            }
        }
        break;
    }
}

/* The death action (busy bit 0x40): animation 16, the fade over 20 frames and the removal, as
   EntityAi_Death with other flag settings; with work byte unk7C set it first puts -1 in work byte
   unk7A of the entity with spawn id work byte unk7B. */
static void FireGenie_Death(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 = 0x40;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x10, 0, 5, 1);
        ENTITY_FLAGS(entity) &= ~0x3800;
        ENTITY_FLAGS(entity) |= 0x4000;
        ENTITY_FLAGS(entity) |= 0x100000;
        ENTITY_FLAGS(entity) = (ENTITY_FLAGS(entity) & ~0xC) | 4;
        ENTITY_FLAGS(entity) &= ~0x200;
        if (unkE0->unk7C != 0) {
            Entity *other = Entity_GetBySpawnId(unkE0->unk7B);

            if (other != NULL) {
                other->unkE0.unk7A = -1;
            }
        }
        unkE0->unk7C = 0;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if ((s32)(ENTITY_FLAGS(entity) >> 34) & 1) {
            ENTITY_FLAGS(entity) &= ~0x4000;
            ENTITY_FLAGS(entity) = (ENTITY_FLAGS(entity) & ~0xC) | 8;
            ENTITY_FLAGS(entity) |= 0x10;
            ENTITY_FLAGS(entity) |= 0x8000000;
            ENTITY_FLAGS(entity) |= 0x4000000;
            entity->unkA0 = 20;
            EntityEvent_OnDeath(entity);
            entity->action_step++;
        }
        break;
    case 3:
        if (entity->unkA0 != 0) {
            entity->model->unk15C = (f32)entity->unkD6 * (f32)entity->unkA0 / 20.0f;
        } else {
            ENTITY_FLAGS(entity) |= 0xC;
            ENTITY_FLAGS(entity) &= ~2;
        }
        break;
    }
}

/* The move handler: this type stays where it is; keeps animation 0. */
static void FireGenie_Move(Entity *entity) {
    Motion_Start(entity, 0, 0, entity->params->unkC4, 0);
}

/* Whether the player is in reach of any of the six attack records (EntityAi_CanUseMeleeFacing for the melee
   ones, EntityAi_CanUseRangedFacing for the ranged), along the heading of the turned part (work float unk48). */
static s32 FireGenie_CanAttack(Entity *entity) {
    f32 angle = EntityMath_WrapAngle(entity->model->unkD0[1] + entity->unkE0.unk48);

    if (EntityAi_CanUseMeleeFacing(entity, angle, 0)) {
        return TRUE;
    }
    if (EntityAi_CanUseMeleeFacing(entity, angle, 1)) {
        return TRUE;
    }
    if (EntityAi_CanUseMeleeFacing(entity, angle, 2)) {
        return TRUE;
    }
    if (EntityAi_CanUseRangedFacing(entity, angle, 0)) {
        return TRUE;
    }
    if (EntityAi_CanUseRangedFacing(entity, angle, 1)) {
        return TRUE;
    }
    return EntityAi_CanUseRangedFacing(entity, angle, 2) != 0;
}

/* Draws one of the six attacks by weight among those that reach the player, sets flag bits 57 to 59
   to it and returns whether it is a ranged one: EntityAi_PickAttack with the heading of the turned part.
   */
static s32 FireGenie_PickAttack(Entity *entity) {
    s32 chances[6];
    s32 choice = 0;
    s32 total = 0;
    s32 chance;
    s32 i;
    s32 roll;
    s32 sum;
    f32 angle;

    angle = EntityMath_WrapAngle(entity->model->unkD0[1] + entity->unkE0.unk48);
    chance = EntityAi_CanUseMeleeFacing(entity, angle, 0) ? (entity->unkC0[0] * entity->params->unk240[0].unk00) >> 6 : 0;
    chances[0] = chance;
    total += chance;
    chance = EntityAi_CanUseMeleeFacing(entity, angle, 1) ? (entity->unkC0[1] * entity->params->unk240[1].unk00) >> 6 : 0;
    chances[1] = chance;
    total += chance;
    chance = EntityAi_CanUseMeleeFacing(entity, angle, 2) ? (entity->unkC0[2] * ((EntityParamsUnk240 *)entity->params->unk350)->unk00) >> 6 : 0;
    chances[2] = chance;
    total += chance;
    chance = EntityAi_CanUseRangedFacing(entity, angle, 0) ? (entity->unkC0[3] * ENTITY_PARAMS_UNK3D8(entity->params, 0)->unk00) >> 6 : 0;
    chances[3] = chance;
    total += chance;
    chance = EntityAi_CanUseRangedFacing(entity, angle, 1) ? (entity->unkC0[4] * ENTITY_PARAMS_UNK3D8(entity->params, 1)->unk00) >> 6 : 0;
    chances[4] = chance;
    total += chance;
    chance = EntityAi_CanUseRangedFacing(entity, angle, 2) ? (entity->unkC0[5] * ENTITY_PARAMS_UNK3D8(entity->params, 2)->unk00) >> 6 : 0;
    chances[5] = chance;
    total += chance;
    if (total == 0) {
        total++;
    }
    roll = rand() % total;
    sum = 0;
    for (i = 0; i < 6; i++) {
        sum += chances[i];
        if (roll < sum) {
            choice = i;
            break;
        }
    }
    if (choice < 3) {
        ENTITY_FLAG_BITS(entity)->bits58 = choice;
        ENTITY_FLAG_BITS(entity)->bit57 = 0;
    } else {
        ENTITY_FLAG_BITS(entity)->bits58 = choice - 3;
        ENTITY_FLAG_BITS(entity)->bit57 = 1;
    }
    EntityAi_LowerAttackWeight(entity);
    return (s32)(entity->flags >> 57) & 1;
}

/* Helper of FireGenie_ThinkDecide: counts the entities with spawn ids 10 to 9 plus
   EntityParams.unk43C that have flag bits 0 and 1 set and flag bits 2-3 below 3 (the count goes to
   work byte unk7A, a bit each to work word unk6A); TRUE when there are fewer than unk43C and the
   pause in work word unk68 is over. */
static s32 FireGenie_ShouldRespawnOther(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    s32 count = 0;
    s32 i;

    unkE0->unk6A = 0;
    for (i = 0; i < entity->params->unk43C; i++) {
        Entity *other = Entity_GetBySpawnId(i + 10);
        if (other != NULL && (other->flags & 3) == 3 && ((s32)(other->flags >> 2) & 3) < 3) {
            unkE0->unk6A |= 1 << i;
            count++;
        }
    }
    unkE0->unk7A = count;
    if (unkE0->unk7A < entity->params->unk43C) {
        if (unkE0->unk68 < 1) {
            return TRUE;
        }
    }
    return FALSE;
}

/* Helper of FireGenie_RespawnOther: draws one of the entities FireGenie_ShouldRespawnOther
   did not count, respawns it when it has flag bit 0 set (EntityEvent_Respawn, which calls
   Entity_ResetToSpawn), keeps its spawn id in work byte unk7B, and sets the pause in work word
   unk68 to EntityParams.unk43E. */
static void FireGenie_RespawnOneOther(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    s32 remaining = 1;
    s32 i;
    s32 slots_left = entity->params->unk43C - unkE0->unk7A;

    for (i = 0; i < entity->params->unk43C; i++) {
        if (!((unkE0->unk6A >> i) & 1)) {
            if (rand() % 100 < remaining * 100 / slots_left) {
                s32 other_id = i + 10;
                Entity *other = Entity_GetBySpawnId(other_id);
                if (other != NULL && ((s32)other->flags & 1)) {
                    EntityEvent_Respawn((char *)other);
                    unkE0->unk7B = other_id;
                    remaining--;
                }
            }
            if (remaining <= 0) {
                break;
            }
            slots_left--;
            if (slots_left <= 0) {
                break;
            }
        }
    }
    unkE0->unk68 = entity->params->unk43E;
}

/* Helper of FireGenie_RespawnOther: puts 1 in work byte unk7A of the entity
   FireGenie_RespawnOneOther respawned (its spawn id is in work byte unk7B). */
static void FireGenie_SignalRespawned(Entity *entity) {
    Entity *other = Entity_GetBySpawnId(entity->unkE0.unk7B);

    if (other != NULL) {
        other->unkE0.unk7A = 1;
    }
}
