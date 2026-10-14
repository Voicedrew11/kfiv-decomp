/* The behaviour of entity type 68 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers that function installs. */
#include "kf_entity.h"
#include "common.h"
#include "kf_shot.h"

#define ENTITY_WORK_AREA_SIZE 0xA0
void func_001958A8(char *, s32);
int func_001958B8(Entity *, void (*)(Entity *), int);
void func_00191478(EntityModel *);
int func_00193378(EntityModel *, s32, s32);
void func_001914B0(EntityModel *, s32, s32, s32);
void func_00216778(void *, void *);
extern s32 D_003E6E40[][2];
extern u32 D_00446ED4;
int rand(void);
void func_00196870(void *, int);
void func_00196CC0(char *);
void func_0018E9C0(void *);
int func_0019A448(void *, void *, int, int, int, int, int);
int func_00182DE0(void *, int);
int func_0018EAC0(void *, void *);
extern PlayerData D_00413F80;
s32 func_0018FD50(Entity *, s32);
void func_00190B88(Entity *, s32);
int func_0018F030(char *);
void func_0018B778(float, void *, void *, void *);
extern f32 D_00431AF0[16];
float func_00195388(float *, float, float);
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0RotMatrixZ(float *, float *, float);
void func_00216BC0(void *, void *, void *);
float func_001954A8(float *, float *);
float func_001957B8(float, float);
void func_0018D600(char **, int, u32, int);
void func_00195898(char *, u8);
s32 func_00199EB8(Entity *, f32 *, f32);
void func_0019A290(Entity *entity, s32 base);
float sqrtf(float);
float atan2f(float, float);
double atan2(double, double);
char *func_0010CAB8(char *, u32);
float func_00194FB8(float);
float func_00194F80(void *, void *);
float func_00195478(float *, char *);
f32 func_00195018(f32, f32, f32);
int func_001B4F30(u16, void *, void *, void *, void *);
void func_00190A30(char *);
int func_0019A310(char *entity, void (*callback)(void), int run);
void func_0018D8B8(Entity *, s32, s32, s32);
void func_0018DCD8(Entity *);
int func_0019A538(void *, void *, int);
s32 func_00191600(Entity *, EntityUnkE0SlotBits *, s32, s32);
void func_0018D670(Entity *, s32, s32, s32);
int func_001960C0(void *, int, int, int, int, float);
int func_00196120(Entity *, void *, s32, void *, f32);

static void func_0017DCD0(Entity *entity, s32 unknown);
static void func_0017DF10(Entity *entity, s32 unknown);
static void func_0017E220(Entity *entity, s32 unknown);
static void func_0017E2E0(Entity *entity, s32 unknown);
static void func_0017E560(Entity *entity);
static void func_0017E5D0(Entity *entity);
static s32 func_0017EA98(Entity *entity, s32 mark_far, f32 range);
static void func_0017EB58(Entity *entity);
static void func_0017EEB0(Entity *entity);
static s32 func_0017F0F0(Entity *entity, EntityParamsUnk3D8 *unk3D8, s32 frame_from, s32 frame_to, f32 *target, f32 pitch_rate, f32 yaw_rate);
static void func_0017F378(Entity *entity);
static void func_0017F408(Entity *entity);
static void func_0017F5A8(Entity *entity);
static void func_0017F790(Entity *entity);
static void func_0017F998(Entity *entity);

