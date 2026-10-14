/* The behaviour of entity type 71 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers that function installs. */
#include "kf_entity.h"
#include "common.h"

void func_001958A8(char *, s32);
int func_001958B8(Entity *, void (*)(Entity *), int);
void func_00191478(EntityModel *);
int func_00193378(EntityModel *, s32, s32);
void func_00216778(void *, void *);
// match: the definition takes an Actor, a struct local to func_00182A08.c
void func_00182A08(void);
void func_00182BD0(char *);
void func_00181038(Entity *);
void func_001818B8(Entity *);
extern s32 D_003E6E40[][2];
extern u32 D_00446ED4;
int func_00182DE0(void *, int);
void func_0018E9C0(void *);
void func_00196870(void *, int);
void func_00196CC0(void *);
int rand(void);
void func_0018B778(float, void *, void *, void *);
s32 func_00190F90(Entity *, s32, f32);
s32 func_0018ED98(Entity *, f32, f32);
s32 func_0018FD50(Entity *, s32);
void func_00190B88(Entity *, s32);
int func_0018F030(char *);
void func_00181C60(Entity *);
void func_00181E48(Entity *);
void func_00182240(Entity *);
extern f32 D_00431AF0[16];
extern PlayerData D_00413F80;
void sceVu0RotMatrixY(float *, float *, float);
void func_00216BC0(void *, void *, void *);
void func_00216D20(void *, void *);
void Vector_Scale(float *, float *, float);
float func_00199E68(Entity *, float);
float func_001954A8(float *, float *);
int func_00192C98(void *, int, int, u32, int);
int func_00195ED0(Entity *, EntityParamsUnk3D8 *, s32, s32);
void func_0019A290(Entity *, s32);

static void func_0014B888(Entity *entity, s32 unknown);
static void func_0014B988(Entity *entity, s32 unknown);
static void func_0014BA98(Entity *entity, s32 unknown);
static void func_0014BCF0(Entity *entity);
static void func_0014BF80(Entity *entity);

/* Set-up of a type 71 entity: clears its work area and installs the type's handlers. */
void EntityType71_Init(Entity *entity) {
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
    unkE0->unk00 = func_0014B888;
    unkE0->unk04 = func_00182A08;
    unkE0->unk08 = (void (*)(void))func_00182BD0;
    unkE0->unk18 = (void (*)(void))func_0014BCF0;
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

static void func_0014B888(Entity *entity, s32 unknown) {
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
        func_0014BA98(entity, unknown);
        return;
    }
    func_0014B988(entity, unknown);
}

static void func_0014B988(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    ENTITY_FLAG_BITS(entity)->bits11 = 0;
    if (entity->unkA0 == 0) {
        if (rand() % 100 >= 21) {
            f32 *point = unkE0->unk30;

            func_0018B778(entity->spawn_info->unk4C, point, entity->spawn_info, entity->model->position);
            entity->target = point;
            entity->unkA0 = rand() % 90 + 240;
        } else {
            entity->target = NULL;
            entity->unkA0 = rand() % 60 + 90;
        }
    }
    entity->unkC8 = func_00190F90(entity, 0, 50.0f);
    func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, unknown);
}

static void func_0014BA98(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    s32 side;

    ENTITY_FLAG_BITS(entity)->bits11 = 0;
    if (func_0018ED98(entity, 300.0f, 0.5235988f)) {
        func_001958B8(entity, func_00181C60, 1);
        return;
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
                if (ENTITY_FLAG_BITS(entity)->bits58 == 0) {
                    func_001958B8(entity, func_0014BF80, unknown);
                } else {
                    func_001958B8(entity, func_00182240, unknown);
                }
            } else {
                func_001958B8(entity, func_00181E48, unknown);
            }
            return;
        case 1:
            func_0018B778(entity->spawn_info->unk4C, unkE0->unk30, entity->spawn_info, entity->model->position);
            entity->target = unkE0->unk30;
            entity->unkA0 = rand() % 60 + 210;
            unkE0->unk78 = 0;
            break;
        case 2:
            entity->unkA0 = rand() % 60 + 90;
            unkE0->unk78 = -1;
            break;
        default:
            unkE0->unk78 = -1;
            break;
        }
    }
    if (unkE0->unk78 == 0) {
        entity->unkC8 = func_00190F90(entity, 0, 50.0f);
    }
    func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, unknown);
}

