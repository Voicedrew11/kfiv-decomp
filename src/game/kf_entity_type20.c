/* The behaviour of entity type 20 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers that function installs. */
#include "kf_entity.h"
#include "common.h"
#include "kf_map_object.h"

void func_001958A8(char *, s32);
int func_001958B8(Entity *, void (*)(Entity *), int);
void func_00191478(EntityModel *);
int func_00193378(EntityModel *, s32, s32);
void func_00216778(void *, void *);
void func_00182BD0(char *);
void func_00183570(Entity *);
void func_00181038(Entity *);
void func_001818B8(Entity *);
extern s32 D_003E6E40[][2];
int func_00192EB0(void *, int);
// match: the callee is void, but retail keeps what it leaves in $2
int func_00194368(EntityModel *, f32);
int func_00196120(Entity *, void *, s32, void *, f32);
double atan2(double, double);
float func_00194FB8(float);
float func_00195388(float *, float, float);
float atan2f(float, float);
float func_001954A8(float *, float *);
float func_001957B8(float, float);
extern u32 D_00446ED4;
void func_00196870(void *, int);
void func_00196CC0(char *);
void func_0018E9C0(void *);
int func_00182DE0(void *, int);
int func_00182FD8(void *, int);
void func_00183150(Entity *, s32);
extern PlayerData D_00413F80;
int rand(void);
s32 func_0018ED98(Entity *, f32, f32);
s32 func_0018EEE0(Entity *, f32, f32);
s32 func_0018FD50(Entity *, s32);
void func_00190B88(Entity *, s32);
int func_0018F030(char *);
float func_00195478(float *, char *);
int func_00190BE0(Entity *);
int func_0018EAC0(void *, void *);
void func_00181AE0(Entity *);
void func_00181C60(Entity *);
void func_00181E48(Entity *);
int func_00192C98(void *, int, int, u32, int);
s32 func_00199EB8(Entity *, f32 *, f32);
int func_0018CF30(Entity *, EntityParamsUnk3D8 *, s32, s32, f32 *, f32, f32);
void func_0019A290(Entity *, s32);
extern u8 D_00349690[];
float func_00194F80(void *, void *);
s32 func_00190F90(Entity *, s32, f32);
int func_002109C8(int, int);
extern int D_00446770;

static void func_00148FE8(Entity *entity);
static void func_001491C0(Entity *entity);
static void func_00149290(Entity *entity);
static void func_00149420(Entity *entity, s32 unknown);
static void func_00149610(Entity *entity, s32 unknown);
static void func_001499F8(Entity *entity);
static void func_00149CA8(Entity *entity, s32 unknown);
static void func_00149ED8(Entity *entity, s32 unknown);
static void func_0014A058(Entity *entity);

/* Set-up of a type 20 entity: clears its work area and installs the type's handlers. */
void EntityType20_Init(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    char *unk138 = entity->model->unk138;
    s32 i;

    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        ENTITY_WORK_BYTES(entity)[i] = 0;
    }
    func_001958A8((char *)entity, D_003E6E40[entity->params->unk02][1]);
    if (entity->spawn_info->unk40 != 0) {
        unkE0->unk00 = func_00149CA8;
    } else {
        unkE0->unk00 = func_00149420;
    }
    unkE0->unk04 = (void (*)(void))func_00148FE8;
    unkE0->unk08 = (void (*)(void))func_00182BD0;
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

static void func_00148FE8(Entity *entity) {
    f32 limits[4];
    u32 blend_frames; // match: converted to float as unsigned
    EntityUnkE0 *unkE0;

    unkE0 = &entity->unkE0;
    if (ENTITY_FLAG_BITS(entity)->bit14) {
        blend_frames = entity->model->unkFC->unk88;
        if (blend_frames != 0) {
            ENTITY_FLAG_BITS(entity)->bit34 = func_00194368(entity->model, (entity->unkC6 & 0x12) ? unkE0->unk40 : 1.0f / (f32)blend_frames);
        } else {
            ENTITY_FLAG_BITS(entity)->bit34 = func_00192EB0(entity, ENTITY_FLAG_BITS(entity)->bits11);
        }
        if ((u32)((u8)entity->unkD1 - 0x17) < 2 && entity->model->unkFC->unk88 == 0) {
            func_001491C0(entity);
        } else {
            unkE0->unk58 = 0.0f;
        }
        func_00149290(entity);
        limits[1] = entity->params->unk430 * 0.017453292f;
        limits[0] = -entity->params->unk430 * 0.017453292f;
        limits[3] = entity->params->unk432 * 0.017453292f;
        limits[2] = -entity->params->unk432 * 0.017453292f;
        if (ENTITY_FLAG_BITS(entity)->bit9) {
            func_00196120(entity, unkE0->unk20, ENTITY_FLAG_BITS(entity)->bit9, limits, 12.0f);
        } else {
            func_00196120(entity, unkE0->unk20, 0, NULL, 8.0f);
        }
    }
}

