/* The behaviour of entity type 39 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers that function installs. */
#include "kf_entity.h"

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
void func_00196D48(Entity *);
float func_00195478(float *, char *);
float func_001957B8(float, float);
int func_00190BE0(Entity *);
int func_0018EAC0(void *, void *);
s32 func_00190F90(Entity *, s32, f32);
void func_00181AE0(Entity *);
void func_00181C60(Entity *);
void func_00181E48(Entity *);
void func_00182240(Entity *);
void func_00190A30(char *);
int func_0019A310(char *, void (*)(void), int);
void func_00195898(char *, u8);
void func_001816A0(Entity *);
float func_001954A8(float *, float *);
float func_00194F80(void *, void *);

static void func_00154580(Entity *entity, s32 unknown);
static void func_00154710(Entity *entity, s32 unknown);
static void func_00154C58(Entity *entity);
static s32 func_00154CD0(Entity *entity, s32 unused, f32 reach);

/* Set-up of a type 39 entity: clears its work area and installs the type's handlers. */
void EntityType39_Init(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    char *unk138 = entity->model->unk138;
    s32 i;

    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        ENTITY_WORK_BYTES(entity)[i] = 0;
    }
    func_001958A8((char *)entity, D_003E6E40[entity->params->unk02][1]);
    unkE0->unk00 = func_00154580;
    unkE0->unk04 = func_00182A08;
    unkE0->unk08 = (void (*)(void))func_00154C58;
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

static void func_00154580(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        func_00196870(entity, 1);
    }
    if (entity->unk2C < entity->params->unk0A >> 1 && unkE0->unk7A == 0) {
        unkE0->unk7A = 1;
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
                func_00154710(entity, unknown);
                break;
            default:
                func_00182FD8(entity, unknown);
                break;
            }
        }
    }
}

static void func_00154710(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    f32 angle;
    s32 side;
    s32 turn;
    s32 mode;

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
    if (unkE0->unk78 != 1) {
        angle = func_001957B8(D_00413F80.unk14, func_00195478(D_00413F80.position, (char *)entity->model->position));
        if (entity->unk60 - entity->radius - 100.0f < 200.0f && ABS(angle) < 0.6981317f) {
            angle = func_001957B8(entity->model->unkD0[1], entity->unk68);
            if (ABS(angle) > 0.6981317f && ABS(angle) < 1.5707964f) {
                unkE0->unk78 = 1;
                entity->target = &D_00413F80;
                entity->unkCD = 1;
                entity->unkA0 = rand() % 60 + 90;
            }
        }
    }
    mode = entity->unkCD;
    if (mode == 1) {
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
                if (unkE0->unk7A != 0) {
                    if (turn > 0) {
                        entity->unkC8 = 0x241;
                    } else if (turn < 0) {
                        entity->unkC8 = 0x141;
                    } else {
                        entity->unkC8 = func_00190F90(entity, 1, entity->radius + 100.0f + 30.0f);
                    }
                } else {
                    if (turn > 0) {
                        entity->unkC8 = 0x201;
                    } else if (turn < 0) {
                        entity->unkC8 = 0x101;
                    } else {
                        entity->unkC8 = func_00190F90(entity, 0, entity->radius + 100.0f + 30.0f);
                    }
                }
                break;
            case 1:
                if (turn > 0) {
                    entity->unkC8 = 0x102;
                } else if (turn < 0) {
                    entity->unkC8 = 0x202;
                } else {
                    // match: the caller passes an integer argument the callee does not take
                    entity->unkC8 = func_00154CD0(entity, 0, entity->radius + 100.0f + 30.0f);
                }
                break;
            }
        }
    }
done:
    func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, unknown);
}

static void func_00154C58(Entity *entity) {
    if (ENTITY_FLAG_BITS(entity)->bits36 == 2) {
        func_00190A30((char *)entity);
        ENTITY_FLAG_BITS(entity)->bit10 = 0;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->target = NULL;
        func_0019A310((char *)entity, (void (*)(void))func_001816A0, 0);
    }
    func_00195898((char *)entity, 2);
}

/* The movement command for walking to the target: a turn bit when the target is off the heading by more than
   the turn limit, and bit 1 while it is farther away than reach. */
static s32 func_00154CD0(Entity *entity, s32 unused, f32 reach) {
    f32 turn;
    f32 limit;
    f32 heading;
    s32 command;

    command = 0;
    if (entity->target == NULL) {
        return 0;
    }
    heading = func_001954A8(entity->model->position, entity->target);
    turn = func_001957B8(entity->model->unkD0[1], heading);
    limit = entity->params->turn_limit;
    if (0.0f <= turn ? limit < turn : limit < -turn) {
        command = 0x100;
        if (0.0f <= turn) {
            command = 0x200;
        }
    }
    if (reach < func_00194F80(entity->model->position, entity->target)) {
        command |= 2;
    }
    return command;
}
