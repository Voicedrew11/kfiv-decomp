/* The behaviour of entity type 65 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers that function installs. */
#include "kf_entity.h"
#include "common.h"
#include "kf_shot.h"

void func_001958A8(char *, s32);
int func_001958B8(Entity *, void (*)(Entity *), int);
void func_00191478(EntityModel *);
int func_00193378(EntityModel *, s32, s32);
void func_00216778(void *, void *);
int rand(void);
void func_00182BD0(char *);
void func_00181038(Entity *);
extern s32 D_003E6E40[][2];
extern u32 D_00446ED4;
void func_00196870(void *, int);
void func_00196CC0(char *);
void func_0018E9C0(void *);
int func_00182DE0(void *, int);
int func_00182FD8(void *, int);
void func_00183150(Entity *, s32);
extern PlayerData D_00413F80;
s32 func_0018ED98(Entity *, f32, f32);
s32 func_0018EEE0(Entity *, f32, f32);
s32 func_0018FD50(Entity *, s32);
void func_00190B88(Entity *, s32);
int func_0018F030(char *);
void func_00196D48(Entity *);
int func_00190BE0(Entity *);
int func_0018EAC0(void *, void *);
s32 func_00190F90(Entity *, s32, f32);
float func_001957B8(float, float);
void func_00181AE0(Entity *);
void func_00181C60(Entity *);
void func_00181E48(Entity *);
int func_00192C98(void *, int, int, u32, int);
void func_0018DCD8(Entity *);
extern f32 D_00431AF0[16];
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0RotMatrixZ(float *, float *, float);
void func_00216BC0(void *, void *, void *);
void Vector_Scale(float *, float *, float);
float func_00194FB8(float);
int func_00192EB0(void *, int);
// match: the callee is void, but retail keeps what it leaves in $2
int func_00194368(EntityModel *, f32);
char *func_0010CAB8(char *, u32);
int func_001B4F30(u16, void *, void *, void *, void *);
void func_0018D600(char **, int, u32, int);
s32 func_00199EB8(Entity *, f32 *, f32);
int func_00195ED0(Entity *, EntityParamsUnk3D8 *, s32, s32);
void func_0019A290(Entity *, s32);

static void func_0014A3F8(Entity *entity, s32 unknown);
static void func_0014A6D0(Entity *entity, s32 unknown);
static void func_0014AAE0(Entity *entity);
static void func_0014AB18(Entity *entity);
static void func_0014ACD0(Entity *entity);
static void func_0014B178(Entity *entity);
static void func_0014B270(Entity *entity);
static void func_0014B450(Entity *entity);

/* Set-up of a type 65 entity: clears its work area and installs the type's handlers. */
void EntityType65_Init(Entity *entity) {
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
    unkE0->unk00 = func_0014A3F8;
    unkE0->unk04 = (void (*)(void))func_0014B270;
    unkE0->unk08 = (void (*)(void))func_00182BD0;
    unkE0->unk18 = (void (*)(void))func_0014AAE0;
    unkE0->unk10 = (void (*)(void))func_00181038;
    unkE0->unk14 = (void (*)(void))func_0014AB18;
    for (i = 4 - 1; i != -1; i--) {
        entity->unkBC[i] = 0x80;
    }
    for (i = 6 - 1; i != -1; i--) {
        entity->unkC0[i] = 0x80;
    }
    ENTITY_UNK5C(entity)->as_s32 = 0;
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
    unkE0->unk6A = rand() % 30;
    func_00191478(entity->model);
    func_00193378(entity->model, 0, 0);
}

