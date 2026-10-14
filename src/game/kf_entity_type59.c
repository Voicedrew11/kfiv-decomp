/* The behaviour of entity type 59 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers that function installs. */
#include "kf_entity.h"
#include "common.h"
#include "kf_shot.h"

#define ENTITY_WORK_AREA_SIZE 0xA0
extern s32 D_003E6E40[][2];
void func_001958A8(char *, s32);
int func_001958B8(Entity *, void (*)(Entity *), int);
void func_00191478(EntityModel *);
void func_001914B0(EntityModel *, s32, s32, s32);
int func_00193378(EntityModel *, s32, s32);
void func_00216778(void *, void *);
void func_00181038(Entity *);
extern u32 D_00446ED4;
void func_00196870(void *, int);
int func_0019A448(void *, void *, int, int, int, int, int);
void func_00196CC0(void *);
void func_0018E9C0(void *);
int func_00182DE0(void *, int);
void func_00183150(Entity *, s32);
extern PlayerData D_00413F80;
int rand(void);
s32 func_0018FD50(Entity *, s32);
f32 func_0018A9F0(void *, void *);
void func_00190B88(Entity *, s32);
int func_0018F030(char *);
int func_0018EAC0(void *, void *);
void func_0018B778(float, void *, void *, void *);
extern f32 D_00431AF0[16];
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0RotMatrixZ(float *, float *, float);
void func_00216BC0(void *, void *, void *);
void Vector_Scale(float *, float *, float);
float func_001954A8(float *, float *);
float func_001957B8(float, float);
float sinf(float);
float cosf(float);
int func_00192C98(void *, int, int, u32, int);
void func_00195898(char *, u8);
float func_00194FB8(float);
s32 func_00199EB8(Entity *, f32 *, f32);
int func_00195AC0(Entity *, void *, s32, s32, s32);
void func_0019A290(Entity *entity, s32 base);
double atan2(double, double);
char *func_0010CAB8(char *, u32);
float func_00194F80(void *, void *);
float sqrtf(float);
float atan2f(float, float);
float func_00195478(float *, char *);
f32 func_00195018(f32, f32, f32);
int func_001B4F30(u16, void *, void *, void *, void *);
void func_00190A30(char *);
int func_0019A310(char *, void (*)(void), int); // match: an int result keeps $2 busy after the call
void func_0018D8B8(Entity *, s32, s32, s32);
void func_0018DCD8(Entity *);
int func_0019A538(void *, void *, int);
s32 func_00191600(Entity *, EntityUnkE0SlotBits *, s32, s32);
void func_0018D670(Entity *, s32, s32, s32);
int func_001960C0(void *, int, int, int, int, float);
int func_00196120(Entity *, void *, s32, void *, f32);

static void func_00171C48(Entity *entity, s32 unknown);
static void func_00171E30(Entity *entity, s32 unknown);
static void func_00172320(Entity *entity, s32 unknown);
static void func_001726C8(Entity *entity);
static void func_00172738(Entity *entity);
static int func_00172BB8(Entity *entity);
static void func_00172C78(Entity *entity);
static void func_00172F00(Entity *entity);
static void func_00173198(Entity *entity);
static void func_001735C8(Entity *entity);
static s32 func_00173AB0(Entity *entity, EntityParamsUnk3D8 *unk3D8, s32 frame_from, s32 frame_to, f32 *target, f32 pitch_rate, f32 yaw_rate);
static s32 func_00173D38(Entity *entity, EntityParamsUnk3D8Full *record, s32 frame_from, s32 frame_to, f32 *angles);
static void func_00173E08(Entity *entity);
static void func_00173E98(Entity *entity);
static void func_001740A8(Entity *entity);
static void func_001742E0(Entity *entity);
static void func_00174530(Entity *entity);