static void func_001491C0(Entity *entity) {
    EntityController *controller;
    EntityModelPart80 *part;
    EntityUnkE0 *unkE0;
    f32 pitch;

    controller = ENTITY_CONTROLLER(entity);
    part = &((EntityModelPart80 *)entity->model->unkF8)[controller->unk434];
    unkE0 = &entity->unkE0;
    if (part != NULL) {
        pitch = func_00194FB8(atan2(D_00413F80.position[1] - entity->model->position[1], entity->distance_to_player));
        if (0.87266463f < pitch) {
            pitch = 0.87266463f;
        } else if (pitch < -0.87266463f) {
            pitch = -0.87266463f;
        }
        func_00195388(&unkE0->unk58, pitch, 5.0f);
        part->unk50[0] = unkE0->unk58;
    }
}

static void func_00149290(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityModelPart80 *parts = (EntityModelPart80 *)entity->model->unkF8;
    f32 pitch;
    f32 pitch_limit;
    f32 half_pitch;
    f32 yaw;
    f32 yaw_limit;
    f32 half_yaw;
    f32 yaw_to_target;

    if (entity->target == NULL || entity->unkC6 != 0) {
        unkE0->unk48 = parts[30].unk50[0];
        unkE0->unk4C = parts[30].unk50[1];
        unkE0->unk50 = parts[31].unk50[0];
        unkE0->unk54 = parts[31].unk50[1];
    } else {
        pitch = atan2f(D_00413F80.position[1] - entity->model->position[1], entity->distance_to_player);
        pitch_limit = 0.5235988f;
        if (pitch_limit < pitch) {
            pitch = pitch_limit;
        } else if (pitch < -pitch_limit) {
            pitch = -pitch_limit;
        }
        half_pitch = pitch * 0.5f;
        func_00195388(&unkE0->unk48, half_pitch, 10.0f);
        func_00195388(&unkE0->unk50, half_pitch, 10.0f);
        parts[20].unk50[0] = unkE0->unk48;
        parts[21].unk50[0] = unkE0->unk50;
        yaw_to_target = func_001954A8(entity->model->position, entity->target);
        yaw = func_001957B8(entity->model->unkD0[1], yaw_to_target);
        yaw_limit = 0.6981317f;
        if (yaw_limit < yaw) {
            yaw = yaw_limit;
        } else if (yaw < -yaw_limit) {
            yaw = -yaw_limit;
        }
        half_yaw = yaw * 0.5f;
        func_00195388(&unkE0->unk4C, half_yaw, 10.0f);
        func_00195388(&unkE0->unk54, half_yaw, 10.0f);
        parts[20].unk50[1] = unkE0->unk4C;
        parts[21].unk50[1] = unkE0->unk54;
    }
}

static void func_00149420(Entity *entity, s32 unknown) {
    if (entity->model->unkFC->unk88 == 0 &&
        ((entity->unkD1 == 0x17 && entity->model->unkFC->unk82 >= 7 && entity->model->unkFC->unk82 < 0x24) ||
         (entity->unkD1 == 0x18 && entity->model->unkFC->unk82 >= 0x19 && entity->model->unkFC->unk82 < 0x32))) {
        ((EntityModelPart80 *)entity->model->unkF8)[29].unk68 |= 0x8000L << 25;
    } else {
        ((EntityModelPart80 *)entity->model->unkF8)[29].unk68 &= ~(0x8000L << 25);
    }
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
        func_00149610(entity, unknown);
        return;
    }
    func_00182FD8(entity, unknown);
}

static void func_00149610(Entity *entity, s32 unknown) {
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
    if (func_0018EEE0(entity, 300.0f, 0.6981317f)) {
        func_001958B8(entity, func_00181AE0, 1);
        return;
    }
    ENTITY_FLAG_BITS(entity)->bit10 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit9 = TRUE;
    side = func_0018FD50(entity, 0);
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
                func_001958B8(entity, func_001499F8, unknown);
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
    angle = func_001957B8(D_00413F80.unk14, func_00195478(D_00413F80.position, (char *)entity->model->position));
    if (entity->unk60 - entity->radius - 100.0f < 200.0f && ABS(angle) < 0.6981317f) {
        if (D_00446ED4 % 10U == entity->unkD7 % 10U &&
            rand() % 100 < (entity->params->unk0A - entity->unk2C) * 100 / entity->params->unk0A) {
            angle = func_001957B8(entity->model->unkD0[1], entity->unk68);
            if (ABS(angle) > 0.6981317f && ABS(angle) < 1.5707964f) {
                func_001958B8(entity, func_00181AE0, 1);
                return;
            }
        }
    }
    if (unkE0->unk78 == 0) {
        turn = func_0018EAC0(entity, direction);
        if (turn > 0) {
            entity->unkC8 = 0x201;
        } else if (turn < 0) {
            entity->unkC8 = 0x101;
        } else {
            entity->unkC8 = func_00190BE0(entity);
        }
    }
    func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, unknown);
}

