/* ExpeditionSoldier: the behaviour of entity type 7 (Entity.params->unk02, the index into D_003E6E40): its
   set-up function, which the table names, and the handlers it installs. Used by: Expedition Soldier, Lee
   Maynor. The game names no monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched
   to the chr.prm records by hit points and experience, with resistances and drops where two monsters share
   those. */
#include "kf_entity.h"
#include "kf_entity_world.h"
#include "kf_map_object.h"

#define ENTITY_WORK_AREA_SIZE 0xA0
void EntityAi_SetUpdate(char *, s32);
int EntityAi_StartAction(Entity *, void (*)(Entity *), int);
void Motion_ClearPartSlots(EntityModel *);
int Motion_StepModel(EntityModel *, s32, s32);
void Vector_Copy(void *, void *);
// match: the definition takes an Actor, a struct local to EntityAi_Animate.c
void EntityAi_Animate(void);
void EntityAi_OnHit(char *);
void EntityAi_Move(Entity *);
void EntityAi_Dormant(Entity *);
extern s32 D_003E6E40[][2];
int Motion_Start(void *, int, int, u32, int);
extern u32 D_00446ED4;
void EntityAi_SensePlayer(void *, int);
void EntityAi_CheckFarFromSpawn(char *);
void EntityAi_StepState(void *);
int EntityAi_ThinkReturn(void *, int);
int EntityAi_ThinkIdle(void *, int);
void EntityAi_ThinkPursue(Entity *, s32);
extern float D_00413F80[];
int rand(void);
s32 EntityAi_ShouldGuard(Entity *, f32, f32);
s32 EntityAi_ShouldDodge(Entity *, f32, f32);
s32 EntityAi_PickCombatChoice(Entity *, s32);
void EntityAi_LowerChoiceWeight(Entity *, s32);
int EntityAi_PickAttack(char *);
void func_00196D48(Entity *);
float EntityMath_AngleDifference(float, float);
int EntityAi_TurnCommand(Entity *);
int EntityAi_TurnFromPush(void *, void *);
s32 EntityAi_MoveCommand(Entity *, s32, f32);
void EntityAi_Dodge(Entity *);
void EntityAi_Guard(Entity *);
void EntityAi_MeleeAttack(Entity *);
void EntityAi_RangedAttack(Entity *);
void EntityEvent_OnDeath(Entity *);
void EntityEvent_Respawn(char *);
s32 EntityAi_CanAttack(Entity *);
float EntityMath_DistanceXZ(void *, void *);

static void ExpeditionSoldier_ThinkHoldAnim18Or19(Entity *entity);
static void ExpeditionSoldier_Think(Entity *entity, s32 unknown);
static void func_001787E0(Entity *entity, s32 unknown);
static void ExpeditionSoldier_Death(Entity *entity);
static void ExpeditionSoldier_SignalMapObject(Entity *entity);
static void ExpeditionSoldier_ThinkUseMapObjectsPosX(Entity *entity, s32 unknown);
static void ExpeditionSoldier_WalkToMapObjectPosX(Entity *entity, s32 unknown);
static void ExpeditionSoldier_ThinkUseMapObjectsNegX(Entity *entity, s32 unknown);
static void ExpeditionSoldier_WalkToMapObjectNegX(Entity *entity, s32 unknown);

/* Set-up of a type 7 entity: clears its work area and installs the type's handlers. */
void ExpeditionSoldier_Init(Entity *entity) {
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
    switch (entity->spawn_info->variant) {
    case 1:
    case 2:
        unkE0->think = (void (*)(Entity *, s32))ExpeditionSoldier_ThinkHoldAnim18Or19;
        unkE0->animate = EntityAi_Animate;
        break;
    case 3:
        unkE0->think = ExpeditionSoldier_ThinkUseMapObjectsPosX;
        unkE0->animate = EntityAi_Animate;
        unkE0->on_hit = (void (*)(void))EntityAi_OnHit;
        unkE0->move = (void (*)(void))EntityAi_Move;
        unkE0->dormant = (void (*)(void))EntityAi_Dormant;
        unkE0->death = (void (*)(void))ExpeditionSoldier_Death;
        break;
    case 4:
        unkE0->think = ExpeditionSoldier_ThinkUseMapObjectsNegX;
        unkE0->animate = EntityAi_Animate;
        unkE0->on_hit = (void (*)(void))EntityAi_OnHit;
        unkE0->move = (void (*)(void))EntityAi_Move;
        unkE0->dormant = (void (*)(void))EntityAi_Dormant;
        unkE0->death = (void (*)(void))ExpeditionSoldier_Death;
        break;
    default:
        unkE0->think = ExpeditionSoldier_Think;
        unkE0->animate = EntityAi_Animate;
        unkE0->on_hit = (void (*)(void))EntityAi_OnHit;
        unkE0->move = (void (*)(void))EntityAi_Move;
        unkE0->dormant = (void (*)(void))EntityAi_Dormant;
        unkE0->death = (void (*)(void))ExpeditionSoldier_Death;
        break;
    }
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
    if (unk138 != NULL) {
        Vector_Copy(unkE0->unk20, unk138 + 0x50);
    }
    unkE0->unk40 = 0.5f;
    entity->unkD6 = (entity->params->flags & 0x4000) ? 0x40 : 0x80;
    entity->model->unk15C = entity->unkD6;
    Motion_ClearPartSlots(entity->model);
    Motion_StepModel(entity->model, 0, 0);
}