/* Set-up of a type 59 entity: clears its work area and installs the type's handlers. */
void EntityType59_Init(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityUnkE0Kind151D30 *slots = (EntityUnkE0Kind151D30 *)&entity->unkE0;
    u8 *unkBC = entity->unkBC;
    u8 *unkC0 = entity->unkC0;
    char *unk138 = entity->model->unk138;
    s32 i;

    // match: the three fills name their target inside the loop; unkBC and unkC0 above are
    // otherwise unused but retail computes them up front and copies them before each loop
    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        u8 *work_area = (u8 *)&entity->unkE0;

        work_area[i] = 0;
    }
    func_001958A8((char *)entity, D_003E6E40[entity->params->unk02][1]);
    unkE0->unk00 = func_00171C48;
    unkE0->unk04 = (void (*)(void))func_00174530;
    unkE0->unk08 = (void (*)(void))func_00173E08;
    unkE0->unk18 = (void (*)(void))func_001726C8;
    unkE0->unk14 = (void (*)(void))func_001742E0;
    if (entity->spawn_info->unk40 == 1) {
        unkE0->unk10 = (void (*)(void))func_00172C78;
    } else {
        unkE0->unk10 = (void (*)(void))func_00181038;
    }
    for (i = 3; i != -1; i--) {
        entity->unkBC[i] = 0x80;
    }
    for (i = 5; i != -1; i--) {
        entity->unkC0[i] = 0x80;
    }
    entity->flags |= 0x10000000;
    entity->target = NULL;
    if (ENTITY_FLAG_WORD(entity->spawn_info->unk38) & (0x8000L << 43)) {
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
    func_001914B0(entity->model, 6, 0xB, 1);
    slots->unk80.bit5 = TRUE;
    slots->unk80.bit6 = TRUE;
    slots->unk88.bit6 = TRUE;
    func_00193378(entity->model, 0, 0);
}

static void func_00171C48(Entity *entity, s32 unknown) {
    EntityUnkE0Kind1740A8 *unkE0 = (EntityUnkE0Kind1740A8 *)&entity->unkE0;

    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        func_00196870(entity, 1);
    }
    if (entity->unkC6 & 0x40) {
        entity->flags &= ~0x10000000;
    } else {
        entity->flags |= 0x10000000;
    }
    if (!(entity->unkC6 & 0x45)) {
        func_0019A448(entity, &unkE0->unk88, 1, 3, 0, entity->params->unkC4, 0);
    }
    if (entity->unkC6 == 0) {
        func_00196CC0(entity);
        func_0018E9C0(entity);
        if (ENTITY_FLAG_BITS(entity)->bit53) {
            func_00182DE0(entity, unknown);
        } else {
            switch (entity->state) {
            case 0:
                unkE0->unk7A = 0;
                func_00172320(entity, unknown);
                break;
            case 1:
                func_00183150(entity, unknown);
                break;
            case 2:
                if (unkE0->unk7A == 0) {
                    func_001958B8(entity, func_00172F00, unknown);
                    unkE0->unk7A = 1;
                } else {
                    func_00171E30(entity, unknown);
                }
                break;
            default:
                func_00172320(entity, unknown);
                break;
            }
        }
    }
    if (entity->unkC6 == 0 || (entity->unkC6 & 6)) {
        func_00172738(entity);
    }
}

