/* GiantLeader: the behaviour of entity type 63 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers it installs. Used by: Giant Leader. The game names no
   monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by
   hit points and experience, with resistances and drops where two monsters share those. */
#include "kf_entity.h"
#include "kf_map.h"
#include "kf_map_object.h"
#include "common.h"

void EntityAi_SetUpdate(char *, s32);
int EntityAi_StartAction(Entity *, void (*)(Entity *), int);
void Motion_ClearPartSlots(EntityModel *);
int Motion_StepModel(EntityModel *, s32, s32);
void Vector_Copy(void *, void *);
// match: the definition takes an Actor, a struct local to EntityAi_Animate.c
void EntityAi_Animate(void);
void EntityAi_Move(Entity *);
void EntityAi_Dormant(Entity *);
void EntityAi_Death(Entity *);
extern s32 D_003E6E40[][2];
extern u32 D_00446ED4;
extern PlayerData D_00413F80;
extern MapSlot D_0038CD20[];
void EntityAi_SensePlayer(void *, int);
void Vector_Subtract(void *, void *, void *);
void Vector_Normalize(void *, void *);
f32 EntityMath_Distance(f32 *, f32 *);
s32 EntityCollision_LineHits(Entity *, f32 *, PlayerData *);
float EntityMath_DistanceXZ(void *, void *);
void EntityAi_CheckFarFromSpawn(char *);
void EntityAi_StepState(void *);
int EntityAi_ThinkReturn(void *, int);
int EntityAi_ThinkIdle(void *, int);
void EntityAi_ThinkPursue(Entity *, s32);
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
int Motion_Start(void *, int, int, u32, int);
float EntityMath_WrapAngle(float);
void EntityAi_SetAttackDelay(Entity *, s32);
extern int D_00446770;
void EntityAi_LowerWeightsAfterHit(char *);
void EntityAi_SetState(char *, u8);
int EntityAi_SetReaction(char *, void (*)(void), int);
void EntityAi_ReactionHeavy(Entity *);
void func_001B4EC8(s32);
s32 EntityAi_TurnToHeading(Entity *, f32 *, f32);
s32 EntityAi_CheckMeleeHit(Entity *, void *, s32, s32, s32);
s32 EntityAi_SendMeleeHit(Entity *, void *, s32, f32);

static void GiantLeader_Think(Entity *entity, s32 unknown);
static void GiantLeader_ThinkCombat(Entity *entity, s32 unknown);
static void GiantLeader_MeleeAttack0(Entity *entity);
static void GiantLeader_BreakMapObject(Entity *entity);
static void GiantLeader_OnHit(Entity *entity);
static void GiantLeader_MeleeAttack1(Entity *entity);
static s32 GiantLeader_CheckPartHit(Entity *entity, EntityParamsUnk240 *unk240, s32 part, s32 frame_from, s32 frame_to, f32 angle);

/* Set-up of a type 63 entity: clears its work area and installs the type's handlers. */
void GiantLeader_Init(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    char *unk138 = entity->model->unk138;
    s32 i;

    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        ENTITY_WORK_BYTES(entity)[i] = 0;
    }
    EntityAi_SetUpdate((char *)entity, D_003E6E40[entity->params->unk02][1]);
    unkE0->think = GiantLeader_Think;
    unkE0->animate = EntityAi_Animate;
    unkE0->on_hit = (void (*)(void))GiantLeader_OnHit;
    unkE0->move = (void (*)(void))EntityAi_Move;
    unkE0->dormant = (void (*)(void))EntityAi_Dormant;
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

/* The think handler: as EntityAi_Think, with GiantLeader_ThinkCombat for state 2. Every tenth
   frame, while work byte unk7A is 0, it also looks whether one of the map objects 0xA0 to 0xA2 lies
   between the entity and the player (EntityCollision_LineHits), with the entity and the player on two given
   map records and one of them near the object; if so it puts the number of the object in unk7A,
   marks it in work byte unk7B (each object is used once) and starts GiantLeader_BreakMapObject. */
