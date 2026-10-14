/* The behaviour of entity type 3 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers that function installs. */
#include "kf_entity.h"
#include "common.h"

void func_001958A8(char *, s32);
int func_001958B8(Entity *, void (*)(Entity *), int);
void func_00191478(EntityModel *);
int func_00193378(EntityModel *, s32, s32);
void func_00182BD0(char *);
void func_00183570(Entity *);
void func_001818B8(Entity *);
extern s32 D_003E6E40[][2];
int func_00192EB0(void *, int);
// match: the callee is void, but retail keeps what it leaves in $2
int func_00194368(EntityModel *, f32);
extern u32 D_00446ED4;
void func_00196870(void *, int);
void func_00196CC0(char *);
void func_0018E9C0(void *);
int func_00182DE0(void *, int);
int rand(void);
int func_0018EAC0(void *, void *);
void func_0018B778(float, void *, void *, void *);
s16 func_00190F90(float, void *, int); // match: this file only matches with this type; func_00190F90.c defines it differently
extern PlayerData D_00413F80;
s32 func_0018ED98(Entity *, f32, f32);
s32 func_0018EEE0(Entity *, f32, f32);
s32 func_0018FD50(Entity *, s32);
void func_00190B88(Entity *, s32);
int func_0018F030(char *);
float func_00194F80(void *, void *);
void func_00196D48(Entity *);
float func_001957B8(float, float);
int func_00190BE0(Entity *);
void func_00181AE0(Entity *);
void func_00181C60(Entity *);
void func_00181E48(Entity *);
int func_00192C98(void *, int, int, u32, int);
s32 func_00199EB8(Entity *, f32 *, f32);
void func_00216778(void *, void *);
int func_0018CF30(Entity *, EntityParamsUnk3D8 *, s32, s32, f32 *, f32, f32);
void func_0019A290(Entity *, s32);
void func_00190A30(char *);
void func_00195898(char *, u8);
float func_00195388(float *, float, float);
void func_0018DCD8(Entity *);
extern f32 D_00431AF0[16];
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0RotMatrixZ(float *, float *, float);
void sceVu0ApplyMatrix(float *, int *, float *);
void Vector_Scale(float *, float *, float);
void func_00216C48(void *, void *, void *);
s32 func_0018AE98(f32 *, f32 *, void *, f32);

static void func_00128B80(Entity *entity);
static void func_00128CA0(Entity *entity, s32 unknown);
static void func_00128DD8(Entity *entity, s32 unknown);
static void func_00128F40(Entity *entity, s32 unknown);
static void func_00129300(Entity *entity);
static void func_001295B0(Entity *entity);
static void func_00129738(Entity *entity);
static void func_00129780(Entity *entity);
static void func_001299F8(Entity *entity);
static void func_00129CA0(Entity *entity);

/* Init of this enemy kind: one handler table when it starts with its action, another when it starts asleep. */
/* Set-up of a type 3 entity: clears its work area and installs the type's handlers. */
void EntityType3_Init(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    u8 *unkBC = entity->unkBC;
    u8 *unkC0 = entity->unkC0;
    s32 i;

    // match: in the three loops the pointer has to be formed inside the loop
    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        u8 *dest = (u8 *)&entity->unkE0;

        dest[i] = 0;
    }
    for (i = 4 - 1; i != -1; i--) {
        entity->unkBC[i] = 0x80;
    }
    for (i = 6 - 1; i != -1; i--) {
        entity->unkC0[i] = 0x80;
    }
    func_001958A8((char *)entity, D_003E6E40[entity->params->unk02][1]);
    ENTITY_UNK5C(entity)->as_s32 = 0;
    ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
    if (entity->spawn_info->unk38 & (0x8000L << 43)) {
        unkE0->unk00 = func_00128CA0;
        unkE0->unk04 = (void (*)(void))func_00128B80;
        unkE0->unk08 = (void (*)(void))func_00182BD0;
        unkE0->unk18 = (void (*)(void))func_00183570;
        unkE0->unk10 = (void (*)(void))func_001295B0;
        unkE0->unk14 = (void (*)(void))func_001818B8;
        func_001958B8(entity, (void (*)(Entity *))unkE0->unk10, 1);
    } else {
        unkE0->unk00 = func_00128CA0;
        unkE0->unk04 = (void (*)(void))func_00128B80;
        unkE0->unk08 = (void (*)(void))func_00129738;
        unkE0->unk18 = (void (*)(void))func_00183570;
        unkE0->unk10 = (void (*)(void))func_001295B0;
        unkE0->unk14 = (void (*)(void))func_00129780;
        func_001958B8(entity, func_001299F8, 1);
    }
    unkE0->unk40 = 0.5f;
    entity->unkD6 = (entity->params->flags & 0x4000) ? 0x40 : 0x80;
    entity->model->unk15C = entity->unkD6;
    func_00191478(entity->model);
    func_00193378(entity->model, 0, 0);
}