/* Set-up of a type 68 entity: clears its work area and installs the type's handlers. */
void EntityType68_Init(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityUnkE0Kind151D30 *work = (EntityUnkE0Kind151D30 *)&entity->unkE0;
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
    unkE0->unk00 = func_0017DCD0;
    unkE0->unk04 = (void (*)(void))func_0017F998;
    unkE0->unk08 = (void (*)(void))func_0017F378;
    unkE0->unk18 = (void (*)(void))func_0017E560;
    unkE0->unk14 = (void (*)(void))func_0017F790;
    unkE0->unk10 = (void (*)(void))func_0017EB58;
    for (i = 4 - 1; i != -1; i--) {
        entity->unkBC[i] = 0x80;
    }
    for (i = 6 - 1; i != -1; i--) {
        entity->unkC0[i] = 0x80;
    }
    ENTITY_UNK5C(entity)->as_s32 = 0;
    ENTITY_FLAG_BITS(entity)->bit28 = TRUE;
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
    func_001914B0(entity->model, 0x10, 0x19, 1);
    work->unk80.bit5 = TRUE;
    work->unk80.bit6 = TRUE;
    work->unk88.bit6 = TRUE;
    func_00193378(entity->model, 0, 0);
}

static void func_0017DCD0(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    s32 side;

    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        func_00196870(entity, 1);
    }
    if (D_00446ED4 % 60U == entity->unkD7 % 10U) {
        unkE0->unk54 = rand() % 10;
    }
    if (entity->unkC6 & 0x40) {
        ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
    } else {
        ENTITY_FLAG_BITS(entity)->bit28 = TRUE;
    }
    if (!(entity->unkC6 & 0x44)) {
        func_0019A448(entity, &((EntityUnkE0Kind151D30 *)unkE0)->unk88, 1, 3, 5, entity->params->unkC4, 0);
    }
    if (entity->unkC6 == 0) {
        func_00196CC0((char *)entity);
        func_0018E9C0(entity);
        if (ENTITY_FLAG_BITS(entity)->bit53) {
            func_00182DE0(entity, unknown);
        } else {
            switch (entity->state) {
            case 0:
                func_0017E2E0(entity, unknown);
                break;
            case 1:
                func_0017E220(entity, unknown);
                break;
            case 2:
                func_0017DF10(entity, unknown);
                break;
            default:
                func_0017E2E0(entity, unknown);
                break;
            }
        }
    }
    if (entity->unkC6 & 0x40) {
        unkE0->unk44 = 0.0f;
        unkE0->unk48 = 0.0f;
    } else {
        side = func_0018EAC0(entity, direction);
        if (side > 0) {
            entity->unkC8 = 0x201;
        } else if (side < 0) {
            entity->unkC8 = 0x101;
        } else {
            // match: called like func_00190F90, with a speed and a mode the callee ignores
            entity->unkC8 = func_0017EA98(entity, 0, 0.0f);
        }
        func_0017E5D0(entity);
    }
}

static void func_0017DF10(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    s32 next_action = 0; // match: the unused initial value changes the instruction count that sizes a compiler hash table, and with it two registers
    s32 side;

    ENTITY_FLAG_BITS(entity)->bits11 = 0;
    if (entity->unkA0 == 0) {
        if (rand() % 100 < entity->params->unk438) {
            if (!(rand() & 1)) {
                entity->target = &D_00413F80;
                entity->unkA0 = rand() % 30 + 30;
                unkE0->unk78 = 0;
                entity->unkCD = 1;
                unkE0->unk79 = 1;
            } else {
                entity->target = &D_00413F80;
                entity->unkA0 = rand() % 30 + 30;
                unkE0->unk78 = 0;
                entity->unkCD = 1;
                unkE0->unk79 = -1;
            }
        } else if (entity->unkA0 == 0) {
            unkE0->unk79 = 0;
        }
    }
    next_action = func_0018FD50(entity, 0);
    if (D_00413F80.position[1] - entity->model->position[1] < entity->params->unk436) {
        entity->target = &D_00413F80;
        entity->unkA0 = rand() % 30 + 30;
        unkE0->unk78 = 0;
        next_action = 1;
        entity->unkCD = next_action;
        unkE0->unk79 = 2;
    }
    if (D_00413F80.position[1] - entity->model->position[1] > (s16)entity->params->unk434) { // match: read signed here
        entity->target = &D_00413F80;
        entity->unkA0 = rand() % 30 + 30;
        unkE0->unk78 = 0;
        next_action = 1;
        entity->unkCD = next_action;
        unkE0->unk79 = -2;
    }
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
            if (func_0018F030((char *)entity)) {
                func_001958B8(entity, func_0017EEB0, unknown);
            }
            return;
        case 1:
            entity->target = &D_00413F80;
            entity->unkA0 = rand() % 30 + 60;
            unkE0->unk78 = 0;
            break;
        default:
            entity->target = &D_00413F80;
            entity->unkA0 = rand() % 30 + 60;
            unkE0->unk78 = 0;
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
                // match: called like func_00190F90, with a speed and a mode the callee ignores
                entity->unkC8 = func_0017EA98(entity, 0, 0.0f);
            }
        }
    }
    func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, unknown);
}

