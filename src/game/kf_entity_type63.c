/* The behaviour of entity type 63 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers that function installs. */
#include "kf_entity.h"
#include "kf_map.h"
#include "kf_map_object.h"
#include "common.h"

void func_001958A8(char *, s32);
int func_001958B8(Entity *, void (*)(Entity *), int);
void func_00191478(EntityModel *);
int func_00193378(EntityModel *, s32, s32);
void func_00216778(void *, void *);
// match: the definition takes an Actor, a struct local to func_00182A08.c
void func_00182A08(void);
void func_00183570(Entity *);
void func_00181038(Entity *);
void func_001818B8(Entity *);
extern s32 D_003E6E40[][2];
extern u32 D_00446ED4;
extern PlayerData D_00413F80;
extern MapSlot D_0038CD20[];
void func_00196870(void *, int);
void Vector_Subtract(void *, void *, void *);
void func_00216D20(void *, void *);
f32 func_00194F48(f32 *, f32 *);
s32 func_0018A970(Entity *, f32 *, PlayerData *);
float func_00194F80(void *, void *);
void func_00196CC0(char *);
void func_0018E9C0(void *);
int func_00182DE0(void *, int);
int func_00182FD8(void *, int);
void func_00183150(Entity *, s32);
int rand(void);
s32 func_0018ED98(Entity *, f32, f32);
s32 func_0018EEE0(Entity *, f32, f32);
s32 func_0018FD50(Entity *, s32);
void func_00190B88(Entity *, s32);
int func_0018F030(char *);
void func_00196D48(Entity *);
float func_001957B8(float, float);
int func_00190BE0(Entity *);
int func_0018EAC0(void *, void *);
s32 func_00190F90(Entity *, s32, f32);
void func_00181AE0(Entity *);
void func_00181C60(Entity *);
void func_00181E48(Entity *);
void func_00182240(Entity *);
int func_00192C98(void *, int, int, u32, int);
float func_00194FB8(float);
void func_0019A290(Entity *, s32);
extern int D_00446770;
void func_00190A30(char *);
void func_00195898(char *, u8);
int func_0019A310(char *, void (*)(void), int);
void func_001816A0(Entity *);
void func_001B4EC8(s32);
s32 func_00199EB8(Entity *, f32 *, f32);
s32 func_00195AC0(Entity *, void *, s32, s32, s32);
s32 func_00195BD8(Entity *, void *, s32, f32);

static void func_00155FE8(Entity *entity, s32 unknown);
static void func_001566A0(Entity *entity, s32 unknown);
static void func_001569E8(Entity *entity);
static void func_00156E88(Entity *entity);
static void func_00157428(Entity *entity);
static void func_001574C0(Entity *entity);
static s32 func_00157898(Entity *entity, EntityParamsUnk240 *unk240, s32 part, s32 frame_from, s32 frame_to, f32 angle);

/* Set-up of a type 63 entity: clears its work area and installs the type's handlers. */
void EntityType63_Init(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    char *unk138 = entity->model->unk138;
    s32 i;

    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        ENTITY_WORK_BYTES(entity)[i] = 0;
    }
    func_001958A8((char *)entity, D_003E6E40[entity->params->unk02][1]);
    unkE0->unk00 = func_00155FE8;
    unkE0->unk04 = func_00182A08;
    unkE0->unk08 = (void (*)(void))func_00157428;
    unkE0->unk18 = (void (*)(void))func_00183570;
    unkE0->unk10 = (void (*)(void))func_00181038;
    unkE0->unk14 = (void (*)(void))func_001818B8;
    for (i = 3; i != -1; i--) {
        entity->unkBC[i] = 0x80;
    }
    for (i = 5; i != -1; i--) {
        entity->unkC0[i] = 0x80;
    }
    ENTITY_FLAG_BITS(entity)->bit28 = 0;
    entity->target = NULL;
    if (ENTITY_SPAWN_FLAGS(entity) & (0x8000L << 43)) {
        func_001958B8(entity, (void (*)(Entity *))unkE0->unk10, 1);
    } else {
        func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, 0);
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