/* The think handler the set-up function installs for EntitySpawnInfo.variant 1 and 2 (it takes the
   entity only): holds animation 18 (variant 1) or 19 with flag bits 16, 18, 20 to 26 and 28 set and
   starts no action. The set-up function installs no other handler but EntityAi_Animate for these.
   */
static void ExpeditionSoldier_ThinkHoldAnim18Or19(Entity *entity) {
    if (entity->spawn_info->variant == 1) {
        Motion_Start(entity, 0x12, 0, 0, 0);
    } else {
        Motion_Start(entity, 0x13, 0, 0, 0);
    }
    entity->flags |= 0x40000;
    entity->flags |= 0x100000;
    entity->flags |= 0x200000;
    entity->flags |= 0x400000;
    entity->flags |= 0x800000;
    entity->flags |= 0x1000000;
    entity->flags |= 0x2000000;
    entity->flags |= 0x4000000;
    entity->flags |= 0x10000;
    entity->flags |= 0x10000000;
}

/* The think handler (the set-up function's default, for any EntitySpawnInfo.variant but 1 to 4): as
   EntityAi_Think, with func_001787E0 for state 2. */
static void ExpeditionSoldier_Think(Entity *entity, s32 unknown) {
    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (entity->unkC6 != 0) {
        return;
    }
    EntityAi_CheckFarFromSpawn((char *)entity);
    EntityAi_StepState(entity);
    if (ENTITY_FLAG_BITS(entity)->bit53) {
        EntityAi_ThinkReturn(entity, unknown);
        return;
    }
    switch (entity->state) {
    case 0:
        EntityAi_ThinkIdle(entity, unknown);
        return;
    case 1:
        EntityAi_ThinkPursue(entity, unknown);
        return;
    case 2:
        func_001787E0(entity, unknown);
        return;
    }
    EntityAi_ThinkIdle(entity, unknown);
}