static void func_0017E220(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[2];
    s32 turn;

    ENTITY_FLAG_BITS(entity)->bits11 = 0;
    ENTITY_FLAG_BITS(entity)->bit10 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
    if (entity->unkA0 == 0) {
        entity->target = &D_00413F80;
    }
    turn = func_0018EAC0(entity, direction);
    if (turn > 0) {
        entity->unkC8 = 0x201;
    } else if (turn < 0) {
        entity->unkC8 = 0x101;
    } else {
        entity->unkC8 = func_0017EA98(entity, 0, 50.0f);
    }
    func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, unknown);
}

static void func_0017E2E0(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    s32 side;

    ENTITY_FLAG_BITS(entity)->bits11 = 0;
    ENTITY_FLAG_BITS(entity)->bit9 = TRUE;
    if (entity->unkA0 == 0) {
        func_0018B778((f32)entity->spawn_info->unk4C, unkE0->unk30, entity->spawn_info, entity->model->position);
        entity->target = unkE0->unk30;
        entity->unkA0 = rand() % 60 + 60;
        if (D_00413F80.position[1] - entity->model->position[1] < entity->params->unk436) {
            entity->target = &D_00413F80;
            entity->unkA0 = rand() % 30 + 30;
            unkE0->unk78 = 0;
            entity->unkCD = 1;
            unkE0->unk79 = 2;
        }
        if (D_00413F80.position[1] - entity->model->position[1] > (s16)entity->params->unk434) { // match: read signed here
            entity->target = &D_00413F80;
            entity->unkA0 = rand() % 30 + 30;
            unkE0->unk78 = 0;
            entity->unkCD = 1;
            unkE0->unk79 = -2;
        }
        if (rand() % 100 < entity->params->unk438) {
            if (!(rand() & 1)) {
                entity->target = &D_00413F80;
                entity->unkA0 = rand() % 30 + 30;
                unkE0->unk78 = 0;
                entity->unkCD = 1;
                unkE0->unk79 = 1;
            } else {
                entity->target = &D_00413F80;
                entity->unkA0 = rand() % 30 + 30;
                unkE0->unk78 = 0;
                entity->unkCD = 1;
                unkE0->unk79 = -1;
            }
        } else {
            unkE0->unk79 = 0;
        }
    }
    side = func_0018EAC0(entity, direction);
    if (side > 0) {
        entity->unkC8 = 0x201;
    } else if (side < 0) {
        entity->unkC8 = 0x101;
    } else {
        // match: called like func_00190F90, with a speed and a mode the callee ignores
        entity->unkC8 = func_0017EA98(entity, 0, 0.0f);
    }
    func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, unknown);
}

static void func_0017E560(Entity *entity) {
    entity->flags &= ~0x3800ULL;
    func_0019A448(entity, entity->unkE0.unk80, 0, 3, 5, ENTITY_CONTROLLER(entity)->unkC4, 0);
    func_0019A448(entity, entity->unkE0.unk88, 1, 3, 5, ENTITY_CONTROLLER(entity)->unkC4, 0);
}