static void func_0014BCF0(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 *vector;
    f32 turn;
    f32 heading;
    f32 amount;

    if (entity == D_00413F80.unk204 && (entity->params->flags & 0x800)) {
        entity->unkC8 = 0;
    }
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
    } else {
        unkE0->unk1C = 0.0f;
    }
    turn = unkE0->unk1C;
    if (entity->unkC8 & 0x400) {
        turn += turn;
    }
    entity->unk94 += turn;
    ENTITY_SCRATCH_VECTOR[0] = ENTITY_SCRATCH_VECTOR[1] = ENTITY_SCRATCH_VECTOR[2] = ENTITY_SCRATCH_VECTOR[3] = 0.0f;
    vector = ENTITY_SCRATCH_VECTOR;
    if (entity->unkC8 & 3) {
        amount = 0.0f;
        if (entity->unkC8 & 1) {
            amount = -1.0f;
        } else if (entity->unkC8 & 2) {
            amount = 1.0f;
        }
        vector[2] += amount;
    }
    if (entity->unkC8 & 0xC) {
        amount = 0.0f;
        if (entity->unkC8 & 4) {
            amount = 1.0f;
        } else if (entity->unkC8 & 8) {
            amount = -1.0f;
        }
        vector[0] += amount;
    }
    if (entity->unkC8 & 0xF) {
        unkE0->unk44 = func_00199E68(entity, unkE0->unk44);
        amount = unkE0->unk44;
        if (entity->unkC8 & 0x40) {
            amount += amount;
        }
        if (entity->target != NULL) {
            heading = func_001954A8(entity->model->position, entity->target);
            sceVu0RotMatrixY(ENTITY_SCRATCH_MATRIX, D_00431AF0, heading);
            func_00216BC0(ENTITY_SCRATCH_VECTOR, ENTITY_SCRATCH_MATRIX, ENTITY_SCRATCH_VECTOR);
            func_00216D20(ENTITY_SCRATCH_VECTOR, ENTITY_SCRATCH_VECTOR);
            Vector_Scale(ENTITY_SCRATCH_VECTOR, ENTITY_SCRATCH_VECTOR, amount);
            entity->unk84 += ENTITY_SCRATCH_VECTOR[0];
            entity->unk88 += ENTITY_SCRATCH_VECTOR[1];
            entity->unk8C += ENTITY_SCRATCH_VECTOR[2];
        }
    } else {
        unkE0->unk44 = 0.0f;
    }
    func_00192C98(entity, 0, 0, entity->params->unkC4, 0);
}

static void func_0014BF80(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityParamsUnk3D8 *unk3D8 = ENTITY_PARAMS_UNK3D8(entity->params, 0);
    EntityParamsUnk3D8 *second_unk3D8 = ENTITY_PARAMS_UNK3D8(entity->params, 1);

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 0x17, 0, entity->params->unkC4 >> 1, 1);
        entity->flags &= ~0x3800;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (entity->model->unkFC->unk82 >= entity->params->unk430 && entity->model->unkFC->unk82 <= entity->params->unk432) {
            if ((s16)entity->params->unk434 == 0) {
                func_00195ED0(entity, unk3D8, 30, 50);
            } else if (D_00446ED4 % (s16)entity->params->unk434 == 0) {
                if (unkE0->unk7A != 0) {
                    func_00195ED0(entity, unk3D8, 30, 50);
                } else {
                    func_00195ED0(entity, second_unk3D8, 30, 50);
                }
                unkE0->unk7A ^= 1;
            }
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