static void GiantLeader_Think(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 position[4];
    f32 target[4];
    f32 delta[4];
    Entity *first;
    Entity *second;
    Entity *third;
    MapRecord *records;

    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        EntityAi_SensePlayer(entity, 1);
        Vector_Copy(position, entity->model->position);
        Vector_Copy(target, &D_00413F80);
        position[1] = entity->model->position[1] - entity->unk34 * 0.5f;
        target[1] = D_00413F80.position[1] - 115.0f;
        Vector_Subtract(delta, target, position);
        Vector_Normalize(delta, delta);
        EntityMath_Distance(position, target);
        first = (Entity *)MapObject_GetById(entity->group_id, 0xA0);
        second = (Entity *)MapObject_GetById(entity->group_id, 0xA1);
        third = (Entity *)MapObject_GetById(entity->group_id, 0xA2);
        if (unkE0->unk7A == 0) {
            records = (MapRecord *)D_0038CD20[entity->group_id].records;
            if (entity->unk14 == &records[0x11]) {
                if (EntityCollision_LineHits(first, entity->model->position, &D_00413F80) == 1 && !(unkE0->unk7B & 1)
                    && (EntityMath_DistanceXZ(&D_00413F80, first->model->position) < 500.0f
                        || EntityMath_DistanceXZ(entity->model->position, first->model->position) < entity->radius + 400.0f)
                    && D_00413F80.unk252 == 0x14) {
                    unkE0->unk7A = 2;
                    unkE0->unk7B |= 1;
                    EntityAi_StartAction(entity, GiantLeader_BreakMapObject, unknown);
                    return;
                }
                if (EntityCollision_LineHits(second, entity->model->position, &D_00413F80) == 1 && !(unkE0->unk7B & 2)
                    && (EntityMath_DistanceXZ(&D_00413F80, second->model->position) < 500.0f
                        || EntityMath_DistanceXZ(entity->model->position, second->model->position) < entity->radius + 400.0f)
                    && D_00413F80.unk252 == 0x16) {
                    unkE0->unk7A = 3;
                    unkE0->unk7B |= 2;
                    EntityAi_StartAction(entity, GiantLeader_BreakMapObject, unknown);
                    return;
                }
            } else if (entity->unk14 == &records[0x14]) {
                if (EntityCollision_LineHits(first, entity->model->position, &D_00413F80) == 1 && !(unkE0->unk7B & 1)
                    && (EntityMath_DistanceXZ(&D_00413F80, first->model->position) < 500.0f
                        || EntityMath_DistanceXZ(entity->model->position, first->model->position) < entity->radius + 400.0f)
                    && D_00413F80.unk252 == 0x11) {
                    unkE0->unk7A = 2;
                    unkE0->unk7B |= 1;
                    EntityAi_StartAction(entity, GiantLeader_BreakMapObject, unknown);
                    return;
                }
                if (EntityCollision_LineHits(third, entity->model->position, &D_00413F80) == 1 && !(unkE0->unk7B & 4)
                    && (EntityMath_DistanceXZ(&D_00413F80, third->model->position) < 500.0f
                        || EntityMath_DistanceXZ(entity->model->position, third->model->position) < entity->radius + 400.0f)
                    && D_00413F80.unk252 == 0x16) {
                    unkE0->unk7A = 1;
                    unkE0->unk7B |= 4;
                    EntityAi_StartAction(entity, GiantLeader_BreakMapObject, unknown);
                    return;
                }
            } else if (entity->unk14 == &records[0x16]) {
                if (EntityCollision_LineHits(second, entity->model->position, &D_00413F80) == 1 && !(unkE0->unk7B & 2)
                    && (EntityMath_DistanceXZ(&D_00413F80, second->model->position) < 500.0f
                        || EntityMath_DistanceXZ(entity->model->position, second->model->position) < entity->radius + 400.0f)
                    && D_00413F80.unk252 == 0x11) {
                    unkE0->unk7A = 3;
                    unkE0->unk7B |= 2;
                    EntityAi_StartAction(entity, GiantLeader_BreakMapObject, unknown);
                    return;
                }
                if (EntityCollision_LineHits(third, entity->model->position, &D_00413F80) == 1 && !(unkE0->unk7B & 4)
                    && (EntityMath_DistanceXZ(&D_00413F80, third->model->position) < 500.0f
                        || EntityMath_DistanceXZ(entity->model->position, third->model->position) < entity->radius + 400.0f)
                    && D_00413F80.unk252 == 0x14) {
                    unkE0->unk7A = 1;
                    unkE0->unk7B |= 4;
                    EntityAi_StartAction(entity, GiantLeader_BreakMapObject, unknown);
                    return;
                }
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
                GiantLeader_ThinkCombat(entity, unknown);
                break;
            default:
                EntityAi_ThinkIdle(entity, unknown);
                break;
            }
        }
    }
}