static void func_00171E30(Entity *entity, s32 unknown) {
    EntityUnkE0Kind1740A8 *unkE0 = (EntityUnkE0Kind1740A8 *)&entity->unkE0;
    f32 direction[4];
    f32 height;
    s32 side;
    s32 turn;

    if (entity->params->flags & 0x100) {
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
    } else {
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
    }
    if (entity->unkA0 == 0) {
        if (rand() % 100 < entity->params->unk438) {
            if (!(rand() & 1)) {
                entity->target = &D_00413F80;
                entity->unkA0 = rand() % 30 + 0x1E;
                unkE0->unk78 = 0;
                entity->unkCD = 1;
                unkE0->unk79 = 1;
            } else {
                entity->target = &D_00413F80;
                entity->unkA0 = rand() % 30 + 0x1E;
                unkE0->unk78 = 0;
                entity->unkCD = 1;
                unkE0->unk79 = -1;
            }
        } else if (entity->unkA0 == 0) {
            unkE0->unk79 = 0;
        }
    }
    side = func_0018FD50(entity, 0);
    if (entity->spawn_info->unk40 == 0) {
        if (func_0018A9F0(entity->unk14, entity->model->position) < (f32)entity->params->unk436 ||
            ENTITY_FLAG_BITS(entity)->bit44 == 1) {
            side = 1;
            entity->target = &D_00413F80;
            entity->unkA0 = rand() % 30 + 0x1E;
            unkE0->unk78 = 0;
            entity->unkCD = 1;
            unkE0->unk79 = 2;
        } else if (func_0018A9F0(entity->unk14, entity->model->position) > (f32)(s16)entity->params->unk434 ||
                   ENTITY_FLAG_BITS(entity)->bit46 == 1) {
            side = 1;
            entity->target = &D_00413F80;
            entity->unkA0 = rand() % 30 + 0x1E;
            unkE0->unk78 = 0;
            entity->unkCD = 1;
            unkE0->unk79 = -2;
        }
    } else if (entity->spawn_info->unk40 == 1 || entity->spawn_info->unk40 == 2) {
        if (entity->spawn_info->spawn_y - entity->model->position[1] <
                (f32)entity->spawn_info->unk42 * (entity->spawn_info->unk43 != 0 ? 100.0f : -100.0f) ||
            ENTITY_FLAG_BITS(entity)->bit44 == 1) {
            side = 1;
            entity->target = &D_00413F80;
            entity->unkA0 = rand() % 30 + 0x1E;
            unkE0->unk78 = 0;
            entity->unkCD = 1;
            unkE0->unk79 = 2;
        } else if (entity->spawn_info->spawn_y - entity->model->position[1] > (f32)entity->spawn_info->variant * 100.0f ||
                   ENTITY_FLAG_BITS(entity)->bit46 == 1) {
            side = 1;
            entity->target = &D_00413F80;
            entity->unkA0 = rand() % 30 + 0x1E;
            unkE0->unk78 = 0;
            entity->unkCD = 1;
            unkE0->unk79 = -2;
        }
    }
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
                if (D_00413F80.position[1] - entity->model->position[1] < 200.0f) {
                    entity->target = &D_00413F80;
                    entity->unkA0 = rand() % 30 + 0x1E;
                    unkE0->unk78 = 0;
                    entity->unkCD = 1;
                    unkE0->unk79 = 2;
                } else {
                    func_001958B8(entity, func_001735C8, unknown);
                    return;
                }
            } else {
                height = D_00413F80.position[1] - entity->model->position[1];
                if (100.0f < height && height < 400.0f) {
                    func_001958B8(entity, func_00173198, unknown);
                } else {
                    func_001958B8(entity, func_00172F00, unknown);
                }
                return;
            }
            break;
        case 1:
            entity->target = &D_00413F80;
            entity->unkA0 = rand() % 30 + 0x3C;
            unkE0->unk78 = 0;
            break;
        default:
            entity->target = &D_00413F80;
            entity->unkA0 = rand() % 30 + 0x3C;
            unkE0->unk78 = 0;
            break;
        }
    }
    if (unkE0->unk78 == 0) {
        turn = func_0018EAC0(entity, direction);
        if (turn > 0) {
            entity->unkC8 = 0x201;
        } else if (turn < 0) {
            entity->unkC8 = 0x101;
        } else {
            // match: retail passes two more arguments here and takes the result as 16 bits
            entity->unkC8 = ((s16 (*)(Entity *, s32, f32))func_00172BB8)(entity, 0, 0.0f);
        }
    }
    func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, unknown);
}

