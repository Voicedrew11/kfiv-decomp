/* The behaviour of entity type 7 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers that function installs. */
#include "kf_entity.h"
#include "kf_entity_world.h"
#include "kf_map_object.h"

#define ENTITY_WORK_AREA_SIZE 0xA0
void func_001958A8(char *, s32);
int func_001958B8(Entity *, void (*)(Entity *), int);
void func_00191478(EntityModel *);
int func_00193378(EntityModel *, s32, s32);
void func_00216778(void *, void *);
// match: the definition takes an Actor, a struct local to func_00182A08.c
void func_00182A08(void);
void func_00182BD0(char *);
void func_00183570(Entity *);
void func_00181038(Entity *);
extern s32 D_003E6E40[][2];
int func_00192C98(void *, int, int, u32, int);
extern u32 D_00446ED4;
void func_00196870(void *, int);
void func_00196CC0(char *);
void func_0018E9C0(void *);
int func_00182DE0(void *, int);
int func_00182FD8(void *, int);
void func_00183150(Entity *, s32);
extern float D_00413F80[];
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
void func_0018DCD8(Entity *);
void func_0018DC40(char *);
s32 func_00197078(Entity *);
float func_00194F80(void *, void *);

static void func_001785D8(Entity *entity);
static void func_00178698(Entity *entity, s32 unknown);
static void func_001787E0(Entity *entity, s32 unknown);
static void func_00178BC0(Entity *entity);
static void func_00178E10(Entity *entity);
static void func_00178F88(Entity *entity, s32 unknown);
static void func_00179118(Entity *entity, s32 unknown);
static void func_00179380(Entity *entity, s32 unknown);
static void func_00179510(Entity *entity, s32 unknown);

/* Set-up of a type 7 entity: clears its work area and installs the type's handlers. */
void EntityType7_Init(Entity *entity) {
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
    func_001958A8((char *)entity, D_003E6E40[entity->params->unk02][1]);
    switch (entity->spawn_info->variant) {
    case 1:
    case 2:
        unkE0->unk00 = (void (*)(Entity *, s32))func_001785D8;
        unkE0->unk04 = func_00182A08;
        break;
    case 3:
        unkE0->unk00 = func_00178F88;
        unkE0->unk04 = func_00182A08;
        unkE0->unk08 = (void (*)(void))func_00182BD0;
        unkE0->unk18 = (void (*)(void))func_00183570;
        unkE0->unk10 = (void (*)(void))func_00181038;
        unkE0->unk14 = (void (*)(void))func_00178BC0;
        break;
    case 4:
        unkE0->unk00 = func_00179380;
        unkE0->unk04 = func_00182A08;
        unkE0->unk08 = (void (*)(void))func_00182BD0;
        unkE0->unk18 = (void (*)(void))func_00183570;
        unkE0->unk10 = (void (*)(void))func_00181038;
        unkE0->unk14 = (void (*)(void))func_00178BC0;
        break;
    default:
        unkE0->unk00 = func_00178698;
        unkE0->unk04 = func_00182A08;
        unkE0->unk08 = (void (*)(void))func_00182BD0;
        unkE0->unk18 = (void (*)(void))func_00183570;
        unkE0->unk10 = (void (*)(void))func_00181038;
        unkE0->unk14 = (void (*)(void))func_00178BC0;
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

static void func_001785D8(Entity *entity) {
    if (entity->spawn_info->variant == 1) {
        func_00192C98(entity, 0x12, 0, 0, 0);
    } else {
        func_00192C98(entity, 0x13, 0, 0, 0);
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

static void func_00178698(Entity *entity, s32 unknown) {
    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        func_00196870(entity, 1);
    }
    if (entity->unkC6 != 0) {
        return;
    }
    func_00196CC0((char *)entity);
    func_0018E9C0(entity);
    if (ENTITY_FLAG_BITS(entity)->bit53) {
        func_00182DE0(entity, unknown);
        return;
    }
    switch (entity->state) {
    case 0:
        func_00182FD8(entity, unknown);
        return;
    case 1:
        func_00183150(entity, unknown);
        return;
    case 2:
        func_001787E0(entity, unknown);
        return;
    }
    func_00182FD8(entity, unknown);
}

static void func_001787E0(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    f32 angle;
    s32 side;
    s32 turn;

    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    if (func_0018ED98(entity, 300.0f, 0.5235988f)) {
        func_001958B8(entity, func_00181C60, 1);
        return;
    }
    if (func_0018EEE0(entity, 300.0f, 0.5235988f)) {
        func_001958B8(entity, func_00181AE0, 1);
        return;
    }
    side = func_0018FD50(entity, 0);
    ENTITY_FLAG_BITS(entity)->bit9 = TRUE;
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
            } else {
                func_001958B8(entity, func_00181E48, unknown);
            }
            return;
        case 1:
            if ((s16)entity->params->unk434 < entity->distance_to_player) {
                angle = func_001957B8(entity->unk68, entity->model->unkD0[1]);
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
            switch (unkE0->unk78) {
            case 0:
                if (turn > 0) {
                    entity->unkC8 = 0x201;
                } else if (turn < 0) {
                    entity->unkC8 = 0x101;
                } else {
                    entity->unkC8 = func_00190BE0(entity);
                }
                break;
            case 1:
                if (turn > 0) {
                    entity->unkC8 = 0x201;
                } else if (turn < 0) {
                    entity->unkC8 = 0x101;
                } else {
                    entity->unkC8 = func_00190F90(entity, 0, entity->radius + 100.0f + 30.0f);
                }
                break;
            }
        }
    }
done:
    func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, unknown);
}