static void func_0014A3F8(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        func_00196870(entity, 1);
    }
    if (entity->unkC6 & 0x40) {
        ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit22 = FALSE;
    } else {
        ENTITY_FLAG_BITS(entity)->bit28 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit22 = TRUE;
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
                func_0014A6D0(entity, unknown);
                break;
            default:
                func_00182FD8(entity, unknown);
                break;
            }
        }
    }
    if (unkE0->unk6A > 0) {
        unkE0->unk6A--;
    }
    if (ENTITY_FLAG_BITS(entity)->bit28 && !(entity->unkC6 & 1)) {
        if (unkE0->unk6A <= 0) {
            if (entity->model->position[1] < D_00413F80.position[1] - 300.0f) {
                unkE0->unk7A = 1;
            } else if (D_00413F80.position[1] - 100.0f < entity->model->position[1]) {
                unkE0->unk7A = 0;
            } else {
                unkE0->unk7A = unkE0->unk7A == 0;
            }
            unkE0->unk6A = rand() % 15 + 20;
        }
        if (unkE0->unk7A != 0) {
            entity->unkC8 |= 0x20;
        } else {
            entity->unkC8 |= 0x10;
        }
    }
    func_0014ACD0(entity);
    if (unkE0->unk68 < 0) {
        unkE0->unk7C = (rand() % 10 + 10) * 2;
        if (rand() % 10 == 1) {
            unkE0->unk7B = 0;
        } else {
            unkE0->unk7B = rand() % entity->params->unk438;
        }
        unkE0->unk68 = unkE0->unk7C * 2;
    }
}

static void func_0014A6D0(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    f32 angle;
    s32 mode;
    s32 side;

    if (entity->params->flags & 0x100) {
        entity->flags &= ~0x3800;
    } else {
        entity->flags = (entity->flags & ~0x3800) | 0x800;
    }
    if (func_0018ED98(entity, 300.0f, 0.5235988f)) {
        func_001958B8(entity, func_00181C60, 1);
        return;
    }
    if (func_0018EEE0(entity, 300.0f, 0.5235988f)) {
        func_001958B8(entity, func_00181AE0, 1);
        return;
    }
    mode = func_0018FD50(entity, 0);
    entity->flags |= 0x200;
    if (entity->unkA0 == 0 || mode == 0) {
        entity->unkCF = entity->unkCD;
        entity->unkCD = mode;
        if (entity->unkCD != entity->unkCF) {
            entity->unk9E = D_00446ED4;
        }
        func_00190B88(entity, mode);
        switch (entity->unkCD) {
        case 0:
            if (entity->model->position[1] < D_00413F80.position[1] - 50.0f && D_00413F80.position[1] - 300.0f < entity->model->position[1]) {
                if (func_0018F030((char *)entity)) {
                    func_001958B8(entity, func_0014B450, unknown);
                } else {
                    func_001958B8(entity, func_00181E48, unknown);
                }
                return;
            }
            if (entity->model->position[1] < D_00413F80.position[1]) {
                unkE0->unk7A = 1;
            } else {
                unkE0->unk7A = 0;
            }
            unkE0->unk6A = rand() % 15 + 15;
            break;
        case 1:
            entity->target = &D_00413F80;
            entity->unkA0 = rand() % 60 + 120;
            if (rand() % 3 == 0) {
                unkE0->unk78 = 1;
            } else {
                unkE0->unk78 = 0;
            }
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
        func_00196D48(entity);
        if ((s32)(entity->flags >> 31) & 1) {
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
            side = func_0018EAC0(entity, direction);
            switch (unkE0->unk78) {
            case 0:
                if (side > 0) {
                    entity->unkC8 = 0x201;
                } else if (side < 0) {
                    entity->unkC8 = 0x101;
                } else {
                    entity->unkC8 = func_00190F90(entity, 0, entity->radius + 100.0f + 30.0f);
                }
                break;
            case 1:
                if (side > 0) {
                    entity->unkC8 = 0x241;
                } else if (side < 0) {
                    entity->unkC8 = 0x141;
                } else {
                    entity->unkC8 = func_00190F90(entity, 1, entity->radius + 100.0f + 30.0f);
                }
                break;
            }
        }
    }
done:
    func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, unknown);
}

static void func_0014AAE0(Entity *entity) {
    if (entity->unkC8 & 1) {
        func_00192C98(entity, 3, 0, entity->params->unkC4, 0);
    } else {
        func_00192C98(entity, 0, 0, entity->params->unkC4, 0);
    }
}