static void func_00155FE8(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 position[4];
    f32 target[4];
    f32 delta[4];
    Entity *first;
    Entity *second;
    Entity *third;
    MapRecord *records;

    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        func_00196870(entity, 1);
        func_00216778(position, entity->model->position);
        func_00216778(target, &D_00413F80);
        position[1] = entity->model->position[1] - entity->unk34 * 0.5f;
        target[1] = D_00413F80.position[1] - 115.0f;
        Vector_Subtract(delta, target, position);
        func_00216D20(delta, delta);
        func_00194F48(position, target);
        first = (Entity *)MapObject_GetById(entity->group_id, 0xA0);
        second = (Entity *)MapObject_GetById(entity->group_id, 0xA1);
        third = (Entity *)MapObject_GetById(entity->group_id, 0xA2);
        if (unkE0->unk7A == 0) {
            records = (MapRecord *)D_0038CD20[entity->group_id].records;
            if (entity->unk14 == &records[0x11]) {
                if (func_0018A970(first, entity->model->position, &D_00413F80) == 1 && !(unkE0->unk7B & 1)
                    && (func_00194F80(&D_00413F80, first->model->position) < 500.0f
                        || func_00194F80(entity->model->position, first->model->position) < entity->radius + 400.0f)
                    && D_00413F80.unk252 == 0x14) {
                    unkE0->unk7A = 2;
                    unkE0->unk7B |= 1;
                    func_001958B8(entity, func_00156E88, unknown);
                    return;
                }
                if (func_0018A970(second, entity->model->position, &D_00413F80) == 1 && !(unkE0->unk7B & 2)
                    && (func_00194F80(&D_00413F80, second->model->position) < 500.0f
                        || func_00194F80(entity->model->position, second->model->position) < entity->radius + 400.0f)
                    && D_00413F80.unk252 == 0x16) {
                    unkE0->unk7A = 3;
                    unkE0->unk7B |= 2;
                    func_001958B8(entity, func_00156E88, unknown);
                    return;
                }
            } else if (entity->unk14 == &records[0x14]) {
                if (func_0018A970(first, entity->model->position, &D_00413F80) == 1 && !(unkE0->unk7B & 1)
                    && (func_00194F80(&D_00413F80, first->model->position) < 500.0f
                        || func_00194F80(entity->model->position, first->model->position) < entity->radius + 400.0f)
                    && D_00413F80.unk252 == 0x11) {
                    unkE0->unk7A = 2;
                    unkE0->unk7B |= 1;
                    func_001958B8(entity, func_00156E88, unknown);
                    return;
                }
                if (func_0018A970(third, entity->model->position, &D_00413F80) == 1 && !(unkE0->unk7B & 4)
                    && (func_00194F80(&D_00413F80, third->model->position) < 500.0f
                        || func_00194F80(entity->model->position, third->model->position) < entity->radius + 400.0f)
                    && D_00413F80.unk252 == 0x16) {
                    unkE0->unk7A = 1;
                    unkE0->unk7B |= 4;
                    func_001958B8(entity, func_00156E88, unknown);
                    return;
                }
            } else if (entity->unk14 == &records[0x16]) {
                if (func_0018A970(second, entity->model->position, &D_00413F80) == 1 && !(unkE0->unk7B & 2)
                    && (func_00194F80(&D_00413F80, second->model->position) < 500.0f
                        || func_00194F80(entity->model->position, second->model->position) < entity->radius + 400.0f)
                    && D_00413F80.unk252 == 0x11) {
                    unkE0->unk7A = 3;
                    unkE0->unk7B |= 2;
                    func_001958B8(entity, func_00156E88, unknown);
                    return;
                }
                if (func_0018A970(third, entity->model->position, &D_00413F80) == 1 && !(unkE0->unk7B & 4)
                    && (func_00194F80(&D_00413F80, third->model->position) < 500.0f
                        || func_00194F80(entity->model->position, third->model->position) < entity->radius + 400.0f)
                    && D_00413F80.unk252 == 0x14) {
                    unkE0->unk7A = 1;
                    unkE0->unk7B |= 4;
                    func_001958B8(entity, func_00156E88, unknown);
                    return;
                }
            }
        }
    }
    if (entity->unkC6 == 0) {
        func_00196CC0((char *)entity);
        func_0018E9C0(entity);
        if (ENTITY_FLAG_BITS(entity)->bit53) {
            func_00182DE0(entity, unknown);
        } else {
            switch (entity->state) {
            case 0:
                func_00182FD8(entity, unknown);
                break;
            case 1:
                func_00183150(entity, unknown);
                break;
            case 2:
                func_001566A0(entity, unknown);
                break;
            default:
                func_00182FD8(entity, unknown);
                break;
            }
        }
    }
}