/* The think function of state 2: as EntityAi_ThinkCombat, but melee record 0 is
   GiantLeader_MeleeAttack0 and melee record 1 GiantLeader_MeleeAttack1; closing in never runs. */
static void GiantLeader_ThinkCombat(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    f32 angle;
    s32 side;
    s32 turn;

    if (entity->params->flags & 0x100) {
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
    } else {
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
    }
    if (EntityAi_ShouldGuard(entity, 300.0f, 0.5235988f)) {
        EntityAi_StartAction(entity, EntityAi_Guard, 1);
        return;
    }
    if (EntityAi_ShouldDodge(entity, 300.0f, 0.5235988f)) {
        EntityAi_StartAction(entity, EntityAi_Dodge, 1);
        return;
    }
    side = EntityAi_PickCombatChoice(entity, 0);
    ENTITY_FLAG_BITS(entity)->bit9 = 1;
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
            } else if (ENTITY_FLAG_BITS(entity)->bits58 == 0) {
                EntityAi_StartAction(entity, GiantLeader_MeleeAttack0, unknown);
            } else if (ENTITY_FLAG_BITS(entity)->bits58 == 1) {
                EntityAi_StartAction(entity, GiantLeader_MeleeAttack1, unknown);
            } else {
                EntityAi_StartAction(entity, EntityAi_MeleeAttack, unknown);
            }
            return;
        case 1:
            entity->target = &D_00413F80;
            entity->unkA0 = rand() % 60 + 120;
            unkE0->unk78 = 0;
            break;
        case 2:
            entity->unkA0 = rand() % 30 + 30;
            unkE0->unk78 = -1;
            break;
        default:
            unkE0->unk78 = -1;
            break;
        }
    }
    if (unkE0->unk78 == 0) {
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
            if (turn > 0) {
                entity->unkC8 = 0x201;
            } else if (turn < 0) {
                entity->unkC8 = 0x101;
            } else {
                entity->unkC8 = EntityAi_MoveCommand(entity, 0, entity->radius + 100.0f + 30.0f);
            }
        }
    }
done:
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}

/* The melee attack with record 0 (busy bit 1): animation 20, no turning.
   GiantLeader_CheckPartHit checks six parts: 0, 2 and 4 towards the back (the heading plus
   180 degrees), 1, 3 and 5 along the heading; each pair shares one hit flag. */