/* moves a flying entity: turn, roll into the turn, pitch towards the level unk79 asks for, then speed */
static void func_0017E5D0(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 roll;
    f32 step;

    ENTITY_FLAG_BITS(entity)->bits11 = 0;
    if (entity->unkC8 & 0x100) {
        unkE0->unk1C -= entity->params->unk58;
        if (unkE0->unk1C < -entity->params->turn_limit) {
            unkE0->unk1C = -entity->params->turn_limit;
        }
        roll = (unkE0->unk54 + 50.0f) * 0.017453292f;
    } else if (entity->unkC8 & 0x200) {
        unkE0->unk1C += entity->params->unk58;
        if (entity->params->turn_limit < unkE0->unk1C) {
            unkE0->unk1C = entity->params->turn_limit;
        }
        roll = (-50.0f - unkE0->unk54) * 0.017453292f;
    } else {
        if (entity->params->unk58 < unkE0->unk1C) {
            unkE0->unk1C -= entity->params->unk58;
        } else if (unkE0->unk1C < -entity->params->unk58) {
            unkE0->unk1C += entity->params->unk58;
        } else {
            unkE0->unk1C = 0.0f;
        }
        roll = 0.0f;
    }
    entity->unk94 += unkE0->unk1C;
    func_00195388(&entity->model->unkD0[2], roll, 60.0f);
    if (unkE0->unk79 > 0) {
        if (ABS((f32)unkE0->unk79) * -0.17453292f < entity->model->unkD0[0]) {
            unkE0->unk4C -= 0.006981317f;
            if (unkE0->unk4C < -0.017453292f) {
                unkE0->unk4C = -0.017453292f;
            }
        } else {
            unkE0->unk4C = 0.0f;
        }
    } else if (unkE0->unk79 < 0) {
        if (entity->model->unkD0[0] < ABS((f32)unkE0->unk79) * 0.17453292f) {
            unkE0->unk4C += 0.006981317f;
            if (0.017453292f < unkE0->unk4C) {
                unkE0->unk4C = 0.017453292f;
            }
        } else {
            unkE0->unk4C = 0.0f;
        }
    } else if (unkE0->unk79 == 0) {
        if (0.017453292f < entity->model->unkD0[0]) {
            unkE0->unk4C = -0.017453292f;
        } else if (entity->model->unkD0[0] < -0.017453292f) {
            unkE0->unk4C = 0.017453292f;
        } else {
            unkE0->unk4C = -entity->model->unkD0[0];
        }
    }
    entity->unk90 += unkE0->unk4C;
    ENTITY_SCRATCH_VECTOR[0] = ENTITY_SCRATCH_VECTOR[1] = ENTITY_SCRATCH_VECTOR[2] = ENTITY_SCRATCH_VECTOR[3] = 0.0f;
    step = entity->params->unk50;
    if (entity->unkC8 & 1) {
        unkE0->unk48 -= step;
        if (unkE0->unk48 < -entity->params->unk54) {
            unkE0->unk48 = -entity->params->unk54;
        }
    } else if (entity->unkC8 & 2) {
        unkE0->unk48 += step;
        if (entity->params->unk54 < unkE0->unk48) {
            unkE0->unk48 = entity->params->unk54;
        }
    } else if (step < unkE0->unk48) {
        unkE0->unk48 -= step;
    } else if (unkE0->unk48 < -step) {
        unkE0->unk48 += step;
    } else {
        unkE0->unk48 = 0.0f;
    }
    if (entity->unkC8 & 4) {
        unkE0->unk44 += entity->params->unk50;
        if (entity->params->unk54 < unkE0->unk44) {
            unkE0->unk44 = entity->params->unk54;
        }
    } else if (entity->unkC8 & 8) {
        unkE0->unk44 -= entity->params->unk50;
        if (unkE0->unk44 < -entity->params->unk54) {
            unkE0->unk44 = -entity->params->unk54;
        }
    } else if (entity->params->unk50 < unkE0->unk44) {
        unkE0->unk44 -= entity->params->unk50;
    } else if (unkE0->unk44 < -entity->params->unk50) {
        unkE0->unk44 += entity->params->unk50;
    } else {
        unkE0->unk44 = 0.0f;
    }
    ENTITY_SCRATCH_VECTOR[0] = unkE0->unk44;
    ENTITY_SCRATCH_VECTOR[1] = 0.0f;
    ENTITY_SCRATCH_VECTOR[2] = unkE0->unk48;
    sceVu0RotMatrixZ(ENTITY_SCRATCH_MATRIX, D_00431AF0, entity->model->unkD0[2]);
    sceVu0RotMatrixX(ENTITY_SCRATCH_MATRIX, ENTITY_SCRATCH_MATRIX, entity->model->unkD0[0]);
    sceVu0RotMatrixY(ENTITY_SCRATCH_MATRIX, ENTITY_SCRATCH_MATRIX, entity->model->unkD0[1]);
    func_00216BC0(ENTITY_SCRATCH_VECTOR, ENTITY_SCRATCH_MATRIX, ENTITY_SCRATCH_VECTOR);
    // match: a test whose result is never used (func_0016C888 doubles the speed on this bit); it leaves
    // no code, but the scratchpad reads below only get their own address loads when it is here
    if (entity->unkC8 & 0x40) {
        step += step;
    }
    entity->unk84 += ENTITY_SCRATCH_VECTOR[0];
    entity->unk88 += ENTITY_SCRATCH_VECTOR[1];
    entity->unk8C += ENTITY_SCRATCH_VECTOR[2];
}