static void func_00172320(Entity *entity, s32 unknown) {
    EntityUnkE0Kind1740A8 *unkE0 = (EntityUnkE0Kind1740A8 *)&entity->unkE0;
    f32 direction[4];
    s32 turn;

    if (entity->params->flags & 0x100) {
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
    } else {
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
    }
    ENTITY_FLAG_BITS(entity)->bit9 = 1;
    if (entity->unkA0 == 0) {
        func_0018B778((f32)entity->spawn_info->unk4C, unkE0->unk30, entity->spawn_info, entity->model->position);
        entity->target = unkE0->unk30;
        entity->unkA0 = rand() % 60 + 0x3C;
        if (entity->spawn_info->unk40 == 0) {
            if (func_0018A9F0(entity->unk14, entity->model->position) < (f32)entity->params->unk436) {
                entity->target = &D_00413F80;
                entity->unkA0 = rand() % 30 + 0x1E;
                unkE0->unk78 = 0;
                entity->unkCD = 1;
                unkE0->unk79 = 2;
            } else if (func_0018A9F0(entity->unk14, entity->model->position) > (f32)(s16)entity->params->unk434) {
                entity->target = &D_00413F80;
                entity->unkA0 = rand() % 30 + 0x1E;
                unkE0->unk78 = 0;
                entity->unkCD = 1;
                unkE0->unk79 = -2;
            }
        } else if (entity->spawn_info->unk40 == 1 || entity->spawn_info->unk40 == 2) {
            if (entity->spawn_info->spawn_y - entity->model->position[1] <
                    (f32)entity->spawn_info->unk42 * (entity->spawn_info->unk43 != 0 ? 100.0f : -100.0f) ||
                ENTITY_FLAG_BITS(entity)->bit44 == 1) {
                entity->target = &D_00413F80;
                entity->unkA0 = rand() % 30 + 0x1E;
                unkE0->unk78 = 0;
                entity->unkCD = 1;
                unkE0->unk79 = 2;
            } else if (entity->spawn_info->spawn_y - entity->model->position[1] > (f32)entity->spawn_info->variant * 100.0f ||
                       ENTITY_FLAG_BITS(entity)->bit46 == 1) {
                entity->target = &D_00413F80;
                entity->unkA0 = rand() % 30 + 0x1E;
                unkE0->unk78 = 0;
                entity->unkCD = 1;
                unkE0->unk79 = -2;
            }
        }
        if (rand() % 100 < entity->params->unk438) {
            if (!(rand() & 1)) {
                entity->target = &D_00413F80;
                entity->unkA0 = rand() % 30 + 0x1E;
                unkE0->unk78 = 0;
                entity->unkCD = 1;
                unkE0->unk79 = 1;
            } else {
                entity->target = &D_00413F80;
                entity->unkA0 = rand() % 30 + 0x1E;
                unkE0->unk78 = 0;
                entity->unkCD = 1;
                unkE0->unk79 = -1;
            }
        } else {
            unkE0->unk79 = 0;
        }
    }
    turn = func_0018EAC0(entity, direction);
    if (turn > 0) {
        entity->unkC8 = 0x201;
    } else if (turn < 0) {
        entity->unkC8 = 0x101;
    } else {
        // match: retail passes two more arguments here and takes the result as 16 bits
        entity->unkC8 = ((s16 (*)(Entity *, s32, f32))func_00172BB8)(entity, 0, 0.0f);
    }
    func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, unknown);
}

static void func_001726C8(Entity *entity) {
    entity->flags &= ~0x3800UL;
    func_0019A448(entity, entity->unkE0.unk80, 0, 3, 0, entity->params->unkC4, 0);
    func_0019A448(entity, entity->unkE0.unk88, 1, 3, 0, entity->params->unkC4, 0);
}