static void func_001787E0(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    f32 angle;
    s32 side;
    s32 turn;

    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    if (EntityAi_ShouldGuard(entity, 300.0f, 0.5235988f)) {
        EntityAi_StartAction(entity, EntityAi_Guard, 1);
        return;
    }
    if (EntityAi_ShouldDodge(entity, 300.0f, 0.5235988f)) {
        EntityAi_StartAction(entity, EntityAi_Dodge, 1);
        return;
    }
    side = EntityAi_PickCombatChoice(entity, 0);
    ENTITY_FLAG_BITS(entity)->bit9 = TRUE;
    if (entity->unkA0 == 0 || side == 0) {
        entity->unkCF = entity->unkCD;
        entity->unkCD = side;
        if (entity->unkCD != entity->unkCF) {
            entity->unk9E = D_00446ED4;
        }
        EntityAi_LowerChoiceWeight(entity, side);
        switch (entity->unkCD) {
        case 0:
            if (EntityAi_PickAttack((char *)entity)) {
                EntityAi_StartAction(entity, EntityAi_RangedAttack, unknown);
            } else {
                EntityAi_StartAction(entity, EntityAi_MeleeAttack, unknown);
            }
            return;
        case 1:
            if ((s16)entity->params->unk434 < entity->distance_to_player) {
                angle = EntityMath_AngleDifference(entity->unk68, entity->model->unkD0[1]);
                angle = ABS(angle);
                if (angle > 0.7853982f) {
                    entity->target = D_00413F80;
                    entity->unkA0 = rand() % 30 + 0x3C;
                    unkE0->unk78 = 0;
                } else {
                    unkE0->unk78 = -1;
                }
            } else {
                entity->target = D_00413F80;
                entity->unkA0 = rand() % 60 + 0x78;
                unkE0->unk78 = 1;
            }
            break;
        case 2:
            entity->unkA0 = rand() % 30 + 0x3C;
            unkE0->unk78 = -1;
            break;
        default:
            unkE0->unk78 = -1;
            break;
        }
    }
    if (entity->unkCD == 1) {
        func_00196D48(entity);
        if (ENTITY_FLAG_BITS(entity)->bit31) {
            angle = EntityMath_AngleDifference(entity->model->unkD0[1], entity->unk68);
            if (unkE0->unk79 == 0) {
                unkE0->unk79 = ABS(angle) > 0.5235988f;
                if (unkE0->unk79 == 0) {
                    goto done;
                }
            }
            entity->unkC8 = EntityAi_TurnCommand(entity);
            if (entity->unkC8 == 0) {
                unkE0->unk79 = 0;
            }
        } else {
            turn = EntityAi_TurnFromPush(entity, direction);
            switch (unkE0->unk78) {
            case 0:
                if (turn > 0) {
                    entity->unkC8 = 0x201;
                } else if (turn < 0) {
                    entity->unkC8 = 0x101;
                } else {
                    entity->unkC8 = EntityAi_TurnCommand(entity);
                }
                break;
            case 1:
                if (turn > 0) {
                    entity->unkC8 = 0x201;
                } else if (turn < 0) {
                    entity->unkC8 = 0x101;
                } else {
                    entity->unkC8 = EntityAi_MoveCommand(entity, 0, entity->radius + 100.0f + 30.0f);
                }
                break;
            }
        }
    }
done:
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}

/* The death action (busy bit 0x40): as EntityAi_Death; when the fade starts and
   EntitySpawnInfo.unk40 is set, it calls EntityEvent_Respawn for the entity with that spawn id (if its
   flag bit 0 is set) and puts it at this entity's position. */
static void ExpeditionSoldier_Death(Entity *entity) {
    entity->unkC6 = 0x40;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x10, 0, 5, 1);
        if (entity->params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            s32 kind = 2;

            if (!(ENTITY_PARAMS_BITS(entity)->bit25 && !ENTITY_PARAMS_BITS(entity)->bit26)) {
                kind = 1;
            }
            ENTITY_FLAG_BITS(entity)->bits11 = kind;
        }
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
            EntityEvent_OnDeath(entity);
            if (entity->spawn_info->unk40 != 0) {
                Entity *partner = Entity_GetBySpawnId(entity->spawn_info->unk40);

                if (partner != NULL && ((s32)partner->flags & 1)) {
                    EntityEvent_Respawn((char *)partner);
                    Vector_Copy(partner->model->position, entity->model->position);
                }
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

/* An action (busy bit 0x1000): plays animation 17 to its end; on frame 20 it calls MapObject_Signal
   with 1 for the map object work byte unk7C names. ExpeditionSoldier_WalkToMapObjectPosX and
   ExpeditionSoldier_WalkToMapObjectNegX start it. */
static void ExpeditionSoldier_SignalMapObject(Entity *entity) {
    EntityUnkE0Kind178358 *unkE0 = (EntityUnkE0Kind178358 *)&entity->unkE0;
    MapObject *map_object;

    entity->unkC6 |= 0x1000;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x11, 0, entity->params->unkC4, 0);
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        unkE0->unk1C = 0;
        entity->action_step++;
        break;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (entity->model->unkFC->unk82 == 20) {
            map_object = MapObject_GetById(entity->group_id, unkE0->unk7C);
            if (map_object != NULL) {
                MapObject_Signal(map_object, 1);
            }
        }
        if ((s32)(entity->flags >> 34) & 1) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if ((s32)(entity->flags >> 5) & 1) {
                unkE0->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            }
        }
        break;
    }
}

/* The think handler the set-up function installs for EntitySpawnInfo.variant 3: as
   ExpeditionSoldier_Think (with func_001787E0 for state 1 too), but it first reads bit 17 of the two map
   objects EntitySpawnInfo.unk42 and unk43 name into work bytes unk7A and unk7B, and while either is
   clear it calls ExpeditionSoldier_WalkToMapObjectPosX instead of stepping the states. PosX: its points
   are at x 950 and 900; ExpeditionSoldier_ThinkUseMapObjectsNegX has them at -950 and -900. */