static void func_001499F8(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    s32 attack_index = (s32)(entity->flags >> 58) & 3;
    EntityParamsUnk3D8 *unk3D8 = ENTITY_PARAMS_UNK3D8(entity->params, attack_index);
    f32 target[4];
    s32 elapsed;

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, attack_index + 0x17, 0, entity->params->unkC4 >> 1, 1);
        if (entity->params->flags & 0x100) {
            ENTITY_FLAGS(entity) &= ~0x3800;
        } else {
            ENTITY_FLAG_BITS(entity)->bits11 = ((((EntityParamsWord *)entity->params)->unk00 & 0x06000000) == 0x02000000) ? 2 : 1;
        }
        ENTITY_FLAGS(entity) &= ~0x200;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        if (unk3D8->unk01 != 0) {
            func_00199EB8(entity, &unkE0->unk1C, entity->unk68);
        }
        break;
    case 2:
        if (unk3D8->unk01 >= entity->model->unkFC->unk82) {
            func_00199EB8(entity, &entity->unkE0.unk1C, entity->unk68);
        } else {
            unkE0->unk1C = 0.0f;
        }
        elapsed = (entity->model->unkFC->unk84 - entity->params->unk80[entity->unkD1]) >> 8;
        func_00216778(target, D_00413F80.position);
        target[1] -= 190.0f;
        func_0018CF30(entity, unk3D8, elapsed, entity->model->unkFC->unk82, target, 0.17453294f, 0.17453294f);
        if (entity->model->unkFC->unk82 > 0 && unk3D8->unk03 >= entity->model->unkFC->unk82) {
            ENTITY_FLAGS(entity) |= 0x80000;
        } else {
            ENTITY_FLAGS(entity) &= ~0x80000;
        }
        if ((s32)(ENTITY_FLAGS(entity) >> 34) & 1) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            func_0019A290(entity, unk3D8->unk02);
            if ((s32)(ENTITY_FLAGS(entity) >> 5) & 1) {
                unkE0->unk00(entity, 1);
            } else {
                func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, 1);
            }
        }
        break;
    }
}

static void func_00149CA8(Entity *entity, s32 unknown) {
    if (entity->model->unkFC->unk88 == 0 &&
        ((entity->unkD1 == 0x17 && entity->model->unkFC->unk82 >= 7 && entity->model->unkFC->unk82 < 0x24) ||
         (entity->unkD1 == 0x18 && entity->model->unkFC->unk82 >= 0x19 && entity->model->unkFC->unk82 < 0x32))) {
        ((EntityModelPart80 *)entity->model->unkF8)[29].unk68 |= 0x8000L << 25;
    } else {
        ((EntityModelPart80 *)entity->model->unkF8)[29].unk68 &= ~(0x8000L << 25);
    }
    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        func_00196870(entity, 1);
    }
    if (entity->unkC6 != 0) {
        return;
    }
    if (D_00349690[entity->spawn_info->unk42 + entity->spawn_info->variant * 64] != 0) {
        func_00149ED8(entity, unknown);
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
        func_00149610(entity, unknown);
        return;
    }
    func_00182FD8(entity, unknown);
}

static void func_00149ED8(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 stand_point[4];
    f32 face_point[4];
    f32 reach;
    MapObject *map_object;

    entity->flags &= ~0x200;
    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    map_object = MapObject_GetById(entity->group_id, entity->spawn_info->unk43);
    if (map_object != NULL) {
        stand_point[0] = map_object->model->position.x;
        stand_point[1] = map_object->model->position.y;
        stand_point[2] = map_object->model->position.z + 100.0f;
        stand_point[3] = 0.0f;
        face_point[0] = map_object->model->position.x;
        face_point[1] = map_object->model->position.y;
        face_point[2] = map_object->model->position.z;
        face_point[3] = 0.0f;
    }
    reach = 100.0f;
    if (func_00194F80(entity->model->position, stand_point) < reach) {
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
        func_00216778(unkE0->unk30, face_point);
        entity->target = unkE0->unk30;
        entity->unkC8 = func_00190BE0(entity);
        if (entity->unkC8 == 0) {
            func_001958B8(entity, func_0014A058, unknown);
            return;
        }
    } else {
        func_00216778(unkE0->unk30, stand_point);
        entity->target = unkE0->unk30;
        entity->unkC8 = func_00190F90(entity, 0, reach * 0.5f);
    }
    func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, unknown);
}

static void func_0014A058(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    MapObject *map_object;

    entity->unkC6 |= 0x1000;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 0x11, 0, entity->params->unkC4, 0);
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        unkE0->unk1C = 0.0f;
        entity->action_step++;
        break;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (entity->model->unkFC->unk82 == entity->spawn_info->unk44) {
            map_object = MapObject_GetById(entity->group_id, entity->spawn_info->unk43);
            if (map_object != NULL && map_object->unk08->unk40 != 0 && entity->group_id == D_00446770) {
                func_002109C8(map_object->unk08->unk40, 0);
            }
        }
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if (ENTITY_FLAG_BITS(entity)->bit5) {
                unkE0->unk00(entity, 1);
            } else {
                func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, 1);
            }
        }
        break;
    }
}