static void func_00172738(Entity *entity) {
    EntityUnkE0Kind1719F8 *unkE0 = (EntityUnkE0Kind1719F8 *)&entity->unkE0;

    if (entity->unkC8 & 0x100) {
        unkE0->unk1C -= entity->params->unk58;
        if (unkE0->unk1C < -entity->params->turn_limit) {
            unkE0->unk1C = -entity->params->turn_limit;
        }
    } else if (entity->unkC8 & 0x200) {
        unkE0->unk1C += entity->params->unk58;
        if (entity->params->turn_limit < unkE0->unk1C) {
            unkE0->unk1C = entity->params->turn_limit;
        }
    } else if (entity->params->unk58 < unkE0->unk1C) {
        unkE0->unk1C -= entity->params->unk58;
    } else if (unkE0->unk1C < -entity->params->unk58) {
        unkE0->unk1C += entity->params->unk58;
    } else {
        unkE0->unk1C = 0.0f;
    }
    entity->unk94 += unkE0->unk1C;
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
    if (entity->unkC8 & 1) {
        unkE0->unk48 -= entity->params->unk50;
        if (unkE0->unk48 < -entity->params->unk54) {
            unkE0->unk48 = -entity->params->unk54;
        }
    } else if (entity->unkC8 & 2) {
        unkE0->unk48 += entity->params->unk50;
        if (entity->params->unk54 < unkE0->unk48) {
            unkE0->unk48 = entity->params->unk54;
        }
    } else if (unkE0->unk48 > entity->params->unk50) {
        unkE0->unk48 -= entity->params->unk50;
    } else if (unkE0->unk48 < -entity->params->unk50) {
        unkE0->unk48 += entity->params->unk50;
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
    if (entity->unkC8 & 0x40) {
        Vector_Scale(ENTITY_SCRATCH_VECTOR, ENTITY_SCRATCH_VECTOR, 2.0f);
    }
    entity->unk84 += ENTITY_SCRATCH_VECTOR[0];
    entity->unk88 += ENTITY_SCRATCH_VECTOR[1];
    entity->unk8C += ENTITY_SCRATCH_VECTOR[2];
}

/* Returns 0 with no target, 1 if the turn to it is within the limit, else 0x101/0x201 by turn direction. */
static int func_00172BB8(Entity *entity) {
    float angle;
    float limit;
    int result;

    if (entity->target == NULL) {
        return 0;
    }
    result = 1;
    angle = func_001954A8(&ENTITY_MODEL(entity)->position.x, entity->target);
    angle = func_001957B8(ENTITY_MODEL(entity)->rotation.y, angle);
    limit = ENTITY_CONTROLLER(entity)->turn_limit;
    if (0.0f <= angle ? limit < angle : limit < -angle) {
        result = 0x101;
        if (0.0f <= angle) {
            result = 0x201;
        }
    }
    return result;
}

static void func_00172C78(Entity *entity) {
    EntityUnkE0Kind1740A8 *unkE0 = (EntityUnkE0Kind1740A8 *)&entity->unkE0;
    f32 angle;

    entity->unkC6 |= 0x20;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 1, 0, 0, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
        unkE0->unk1C = 0;
        ENTITY_FLAG_BITS(entity)->bit21 = 1;
        ENTITY_FLAG_BITS(entity)->bit22 = 1;
        ENTITY_FLAG_BITS(entity)->bit23 = 1;
        ENTITY_FLAG_BITS(entity)->bit24 = 1;
        ENTITY_FLAG_BITS(entity)->bit18 = 1;
        ENTITY_FLAG_BITS(entity)->bit28 = 1;
        ENTITY_FLAG_BITS(entity)->bit10 = 0;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->action_step++;
        break;
    case 1:
        if (entity->distance_to_player < (f32)entity->spawn_info->unk54 || ENTITY_FLAG_BITS(entity)->bit43) {
            ENTITY_FLAG_BITS(entity)->bits36 = 0;
            func_00195898((char *)entity, 2);
            ENTITY_FLAG_BITS(entity)->bit21 = 0;
            ENTITY_FLAG_BITS(entity)->bit22 = 0;
            ENTITY_FLAG_BITS(entity)->bit23 = 0;
            ENTITY_FLAG_BITS(entity)->bit24 = 0;
            ENTITY_FLAG_BITS(entity)->bit18 = 0;
            if (!(((EntityParamsWord *)entity->params)->unk00 & 0x06000000)) {
                ENTITY_FLAG_BITS(entity)->bit28 = 0;
            }
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            angle = func_00194FB8((f32)(rand() % 361) * 0.017453292f);
            entity->model->position[0] = sinf(angle) * 2000.0f;
            entity->model->position[2] = cosf(angle) * 2000.0f;
            if (ENTITY_FLAG_BITS(entity)->bit5) {
                unkE0->unk00(entity, 1);
            } else {
                func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, 1);
            }
        }
        break;
    }
}