/* Called like func_00190F90; the last two parameters are not used. */
static s32 func_0017EA98(Entity *entity, s32 mark_far, f32 range) {
    s32 result;
    f32 angle;
    f32 limit;

    if (entity->target == NULL) {
        return 0;
    }
    result = 1;
    angle = func_001954A8(entity->model->position, entity->target);
    angle = func_001957B8(entity->model->unkD0[1], angle);
    limit = entity->params->turn_limit;
    if ((0.0f <= angle) ? (limit < angle) : (limit < -angle)) {
        if (0.0f <= angle) {
            result = 0x201;
        } else {
            result = 0x101;
        }
    }
    return result;
}

static void func_0017EB58(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityUnkE0Kind151D30 *slots = (EntityUnkE0Kind151D30 *)unkE0;

    entity->unkC6 |= 0x20;
    switch (entity->action_step) {
    case 0:
        func_0019A448(entity, &slots->unk80, 0, 1, 0, 0, 1);
        func_0019A448(entity, &slots->unk88, 1, 1, 0, 0, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
        unkE0->unk1C = 0.0f;
        ENTITY_FLAG_BITS(entity)->bit21 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit22 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit23 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit24 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit10 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
        entity->action_step++;
        break;
    case 1:
        if (entity->distance_to_player < entity->spawn_info->unk54 || ENTITY_FLAG_BITS(entity)->bit43) {
            unkE0->unk50 = entity->params->unk43A;
            func_0018D600((char **)entity, 0x166, 0xFF, (int)entity->model->unkD0);
            entity->action_step++;
        }
        break;
    case 2:
        entity->model->position[1] -= unkE0->unk50;
        unkE0->unk50 -= 3.0f;
        if (unkE0->unk50 <= 0.0f) {
            func_0019A448(entity, &slots->unk80, 0, 2, 0, entity->params->unkC4, 1);
            func_0019A448(entity, &slots->unk88, 1, 2, 0, entity->params->unkC4, 1);
            entity->action_step++;
        }
        break;
    case 3:
        entity->model->position[1] -= unkE0->unk50;
        unkE0->unk50 -= 1.0f;
        if (slots->unk80.unk01 == 0) {
            entity->action_step++;
        }
        break;
    case 4:
        if (slots->unk80.unk02 < 0x32) {
            entity->model->position[1] -= unkE0->unk50;
            unkE0->unk50 -= 0.1f;
        }
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            ENTITY_FLAG_BITS(entity)->bits36 = 0;
            func_00195898((char *)entity, 2);
            ENTITY_FLAG_BITS(entity)->bit21 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit22 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit23 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit24 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if (ENTITY_FLAG_BITS(entity)->bit5) {
                unkE0->unk00(entity, 1);
            } else {
                func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, 1);
            }
        }
        entity->unkC8 = 1;
        func_0017E5D0(entity);
        break;
    }
}

