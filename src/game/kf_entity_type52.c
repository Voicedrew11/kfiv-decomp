/* The behaviour of entity type 52 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers that function installs. */
#include "kf_entity.h"
#include "common.h"

#define ENTITY_WORK_AREA_SIZE 0xA0
void func_001958A8(char *, s32);
int func_001958B8(Entity *entity, void (*handler)(Entity *), int run_now);
void func_00191478(EntityModel *);
int func_00193378(EntityModel *, s32, s32);
void func_00216778(void *, void *);
// match: the definition takes an Actor, a struct local to func_00182A08.c
void func_00182A08(void);
void func_00181038(Entity *);
extern s32 D_003E6E40[][2];
void func_00190A30(char *);
int func_0019A310(char *, void (*)(void), int); // match: an int result keeps $2 busy after the call
void func_00195898(char *, u8);
void func_00181378(Entity *);
void func_001814B0(Entity *);
void func_001816A0(Entity *);
extern u32 D_00446ED4;
// match: the size is unknown; a complete small type would be read through $gp
extern f32 D_00446834[];
void func_00196870(void *, int);
void func_00196CC0(void *);
void func_0018E9C0(void *);
int func_00182DE0(void *, int);
int func_00182FD8(void *, int);
void func_00183150(Entity *, s32);
f32 func_0018A9F0(void *, void *);
extern PlayerData D_00413F80;
int rand(void);
s32 func_0018ED98(Entity *, f32, f32);
s32 func_0018EEE0(Entity *, f32, f32);
s32 func_0018FD50(Entity *, s32);
void func_00190B88(Entity *, s32);
int func_0018F030(char *);
float func_00194FB8(float);
float func_00195478(float *, char *);
float func_001957B8(float, float);
s32 func_0018EC78(Entity *, f32);
int func_00190BE0(Entity *entity);
int func_0018EAC0(void *, void *);
s16 func_00190F90(float, void *, int); // match: this file only matches with this type; func_00190F90.c defines it differently
void func_00181AE0(Entity *);
void func_00181C60(Entity *);
void func_00196D48(Entity *);
int func_00192C98(void *, int, int, u32, int);
extern int D_00414184[];
extern f32 D_00431AF0[16];
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0RotMatrixZ(float *, float *, float);
void func_00216BC0(void *, void *, void *);
void Vector_Scale(float *, float *, float);
int func_00195AC0(Entity *, void *, s32, s32, s32);
void func_0019A290(Entity *, s32);
char *func_0010CAB8(char *, u32);
int func_00195E48(void *, int, void *, void *);
int func_00195ED0(Entity *, EntityParamsUnk3D8 *, s32, s32);
int func_0019A3F0(void *, int);
void func_0018DCD8(Entity *);

static void func_001701D8(Entity *entity);
static void func_00170280(Entity *entity, s32 unknown);
static void func_001704A8(Entity *entity, s32 unknown);
static void func_00170A08(Entity *entity);
static void func_00170AB8(Entity *entity);
static void func_00170EB0(Entity *entity);
static void func_001712B8(Entity *entity);
static void func_00171558(Entity *entity);
static void func_00171808(Entity *entity);

/* Set-up of a type 52 entity: clears its work area and installs the type's handlers. */
void EntityType52_Init(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
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
    unkE0->unk00 = func_00170280;
    unkE0->unk04 = func_00182A08;
    unkE0->unk08 = (void (*)(void))func_001701D8;
    unkE0->unk18 = (void (*)(void))func_00170A08;
    unkE0->unk10 = (void (*)(void))func_00181038;
    unkE0->unk14 = (void (*)(void))func_00171808;
    for (i = 3; i != -1; i--) {
        entity->unkBC[i] = 0x80;
    }
    for (i = 5; i != -1; i--) {
        entity->unkC0[i] = 0x80;
    }
    entity->target = NULL;
    entity->flags &= ~0x10000000;
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
    func_00193378(entity->model, 0, 0);
}