static void func_001566A0(Entity *entity, s32 unknown) {
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
    if (func_0018ED98(entity, 300.0f, 0.5235988f)) {
        func_001958B8(entity, func_00181C60, 1);
        return;
    }
    if (func_0018EEE0(entity, 300.0f, 0.5235988f)) {
        func_001958B8(entity, func_00181AE0, 1);
        return;
    }
    side = func_0018FD50(entity, 0);
    ENTITY_FLAG_BITS(entity)->bit9 = 1;
    if (entity->unkA0 == 0 || side == 0) {
        entity->unkCF = entity->unkCD;
        entity->unkCD = side;
        if (entity->unkCD != entity->unkCF) {
            entity->unk9E = D_00446ED4;
        }
        func_00190B88(entity, side);
        switch (entity->unkCD) {
        case 0:
            if (func_0018F030((char *)entity)) {
                func_001958B8(entity, func_00182240, unknown);
            } else if (ENTITY_FLAG_BITS(entity)->bits58 == 0) {
                func_001958B8(entity, func_001569E8, unknown);
            } else if (ENTITY_FLAG_BITS(entity)->bits58 == 1) {
                func_001958B8(entity, func_001574C0, unknown);
            } else {
                func_001958B8(entity, func_00181E48, unknown);
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
            angle = func_001957B8(entity->model->unkD0[1], entity->unk68);
            if (unkE0->unk79 == 0) {
                unkE0->unk79 = ABS(angle) > 0.5235988f;
                if (unkE0->unk79 == 0) {
                    goto done;
                }
            }
            entity->unkC8 = func_00190BE0(entity);
            if (entity->unkC8 == 0) {
                unkE0->unk79 = 0;
            }
        } else {
            turn = func_0018EAC0(entity, direction);
            if (turn > 0) {
                entity->unkC8 = 0x201;
            } else if (turn < 0) {
                entity->unkC8 = 0x101;
            } else {
                entity->unkC8 = func_00190F90(entity, 0, entity->radius + 100.0f + 30.0f);
            }
        }
    }
done:
    func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, unknown);
}

static void func_001569E8(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityParamsUnk240 *unk240 = &entity->params->unk240[0];
    s32 frame;

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 0x14, 0, entity->params->unkC4 >> 1, 1);
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
            ENTITY_FLAG_BITS(entity)->bit54 = func_00157898(entity, unk240, 0, frame, entity->model->unkFC->unk82, func_00194FB8(entity->model->unkD0[1] + 3.1415927f));
            if (!ENTITY_FLAG_BITS(entity)->bit54) {
                ENTITY_FLAG_BITS(entity)->bit54 = func_00157898(entity, unk240, 1, frame, entity->model->unkFC->unk82, entity->model->unkD0[1]);
            }
        }
        if (!ENTITY_FLAG_BITS(entity)->bit55) {
            ENTITY_FLAG_BITS(entity)->bit55 = func_00157898(entity, unk240, 2, frame, entity->model->unkFC->unk82, func_00194FB8(entity->model->unkD0[1] + 3.1415927f));
            if (!ENTITY_FLAG_BITS(entity)->bit55) {
                ENTITY_FLAG_BITS(entity)->bit55 = func_00157898(entity, unk240, 3, frame, entity->model->unkFC->unk82, entity->model->unkD0[1]);
            }
        }
        if (!ENTITY_FLAG_BITS(entity)->bit56) {
            ENTITY_FLAG_BITS(entity)->bit56 = func_00157898(entity, unk240, 4, frame, entity->model->unkFC->unk82, func_00194FB8(entity->model->unkD0[1] + 3.1415927f));
            if (!ENTITY_FLAG_BITS(entity)->bit56) {
                ENTITY_FLAG_BITS(entity)->bit56 = func_00157898(entity, unk240, 5, frame, entity->model->unkFC->unk82, entity->model->unkD0[1]);
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
            func_0019A290(entity, unk240->unk02);
            if (ENTITY_FLAG_BITS(entity)->bit5) {
                unkE0->unk00(entity, 1);
            } else {
                func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, 1);
            }
        }
        break;
    }
}