static void func_0017EEB0(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityUnkE0Kind151D30 *slots = (EntityUnkE0Kind151D30 *)unkE0;
    EntityParamsUnk3D8 *unk3D8 = ENTITY_PARAMS_UNK3D8(entity->params, ENTITY_FLAG_BITS(entity)->bits58);
    s32 frame;
    s32 launch_frame;
    s32 current_frame;
    f32 unused_vector[4]; // match: the frame has 16 unused bytes, and they keep the tail calls from becoming jumps

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        func_0019A448(entity, &((EntityUnkE0Kind151D30 *)&entity->unkE0)->unk80, 0, ENTITY_FLAG_BITS(entity)->bits58 + 0x17, 0, entity->params->unkC4 >> 1, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
        entity->action_step++;
    case 1:
        if (slots->unk80.unk01 == 0) {
            entity->action_step++;
        }
        if (unk3D8->unk01 != 0) {
            func_00199EB8(entity, &unkE0->unk1C, entity->unk68);
        }
        break;
    case 2:
        if (slots->unk80.unk02 <= unk3D8->unk01) {
            func_00199EB8(entity, &entity->unkE0.unk1C, entity->unk68);
        } else {
            unkE0->unk1C = 0.0f;
        }
        frame = (slots->unk80.unk04 - entity->params->unk80[entity->unkD1]) >> 8;
        current_frame = slots->unk80.unk02;
        launch_frame = ((EntityParamsUnk3D8Full *)unk3D8)->unk10[0].unk02;
        if (launch_frame != 0 && frame < launch_frame && launch_frame <= current_frame) {
            func_0017F0F0(entity, unk3D8, frame, current_frame, D_00413F80.position, 0.7853982f, 0.7853982f);
        }
        if (slots->unk80.unk02 > 0 && unk3D8->unk03 >= slots->unk80.unk02) {
            ENTITY_FLAG_BITS(entity)->bit19 = 1;
        } else {
            ENTITY_FLAG_BITS(entity)->bit19 = 0;
        }
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            func_0019A290(entity, unk3D8->unk02);
            if (ENTITY_FLAG_BITS(entity)->bit5) {
                unkE0->unk00(entity, 1);
            } else {
                func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, 1);
            }
        }
        break;
    }
}