static void GiantLeader_MeleeAttack0(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityParamsUnk240 *unk240 = &entity->params->unk240[0];
    s32 frame;

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x14, 0, entity->params->unkC4 >> 1, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        ENTITY_FLAG_BITS(entity)->bit54 = 0;
        ENTITY_FLAG_BITS(entity)->bit55 = 0;
        ENTITY_FLAG_BITS(entity)->bit56 = 0;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        unkE0->unk1C = 0.0f;
        frame = (entity->model->unkFC->unk84 - entity->params->unk80[entity->unkD1]) >> 8;
        if (!ENTITY_FLAG_BITS(entity)->bit54) {
            ENTITY_FLAG_BITS(entity)->bit54 = GiantLeader_CheckPartHit(entity, unk240, 0, frame, entity->model->unkFC->unk82, EntityMath_WrapAngle(entity->model->unkD0[1] + 3.1415927f));
            if (!ENTITY_FLAG_BITS(entity)->bit54) {
                ENTITY_FLAG_BITS(entity)->bit54 = GiantLeader_CheckPartHit(entity, unk240, 1, frame, entity->model->unkFC->unk82, entity->model->unkD0[1]);
            }
        }
        if (!ENTITY_FLAG_BITS(entity)->bit55) {
            ENTITY_FLAG_BITS(entity)->bit55 = GiantLeader_CheckPartHit(entity, unk240, 2, frame, entity->model->unkFC->unk82, EntityMath_WrapAngle(entity->model->unkD0[1] + 3.1415927f));
            if (!ENTITY_FLAG_BITS(entity)->bit55) {
                ENTITY_FLAG_BITS(entity)->bit55 = GiantLeader_CheckPartHit(entity, unk240, 3, frame, entity->model->unkFC->unk82, entity->model->unkD0[1]);
            }
        }
        if (!ENTITY_FLAG_BITS(entity)->bit56) {
            ENTITY_FLAG_BITS(entity)->bit56 = GiantLeader_CheckPartHit(entity, unk240, 4, frame, entity->model->unkFC->unk82, EntityMath_WrapAngle(entity->model->unkD0[1] + 3.1415927f));
            if (!ENTITY_FLAG_BITS(entity)->bit56) {
                ENTITY_FLAG_BITS(entity)->bit56 = GiantLeader_CheckPartHit(entity, unk240, 5, frame, entity->model->unkFC->unk82, entity->model->unkD0[1]);
            }
        }
        if (entity->model->unkFC->unk82 > 0 && unk240->unk03 >= entity->model->unkFC->unk82) {
            ENTITY_FLAG_BITS(entity)->bit19 = 1;
        } else {
            ENTITY_FLAG_BITS(entity)->bit19 = 0;
        }
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            EntityAi_SetAttackDelay(entity, unk240->unk02);
            if (ENTITY_FLAG_BITS(entity)->bit5) {
                unkE0->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            }
        }
        break;
    }
}

/* An action (busy bit 0x1000): goes past one of three map objects (0xA0-0xA2); work byte unk7A
   says which. The entity is put at a fixed place next to the object, facing it, and plays
   animation 17, breaks the map object (MapObject_Break) on frame EntityParams.unk43C, then walks in
   animation 3 to a point on the far side and clears unk7A. GiantLeader_Think starts it. */