static void func_00128B80(Entity *entity) {
    u32 blend_frames; // match: converted to float as unsigned
    s64 flags;
    EntityUnkE0 *unkE0;

    flags = entity->flags;
    unkE0 = &entity->unkE0;
    if (ENTITY_FLAG_BITS(entity)->bit14) {
        blend_frames = entity->model->unkFC->unk88;
        if (blend_frames != 0) {
            ENTITY_FLAG_BITS(entity)->bit34 = func_00194368(entity->model, (entity->unkC6 & 0x12) ? unkE0->unk40 : 1.0f / (f32)blend_frames);
        } else {
            ENTITY_FLAG_BITS(entity)->bit34 = func_00192EB0(entity, (s32)(flags >> 11) & 7);
        }
    }
    if (unkE0->unk7A != 0) {
        if (ENTITY_FLAG_BITS(entity)->bit53) {
            entity->unk94 = -entity->unk94;
        }
    }
}

/* Per-frame update of this kind: wakes up when the player comes near, else runs the handler of its state. */
static void func_00128CA0(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        func_00196870(entity, 1);
    }
    if (entity->unkC6 != 0) {
        return;
    }
    if (unkE0->unk7A != 0 && entity->distance_to_player < (float)entity->params->unk432) {
        unkE0->unk7A = 0;
        func_001958B8(entity, func_00129CA0, 1);
        return;
    }
    func_00196CC0((char *)entity);
    func_0018E9C0(entity);
    if (ENTITY_FLAG_BITS(entity)->bit53) {
        func_00182DE0(entity, unknown);
    } else if (entity->state == 2) {
        func_00128F40(entity, unknown);
    } else {
        func_00128DD8(entity, unknown);
    }
}

/* wander: every so often pick a point near the spawn point to walk to, or stand still */
static void func_00128DD8(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    s32 turn;

    if (entity->params->flags & 0x100) {
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
    } else {
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
    }
    if (entity->unkA0 == 0) {
        if (rand() % 100 > 40) {
            func_0018B778(entity->spawn_info->unk4C, unkE0->unk30, entity->spawn_info, entity->model->position);
            entity->target = unkE0->unk30;
            entity->unkA0 = rand() % 60 + 120;
        } else {
            entity->target = NULL;
            entity->unkA0 = rand() % 30 + 60;
        }
    }
    turn = func_0018EAC0(entity, direction);
    if (turn > 0) {
        entity->unkC8 = 0x201;
    } else if (turn < 0) {
        entity->unkC8 = 0x101;
    } else {
        entity->unkC8 = func_00190F90(50.0f, entity, 0);
    }
    func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, unknown);
}

/* chase: attack when in reach, else go for the player while he is near the spawn point, or back to it */
static void func_00128F40(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    f32 angle;
    s32 side;
    s32 turn;

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
                func_001958B8(entity, func_00129300, unknown);
            } else {
                func_001958B8(entity, func_00181E48, unknown);
            }
            return;
        case 1:
            if (func_00194F80(&D_00413F80, entity->spawn_info) < (f32)entity->spawn_info->unk4C) {
                entity->target = &D_00413F80;
                entity->unkA0 = rand() % 60 + 120;
                unkE0->unk78 = 0;
            } else {
                func_0018B778(entity->spawn_info->unk4C, unkE0->unk30, entity->spawn_info, entity->model->position);
                entity->target = unkE0->unk30;
                entity->unkA0 = rand() % 60 + 120;
                unkE0->unk78 = 1;
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
                    entity->unkC8 = func_00190F90(entity->radius + 100.0f + 30.0f, entity, 0);
                }
            case 1:
                if (turn > 0) {
                    entity->unkC8 = 0x201;
                } else if (turn < 0) {
                    entity->unkC8 = 0x101;
                } else {
                    entity->unkC8 = func_00190F90(50.0f, entity, 0);
                }
                break;
            }
        }
    }
done:
    func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, unknown);
}

