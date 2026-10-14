/* The behaviour of entity type 11 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers that function installs. */
#include "kf_entity.h"
#include "common.h"

void func_001958A8(char *, s32);
int func_001958B8(Entity *, void (*)(Entity *), int);
void func_00191478(EntityModel *);
int func_00193378(EntityModel *, s32, s32);
// match: the definition takes an Actor, a struct local to func_00182A08.c
void func_00182A08(void);
void func_00183570(Entity *);
void func_00181038(Entity *);
void func_001818B8(Entity *);
extern s32 D_003E6E40[][2];
void func_00190A30(char *);
void func_00195898(char *, u8);
int func_0019A310(char *, void (*)(void), int);
extern void func_00181378(Entity *);
int rand(void);
void func_0018D8B8(Entity *, s32, s32, s32);
int func_00192C98(void *, int, int, u32, int);
void func_0019A290(Entity *, s32);
void func_00182240(Entity *);
extern u32 D_00446ED4;
int func_00182FD8(void *, int);
int func_00182DE0(void *, int);
void func_0018E9C0(void *);
void func_00196870(void *, int);
void func_00196CC0(void *);
extern PlayerData D_00413F80;
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

static void func_00150768(Entity *entity);
static void func_00150818(Entity *entity);
static void func_00150A98(Entity *entity);
static void func_00150D40(Entity *entity, s32 unknown);
static void func_00150E40(Entity *entity, s32 unknown);

/* Set-up of a type 11 entity: clears its work area and installs the type's handlers. */
void EntityType11_Init(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    s32 i;

    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        ENTITY_WORK_BYTES(entity)[i] = 0;
    }
    func_001958A8((char *)entity, D_003E6E40[entity->params->unk02][1]);
    unkE0->unk00 = (void (*)(Entity *, s32))func_00150D40;
    unkE0->unk04 = func_00182A08;
    unkE0->unk08 = (void (*)(void))func_00150768;
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
    unkE0->unk40 = 0.5f;
    entity->unkD6 = (entity->params->flags & 0x4000) ? 0x40 : 0x80;
    entity->model->unk15C = entity->unkD6;
    func_00191478(entity->model);
    func_00193378(entity->model, 0, 0);
}

static void func_00150768(Entity *entity) {
    u64 flags;
    u64 kind;

    func_00190A30((char *)entity);
    entity->target = NULL;
    entity->flags &= ~0x400;
    entity->flags &= ~0x200;
    flags = entity->flags;
    kind = (flags >> 36) & 3;
    if (kind == 1) {
        if ((int)(flags >> 19) & 1) {
            func_0019A310((char *)entity, (void (*)(void))func_00181378, 0);
        } else {
            func_0019A310((char *)entity, (void (*)(void))func_00150818, 0);
        }
    } else if (kind == 2) {
        func_0019A310((char *)entity, (void (*)(void))func_00150A98, 0);
    }
    func_00195898((char *)entity, 2);
}

static void func_00150818(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 = 2;
    switch (entity->unkCB) {
    case 0:
        func_0018D8B8(entity, 0xE, 0, 0);
        func_00192C98(entity, 0xE, 0, 5, 1);
        if (entity->params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->bits11 = (((EntityParamsWord *)entity->params)->unk00 & 0x6000000) == 0x2000000 ? 2 : 1;
        }
        unkE0->unk1C = 0.0f;
        ENTITY_FLAG_BITS(entity)->bit14 = 1;
        ENTITY_FLAG_BITS(entity)->bit19 = 0;
        ENTITY_FLAG_BITS(entity)->bit35 = 0;
        ENTITY_FLAG_BITS(entity)->bit10 = 0;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->unkCB++;
        break;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->unkCB++;
        }
        break;
    case 2:
        func_0019A290(entity, entity->params->unkC8);
        func_0019A310((char *)entity, NULL, 0);
        entity->unkC6 = 0;
        entity->unkC8 = 0;
        entity->unkA0 = 0;
        if (entity->spawn_info->unk40 == 0 && rand() % 100 < entity->params->unk436) {
            ENTITY_FLAG_BITS(entity)->bit57 = 1;
            ENTITY_FLAG_BITS(entity)->bits58 = 0;
            func_001958B8(entity, func_00182240, 1);
        } else if (ENTITY_FLAG_BITS(entity)->bit5) {
            unkE0->unk00(entity, 1);
        } else {
            func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, 1);
        }
        break;
    }
}

static void func_00150A98(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 = 4;
    switch (entity->unkCB) {
    case 0:
        func_00192C98(entity, 0xF, 0, 5, 1);
        if (entity->params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->bits11 = (((EntityParamsWord *)entity->params)->unk00 & 0x6000000) == 0x2000000 ? 2 : 1;
        }
        unkE0->unk1C = 0.0f;
        ENTITY_FLAG_BITS(entity)->bit14 = 1;
        ENTITY_FLAG_BITS(entity)->bit19 = 0;
        ENTITY_FLAG_BITS(entity)->bit35 = 0;
        if (!(((EntityParamsWord *)entity->params)->unk00 & 0x6000000)) {
            ENTITY_FLAG_BITS(entity)->bit28 = 0;
        }
        ENTITY_FLAG_BITS(entity)->bit10 = 0;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->unkCB++;
        break;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->unkCB++;
        }
        break;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            func_0019A290(entity, entity->params->unkC8);
            func_0019A310((char *)entity, NULL, 0);
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if (entity->spawn_info->unk40 == 0 && rand() % 100 < entity->params->unk436) {
                ENTITY_FLAG_BITS(entity)->bit57 = 1;
                ENTITY_FLAG_BITS(entity)->bits58 = 0;
                func_001958B8(entity, func_00182240, 1);
            } else if (ENTITY_FLAG_BITS(entity)->bit5) {
                unkE0->unk00(entity, 1);
            } else {
                func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, 1);
            }
        }
        break;
    }
}

static void func_00150D40(Entity *entity, s32 unknown) {
    u32 phase;

    phase = D_00446ED4 % 10U;
    if (phase == entity->unkD7 % 10U) {
        func_00196870(entity, 1);
    }
    if (entity->unkC6 != 0) {
        return;
    }
    func_00196CC0(entity);
    func_0018E9C0(entity);
    if (ENTITY_FLAG_BITS(entity)->bit53) {
        func_00182DE0(entity, unknown);
        return;
    }
    switch (entity->state) {
    case 0:
    case 1:
        break;
    case 2:
        func_00150E40(entity, unknown);
        return;
    }
    func_00182FD8(entity, unknown);
}

static void func_00150E40(Entity *entity, s32 unknown) {
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
    if (entity->spawn_info->unk40 == 0 && D_00446ED4 % (u32)(unkE0->unk7A + 0x78) == 0) {
        unkE0->unk7A = rand() % 60;
        if (rand() % 100 < (s16)entity->params->unk434) {
            ENTITY_FLAG_BITS(entity)->bit57 = 1;
            ENTITY_FLAG_BITS(entity)->bits58 = 0;
            func_001958B8(entity, func_00182240, unknown);
            return;
        }
    }
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
                func_001958B8(entity, func_00182240, unknown);
            } else {
                func_001958B8(entity, func_00181E48, unknown);
            }
            return;
        case 1:
            entity->target = &D_00413F80;
            entity->unkA0 = rand() % 60 + 0x78;
            unkE0->unk78 = 0;
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