static void func_00178BC0(Entity *entity) {
    entity->unkC6 = 0x40;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 0x10, 0, 5, 1);
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
            func_0018DCD8(entity);
            if (entity->spawn_info->unk40 != 0) {
                Entity *partner = Entity_GetBySpawnId(entity->spawn_info->unk40);

                if (partner != NULL && ((s32)partner->flags & 1)) {
                    func_0018DC40((char *)partner);
                    func_00216778(partner->model->position, entity->model->position);
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

static void func_00178E10(Entity *entity) {
    EntityUnkE0Kind178358 *unkE0 = (EntityUnkE0Kind178358 *)&entity->unkE0;
    MapObject *map_object;

    entity->unkC6 |= 0x1000;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 0x11, 0, entity->params->unkC4, 0);
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
                unkE0->unk00(entity, 1);
            } else {
                func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, 1);
            }
        }
        break;
    }
}

static void func_00178F88(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    MapObject *map_object;

    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        func_00196870(entity, 1);
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
        func_00179118(entity, unknown);
        return;
    }
    func_00196CC0((char *)entity);
    func_0018E9C0(entity);
    if (ENTITY_FLAG_BITS(entity)->bit53) {
        func_00182DE0(entity, unknown);
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
    func_00182FD8(entity, unknown);
}

static void func_00179118(Entity *entity, s32 unknown) {
    EntityUnkE0Kind178358 *unkE0 = (EntityUnkE0Kind178358 *)&entity->unkE0;
    f32 stand_point[4];
    f32 face_point[4];
    f32 reach;

    // match: the shared prototype takes no argument; retail passes the entity
    if (((int (*)(Entity *))func_00197078)(entity)) {
        if (func_0018F030((char *)entity)) {
            func_001958B8(entity, func_00182240, 1);
        } else {
            func_001958B8(entity, func_00181E48, 1);
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
        entity->unkC8 = func_00190F90(entity, 0, 50.0f);
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
        if (func_00194F80(entity->model->position, stand_point) < reach) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
            func_00216778(unkE0->unk30, face_point);
            entity->target = unkE0->unk30;
            entity->unkC8 = func_00190BE0(entity);
            if (entity->unkC8 == 0) {
                func_001958B8(entity, func_00178E10, unknown);
                return;
            }
        } else {
            func_00216778(unkE0->unk30, stand_point);
            entity->target = unkE0->unk30;
            entity->unkC8 = func_00190F90(entity, 0, reach * 0.5f);
        }
    }
    func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, unknown);
}

static void func_00179380(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    MapObject *map_object;

    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        func_00196870(entity, 1);
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
        func_00179510(entity, unknown);
        return;
    }
    func_00196CC0((char *)entity);
    func_0018E9C0(entity);
    if (ENTITY_FLAG_BITS(entity)->bit53) {
        func_00182DE0(entity, unknown);
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
    func_00182FD8(entity, unknown);
}

static void func_00179510(Entity *entity, s32 unknown) {
    EntityUnkE0Kind178358 *unkE0 = (EntityUnkE0Kind178358 *)&entity->unkE0;
    f32 stand_point[4];
    f32 face_point[4];
    f32 reach;

    // match: the shared prototype takes no argument; retail passes the entity
    if (((int (*)(Entity *))func_00197078)(entity)) {
        if (func_0018F030((char *)entity)) {
            func_001958B8(entity, func_00182240, 1);
        } else {
            func_001958B8(entity, func_00181E48, 1);
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
        entity->unkC8 = func_00190F90(entity, 0, 50.0f);
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
        if (func_00194F80(entity->model->position, stand_point) < reach) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
            func_00216778(unkE0->unk30, face_point);
            entity->target = unkE0->unk30;
            entity->unkC8 = func_00190BE0(entity);
            if (entity->unkC8 == 0) {
                func_001958B8(entity, func_00178E10, unknown);
                return;
            }
        } else {
            func_00216778(unkE0->unk30, stand_point);
            entity->target = unkE0->unk30;
            entity->unkC8 = func_00190F90(entity, 0, reach * 0.5f);
        }
    }
    func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, unknown);
}