static void func_00172F00(Entity *entity) {
    EntityUnkE0Kind1740A8 *unkE0 = (EntityUnkE0Kind1740A8 *)&entity->unkE0;
    f32 direction[4];
    f32 height;
    s32 turn;

    entity->unkC6 |= 0x3000;
    switch (entity->action_step) {
    case 0:
        func_0019A448(entity, &unkE0->unk80, 0, 3, 0, entity->params->unkC4, 0);
        func_0019A448(entity, &unkE0->unk88, 1, 3, 0, entity->params->unkC4, 0);
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
        ENTITY_FLAG_BITS(entity)->bit10 = 1;
        ENTITY_FLAG_BITS(entity)->bit9 = 1;
        entity->target = &D_00413F80;
        unkE0->unk68 = 0;
        entity->action_step++;
    case 1:
        if (entity->distance_to_player < entity->radius + 250.0f + 100.0f) {
            f32 above = D_00413F80.position[1] - entity->model->position[1];

            if (100.0f < above && above < 400.0f) {
                entity->unkC6 = 0;
                entity->unkC8 = 0;
                entity->unkA0 = 0;
                func_001958B8(entity, func_00173198, 1);
                return;
            }
        }
        if (unkE0->unk68++ >= 0xD3) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if (ENTITY_FLAG_BITS(entity)->bit5) {
                unkE0->unk00(entity, 1);
            } else {
                func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, 1);
            }
            return;
        }
        turn = func_0018EAC0(entity, direction);
        if (turn > 0) {
            entity->unkC8 = 0x201;
        } else if (turn < 0) {
            entity->unkC8 = 0x101;
        } else {
            // match: retail passes two more arguments here and takes the result as 16 bits
            entity->unkC8 = ((s16 (*)(Entity *, s32, f32))func_00172BB8)(entity, 0, 0.0f);
        }
        height = D_00413F80.position[1] - entity->model->position[1];
        if (height < 200.0f || ENTITY_FLAG_BITS(entity)->bit44 == 1 || ENTITY_FLAG_BITS(entity)->bit45 == 1) {
            unkE0->unk79 = 2;
        } else if (400.0f < height || ENTITY_FLAG_BITS(entity)->bit46 == 1) {
            unkE0->unk79 = -2;
        } else {
            unkE0->unk79 = 0;
        }
        func_00172738(entity);
        break;
    }
}

static void func_00173198(Entity *entity) {
    EntityUnkE0Kind1740A8 *unkE0 = (EntityUnkE0Kind1740A8 *)&entity->unkE0;
    s32 kind = (s32)(ENTITY_FLAG_WORD(entity->flags) >> 58) & 3;
    EntityParamsUnk240 *unk240 = &entity->params->unk240[kind];
    s32 elapsed;

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        func_0019A448(entity, &unkE0->unk80, 0, kind | 0x14, 0, entity->params->unkC4 >> 1, 1);
        func_0019A448(entity, &unkE0->unk88, 1, ((s32)(entity->flags >> 58) & 3) | 0x14, 0, entity->params->unkC4 >> 1, 1);
        if (entity->params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->bits11 = ((((EntityParamsWord *)entity->params)->unk00 & 0x06000000) == 0x02000000) ? 2 : 1;
        }
        ENTITY_FLAG_BITS(entity)->bit54 = 0;
        ENTITY_FLAG_BITS(entity)->bit55 = 0;
        ENTITY_FLAG_BITS(entity)->bit56 = 0;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->action_step++;
    case 1:
        if (unkE0->unk80.unk01 == 0) {
            entity->action_step++;
        }
        if (unk240->unk01 != 0) {
            func_00199EB8(entity, (f32 *)&unkE0->unk1C, entity->unk68);
        }
        break;
    case 2:
        if (unkE0->unk80.unk02 <= unk240->unk01) {
            func_00199EB8(entity, (f32 *)&unkE0->unk1C, entity->unk68);
        } else {
            unkE0->unk1C = 0;
        }
        elapsed = (unkE0->unk80.unk04 - entity->params->unk80[entity->unkD1]) >> 8;
        if (!((s32)(entity->flags >> 54) & 1)) {
            ENTITY_FLAG_BITS(entity)->bit54 = func_00195AC0(entity, unk240, 0, elapsed, unkE0->unk80.unk02);
        }
        if (!((s32)(entity->flags >> 55) & 1)) {
            ENTITY_FLAG_BITS(entity)->bit55 = func_00195AC0(entity, unk240, 1, elapsed, unkE0->unk80.unk02);
        }
        if (!((s32)(entity->flags >> 56) & 1)) {
            ENTITY_FLAG_BITS(entity)->bit56 = func_00195AC0(entity, unk240, 2, elapsed, unkE0->unk80.unk02);
        }
        if (unkE0->unk80.unk02 > 0 && unk240->unk03 >= unkE0->unk80.unk02) {
            entity->flags |= 0x80000;
        } else {
            entity->flags &= ~0x80000;
        }
        if ((s32)(entity->flags >> 34) & 1) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            func_0019A290(entity, unk240->unk02);
            if ((s32)(entity->flags >> 5) & 1) {
                unkE0->unk00(entity, 1);
            } else {
                func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, 1);
            }
        }
        break;
    }
}

