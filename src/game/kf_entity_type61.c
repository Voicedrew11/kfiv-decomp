/* The behaviour of entity type 61 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers that function installs. */
#include "kf_entity.h"
#include "kf_entity_world.h"
#include "common.h"

void func_001958A8(char *, s32);
int func_001958B8(Entity *, void (*)(Entity *), int);
void func_00191478(EntityModel *);
void func_001914B0(EntityModel *, s32, s32, s32);
int func_00193378(EntityModel *, s32, s32);
void func_00216778(void *, void *);
extern s32 D_003E6E40[][2];
int func_0019A538(void *, void *, int);
s32 func_00191600(Entity *, EntityUnkE0SlotBits *, s32, s32);
void func_0018D670(Entity *, s32, s32, s32);
void func_0018D8B8(Entity *, s32, s32, s32);
int func_00196120(Entity *, void *, s32, void *, f32);
extern u32 D_00446ED4;
void func_00196870(void *, int);
void func_0018E9C0(void *);
s32 func_001992F0(Entity *, s32);
int func_001958F8(Entity *, void (*)(Entity *entity), int, int, int);
int func_0019A448(void *, void *, int, int, int, int, int);
extern PlayerData D_00413F80;
int func_00195E48(void *, int, void *, void *);
void func_0018DCD8(Entity *);
extern EntityFrameTable5 D_0043E8E8;
int rand(void);
void func_0019A290(Entity *, s32);
void func_0018D600(char **, int, u32, int);
// match: the definition takes an Actor, a struct local to func_00182A08.c
void func_00182A08(void);
void func_00182BD0(char *);
void func_00183570(Entity *);
void func_00196CC0(char *);
float func_00194F80(void *, void *);
int func_00182DE0(void *, int);
int func_00182FD8(void *, int);
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
extern EntityFrameTable D_0043E900;
float atan2f(float, float);
char *func_0010CAB8(char *, u32);
float func_00194FB8(float);
void func_0018DC40(char *);

static void func_0014E7F8(Entity *entity);
static void func_0014EA70(Entity *entity, s32 unknown);
static void func_0014EBA8(Entity *entity, s32 unknown);
static void func_0014EC80(Entity *entity);
static void func_0014ECE8(Entity *entity);
static void func_0014EFB0(Entity *entity);
static void func_0014F240(Entity *entity);
static void func_0014F3F8(Entity *entity);
static void func_0014F5A8(Entity *entity, s32 unknown);
static void func_0014F6E8(Entity *entity, s32 unknown);
static void func_0014FA28(Entity *entity);
static void func_0014FCC0(Entity *entity);
static void func_0014FF68(Entity *entity);
static void func_00150200(Entity *entity);
static s32 func_001503A8(Entity *entity);
static void func_00150478(Entity *entity);

/* Set-up of a type 61 entity: clears its work area and installs the type's handlers. */
void EntityType61_Init(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    u8 *unkBC = entity->unkBC;
    u8 *unkC0 = entity->unkC0;
    char *unk138 = entity->model->unk138;
    s32 i;
    Entity *other;

    // match: in the three loops the pointer has to be formed inside the loop
    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        u8 *dest = (u8 *)&entity->unkE0;

        dest[i] = 0;
    }
    func_001958A8((char *)entity, D_003E6E40[entity->params->unk02][1]);
    unkE0->unk00 = func_0014EA70;
    unkE0->unk04 = (void (*)(void))func_0014E7F8;
    unkE0->unk18 = (void (*)(void))func_0014EC80;
    unkE0->unk14 = (void (*)(void))func_0014ECE8;
    unkE0->unk08 = NULL;
    for (i = 4 - 1; i != -1; i--) {
        entity->unkBC[i] = 0x80;
    }
    for (i = 6 - 1; i != -1; i--) {
        entity->unkC0[i] = 0x80;
    }
    ENTITY_UNK5C(entity)->as_s32 = 0;
    ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit25 = TRUE;
    func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, 0);
    if (unk138 != NULL) {
        func_00216778(unkE0->unk20, unk138 + 0x50);
    }
    unkE0->unk40 = 0.5f;
    entity->unkD6 = (entity->params->flags & 0x4000) ? 0x40 : 0x80;
    entity->model->unk15C = entity->unkD6;
    func_00191478(entity->model);
    func_001914B0(entity->model, 5, 0x16, 1);
    ((EntityUnkE0Kind151D30 *)unkE0)->unk80.bit5 = 1;
    ((EntityUnkE0Kind151D30 *)unkE0)->unk80.bit6 = 1;
    ((EntityUnkE0Kind151D30 *)unkE0)->unk88.bit6 = 1;
    func_00193378(entity->model, 0, 0);
    for (i = 0; i < 6; i++) {
        other = Entity_GetBySpawnId(i + 10);
        if (other != NULL) {
            other->flags &= ~2;
            other->flags &= ~0x80;
            other->flags &= ~0x100;
        }
    }
}