static void ExpeditionSoldier_ThinkUseMapObjectsPosX(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    MapObject *map_object;

    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (entity->unkC6 != 0) {
        return;
    }
    map_object = MapObject_GetById(entity->group_id, entity->spawn_info->unk42);
    if (map_object != NULL) {
        unkE0->unk7A = (map_object->unk10 >> 17) & 1;
    }
    map_object = MapObject_GetById(entity->group_id, entity->spawn_info->unk43);
    if (map_object != NULL) {
        unkE0->unk7B = (map_object->unk10 >> 17) & 1;
    }
    if (!unkE0->unk7A || !unkE0->unk7B) {
        ExpeditionSoldier_WalkToMapObjectPosX(entity, unknown);
        return;
    }
    EntityAi_CheckFarFromSpawn((char *)entity);
    EntityAi_StepState(entity);
    if (ENTITY_FLAG_BITS(entity)->bit53) {
        EntityAi_ThinkReturn(entity, unknown);
        return;
    }
    switch (entity->state) {
    case 0:
        break;
    case 1:
    case 2:
        func_001787E0(entity, unknown);
        return;
    }
    EntityAi_ThinkIdle(entity, unknown);
}

/* Helper of ExpeditionSoldier_ThinkUseMapObjectsPosX, which calls it in place of the state functions: attacks when
   EntityAi_CanAttack says so; else walks to a fixed point (x 950) and from there to a point next to the
   map object whose bit is clear, turns to face it and starts ExpeditionSoldier_SignalMapObject for it; until
   then it starts the move action. */
static void ExpeditionSoldier_WalkToMapObjectPosX(Entity *entity, s32 unknown) {
    EntityUnkE0Kind178358 *unkE0 = (EntityUnkE0Kind178358 *)&entity->unkE0;
    f32 stand_point[4];
    f32 face_point[4];
    f32 reach;

    // match: the shared prototype takes no argument; retail passes the entity
    if (((int (*)(Entity *))EntityAi_CanAttack)(entity)) {
        if (EntityAi_PickAttack((char *)entity)) {
            EntityAi_StartAction(entity, EntityAi_RangedAttack, 1);
        } else {
            EntityAi_StartAction(entity, EntityAi_MeleeAttack, 1);
        }
        return;
    }
    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    reach = 100.0f;
    if (1200.0f < entity->model->position[0] || entity->model->position[0] < 800.0f) {
        unkE0->unk30[0] = 950.0f;
        unkE0->unk30[1] = entity->spawn_info->spawn_y;
        unkE0->unk30[2] = -1400.0f;
        unkE0->unk30[3] = 0.0f;
        entity->target = unkE0->unk30;
        entity->unkC8 = EntityAi_MoveCommand(entity, 0, 50.0f);
    } else {
        if (unkE0->unk7A == 0) {
            stand_point[0] = 900.0f;
            stand_point[1] = entity->spawn_info->spawn_y;
            stand_point[2] = -1800.0f;
            stand_point[3] = 0.0f;
            face_point[0] = 800.0f;
            face_point[1] = entity->spawn_info->spawn_y;
            face_point[2] = -1800.0f;
            face_point[3] = 0.0f;
            unkE0->unk7C = entity->spawn_info->unk42;
        } else if (unkE0->unk7B == 0) {
            stand_point[0] = 900.0f;
            stand_point[1] = entity->spawn_info->spawn_y;
            stand_point[2] = -1000.0f;
            stand_point[3] = 0.0f;
            face_point[0] = 800.0f;
            face_point[1] = entity->spawn_info->spawn_y;
            face_point[2] = -1000.0f;
            face_point[3] = 0.0f;
            unkE0->unk7C = entity->spawn_info->unk43;
        }
        if (EntityMath_DistanceXZ(entity->model->position, stand_point) < reach) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
            Vector_Copy(unkE0->unk30, face_point);
            entity->target = unkE0->unk30;
            entity->unkC8 = EntityAi_TurnCommand(entity);
            if (entity->unkC8 == 0) {
                EntityAi_StartAction(entity, ExpeditionSoldier_SignalMapObject, unknown);
                return;
            }
        } else {
            Vector_Copy(unkE0->unk30, stand_point);
            entity->target = unkE0->unk30;
            entity->unkC8 = EntityAi_MoveCommand(entity, 0, reach * 0.5f);
        }
    }
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}