static void func_001701D8(Entity *entity) {
    func_00190A30((char *)entity);
    entity->flags &= ~0x200;
    if (((s32)(entity->flags >> 36) & 3) == 1) {
        if ((s32)(entity->flags >> 19) & 1) {
            func_0019A310((char *)entity, (void (*)(void))func_00181378, 0);
        } else {
            func_0019A310((char *)entity, (void (*)(void))func_001814B0, 0);
        }
    } else if (((s32)(entity->flags >> 36) & 3) == 2) {
        func_0019A310((char *)entity, (void (*)(void))func_001816A0, 0);
    }
    func_00195898((char *)entity, 2);
}

static void func_00170280(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 height;

    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        func_00196870(entity, 1);
    }
    if (entity->unkC6 == 0) {
        func_00196CC0(entity);
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
                func_001704A8(entity, unknown);
                break;
            default:
                func_00182FD8(entity, unknown);
                break;
            }
        }
    }
    if (entity->unkC6 & 0x46) {
        entity->flags &= ~0x10000000;
    } else {
        entity->flags |= 0x10000000;
    }
    if ((s32)(entity->flags >> 28) & 1) {
        height = func_0018A9F0(entity->unk14, entity->model->position);
        if (height != D_00446834[0]) {
            if ((f32)entity->params->unk43C < height) {
                unkE0->unk7A = 1;
            } else if (height < (f32)entity->params->unk43E) {
                unkE0->unk7A = 0;
            }
            if (unkE0->unk7A != 0) {
                entity->unkC8 |= 0x20;
            } else {
                entity->unkC8 |= 0x10;
            }
        }
    }
    if (entity->unkC6 == 0 || (entity->unkC6 & 6)) {
        func_00170AB8(entity);
    }
}

static void func_001704A8(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    f32 angle;
    s32 side;
    s32 turn;

    ENTITY_FLAG_BITS(entity)->bits11 = 0;
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
                if (ENTITY_FLAG_BITS(entity)->bits58 == 0) {
                    func_001958B8(entity, func_001712B8, unknown);
                } else {
                    func_001958B8(entity, func_00171558, unknown);
                }
            } else {
                func_001958B8(entity, func_00170EB0, unknown);
            }
            return;
        case 1:
            entity->target = &D_00413F80;
            if ((f32)entity->params->unk438 < entity->distance_to_player) {
                unkE0->unk78 = 0;
            } else if (entity->distance_to_player < (f32)entity->params->unk436) {
                unkE0->unk78 = 1;
            } else if (func_001957B8(D_00413F80.unk14,
                                     func_00195478(D_00413F80.position, (char *)entity->model->position)) > 0.0f) {
                unkE0->unk78 = 2;
            } else {
                unkE0->unk78 = 3;
            }
            entity->unkA0 = entity->params->unk43A;
            break;
        case 2:
            entity->target = &D_00413F80;
            entity->unkA0 = rand() % 30 + 30;
            unkE0->unk78 = 4;
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
            switch (unkE0->unk78) {
            case 0:
                turn = func_0018EAC0(entity, direction);
                if (turn > 0) {
                    entity->unkC8 |= 0x201;
                } else if (turn < 0) {
                    entity->unkC8 |= 0x101;
                } else {
                    entity->unkC8 |= func_00190F90(entity->radius + 100.0f + 30.0f, entity, 0);
                }
                break;
            case 1:
                if (func_0018EC78(entity, func_00194FB8(entity->model->unkD0[1] + 3.1415927f))) {
                    unkE0->unk78 = rand() % 3 + 2;
                } else {
                    entity->unkC8 |= ((u16 (*)(Entity *))func_00190BE0)(entity); // match: read as unsigned here
                    entity->unkC8 |= 2;
                }
                break;
            case 2:
                if (func_0018EC78(entity, func_00194FB8(entity->model->unkD0[1] - 1.5707964f))) {
                    unkE0->unk78 = 3;
                } else {
                    entity->unkC8 |= ((u16 (*)(Entity *))func_00190BE0)(entity); // match: read as unsigned here
                    entity->unkC8 |= 4;
                    if ((f32)entity->params->unk438 < entity->distance_to_player) {
                        entity->unkC8 |= 1;
                    } else if (entity->distance_to_player < (f32)entity->params->unk436) {
                        entity->unkC8 |= 2;
                    }
                }
                break;
            case 3:
                if (func_0018EC78(entity, func_00194FB8(entity->model->unkD0[1] + 1.5707964f))) {
                    unkE0->unk78 = 2;
                } else {
                    entity->unkC8 |= ((u16 (*)(Entity *))func_00190BE0)(entity); // match: read as unsigned here
                    entity->unkC8 |= 8;
                    if ((f32)entity->params->unk438 < entity->distance_to_player) {
                        entity->unkC8 |= 1;
                    } else if (entity->distance_to_player < (f32)entity->params->unk436) {
                        entity->unkC8 |= 2;
                    }
                }
                break;
            case 4:
                entity->unkC8 |= func_00190BE0(entity);
                break;
            }
        }
    }