static void func_0014E7F8(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityUnkE0Kind151D30 *slots = (EntityUnkE0Kind151D30 *)unkE0;
    f32 limits[4];
    s32 previous;

    if (!ENTITY_FLAG_BITS(entity)->bit14) {
        return;
    }
    if (slots->unk80.unk01 != 0) {
        // match: the caller passes a float the callee does not take
        ENTITY_FLAG_BITS(entity)->bit34 = ((int (*)(void *, void *, int, float))func_0019A538)(entity, &((EntityUnkE0Kind151D30 *)&entity->unkE0)->unk80, 0, 1.0f / slots->unk80.unk01);
    } else {
        previous = slots->unk80.unk04;
        func_00191600(entity, &((EntityUnkE0Kind151D30 *)&entity->unkE0)->unk80, 0, ENTITY_FLAG_BITS(entity)->bits11);
        if (slots->unk80.index == 1) {
            func_0018D670(entity, slots->unk80.index, previous, slots->unk80.unk04);
            func_0018D8B8(entity, slots->unk80.index, previous, slots->unk80.unk04);
        }
    }
    if (slots->unk88.unk01 != 0) {
        // match: the caller passes a float the callee does not take
        ((int (*)(void *, void *, int, float))func_0019A538)(entity, &slots->unk88, 1, 1.0f / slots->unk88.unk01);
    } else {
        previous = slots->unk88.unk04;
        ENTITY_FLAG_BITS(entity)->bit34 = func_00191600(entity, &slots->unk88, 1, ENTITY_FLAG_BITS(entity)->bits11);
        if (slots->unk88.index != 1) {
            func_0018D670(entity, slots->unk88.index, previous, slots->unk88.unk04);
            func_0018D8B8(entity, slots->unk88.index, previous, slots->unk88.unk04);
        }
    }
    limits[1] = entity->params->unk430 * 0.017453292f;
    limits[0] = -entity->params->unk430 * 0.017453292f;
    limits[3] = entity->params->unk432 * 0.017453292f;
    limits[2] = -entity->params->unk432 * 0.017453292f;
    if (ENTITY_FLAG_BITS(entity)->bit9) {
        func_00196120(entity, unkE0->unk20, ENTITY_FLAG_BITS(entity)->bit9, limits, 12.0f);
    } else {
        func_00196120(entity, unkE0->unk20, -1, NULL, 8.0f);
    }
}

static void func_0014EA70(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        func_00196870(entity, 0);
    }
    if (entity->unkC6 == 0) {
        func_0018E9C0(entity);
        if (entity->unk2C < (s32)(entity->params->unk0A * 0.75f)) {
            func_001958B8(entity, func_0014F3F8, unknown);
            return;
        }
        switch (entity->state) {
        case 2:
            func_0014EBA8(entity, unknown);
            break;
        case 0:
        case 1:
        default:
            func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, unknown);
            break;
        }
    }
    if (unkE0->unk68 > 0) {
        unkE0->unk68--;
    }
}

static void func_0014EBA8(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    ENTITY_FLAG_BITS(entity)->bits11 = 0;
    ENTITY_FLAG_BITS(entity)->bit9 = TRUE;
    if (func_001503A8(entity)) {
        func_001958B8(entity, func_0014F240, unknown);
    } else if (entity->unkA2 == 0 && func_001992F0(entity, 0)) {
        func_001958F8(entity, func_0014EFB0, unknown, 1, 0);
    } else {
        func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, unknown);
    }
}

static void func_0014EC80(Entity *entity) {
    entity->unkE0.unk1C = 0.0f;
    func_0019A448(entity, entity->unkE0.unk80, 0, 1, 0, entity->params->unkC4, 0);
    func_0019A448(entity, entity->unkE0.unk88, 1, 1, 0, entity->params->unkC4, 0);
}

