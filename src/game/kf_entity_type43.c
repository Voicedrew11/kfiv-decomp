/* The behaviour of entity type 43 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers that function installs. */
#include "kf_entity.h"

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
void func_001818B8(Entity *);
extern s32 D_003E6E40[][2];
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
int func_0018EAC0(void *, void *);
s32 func_00190F90(Entity *, s32, f32);
void func_00181AE0(Entity *);
void func_00181C60(Entity *);
int func_00192C98(void *, int, int, u32, int);
s32 func_00199EB8(Entity *, f32 *, f32);
void func_0019A290(Entity *, s32);
double atan(double);
void *memset(void *, int, u32);
float func_00195478(float *, char *);
float func_001957B8(float, float);
float func_00194FB8(float);
void func_001B2C48(void *);
s32 func_001B1470(Unk1B1470Request *, Unk1B1470Extra *);

static void func_001749F0(Entity *entity, s32 unknown);
static void func_00174B68(Entity *entity, s32 unknown);
static void func_00174DF0(Entity *entity);
static s32 func_00175110(Entity *entity, EntityParamsUnk240 *record, s32 index, s32 frame, s32 current_frame, f32 angle);

/* Set-up of a type 43 entity: clears its work area and installs the type's handlers. */
void EntityType43_Init(Entity *entity) {
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
    unkE0->unk00 = func_001749F0;
    unkE0->unk04 = func_00182A08;
    unkE0->unk08 = (void (*)(void))func_00182BD0;
    unkE0->unk18 = (void (*)(void))func_00183570;
    unkE0->unk10 = (void (*)(void))func_00181038;
    unkE0->unk14 = (void (*)(void))func_001818B8;
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

static void func_001749F0(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        func_00196870(entity, 1);
    }
    if (unkE0->unk68 > 0) {
        unkE0->unk68--;
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
        func_00174B68(entity, unknown);
        return;
    }
    func_00182FD8(entity, unknown);
}

static void func_00174B68(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    f32 range;
    f32 angle;
    s32 next_action;
    s32 side;

    if (entity->params->flags & 0x100) {
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
    } else {
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
    }
    range = 300.0f;
    angle = 0.5235988f;
    if (func_0018ED98(entity, range, angle)) {
        func_001958B8(entity, func_00181C60, 1);
        return;
    }
    if (func_0018EEE0(entity, range, angle)) {
        func_001958B8(entity, func_00181AE0, 1);
        return;
    }
    next_action = func_0018FD50(entity, 0);
    ENTITY_FLAG_BITS(entity)->bit9 = TRUE;
    if (entity->unkA0 == 0 || next_action == 0) {
        entity->unkCF = entity->unkCD;
        entity->unkCD = next_action;
        if (entity->unkCD != entity->unkCF) {
            entity->unk9E = D_00446ED4;
        }
        // match: called with one argument here
        ((void (*)(Entity *))func_00190B88)(entity);
        switch (entity->unkCD) {
        case 0:
            if (unkE0->unk68 == 0) {
                unkE0->unk68 = (u16)entity->params->unk430;
                func_001958B8(entity, func_00174DF0, unknown);
                return;
            }
            unkE0->unk78 = -1;
            break;
        case 1:
            entity->target = D_00413F80;
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
    if (entity->unkCD == 1) {
        side = func_0018EAC0(entity, direction);
        if (unkE0->unk78 == 0) {
            if (side > 0) {
                entity->unkC8 = 0x201;
            } else if (side < 0) {
                entity->unkC8 = 0x101;
            } else {
                entity->unkC8 = func_00190F90(entity, 0, entity->radius + 100.0f + 30.0f);
            }
        }
    }
    func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, unknown);
}

static void func_00174DF0(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityParamsUnk240 *unk240 = &entity->params->unk240[0];
    s32 elapsed;

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 0x14, 0, entity->params->unkC4 >> 1, 1);
        if (entity->params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            s32 kind = 2;

            if (!(ENTITY_PARAMS_BITS(entity)->bit25 && !ENTITY_PARAMS_BITS(entity)->bit26)) {
                kind = 1;
            }
            ENTITY_FLAG_BITS(entity)->bits11 = kind;
        }
        ENTITY_FLAG_BITS(entity)->bit54 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit55 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit56 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
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
        elapsed = (entity->model->unkFC->unk84 - entity->params->unk80[entity->unkD1]) >> 8;
        if (!ENTITY_FLAG_BITS(entity)->bit54) {
            ENTITY_FLAG_BITS(entity)->bit54 = func_00175110(entity, unk240, 0, elapsed, entity->model->unkFC->unk82, 0.0f);
        }
        if (entity->model->unkFC->unk82 > 0 && unk240->unk03 >= entity->model->unkFC->unk82) {
            entity->flags |= 0x80000;
        } else {
            entity->flags &= ~0x80000;
        }
        if ((s32)(ENTITY_FLAGS(entity) >> 34) & 1) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            func_0019A290(entity, unk240->unk02);
            if ((s32)(ENTITY_FLAGS(entity) >> 5) & 1) {
                unkE0->unk00(entity, 1);
            } else {
                func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, 1);
            }
        }
        break;
    }
}

static s32 func_00175110(Entity *entity, EntityParamsUnk240 *record, s32 index, s32 frame, s32 current_frame, f32 angle) {
    Unk1B1470Request request;
    Unk1B1470Extra extra;
    f32 yaw;
    f32 pitch;
    f32 spread;

    if (record->unk10[index].unk24 != 0 && !(frame < record->unk10[index].unk24 && current_frame < record->unk10[index].unk24) &&
        !(record->unk10[index].unk26 < frame && record->unk10[index].unk26 < current_frame)) {
        yaw = func_001957B8(D_00413F80[5], func_00195478(D_00413F80, (char *)entity->model->position));
        pitch = (f32)atan((entity->model->position[1] - D_00413F80[1] + 220.0f) / entity->distance_to_player) + D_00413F80[4];
        if (ABS(yaw) < entity->params->unk432 * 0.017453292f && ABS(pitch) < (s16)entity->params->unk434 * 0.017453292f) {
            spread = func_00194FB8(record->unk10[0].unk22 * 0.017453292f);
            if (ABS(func_00194FB8(entity->unk68 - entity->model->unkD0[1])) < spread) {
                func_001B2C48(&entity->params->unk240[0].unk10[0].unk12);
                memset(&request, 0, sizeof(request));
                memset(&extra, 0, sizeof(extra));
                func_00216778(request.position, entity->model->position);
                request.unk34 = entity->height + 10.0f;
                request.unk30 = entity->radius + record->unk10[index].unk20;
                request.unk38 = func_00194FB8(angle + 3.1415927f);
                request.unk3C = func_00194FB8(record->unk10[index].unk22 * 0.017453292f);
                request.owner = entity;
                request.unk4C_0 = 3;
                request.unk4D = 1;
                request.unk50 = record->unk10[index].unk00;
                request.unk62 = record->unk10[index].unk12;
                request.unk4E = record->unk0C;
                extra.unk00 = 0;
                extra.unk02 = 0;
                extra.unk20 = 0;
                extra.unk22 = 0;
                request.unk4C_3 = record->unk10[index].unk1E & 1;
                return func_001B1470(&request, &extra);
            }
        }
    }
    return 0;
}