/* The think handler the set-up function installs for EntitySpawnInfo.variant 4: as
   ExpeditionSoldier_ThinkUseMapObjectsPosX, with ExpeditionSoldier_WalkToMapObjectNegX (the points at x -950
   and -900). */
static void ExpeditionSoldier_ThinkUseMapObjectsNegX(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    MapObject *map_object;

    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (entity->unkC6 != 0) {
        return;
    }
    map_object = MapObject_GetById(entity->group_id, entity->spawn_info->unk42);
    if (map_object != NULL) {
        unkE0->unk7A = (map_object->unk10 >> 17) & 1;
    }
    map_object = MapObject_GetById(entity->group_id, entity->spawn_info->unk43);
    if (map_object != NULL) {
        unkE0->unk7B = (map_object->unk10 >> 17) & 1;
    }
    if (!unkE0->unk7A || !unkE0->unk7B) {
        ExpeditionSoldier_WalkToMapObjectNegX(entity, unknown);
        return;
    }
    EntityAi_CheckFarFromSpawn((char *)entity);
    EntityAi_StepState(entity);
    if (ENTITY_FLAG_BITS(entity)->bit53) {
        EntityAi_ThinkReturn(entity, unknown);
        return;
    }
    switch (entity->state) {
    case 0:
        break;
    case 1:
    case 2:
        func_001787E0(entity, unknown);
        return;
    }
    EntityAi_ThinkIdle(entity, unknown);
}

/* Helper of ExpeditionSoldier_ThinkUseMapObjectsNegX: as ExpeditionSoldier_WalkToMapObjectPosX with its own points (x -950). */
static void ExpeditionSoldier_WalkToMapObjectNegX(Entity *entity, s32 unknown) {
    EntityUnkE0Kind178358 *unkE0 = (EntityUnkE0Kind178358 *)&entity->unkE0;
    f32 stand_point[4];
    f32 face_point[4];
    f32 reach;

    // match: the shared prototype takes no argument; retail passes the entity
    if (((int (*)(Entity *))EntityAi_CanAttack)(entity)) {
        if (EntityAi_PickAttack((char *)entity)) {
            EntityAi_StartAction(entity, EntityAi_RangedAttack, 1);
        } else {
            EntityAi_StartAction(entity, EntityAi_MeleeAttack, 1);
        }
        return;
    }
    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    reach = 100.0f;
    if (-800.0f < entity->model->position[0] || entity->model->position[0] < -1200.0f) {
        unkE0->unk30[0] = -950.0f;
        unkE0->unk30[1] = entity->spawn_info->spawn_y;
        unkE0->unk30[2] = -1400.0f;
        unkE0->unk30[3] = 0.0f;
        entity->target = unkE0->unk30;
        entity->unkC8 = EntityAi_MoveCommand(entity, 0, 50.0f);
    } else {
        if (unkE0->unk7A == 0) {
            stand_point[0] = -900.0f;
            stand_point[1] = entity->spawn_info->spawn_y;
            stand_point[2] = -1000.0f;
            stand_point[3] = 0.0f;
            face_point[0] = -800.0f;
            face_point[1] = entity->spawn_info->spawn_y;
            face_point[2] = -1000.0f;
            face_point[3] = 0.0f;
            unkE0->unk7C = entity->spawn_info->unk42;
        } else if (unkE0->unk7B == 0) {
            stand_point[0] = -900.0f;
            stand_point[1] = entity->spawn_info->spawn_y;
            stand_point[2] = -1800.0f;
            stand_point[3] = 0.0f;
            face_point[0] = -800.0f;
            face_point[1] = entity->spawn_info->spawn_y;
            face_point[2] = -1800.0f;
            face_point[3] = 0.0f;
            unkE0->unk7C = entity->spawn_info->unk43;
        }
        if (EntityMath_DistanceXZ(entity->model->position, stand_point) < reach) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
            Vector_Copy(unkE0->unk30, face_point);
            entity->target = unkE0->unk30;
            entity->unkC8 = EntityAi_TurnCommand(entity);
            if (entity->unkC8 == 0) {
                EntityAi_StartAction(entity, ExpeditionSoldier_SignalMapObject, unknown);
                return;
            }
        } else {
            Vector_Copy(unkE0->unk30, stand_point);
            entity->target = unkE0->unk30;
            entity->unkC8 = EntityAi_MoveCommand(entity, 0, reach * 0.5f);
        }
    }
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}