static void func_0014ECE8(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityUnkE0Kind151D30 *slots = (EntityUnkE0Kind151D30 *)unkE0;
    f32 target[4];

    entity->unkC6 = 0x40;
    switch (entity->action_step) {
    case 0:
        func_0019A448(entity, &slots->unk80, 0, 2, 0, entity->params->unkC4, 0);
        func_0019A448(entity, &slots->unk88, 1, 2, 0, entity->params->unkC4, 0);
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
        entity->action_step++;
        break;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            func_0019A448(entity, &slots->unk80, 0, 0x10, 5, entity->params->unkC4, 1);
            func_0019A448(entity, &slots->unk88, 1, 0x10, 5, entity->params->unkC4, 1);
            ENTITY_FLAG_BITS(entity)->bits11 = 1;
            ENTITY_FLAG_BITS(entity)->bit14 = TRUE;
            ENTITY_FLAG_BITS(entity)->bit20 = TRUE;
            ENTITY_FLAG_BITS(entity)->bits2 = 1;
            ENTITY_FLAG_BITS(entity)->bit27 = TRUE;
            ENTITY_FLAG_BITS(entity)->bit26 = TRUE;
            ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
            entity->action_step++;
        }
        break;
    case 1:
    case 3:
        if (slots->unk80.unk01 == 0) {
            entity->action_step++;
        }
        break;
    case 4:
        if ((slots->unk88.unk04 - entity->params->unk80[slots->unk88.index]) >> 8 < 0x2B && slots->unk88.unk02 >= 0x2B) {
            func_00216778(target, D_00413F80.position);
            func_00195E48(entity, ((EntityParamsUnk3D8Full *)&entity->params->unk3D8[0])->unk10[0].unk00, target,
                          entity->model->unkD0);
        }
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            ENTITY_FLAG_BITS(entity)->bit14 = FALSE;
            ENTITY_FLAG_BITS(entity)->bits2 = 2;
            ENTITY_FLAG_BITS(entity)->bit4 = TRUE;
            entity->unkA0 = 20;
            func_0018DCD8(entity);
            entity->action_step++;
        }
        break;
    case 5:
        if (entity->unkA0 != 0) {
            entity->model->unk15C = (f32)entity->unkD6 * (f32)entity->unkA0 / 20.0f;
        } else {
            ENTITY_FLAG_BITS(entity)->bits2 = 3;
            ENTITY_FLAG_BITS(entity)->bit1 = FALSE;
        }
        break;
    }
}

static void func_0014EFB0(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityParamsUnk3D8Full *unk3D8 = (EntityParamsUnk3D8Full *)&entity->params->unk3D8[0];
    f32 target[4];
    EntityFrameTable5 launch_frames = D_0043E8E8;
    s32 elapsed;
    s32 i;

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        func_0019A448(entity, entity->unkE0.unk88, 1, 0x17, 0, entity->params->unkC4 >> 1, 1);
        entity->flags &= ~0x3800;
        entity->flags &= ~0x200;
        entity->action_step++;
    case 1:
        if (((EntityUnkE0Slot *)unkE0->unk88)->unk01 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        elapsed = (((EntityUnkE0Slot *)unkE0->unk88)->unk04 -
                   entity->params->unk80[((EntityUnkE0SlotBits *)unkE0->unk88)->index]) >> 8;
        for (i = 0; i < 5; i++) {
            if (entity->unk2A == 0 || unk3D8->unk0C != 0) {
                if (elapsed < launch_frames.frames[i] && ((EntityUnkE0Slot *)unkE0->unk88)->unk02 >= launch_frames.frames[i]) {
                    func_00216778(target, D_00413F80.position);
                    target[0] += rand() % 1201 - 600;
                    target[2] += rand() % 1201 - 600;
                    func_00195E48(entity, unk3D8->unk10[0].unk00, target, entity->model->unkD0);
                }
            }
        }
        if ((s32)(entity->flags >> 34) & 1) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            func_0019A290(entity, unk3D8->unk02);
            if ((s32)(entity->flags >> 5) & 1) {
                unkE0->unk00(entity, 1);
            } else {
                func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, 1);
            }
        }
        break;
    }
}

static void func_0014F240(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 0x1000;
    switch (entity->action_step) {
    case 0:
        func_0019A448(entity, entity->unkE0.unk88, 1, 0x11, 0, entity->params->unkC4, 1);
        entity->flags &= ~0x3800;
        entity->flags &= ~0x200;
        entity->action_step++;
    case 1:
        if (((EntityUnkE0Slot *)unkE0->unk88)->unk01 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (((EntityUnkE0Slot *)unkE0->unk88)->unk02 == entity->params->unk438) {
            func_0018D600((char **)entity, entity->params->unk436, entity->params->unk43E, (int)entity->model->unkD0);
        }
        if (entity->params->unk43C < ((EntityUnkE0Slot *)unkE0->unk88)->unk02) {
            func_0018D600((char **)entity, entity->params->unk43A, entity->params->unk43E, (int)entity->model->unkD0);
            func_00150478(entity);
            entity->action_step++;
        }
        break;
    case 3:
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if (ENTITY_FLAG_BITS(entity)->bit5) {
                entity->unkE0.unk00(entity, 1);
            } else {
                func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, 1);
            }
        }
        break;
    }
}