done:
    func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, unknown);
}

static void func_00170A08(Entity *entity) {
    if (entity == (Entity *)D_00414184[0]) {
        if (entity->params->flags & 0x800) {
            entity->unkC8 = 0;
        }
    }
    if (entity->unkC8 & 4) {
        func_00192C98(entity, 6, 0, entity->params->unkC4, 0);
        return;
    }
    if (entity->unkC8 & 8) {
        func_00192C98(entity, 7, 0, entity->params->unkC4, 0);
        return;
    }
    if (entity->unkC8 & 1) {
        func_00192C98(entity, 3, 0, entity->params->unkC4, 0);
        return;
    }
    if (entity->unkC8 & 2) {
        func_00192C98(entity, 5, 0, entity->params->unkC4, 0);
        return;
    }
    func_00192C98(entity, 0, 0, entity->params->unkC4, 0);
}

static void func_00170AB8(Entity *entity) {
    EntityUnkE0Kind16BF98 *unkE0 = (EntityUnkE0Kind16BF98 *)&entity->unkE0;
    f32 turn;
    f32 step;
    f32 limit;

    if (entity == D_00413F80.unk204 && (entity->params->flags & 0x800)) {
        entity->unkC8 = 0;
    }
    turn = 0.0f;
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
    } else if (unkE0->unk1C > entity->params->unk58) {
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
    ENTITY_SCRATCH_VECTOR[0] = ENTITY_SCRATCH_VECTOR[1] = ENTITY_SCRATCH_VECTOR[2] = ENTITY_SCRATCH_VECTOR[3] = 0.0f;
    step = entity->params->unk50 * 0.5f;
    limit = entity->params->unk54 * 0.2f;
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
    } else if (entity->params->unk50 < unkE0->unk4C) {
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
    } else if (entity->params->unk50 < unkE0->unk44) {
        unkE0->unk44 -= entity->params->unk50;
    } else if (unkE0->unk44 < -entity->params->unk50) {
        unkE0->unk44 += entity->params->unk50;
    } else {
        unkE0->unk44 = 0.0f;
    }
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

static void func_00170EB0(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    s32 kind = (s32)(entity->flags >> 58) & 3;
    EntityParamsUnk240 *unk240 = &entity->params->unk240[kind];
    s32 elapsed;

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, kind | 0x14, 0, entity->params->unkC4 >> 1, 1);
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
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        if (unk240->unk01 != 0) {
            entity->unkC8 |= func_00190BE0(entity);
        }
        break;
    case 2:
        if (unk240->unk01 >= entity->model->unkFC->unk82) {
            entity->unkC8 |= func_00190BE0(entity);
        }
        elapsed = (entity->model->unkFC->unk84 - entity->params->unk80[entity->unkD1]) >> 8;
        if (!((s32)(entity->flags >> 54) & 1)) {
            ENTITY_FLAG_BITS(entity)->bit54 = func_00195AC0(entity, unk240, 0, elapsed, entity->model->unkFC->unk82);
        }
        if (!((s32)(entity->flags >> 55) & 1)) {
            ENTITY_FLAG_BITS(entity)->bit55 = func_00195AC0(entity, unk240, 1, elapsed, entity->model->unkFC->unk82);
        }
        if (!((s32)(entity->flags >> 56) & 1)) {
            ENTITY_FLAG_BITS(entity)->bit56 = func_00195AC0(entity, unk240, 2, elapsed, entity->model->unkFC->unk82);
        }
        if (entity->model->unkFC->unk82 > 0 && unk240->unk03 >= entity->model->unkFC->unk82) {
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
                return;
            }
            func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, 1);
            return;
        }
        break;
    }
    func_00170AB8(entity);
}