static void GiantLeader_BreakMapObject(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    void *map_object;
    f32 coord; /* also the reach tested in step 4 */
    f32 centre;
    f32 limit;
    f32 angle;

    entity->unkC6 |= 0x1000;
    switch (entity->action_step) {
    case 0:
        ENTITY_FLAG_BITS(entity)->bit23 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit28 = TRUE;
        switch (unkE0->unk7A) {
        case 1:
            centre = 4200.0f;
            coord = (entity->model->position[0] > centre ? entity->radius + 50.0f : -(entity->radius + 50.0f)) + centre;
            entity->model->position[0] = coord;
            entity->model->position[1] = 640.0f;
            entity->model->position[2] = -2400.0f;
            entity->model->position[3] = 0.0f;
            centre = 4200.0f;
            angle = -1.5707964f;
            if (entity->model->position[0] > centre) {
                angle = 1.5707964f;
            }
            entity->model->unkD0[1] = angle;
            coord = (entity->model->position[0] > centre ? -(entity->radius + 100.0f) : entity->radius + 100.0f) + centre;
            unkE0->unk30[0] = coord;
            unkE0->unk30[1] = 640.0f;
            unkE0->unk30[2] = -2400.0f;
            unkE0->unk30[3] = 0.0f;
            break;
        case 2:
            centre = -1650.0f;
            coord = (entity->model->position[2] > centre ? entity->radius + 50.0f : -(entity->radius + 50.0f)) + centre;
            limit = -1650.0f;
            entity->model->position[0] = 5200.0f;
            entity->model->position[1] = 640.0f;
            entity->model->position[2] = coord;
            entity->model->position[3] = 0.0f;
            entity->model->unkD0[1] = EntityMath_WrapAngle(entity->model->position[2] > limit ? 0.0f : 3.1415927f);
            coord = (entity->model->position[2] > limit ? -(entity->radius + 100.0f) : entity->radius + 100.0f) + limit;
            unkE0->unk30[0] = 5200.0f;
            unkE0->unk30[1] = 640.0f;
            unkE0->unk30[2] = coord;
            unkE0->unk30[3] = 0.0f;
            break;
        case 3:
            centre = -1650.0f;
            coord = (entity->model->position[2] > centre ? entity->radius + 50.0f : -(entity->radius + 50.0f)) + centre;
            limit = -1650.0f;
            entity->model->position[0] = 3400.0f;
            entity->model->position[1] = 640.0f;
            entity->model->position[2] = coord;
            entity->model->position[3] = 0.0f;
            entity->model->unkD0[1] = EntityMath_WrapAngle(entity->model->position[2] > limit ? 0.0f : 3.1415927f);
            coord = (entity->model->position[2] > limit ? -(entity->radius + 100.0f) : entity->radius + 100.0f) + limit;
            unkE0->unk30[0] = 3400.0f;
            unkE0->unk30[1] = 640.0f;
            unkE0->unk30[2] = coord;
            unkE0->unk30[3] = 0.0f;
            break;
        }
        Motion_Start(entity, 0x11, 0, 0, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (entity->model->unkFC->unk82 == entity->params->unk43C) {
            switch (unkE0->unk7A) {
            case 1:
                if (entity->group_id == D_00446770) {
                    map_object = MapObject_GetById(entity->group_id, 0xA2);
                    if (map_object != NULL) {
                        MapObject_Break(map_object);
                    }
                }
                break;
            case 2:
                if (entity->group_id == D_00446770) {
                    map_object = MapObject_GetById(entity->group_id, 0xA0);
                    if (map_object != NULL) {
                        MapObject_Break(map_object);
                    }
                }
                break;
            case 3:
                if (entity->group_id == D_00446770) {
                    map_object = MapObject_GetById(entity->group_id, 0xA1);
                    if (map_object != NULL) {
                        MapObject_Break(map_object);
                    }
                }
                break;
            }
        }
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            Motion_Start(entity, 3, 0, entity->params->unkC4, 1);
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
            entity->action_step++;
        }
        break;
    case 3:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
            ENTITY_FLAG_BITS(entity)->bits11 = 1;
        }
        break;
    case 4:
        coord = 100.0f;
        if (EntityMath_DistanceXZ(entity->model->position, unkE0->unk30) < coord) {
            ENTITY_FLAG_BITS(entity)->bit23 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            unkE0->unk7A = 0;
            if (ENTITY_FLAG_BITS(entity)->bit5) {
                unkE0->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            }
        }
        break;
    }
}

/* The on_hit handler: as EntityAi_OnHit, but only a heavy hit forgets the target and starts a
   reaction (EntityAi_ReactionHeavy); it also clears flag bits 23 and 28, which
   GiantLeader_BreakMapObject sets. */
static void GiantLeader_OnHit(Entity *entity) {
    if (ENTITY_FLAG_BITS(entity)->bits36 == 2) {
        EntityAi_LowerWeightsAfterHit((char *)entity);
        entity->target = NULL;
        ENTITY_FLAG_BITS(entity)->bit10 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
        EntityAi_SetReaction((char *)entity, (void (*)(void))EntityAi_ReactionHeavy, 0);
    }
    ENTITY_FLAG_BITS(entity)->bit23 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
    EntityAi_SetState((char *)entity, 2);
}

/* The melee attack with record 1 (busy bit 1): animation 21, as EntityAi_MeleeAttack; the first
   time part 0 has hit (flag bit 54) it calls func_001B4EC8 with EntityParams.unk43E. */