static void func_0014F3F8(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 0x2000;
    switch (entity->action_step) {
    case 0:
        func_0019A448(entity, entity->unkE0.unk80, 0, 2, 0, entity->params->unkC4, 0);
        func_0019A448(entity, entity->unkE0.unk88, 1, 2, 0, entity->params->unkC4, 0);
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
        entity->action_step++;
        break;
    case 1:
        if (((EntityUnkE0Slot *)unkE0->unk80)->unk01 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            unkE0->unk04 = func_00182A08;
            unkE0->unk08 = (void (*)(void))func_00182BD0;
            unkE0->unk18 = (void (*)(void))func_00183570;
            unkE0->unk14 = (void (*)(void))func_0014FA28;
            entity->unkE0.unk00 = func_0014F5A8;
            ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if (ENTITY_FLAG_BITS(entity)->bit5) {
                entity->unkE0.unk00(entity, 1);
            } else {
                func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, 1);
            }
        }
        break;
    }
}

static void func_0014F5A8(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        func_00196870(entity, 0);
    }
    if (func_00194F80(entity->model->position, entity->spawn_info) > 160.0f) {
        ENTITY_FLAG_BITS(entity)->bit25 = 0;
    }
    if (entity->unkC6 == 0) {
        func_00196CC0((char *)entity);
        func_0018E9C0(entity);
        if (ENTITY_FLAG_BITS(entity)->bit53) {
            func_00182DE0(entity, unknown);
        } else {
            switch (entity->state) {
            case 2:
                func_0014F6E8(entity, unknown);
                break;
            case 0:
            case 1:
            default:
                func_00182FD8(entity, unknown);
                break;
            }
        }
    }
    if (unkE0->unk68 > 0) {
        unkE0->unk68--;
    }
}

static void func_0014F6E8(Entity *entity, s32 unknown) {
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
    if (func_001503A8(entity)) {
        func_001958B8(entity, func_00150200, unknown);
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
                if (ENTITY_FLAG_BITS(entity)->bits58 == 0) {
                    func_001958B8(entity, func_0014FCC0, unknown);
                } else if (ENTITY_FLAG_BITS(entity)->bits58 == 1) {
                    func_001958B8(entity, func_0014FF68, unknown);
                } else {
                    func_001958B8(entity, func_00182240, unknown);
                }
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
            entity->unkA0 = rand() % 30 + 0x1E;
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

static void func_0014FA28(Entity *entity) {
    f32 target[4];

    entity->unkC6 = 0x40;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 0x10, 0, 5, 1);
        if (entity->params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->bits11 = (((EntityParamsWord *)entity->params)->unk00 & 0x6000000) == 0x2000000 ? 2 : 1;
        }
        if ((((EntityParamsWord *)entity->params)->unk00 & 0x6000000) == 0x4000000) {
            entity->unk30 = 20.0f;
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
        if ((entity->model->unkFC->unk84 - entity->params->unk80[entity->unkD1]) >> 8 < 0x2B &&
            entity->model->unkFC->unk82 >= 0x2B) {
            func_00216778(target, D_00413F80.position);
            func_00195E48(entity, ((EntityParamsUnk3D8Full *)&entity->params->unk3D8[0])->unk10[0].unk00, target,
                          entity->model->unkD0);
        }
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            ENTITY_FLAG_BITS(entity)->bit14 = FALSE;
            ENTITY_FLAG_BITS(entity)->bits2 = 2;
            ENTITY_FLAG_BITS(entity)->bit4 = TRUE;
            entity->unkA0 = 20;
            func_0018DCD8(entity);
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

static void func_0014FCC0(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityParamsUnk3D8Full *unk3D8 = (EntityParamsUnk3D8Full *)&entity->params->unk3D8[0];
    f32 target[4];
    EntityFrameTable5 launch_frames = D_0043E8E8;
    s32 elapsed;
    s32 i;

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 0x17, 0, entity->params->unkC4 >> 1, 1);
        ENTITY_FLAGS(entity) = (ENTITY_FLAGS(entity) & ~0x3800) | 0x800;
        ENTITY_FLAGS(entity) &= ~0x200;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        elapsed = (entity->model->unkFC->unk84 - entity->params->unk80[entity->unkD1]) >> 8;
        for (i = 0; i < 5; i++) {
            if (entity->unk2A == 0 || unk3D8->unk0C != 0) {
                if (elapsed < launch_frames.frames[i] && entity->model->unkFC->unk82 >= launch_frames.frames[i]) {
                    func_00216778(target, D_00413F80.position);
                    target[0] += rand() % 401 - 200;
                    target[2] += rand() % 401 - 200;
                    func_00195E48(entity, unk3D8->unk10[0].unk00, target, entity->model->unkD0);
                }
            }
        }
        if ((s32)(entity->flags >> 34) & 1) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            func_0019A290(entity, unk3D8->unk02);
            if ((s32)(entity->flags >> 5) & 1) {
                unkE0->unk00(entity, 1);
            } else {
                func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, 1);
            }
        }
        break;
    }
}