static void func_001735C8(Entity *entity) {
    EntityUnkE0Kind1719F8 *unkE0 = (EntityUnkE0Kind1719F8 *)&entity->unkE0;
    s32 kind = (s32)(ENTITY_FLAG_WORD(entity->flags) >> 58) & 3;
    EntityParamsUnk3D8Full *record = (EntityParamsUnk3D8Full *)ENTITY_PARAMS_UNK3D8(entity->params, kind);
    f32 target[4];
    f32 angles[4];
    f32 *point;
    s32 frame;
    s32 elapsed;
    s32 launch_frame;
    s32 i;

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        func_0019A448(entity, &unkE0->unk80, 0, kind + 0x17, 0, entity->params->unkC4 >> 1, 1);
        func_0019A448(entity, &unkE0->unk88, 1, ((s32)(entity->flags >> 58) & 3) + 0x17, 0, entity->params->unkC4 >> 1, 1);
        if (entity->params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->bits11 = ((((EntityParamsWord *)entity->params)->unk00 & 0x06000000) == 0x02000000) ? 2 : 1;
        }
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->action_step++;
    case 1:
        if (unkE0->unk80.unk01 == 0) {
            entity->action_step++;
        }
        if (record->unk01 != 0) {
            func_00199EB8(entity, &unkE0->unk1C, entity->unk68);
        }
        break;
    case 2:
        if (unkE0->unk80.unk02 <= record->unk01) {
            func_00199EB8(entity, &unkE0->unk1C, entity->unk68);
        } else {
            unkE0->unk1C = 0.0f;
        }
        frame = unkE0->unk80.unk02;
        elapsed = (unkE0->unk80.unk04 - entity->params->unk80[entity->unkD1]) >> 8;
        if (((s32)(entity->flags >> 58) & 3) == 1) {
            for (i = 0; i < 3; i++) {
                switch (i) {
                case 0:
                    launch_frame = record->unk10[0].unk02;
                    break;
                case 1:
                    launch_frame = entity->params->unk43A;
                    break;
                case 2:
                    launch_frame = entity->params->unk43C;
                    break;
                }
                if (launch_frame != 0 && elapsed < launch_frame && frame >= launch_frame) {
                    point = (f32 *)func_0010CAB8((char *)entity->model, 0);
                    angles[0] = func_00194FB8(-atan2(D_00413F80.position[1] - point[1], func_00194F80(point, &D_00413F80)));
                    if (!(rand() & 1)) {
                        angles[1] = func_00194FB8(entity->model->unkD0[1] + 3.1415927f + (f32)(rand() % 15) * 0.017453292f);
                    } else {
                        angles[1] = func_00194FB8(entity->model->unkD0[1] + 3.1415927f - (f32)(rand() % 15) * 0.017453292f);
                    }
                    angles[2] = 0.0f;
                    func_00173D38(entity, record, elapsed, unkE0->unk80.unk02, angles);
                }
            }
        } else {
            func_00216778(target, &D_00413F80);
            target[1] -= 200.0f;
            if (record->unk10[0].unk02 != 0 && elapsed < record->unk10[0].unk02 && frame >= record->unk10[0].unk02) {
                func_00173AB0(entity, (EntityParamsUnk3D8 *)record, elapsed, unkE0->unk80.unk02, target, 0.7853982f, 0.0f);
            }
        }
        if (unkE0->unk80.unk02 > 0 && record->unk03 >= unkE0->unk80.unk02) {
            entity->flags |= 0x80000;
        } else {
            entity->flags &= ~0x80000;
        }
        if ((s32)(entity->flags >> 34) & 1) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            entity->unkA2 = record->unk02;
            if ((s32)(entity->flags >> 5) & 1) {
                unkE0->unk00(entity, 1);
            } else {
                func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, 1);
            }
        }
        break;
    }
}