static void GiantLeader_MeleeAttack1(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityParamsUnk240 *unk240 = &entity->params->unk240[1];
    s32 frame;

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x15, 0, entity->params->unkC4 >> 1, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        unkE0->unk7C = 0;
        ENTITY_FLAG_BITS(entity)->bit54 = 0;
        ENTITY_FLAG_BITS(entity)->bit55 = 0;
        ENTITY_FLAG_BITS(entity)->bit56 = 0;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        if (unk240->unk01 != 0) {
            EntityAi_TurnToHeading(entity, &unkE0->unk1C, entity->unk68);
        }
        break;
    case 2:
        if (unk240->unk01 >= entity->model->unkFC->unk82) {
            EntityAi_TurnToHeading(entity, &entity->unkE0.unk1C, entity->unk68);
        } else {
            unkE0->unk1C = 0.0f;
        }
        frame = (entity->model->unkFC->unk84 - entity->params->unk80[entity->unkD1]) >> 8;
        if (!ENTITY_FLAG_BITS(entity)->bit54) {
            ENTITY_FLAG_BITS(entity)->bit54 = EntityAi_CheckMeleeHit(entity, unk240, 0, frame, entity->model->unkFC->unk82);
        }
        if (!ENTITY_FLAG_BITS(entity)->bit55) {
            ENTITY_FLAG_BITS(entity)->bit55 = EntityAi_CheckMeleeHit(entity, unk240, 1, frame, entity->model->unkFC->unk82);
        }
        if (!ENTITY_FLAG_BITS(entity)->bit56) {
            ENTITY_FLAG_BITS(entity)->bit56 = EntityAi_CheckMeleeHit(entity, unk240, 2, frame, entity->model->unkFC->unk82);
        }
        if (entity->model->unkFC->unk82 > 0 && unk240->unk03 >= entity->model->unkFC->unk82) {
            ENTITY_FLAG_BITS(entity)->bit19 = 1;
        } else {
            ENTITY_FLAG_BITS(entity)->bit19 = 0;
        }
        if (unkE0->unk7C == 0 && ENTITY_FLAG_BITS(entity)->bit54) {
            func_001B4EC8(entity->params->unk43E);
            unkE0->unk7C = 1;
        }
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            EntityAi_SetAttackDelay(entity, unk240->unk02);
            if (ENTITY_FLAG_BITS(entity)->bit5) {
                unkE0->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            }
        }
        break;
    }
}

/* Helper of GiantLeader_MeleeAttack0: runs the hit check EntityAi_SendMeleeHit for one part of an attack
   record, unless the animation frames [frame_from, frame_to] lie outside the frames the part is
   active on (as EntityAi_CheckMeleeHitAt). Parts 3 to 5 take their frames from the four frame marks at
   EntityParams.unk434 instead and run as part 1, 0 and 1. */
static s32 GiantLeader_CheckPartHit(Entity *entity, EntityParamsUnk240 *unk240, s32 part, s32 frame_from, s32 frame_to, f32 angle) {
    if (part < 3) {
        if (unk240->unk10[part].unk24 == 0 ||
            (frame_from < unk240->unk10[part].unk24 && frame_to < unk240->unk10[part].unk24) ||
            (unk240->unk10[part].unk26 < frame_from && unk240->unk10[part].unk26 < frame_to)) {
            return 0;
        }
    } else if (part == 3) {
        // match: this kind's unk434 is a signed frame number (lh); the shared header has it u16
        if ((s16)entity->params->unk434 == 0 ||
            (frame_from < (s16)entity->params->unk434 && frame_to < (s16)entity->params->unk434) || // match: as above
            (entity->params->unk436 - 1 < frame_from && entity->params->unk436 - 1 < frame_to)) {
            return 0;
        }
        part = 1;
    } else if (part == 4) {
        if (entity->params->unk436 == 0 ||
            (frame_from < entity->params->unk436 && frame_to < entity->params->unk436) ||
            (entity->params->unk438 - 1 < frame_from && entity->params->unk438 - 1 < frame_to)) {
            return 0;
        }
        part = 0;
    } else if (part == 5) {
        if (entity->params->unk438 == 0 ||
            (frame_from < entity->params->unk438 && frame_to < entity->params->unk438) ||
            (entity->params->unk43A - 1 < frame_from && entity->params->unk43A - 1 < frame_to)) {
            return 0;
        }
        part = 1;
    }
    return EntityAi_SendMeleeHit(entity, unk240, part, angle);
}