static void func_0014AB18(Entity *entity) {
    entity->unkC6 = 0x40;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 0x10, 0, 5, 1);
        entity->flags &= ~0x3800;
        entity->flags |= 0x4000;
        entity->flags |= 0x100000;
        entity->flags = (entity->flags & ~0xC) | 4;
        entity->flags |= 0x8000000;
        entity->flags |= 0x4000000;
        entity->flags &= ~0x10000000;
        entity->unk30 = 80.0f;
        entity->flags &= ~0x400000;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            ENTITY_FLAG_BITS(entity)->bit14 = 0;
            ENTITY_FLAG_BITS(entity)->bits2 = 2;
            ENTITY_FLAG_BITS(entity)->bit4 = 1;
            entity->unkA0 = 20;
            func_0018DCD8(entity);
            entity->action_step++;
        }
        break;
    case 3:
        if (entity->unkA0 != 0) {
            entity->model->unk15C = (f32)entity->unkD6 * (f32)entity->unkA0 / 20.0f;
        } else {
            entity->flags |= 0xC;
            entity->flags &= ~2;
        }
        break;
    }
}

static void func_0014ACD0(Entity *entity) {
    EntityUnkE0Kind16BF98 *unkE0 = (EntityUnkE0Kind16BF98 *)&entity->unkE0;
    f32 turn = 0.0f;
    f32 step;
    f32 limit;

    if (entity->unkC6 == 0 || (entity->unkC6 & 6)) {
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
        turn = unkE0->unk1C;
        if (entity->unkC8 & 0x400) {
            turn += turn;
        }
        entity->unk94 += turn;
        if (entity->unkC8 & 1) {
            unkE0->unk4C -= entity->params->unk50;
            if (unkE0->unk4C < -entity->params->unk54) {
                unkE0->unk4C = -entity->params->unk54;
            }
        } else if (entity->unkC8 & 2) {
            unkE0->unk4C += entity->params->unk50;
            if (entity->params->unk54 < unkE0->unk4C) {
                unkE0->unk4C = entity->params->unk54;
            }
        } else if (unkE0->unk4C > entity->params->unk50) {
            unkE0->unk4C -= entity->params->unk50;
        } else if (unkE0->unk4C < -entity->params->unk50) {
            unkE0->unk4C += entity->params->unk50;
        } else {
            unkE0->unk4C = 0.0f;
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
        } else if (unkE0->unk44 > entity->params->unk50) {
            unkE0->unk44 -= entity->params->unk50;
        } else if (unkE0->unk44 < -entity->params->unk50) {
            unkE0->unk44 += entity->params->unk50;
        } else {
            unkE0->unk44 = 0.0f;
        }
    } else {
        if (entity->params->unk58 < unkE0->unk1C) {
            unkE0->unk1C -= entity->params->unk58;
        } else if (unkE0->unk1C < -entity->params->unk58) {
            unkE0->unk1C += entity->params->unk58;
        } else {
            unkE0->unk1C = 0.0f;
        }
        if (unkE0->unk4C > entity->params->unk50) {
            unkE0->unk4C -= entity->params->unk50;
        } else if (unkE0->unk4C < -entity->params->unk50) {
            unkE0->unk4C += entity->params->unk50;
        } else {
            unkE0->unk4C = 0.0f;
        }
        if (entity->params->unk50 < unkE0->unk44) {
            unkE0->unk44 -= entity->params->unk50;
        } else if (unkE0->unk44 < -entity->params->unk50) {
            unkE0->unk44 += entity->params->unk50;
        } else {
            unkE0->unk44 = 0.0f;
        }
    }
    ENTITY_SCRATCH_VECTOR[0] = ENTITY_SCRATCH_VECTOR[1] = ENTITY_SCRATCH_VECTOR[2] = ENTITY_SCRATCH_VECTOR[3] = 0.0f;
    step = entity->params->unk50 * 0.8f;
    limit = entity->params->unk54 * 0.5f;
    if (entity->unkC8 & 0x10) {
        unkE0->unk48 -= step;
        if (unkE0->unk48 < -limit) {
            unkE0->unk48 = -limit;
        }
    } else if (entity->unkC8 & 0x20) {
        unkE0->unk48 += step;
        if (limit < unkE0->unk48) {
            unkE0->unk48 = limit;
        }
    } else if (step < unkE0->unk48) {
        unkE0->unk48 -= step;
    } else if (unkE0->unk48 < -step) {
        unkE0->unk48 += step;
    } else {
        unkE0->unk48 = 0.0f;
    }
    ENTITY_SCRATCH_VECTOR[0] = unkE0->unk44;
    ENTITY_SCRATCH_VECTOR[1] = unkE0->unk48;
    ENTITY_SCRATCH_VECTOR[2] = unkE0->unk4C;
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

static void func_0014B178(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityModelPart80 *part = &((EntityModelPart80 *)entity->model->unkF8)[3];
    f32 angle;

    if (part != NULL) {
        if (unkE0->unk68 >= unkE0->unk7C) {
            unkE0->unk50 += (f32)unkE0->unk7B / (f32)unkE0->unk7C * 0.017453292f;
            if (unkE0->unk7B * 0.017453292f < unkE0->unk50) {
                unkE0->unk50 = unkE0->unk7B * 0.017453292f;
            }
        } else {
            unkE0->unk50 -= (f32)unkE0->unk7B / (f32)unkE0->unk7C * 0.017453292f;
            if (unkE0->unk50 < 0.0f) {
                unkE0->unk50 = 0.0f;
            }
        }
        angle = unkE0->unk50;
        part->unk50[0] = angle;
        part->unk50[0] = func_00194FB8(angle);
        unkE0->unk68--;
    }
}

static void func_0014B270(Entity *entity) {
    u32 blend_frames; // match: converted to float as unsigned
    s64 flags;
    EntityUnkE0 *unkE0;
    EntityUnk1B1390 *shot;
    f32 unused[4]; /* retail reserves the 0x10 bytes */

    flags = entity->flags;
    unkE0 = &entity->unkE0;
    if (ENTITY_FLAG_BITS(entity)->bit14) {
        if ((((EntityParamsWord *)entity->params)->unk00 & 0x6000000) == 0x6000000) {
            entity->flags = flags & ~0x3800L;
        }
        blend_frames = entity->model->unkFC->unk88;
        if (blend_frames != 0) {
            ENTITY_FLAG_BITS(entity)->bit34 = func_00194368(entity->model, (entity->unkC6 & 0x12) ? unkE0->unk40 : 1.0f / (f32)blend_frames);
        } else {
            ENTITY_FLAG_BITS(entity)->bit34 = func_00192EB0(entity, ENTITY_FLAG_BITS(entity)->bits11);
        }
        if (entity->unkC6 == 0 && entity->model->unkFC->unk88 == 0) {
            func_0014B178(entity);
        } else {
            unkE0->unk50 = 0.0f;
        }
        if (D_00446ED4 % 10U == 0) {
            shot = Shot_Alloc();
            shot->unk00 = 1;
            shot->unk20 = 0;
            shot->unk22 = 0;
            shot->unk28 = entity->params->unk238;
            func_001B4F30(0xD0, shot, func_0010CAB8((char *)entity->model, 0xFF), entity->model->unkD0, entity);
        } else {
            func_0018D600((char **)entity, 0x39D, 0xFF, (int)entity->model->unkD0);
        }
    }
}

static void func_0014B450(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityParamsUnk3D8 *unk3D8 = &entity->params->unk3D8[0].record;

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
        if (entity->model->unkFC->unk82 >= entity->params->unk43A && entity->model->unkFC->unk82 <= entity->params->unk43C &&
            (D_00446ED4 & 1)) {
            func_00195ED0(entity, unk3D8, entity->params->unk43A, entity->params->unk43C);
        }
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