/* launches both shots of a record at once, whatever the animation frame */
static s32 func_0017F0F0(Entity *entity, EntityParamsUnk3D8 *unk3D8, s32 frame_from, s32 frame_to, f32 *target, f32 pitch_rate, f32 yaw_rate) {
    EntityParamsUnk3D8Full *record = (EntityParamsUnk3D8Full *)unk3D8;
    EntityUnk1B1390 *shot;
    f32 angles[4];
    f32 *from;
    f32 *tip;
    f32 delta_x;
    f32 delta_y;
    f32 delta_z;
    f32 pitch;
    f32 yaw;
    s32 i;

    if (entity->unk2A != 0 && record->unk0C == 0) {
        return 0;
    }
    for (i = 0; i < 2; i++) {
        shot = Shot_Alloc();
        shot->unk22 = shot->unk20 = 0;
        shot->unk00 = 1;
        shot->unk28 = entity->params->unk238;
        from = (f32 *)func_0010CAB8((char *)entity->model, record->unk10[i].unk01);
        tip = (f32 *)func_0010CAB8((char *)entity->model, record->unk10[i].unk01 + 1);
        if (from != NULL && tip != NULL) {
            if (record->unk10[i].unk01 != 0xFF) {
                delta_x = tip[0] - from[0];
                delta_y = tip[1] - from[1];
                delta_z = tip[2] - from[2];
                angles[0] = -atan2f(delta_y, sqrtf(delta_x * delta_x + delta_z * delta_z));
                angles[1] = func_00194FB8(-atan2f(delta_z, delta_x) + 1.5707964f);
                angles[2] = angles[3] = 0.0f;
            } else {
                func_00216778(angles, entity->model->unkD0);
                angles[1] = func_00194FB8(angles[1] + 3.1415927f);
            }
            if (target != NULL) {
                pitch = func_00194FB8(-atan2(target[1] - from[1], func_00194F80(from, target)));
                yaw = func_00195478(from, (char *)target);
                angles[0] = func_00195018(angles[0], pitch, pitch_rate);
                angles[1] = func_00195018(angles[1], yaw, yaw_rate);
            }
            func_001B4F30(record->unk10[i].unk00, shot, from, angles, entity);
        }
    }
    return 0;
}

static void func_0017F378(Entity *entity) {
    func_00190A30((char *)entity);
    entity->target = NULL;
    ENTITY_FLAG_BITS(entity)->bit10 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
    if (ENTITY_FLAG_BITS(entity)->bits36 == 1) {
        func_0019A310((char *)entity, (void (*)(void))func_0017F408, 0);
    } else if (ENTITY_FLAG_BITS(entity)->bits36 == 2) {
        func_0019A310((char *)entity, (void (*)(void))func_0017F5A8, 0);
    }
    func_00195898((char *)entity, 2);
}

static void func_0017F408(Entity *entity) {
    EntityUnkE0Kind1740A8 *unkE0 = (EntityUnkE0Kind1740A8 *)&entity->unkE0;

    entity->unkC6 = 2;
    switch (entity->unkCB) {
    case 0:
        func_0018D8B8(entity, 0xE, 0, 0);
        func_0019A448(entity, &((EntityUnkE0Kind1740A8 *)&entity->unkE0)->unk80, 0, 0xE, 0, 5, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
        unkE0->unk1C = 0;
        ENTITY_FLAG_BITS(entity)->bit14 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit19 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit35 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit10 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
        entity->unkCB++;
        break;
    case 1:
        if (unkE0->unk80.unk01 == 0) {
            entity->unkCB++;
        }
        break;
    case 2:
        func_0019A290(entity, entity->params->unkC8);
        func_0019A310((char *)entity, NULL, 0);
        entity->unkC6 = 0;
        entity->unkC8 = 0;
        entity->unkA0 = 0;
        if (ENTITY_FLAG_BITS(entity)->bit5) {
            entity->unkE0.unk00(entity, 1);
        } else {
            func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, 1);
        }
        break;
    }
}

static void func_0017F5A8(Entity *entity) {
    EntityUnkE0Kind1740A8 *unkE0 = (EntityUnkE0Kind1740A8 *)&entity->unkE0;

    entity->unkC6 = 4;
    switch (entity->unkCB) {
    case 0:
        func_0019A448(entity, &((EntityUnkE0Kind1740A8 *)&entity->unkE0)->unk80, 0, 0xF, 0, 5, 1);
        func_0019A448(entity, &((EntityUnkE0Kind1740A8 *)&entity->unkE0)->unk88, 1, 0xF, 0, 5, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
        unkE0->unk1C = 0;
        ENTITY_FLAG_BITS(entity)->bit14 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit19 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit35 = FALSE;
        if (!ENTITY_PARAMS_BITS(entity)->bit25 && !ENTITY_PARAMS_BITS(entity)->bit26) {
            ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
        }
        ENTITY_FLAG_BITS(entity)->bit10 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
        entity->unkCB++;
        break;
    case 1:
        if (unkE0->unk80.unk01 == 0) {
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
            if (ENTITY_FLAG_BITS(entity)->bit5) {
                entity->unkE0.unk00(entity, 1);
            } else {
                func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, 1);
            }
        }
        break;
    }
}