/* walks through one of three doorways (map objects 0xA0-0xA2): unk7A says which. It is put just
   inside the doorway facing through it, opens the door on an animation frame and walks to a point
   on the far side. */
static void func_00156E88(Entity *entity) {
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
            entity->model->unkD0[1] = func_00194FB8(entity->model->position[2] > limit ? 0.0f : 3.1415927f);
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
            entity->model->unkD0[1] = func_00194FB8(entity->model->position[2] > limit ? 0.0f : 3.1415927f);
            coord = (entity->model->position[2] > limit ? -(entity->radius + 100.0f) : entity->radius + 100.0f) + limit;
            unkE0->unk30[0] = 3400.0f;
            unkE0->unk30[1] = 640.0f;
            unkE0->unk30[2] = coord;
            unkE0->unk30[3] = 0.0f;
            break;
        }
        func_00192C98(entity, 0x11, 0, 0, 1);
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
            func_00192C98(entity, 3, 0, entity->params->unkC4, 1);
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
        if (func_00194F80(entity->model->position, unkE0->unk30) < coord) {
            ENTITY_FLAG_BITS(entity)->bit23 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            unkE0->unk7A = 0;
            if (ENTITY_FLAG_BITS(entity)->bit5) {
                unkE0->unk00(entity, 1);
            } else {
                func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, 1);
            }
        }
        break;
    }
}

static void func_00157428(Entity *entity) {
    if (ENTITY_FLAG_BITS(entity)->bits36 == 2) {
        func_00190A30((char *)entity);
        entity->target = NULL;
        ENTITY_FLAG_BITS(entity)->bit10 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
        func_0019A310((char *)entity, (void (*)(void))func_001816A0, 0);
    }
    ENTITY_FLAG_BITS(entity)->bit23 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
    func_00195898((char *)entity, 2);
}

static void func_001574C0(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityParamsUnk240 *unk240 = &entity->params->unk240[1];
    s32 frame;

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 0x15, 0, entity->params->unkC4 >> 1, 1);
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
            func_00199EB8(entity, &unkE0->unk1C, entity->unk68);
        }
        break;
    case 2:
        if (unk240->unk01 >= entity->model->unkFC->unk82) {
            func_00199EB8(entity, &entity->unkE0.unk1C, entity->unk68);
        } else {
            unkE0->unk1C = 0.0f;
        }
        frame = (entity->model->unkFC->unk84 - entity->params->unk80[entity->unkD1]) >> 8;
        if (!ENTITY_FLAG_BITS(entity)->bit54) {
            ENTITY_FLAG_BITS(entity)->bit54 = func_00195AC0(entity, unk240, 0, frame, entity->model->unkFC->unk82);
        }
        if (!ENTITY_FLAG_BITS(entity)->bit55) {
            ENTITY_FLAG_BITS(entity)->bit55 = func_00195AC0(entity, unk240, 1, frame, entity->model->unkFC->unk82);
        }
        if (!ENTITY_FLAG_BITS(entity)->bit56) {
            ENTITY_FLAG_BITS(entity)->bit56 = func_00195AC0(entity, unk240, 2, frame, entity->model->unkFC->unk82);
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
            func_0019A290(entity, unk240->unk02);
            if (ENTITY_FLAG_BITS(entity)->bit5) {
                unkE0->unk00(entity, 1);
            } else {
                func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, 1);
            }
        }
        break;
    }
}

/* Runs func_00195BD8 for one part of an attack record, unless the animation frames [frame_from, frame_to]
   lie outside the frames the part is active on. Parts 3 to 5 take their frames from the four frame marks
   at params+0x434 instead and run as part 1, 0 and 1. */
static s32 func_00157898(Entity *entity, EntityParamsUnk240 *unk240, s32 part, s32 frame_from, s32 frame_to, f32 angle) {
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
    return func_00195BD8(entity, unk240, part, angle);
}