/* launches both shots of a record at once, whatever the animation frame */
static s32 func_00173AB0(Entity *entity, EntityParamsUnk3D8 *unk3D8, s32 frame_from, s32 frame_to, f32 *target, f32 pitch_rate, f32 yaw_rate) {
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

static s32 func_00173D38(Entity *entity, EntityParamsUnk3D8Full *record, s32 frame_from, s32 frame_to, f32 *angles) {
    EntityUnk1B1390 *shot;
    char *point;
    s32 i;

    if (entity->unk2A != 0 && record->unk0C == 0) {
        return 0;
    }
    for (i = 0; i < 3; i++) {
        shot = Shot_Alloc();
        shot->unk00 = 1;
        shot->unk20 = 0;
        shot->unk22 = 0;
        shot->unk28 = entity->params->unk238;
        point = func_0010CAB8((char *)entity->model, record->unk10[i].unk01);
        if (point != NULL) {
            func_001B4F30(record->unk10[i].unk00, shot, point, angles, entity);
        }
    }
    return 0;
}

static void func_00173E08(Entity *entity) {
    func_00190A30((char *)entity);
    entity->target = NULL;
    entity->flags &= ~0x400;
    entity->flags &= ~0x200;
    if (((s32)(entity->flags >> 36) & 3) == 1) {
        func_0019A310((char *)entity, (void (*)(void))func_00173E98, 0);
    } else if (((s32)(entity->flags >> 36) & 3) == 2) {
        func_0019A310((char *)entity, (void (*)(void))func_001740A8, 0);
    }
    func_00195898((char *)entity, 2);
}

static void func_00173E98(Entity *entity) {
    EntityUnkE0Kind1740A8 *unkE0 = (EntityUnkE0Kind1740A8 *)&entity->unkE0;

    entity->unkC6 = 2;
    switch (entity->unkCB) {
    case 0:
        func_0018D8B8(entity, 0xE, 0, 0);
        func_0019A448(entity, &unkE0->unk80, 0, 0xE, 0, 5, 1);
        func_0019A448(entity, &unkE0->unk88, 1, 3, 0, 5, 0);
        if (entity->params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->bits11 = ((((EntityParamsWord *)entity->params)->unk00 & 0x06000000) == 0x02000000) ? 2 : 1;
        }
        unkE0->unk1C = 0;
        ENTITY_FLAG_BITS(entity)->bit14 = 1;
        ENTITY_FLAG_BITS(entity)->bit19 = 0;
        ENTITY_FLAG_BITS(entity)->bit35 = 0;
        ENTITY_FLAG_BITS(entity)->bit10 = 0;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
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
            unkE0->unk00(entity, 1);
        } else {
            func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, 1);
        }
        break;
    }
}

static void func_001740A8(Entity *entity) {
    EntityUnkE0Kind1740A8 *unkE0 = (EntityUnkE0Kind1740A8 *)&entity->unkE0;

    entity->unkC6 = 4;
    switch (entity->unkCB) {
    case 0:
        func_0019A448(entity, &unkE0->unk80, 0, 0xF, 0, 5, 1);
        func_0019A448(entity, &unkE0->unk88, 1, 0xF, 0, 5, 1);
        if (entity->params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            s32 kind = 2;

            if (!(ENTITY_PARAMS_BITS(entity)->bit25 && !ENTITY_PARAMS_BITS(entity)->bit26)) {
                kind = 1;
            }
            ENTITY_FLAG_BITS(entity)->bits11 = kind;
        }
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
                unkE0->unk00(entity, 1);
            } else {
                func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, 1);
            }
        }
        break;
    }
}

static void func_001742E0(Entity *entity) {
    EntityUnkE0Kind1740A8 *unkE0 = (EntityUnkE0Kind1740A8 *)&entity->unkE0;

    entity->unkC6 = 0x40;
    switch (entity->action_step) {
    case 0:
        func_0019A448(entity, &unkE0->unk80, 0, 0x10, 0, 5, 1);
        func_0019A448(entity, &unkE0->unk88, 1, 0x10, 0, 5, 1);
        if (entity->params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            s32 kind = 2;

            if (!(ENTITY_PARAMS_BITS(entity)->bit25 && !ENTITY_PARAMS_BITS(entity)->bit26)) {
                kind = 1;
            }
            ENTITY_FLAG_BITS(entity)->bits11 = kind;
        }
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

static void func_00174530(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityUnkE0Kind151D30 *slots = (EntityUnkE0Kind151D30 *)unkE0;
    f32 limits[4];
    f32 unused_vector[4]; // match: retail reserves 16 more bytes of stack than it uses
    s32 previous;
    f32 rate;

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