static void func_0017F790(Entity *entity) {
    EntityUnkE0Kind1740A8 *unkE0 = (EntityUnkE0Kind1740A8 *)&entity->unkE0;

    entity->unkC6 = 0x40;
    switch (entity->action_step) {
    case 0:
        func_0019A448(entity, &((EntityUnkE0Kind1740A8 *)&entity->unkE0)->unk80, 0, 0x10, 0, 5, 1);
        func_0019A448(entity, &((EntityUnkE0Kind1740A8 *)&entity->unkE0)->unk88, 1, 0x10, 0, 5, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
        if (!ENTITY_PARAMS_BITS(entity)->bit25 && ENTITY_PARAMS_BITS(entity)->bit26) {
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
        if (unkE0->unk80.unk01 == 0) {
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

static void func_0017F998(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityUnkE0Kind151D30 *slots = (EntityUnkE0Kind151D30 *)unkE0;
    f32 limits[4];
    f32 unused_vector[4]; // match: the frame has 16 unused bytes
    s32 previous;
    f32 rate;

    ENTITY_FLAG_BITS(entity)->bits11 = 0;
    if (!ENTITY_FLAG_BITS(entity)->bit14) {
        return;
    }
    if (slots->unk80.unk01 != 0) {
        if (entity->unkC6 & 0x12) {
            rate = unkE0->unk40;
        } else {
            rate = 1.0f / slots->unk80.unk01;
        }
        // match: the caller passes a float the callee does not take
        ENTITY_FLAG_BITS(entity)->bit34 = ((int (*)(void *, void *, int, float))func_0019A538)(entity, &slots->unk80, 0, rate);
    } else {
        previous = slots->unk80.unk04;
        ENTITY_FLAG_BITS(entity)->bit34 = func_00191600(entity, &((EntityUnkE0Kind151D30 *)&entity->unkE0)->unk80, 0, ENTITY_FLAG_BITS(entity)->bits11);
        if (slots->unk80.index != 3) {
            func_0018D670(entity, slots->unk80.index, previous, slots->unk80.unk04);
            func_0018D8B8(entity, slots->unk80.index, previous, slots->unk80.unk04);
        }
    }
    if (slots->unk88.unk01 != 0) {
        if (entity->unkC6 & 0x12) {
            rate = unkE0->unk40;
        } else {
            rate = 1.0f / slots->unk88.unk01;
        }
        // match: the caller passes a float the callee does not take
        ((int (*)(void *, void *, int, float))func_0019A538)(entity, &slots->unk88, 1, rate);
    } else {
        previous = slots->unk88.unk04;
        func_00191600(entity, &slots->unk88, 1, ENTITY_FLAG_BITS(entity)->bits11);
        if (slots->unk88.index == 3) {
            func_0018D670(entity, slots->unk88.index, previous, slots->unk88.unk04);
            func_0018D8B8(entity, slots->unk88.index, previous, slots->unk88.unk04);
        }
    }
    limits[1] = entity->params->unk430 * 0.017453292f;
    limits[0] = -entity->params->unk430 * 0.017453292f;
    limits[3] = entity->params->unk432 * 0.017453292f;
    limits[2] = -entity->params->unk432 * 0.017453292f;
    if (ENTITY_FLAG_BITS(entity)->bit9 && entity->target != NULL) {
        func_001960C0(entity, (int)unkE0->unk20, ENTITY_FLAG_BITS(entity)->bit9, (int)entity->target, (int)limits, 12.0f);
    } else {
        func_00196120(entity, unkE0->unk20, ENTITY_FLAG_BITS(entity)->bit9, NULL, 8.0f);
    }
}