static void func_001712B8(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityParamsUnk3D8Array *unk3D8 = entity->params->unk3D8;
    f32 target[4]; // match: never used, but retail keeps 16 bytes of stack for it
    char *point;

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 0x17, 0, entity->params->unkC4 >> 1, 1);
        if (entity->params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->bits11 = ((((EntityParamsWord *)entity->params)->unk00 & 0x06000000) == 0x02000000) ? 2 : 1;
        }
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        if (unk3D8->record.unk01 != 0) {
            entity->unkC8 |= func_00190BE0(entity);
        }
        break;
    case 2:
        if (unk3D8->record.unk01 >= entity->model->unkFC->unk82) {
            entity->unkC8 |= func_00190BE0(entity);
        }
        if (((s32)(entity->flags >> 58) & 3) == 0 && entity->model->unkFC->unk82 == 0x1E &&
            (entity->unk2A == 0 || unk3D8->unk0C != 0)) {
            point = func_0010CAB8((char *)entity->model, unk3D8->unk11);
            if (point != NULL) {
                func_00195E48(entity, unk3D8->unk10, point, entity->model->unkD0);
            }
        }
        if (entity->model->unkFC->unk82 > 0 && unk3D8->record.unk03 >= entity->model->unkFC->unk82) {
            entity->flags |= 0x80000;
        } else {
            entity->flags &= ~0x80000;
        }
        if ((s32)(entity->flags >> 34) & 1) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            func_0019A290(entity, unk3D8->record.unk02);
            if ((s32)(entity->flags >> 5) & 1) {
                unkE0->unk00(entity, 1);
            } else {
                func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, 1);
            }
            return;
        }
        break;
    }
    func_00170AB8(entity);
}

static void func_00171558(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    s32 kind = (s32)(entity->flags >> 58) & 3;
    EntityParamsUnk3D8 *unk3D8 = ENTITY_PARAMS_UNK3D8(entity->params, kind);

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, kind + 0x17, 0, entity->params->unkC4 >> 1, 1);
        if (entity->params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->bits11 = ((((EntityParamsWord *)entity->params)->unk00 & 0x06000000) == 0x02000000) ? 2 : 1;
        }
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        if (unk3D8->unk01 != 0) {
            entity->unkC8 |= func_00190BE0(entity);
        }
        break;
    case 2:
        if (unk3D8->unk01 >= entity->model->unkFC->unk82) {
            entity->unkC8 |= func_00190BE0(entity);
        }
        func_00195ED0(entity, unk3D8, (entity->model->unkFC->unk84 - entity->params->unk80[entity->unkD1]) >> 8,
                      entity->model->unkFC->unk82);
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
                return;
            }
            func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, 1);
            return;
        }
        break;
    }
    func_00170AB8(entity);
}

static void func_00171808(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

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
        entity->flags &= ~0x200;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if ((s16)entity->params->unk434 < entity->model->unkFC->unk82) {
            entity->unkA0 = unkE0->unk68 = func_0019A3F0(entity, entity->unkD1) - entity->model->unkFC->unk82;
            ENTITY_FLAG_BITS(entity)->bits2 = 2;
            ENTITY_FLAG_BITS(entity)->bit4 = 1;
            func_0018DCD8(entity);
            entity->action_step++;
        }
        break;
    case 3:
        entity->model->unk15C = (f32)entity->unkD6 * (f32)entity->unkA0 / (f32)unkE0->unk68;
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            ENTITY_FLAG_BITS(entity)->bit14 = 0;
            ENTITY_FLAG_BITS(entity)->bits2 = 3;
            ENTITY_FLAG_BITS(entity)->bit1 = 0;
        }
        break;
    }
}