/* Action: one of the ranged attacks, aimed a little below the player. */
static void func_00129300(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    s32 kind = (s32)(entity->flags >> 58) & 3;
    EntityParamsUnk3D8 *unk3D8 = ENTITY_PARAMS_UNK3D8(entity->params, kind);
    f32 direction[4];
    s32 elapsed;

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, kind + 0x17, 0, entity->params->unkC4 >> 1, 1);
        if (entity->params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->bits11 = (ENTITY_PARAMS_BITS(entity)->bit25 && !ENTITY_PARAMS_BITS(entity)->bit26) ? 2 : 1;
        }
        ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
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
        func_00216778(direction, &D_00413F80);
        direction[1] -= 30.0f;
        func_0018CF30(entity, unk3D8, elapsed, entity->model->unkFC->unk82, direction, 0.7853982f, 0.17453294f);
        if (entity->model->unkFC->unk82 > 0 && unk3D8->unk03 >= entity->model->unkFC->unk82) {
            entity->flags |= 0x80000;
        } else {
            entity->flags &= ~0x80000;
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

/* Action: plays animation 3 until the entity is far enough from its spawn point, then goes back to its handler. */
static void func_001295B0(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 0x20;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 3, 0, 0, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        ENTITY_FLAG_BITS(entity)->bit22 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit23 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit24 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit28 = TRUE;
        entity->action_step++;
        break;
    case 1:
        if (func_00194F80(entity->model->position, entity->spawn_info) >= (f32)entity->params->unk430) {
            ENTITY_FLAG_BITS(entity)->bit22 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit23 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit24 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
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

static void func_00129738(Entity *entity) {
    func_00190A30((char *)entity);
    entity->target = NULL;
    func_001958B8(entity, func_00129CA0, 0);
    func_00195898((char *)entity, 2);
}

/* Action: dies. Levels out, plays the death animation, drops what it carries and fades away. */
static void func_00129780(Entity *entity) {
    entity->unkC6 = 0x40;
    func_00195388(&entity->model->unkD0[0], 0.0f, 6.0f);
    func_00195388(&entity->model->unkD0[2], 0.0f, 6.0f);
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 0x10, 0, 5, 1);
        if (entity->params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->bits11 = (ENTITY_PARAMS_BITS(entity)->bit25 && !ENTITY_PARAMS_BITS(entity)->bit26) ? 2 : 1;
        }
        ENTITY_FLAG_BITS(entity)->bit14 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit20 = TRUE;
        ENTITY_FLAG_BITS(entity)->bits2 = 1;
        ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit27 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit22 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit23 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit24 = FALSE;
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
            ENTITY_FLAG_BITS(entity)->bit27 = TRUE;
            ENTITY_FLAG_BITS(entity)->bit26 = TRUE;
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

/* Action: drops along its own up axis until it lands on a floor (awake) or on something else (asleep). */
static void func_001299F8(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 landing[4];
    f32 direction[4];
    f32 matrix[16];
    s32 hit;

    ENTITY_FLAG_BITS(entity)->bit27 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit22 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit23 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit24 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit28 = TRUE;
    direction[0] = 0.0f;
    direction[1] = 1.0f;
    direction[2] = 0.0f;
    direction[3] = 0.0f;
    entity->unkC6 = 0x1000;
    sceVu0RotMatrixZ(matrix, D_00431AF0, entity->model->unkD0[2]);
    sceVu0RotMatrixX(matrix, matrix, entity->model->unkD0[0]);
    sceVu0RotMatrixY(matrix, matrix, entity->model->unkD0[1]);
    sceVu0ApplyMatrix(direction, (int *)matrix, direction);
    hit = func_0018AE98(entity->model->position, direction, landing, 200.0f);
    if (hit == 1) {
        func_00216778(entity->model->position, landing);
        ENTITY_FLAG_BITS(entity)->bit27 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit22 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit23 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit24 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
        unkE0->unk7A = 0;
        unkE0->unk08 = (void (*)(void))func_00182BD0;
        unkE0->unk14 = (void (*)(void))func_001818B8;
        entity->unkC6 = 0;
        entity->unkC8 = 0;
        entity->unkA0 = 0;
        if (ENTITY_FLAG_BITS(entity)->bit5) {
            unkE0->unk00(entity, 1);
        } else {
            func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, 1);
        }
    } else if (hit == 2 || hit == 3) {
        func_00216778(entity->model->position, landing);
        unkE0->unk48 = landing[0];
        unkE0->unk4C = landing[1];
        unkE0->unk50 = landing[2];
        ENTITY_FLAG_BITS(entity)->bit23 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
        unkE0->unk7A = 1;
        unkE0->unk08 = (void (*)(void))func_00129738;
        unkE0->unk14 = (void (*)(void))func_00129780;
        entity->unkC6 = 0;
        entity->unkC8 = 0;
        entity->unkA0 = 0;
        if (ENTITY_FLAG_BITS(entity)->bit5) {
            unkE0->unk00(entity, 1);
        } else {
            func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, 1);
        }
    } else {
        Vector_Scale(direction, direction, 20.0f);
        func_00216C48(entity->model->position, entity->model->position, direction);
    }
}

/* Action: levels the model out (pitch and roll back to 0), then goes back to its handler. */
static void func_00129CA0(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 pitch_left;
    f32 roll_left;

    ENTITY_FLAG_BITS(entity)->bit27 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit22 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit23 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit24 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit14 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
    entity->unkC6 = 0x2000;
    pitch_left = func_00195388(&entity->model->unkD0[0], 0.0f, 6.0f);
    roll_left = func_00195388(&entity->model->unkD0[2], 0.0f, 6.0f);
    if (entity->flags & (0x8200L << 35)) {
        pitch_left = ABS(pitch_left);
        roll_left = ABS(roll_left);
        if (ABS(pitch_left) < 0.01f && ABS(roll_left) < 0.01f) {
            entity->model->unkD0[0] = 0.0f;
            entity->model->unkD0[2] = 0.0f;
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            ENTITY_FLAG_BITS(entity)->bit14 = TRUE;
            ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
            unkE0->unk08 = (void (*)(void))func_00182BD0;
            unkE0->unk14 = (void (*)(void))func_001818B8;
            func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, 1);
        }
    }
}