static void func_0014FF68(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityParamsUnk3D8Full *unk3D8 = (EntityParamsUnk3D8Full *)ENTITY_PARAMS_UNK3D8(entity->params, 1);
    f32 direction[4];
    EntityFrameTable launch_frames = D_0043E900;
    f32 *source;
    f32 delta_x;
    f32 delta_z;
    s32 elapsed;
    s32 i;

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 0x18, 0, entity->params->unkC4 >> 1, 1);
        ENTITY_FLAGS(entity) = (ENTITY_FLAGS(entity) & ~0x3800) | 0x800;
        ENTITY_FLAGS(entity) &= ~0x200;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        elapsed = (entity->model->unkFC->unk84 - entity->params->unk80[entity->unkD1]) >> 8;
        for (i = 0; i < 4; i++) {
            if (entity->unk2A == 0 || unk3D8->unk0C != 0) {
                if (elapsed < launch_frames.frames[i] && entity->model->unkFC->unk82 >= launch_frames.frames[i]) {
                    source = (f32 *)func_0010CAB8((char *)entity->model, entity->params->unk43E);
                    if (source != NULL) {
                        delta_x = source[0] - entity->model->position[0];
                        delta_z = source[2] - entity->model->position[2];
                        direction[0] = -0.17453292f;
                        direction[1] = func_00194FB8(-atan2f(delta_z, delta_x) + 1.5707964f);
                        direction[2] = direction[3] = 0.0f;
                        func_00195E48(entity, unk3D8->unk10[0].unk00, source, direction);
                    }
                }
            }
        }
        if ((s32)(entity->flags >> 34) & 1) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            func_0019A290(entity, unk3D8->unk02);
            if ((s32)(entity->flags >> 5) & 1) {
                unkE0->unk00(entity, 1);
            } else {
                func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, 1);
            }
        }
        break;
    }
}

static void func_00150200(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 0x1000;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 0x11, 0, entity->params->unkC4, 1);
        entity->flags = (entity->flags & ~0x3800) | 0x800;
        entity->flags &= ~0x200;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (((EntityUnkE0Slot *)unkE0->unk88)->unk02 == entity->params->unk438) {
            func_0018D600((char **)entity, entity->params->unk436, entity->params->unk43E, (int)entity->model->unkD0);
        }
        if (entity->params->unk43C < entity->model->unkFC->unk82) {
            func_0018D600((char **)entity, entity->params->unk43A, entity->params->unk43E, (int)entity->model->unkD0);
            func_00150478(entity);
            entity->action_step++;
        }
        break;
    case 3:
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

static s32 func_001503A8(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    s32 count = 0;
    s32 i;

    unkE0->unk6A = 0;
    for (i = 0; i < 6; i++) {
        Entity *other = Entity_GetBySpawnId(i + 10);
        if (other != NULL && (other->flags & 3) == 3 && ((s32)(other->flags >> 2) & 3) < 3) {
            unkE0->unk6A |= 1 << i;
            count++;
        }
    }
    unkE0->unk7A = count;
    if (unkE0->unk7A < 5) {
        if (unkE0->unk68 < 1) {
            return TRUE;
        }
    }
    return FALSE;
}

static void func_00150478(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    s32 remaining = 2;
    s32 chance = 200;
    s32 i;
    s32 slots_left = 6 - unkE0->unk7A;

    for (i = 0; i < 6; i++) {
        if (!((unkE0->unk6A >> i) & 1)) {
            if (rand() % 100 < chance / slots_left) {
                Entity *other = Entity_GetBySpawnId(i + 10);
                if (other != NULL && ((s32)other->flags & 1)) {
                    chance -= 100;
                    func_0018DC40((char *)other);
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
    unkE0->unk68 = entity->params->unk434;
}
