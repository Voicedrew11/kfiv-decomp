/* The behaviour of entity type 17 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers that function installs. */
#include "kf_entity.h"
#include "common.h"

#define ENTITY_WORK_AREA_SIZE 0xA0
void func_001958A8(char *, s32);
int func_001958B8(Entity *, void (*)(Entity *), int);
void func_00191478(EntityModel *);
int func_00193378(EntityModel *, s32, s32);
void func_00182BD0(char *);
void func_00181038(Entity *);
void func_001818B8(Entity *);
extern s32 D_003E6E40[][2];
int func_00192EB0(void *, int);
// match: the callee is void, but retail keeps what it leaves in $2
int func_00194368(EntityModel *, f32);
extern u32 D_00446ED4;
void func_00196870(void *, int);
void func_00196C28(char *);
void func_0018E9C0(void *);
f32 func_0018B518(f32 *);
int func_0018EAC0(void *, void *);
s32 func_00190D50(Entity *, s32, f32);
void func_00216D20(void *, void *);
void func_00216CD8(void *, void *, void *);
void Vector_Subtract(void *, void *, void *);
int rand(void);
void func_0018B6F8(f32 *, f32, f32, void *, void *);
int func_00191340(Entity *, int);
extern PlayerData D_00413F80;
s32 func_00197078(Entity *);
int func_0018F030(char *);
float func_001957B8(float, float);
void func_00216778(void *, void *);
f32 func_00194F48(f32 *, f32 *);
extern f32 D_00431AF0[16];
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0RotMatrixZ(float *, float *, float);
void func_00216BC0(void *, void *, void *);
void Vector_Scale(float *, float *, float);
float func_00199E68(Entity *, float);
int func_00192C98(void *, int, int, u32, int);
s32 func_00199EB8(Entity *, f32 *, f32);
s32 func_00195AC0(Entity *, void *, s32, s32, s32);
void func_0019A290(Entity *, s32);
int func_00195ED0(Entity *, EntityParamsUnk3D8 *, s32, s32);

static void func_0012BC28(Entity *entity);
static void func_0012BD18(Entity *entity, s32 unknown);
static void func_0012BE50(Entity *entity, s32 unknown);
static void func_0012C088(Entity *entity, s32 unknown);
static void func_0012C1D0(Entity *entity, s32 unknown);
static void func_0012C7B8(Entity *entity);
static void func_0012CCA0(Entity *entity);
static void func_0012D040(Entity *entity);

/* Set-up of a type 17 entity: clears its work area and installs the type's handlers. */
void EntityType17_Init(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    u8 *unkBC = entity->unkBC;
    u8 *unkC0 = entity->unkC0;
    s32 i;

    // match: in the three loops the pointer has to be formed inside the loop
    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        u8 *dest = (u8 *)&entity->unkE0;

        dest[i] = 0;
    }
    func_001958A8((char *)entity, D_003E6E40[entity->params->unk02][1]);
    unkE0->unk00 = (void (*)(Entity *, s32))func_0012BD18;
    unkE0->unk04 = (void (*)(void))func_0012BC28;
    unkE0->unk08 = (void (*)(void))func_00182BD0;
    unkE0->unk18 = (void (*)(void))func_0012C7B8;
    unkE0->unk10 = (void (*)(void))func_00181038;
    unkE0->unk14 = (void (*)(void))func_001818B8;
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
    ENTITY_FLAG_BITS(entity)->bit28 = TRUE;
    unkE0->unk40 = 0.5f;
    entity->unkD6 = (entity->params->flags & 0x4000) ? 0x40 : 0x80;
    entity->model->unk15C = entity->unkD6;
    func_00191478(entity->model);
    func_00193378(entity->model, 0, 0);
}

static void func_0012BC28(Entity *entity) {
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
}

static void func_0012BD18(Entity *entity, s32 unknown) {
    f32 floor_distance;

    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        func_00196870(entity, 1);
    }
    if (entity->unkC6 != 0) {
        return;
    }
    func_00196C28((char *)entity);
    func_0018E9C0(entity);
    floor_distance = func_0018B518(entity->model->position);
    if (floor_distance < entity->unk30) {
        entity->model->position[1] -= entity->unk30 - floor_distance;
    }
    if (ENTITY_FLAG_BITS(entity)->bit53) {
        func_0012BE50(entity, unknown);
        return;
    }
    switch (entity->state) {
    case 0:
    case 1:
        break;
    case 2:
        func_0012C1D0(entity, unknown);
        return;
    }
    func_0012C088(entity, unknown);
}

/* walk back to the spawn point, turning away from whatever is pushing the entity */
static void func_0012BE50(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 to_spawn[4];
    f32 push[4];
    f32 direction[4];
    s32 turn;

    entity->unkA0 = 0;
    ENTITY_FLAG_BITS(entity)->bits11 = 0;
    turn = func_0018EAC0(entity, direction);
    if (turn == 0) {
        entity->target = entity->spawn_info;
        entity->unkC8 = func_00190D50(entity, 1, 50.0f);
    } else {
        push[0] = push[1] = push[2] = push[3] = 0.0f;
        if (entity->flags & (0xC000L << 31)) {
            push[0] += entity->unkA4[0];
            push[1] += entity->unkA4[1];
            push[2] += entity->unkA4[2];
        }
        if ((s32)(entity->flags >> 48) & 1) {
            push[0] += entity->unkB0[0];
            push[1] += entity->unkB0[1];
            push[2] += entity->unkB0[2];
        }
        func_00216D20(push, push);
        Vector_Subtract(to_spawn, entity->spawn_info, entity->model->position);
        func_00216D20(to_spawn, to_spawn);
        func_00216CD8(to_spawn, push, to_spawn);
        if (to_spawn[1] <= 0.0f) {
            if (turn == -1) {
                turn = 1;
            }
        } else if (turn == 1) {
            turn = -1;
        }
        entity->target = entity->spawn_info;
        if (turn > 0) {
            entity->unkC8 = 0x241;
            if (push[1] > 0.0f) {
                entity->unkC8 = 0x261;
            } else if (push[1] < 0.0f) {
                entity->unkC8 = 0x251;
            }
        } else if (turn < 0) {
            entity->unkC8 = 0x141;
            if (push[1] > 0.0f) {
                entity->unkC8 = 0x161;
            } else if (push[1] < 0.0f) {
                entity->unkC8 = 0x151;
            }
        } else {
            entity->unkC8 = func_00190D50(entity, 1, 50.0f);
        }
    }
    func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, unknown);
}

static void func_0012C088(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    s32 turn;

    ENTITY_FLAG_BITS(entity)->bits11 = 0;
    if (entity->unkA0 == 0) {
        if (rand() % 100 > 20) {
            func_0018B6F8(unkE0->unk30, entity->spawn_info->unk4C, entity->spawn_info->unk56, entity->spawn_info, entity->model->position);
            entity->target = unkE0->unk30;
            entity->unkA0 = rand() % 60 + 120;
        } else {
            entity->target = NULL;
            entity->unkA0 = rand() % 30 + 30;
        }
    }
    turn = func_0018EAC0(entity, direction);
    if (turn != 0) {
        entity->unkC8 = func_00191340(entity, turn);
    } else {
        entity->unkC8 = func_00190D50(entity, 1, 50.0f);
    }
    func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, unknown);
}

static void func_0012C1D0(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 target[4];
    f32 direction[4];
    f32 delta; /* angle to the wanted heading, then height to the wanted level */
    s32 turn;
    s32 roll;

    ENTITY_FLAG_BITS(entity)->bits11 = 0;
    if (unkE0->unk78 != 4 && entity->unkA2 == 0 && ((int (*)(Entity *))func_00197078)(entity)) { // match: the entity is passed
        if (func_0018F030((char *)entity)) {
            func_001958B8(entity, func_0012D040, unknown);
        } else {
            func_001958B8(entity, func_0012CCA0, unknown);
        }
        unkE0->unk48 = 0.0f;
        unkE0->unk78 = 3;
        return;
    }
    if (unkE0->unk78 == 0) {
        if (rand() % 100 < 75) {
            roll = rand() % 60;
            unkE0->unk78 = 1;
            unkE0->unk7B = roll;
        } else {
            unkE0->unk4C = D_00413F80.position[1] - 220.0f - (f32)(rand() % 700);
            if (entity->radius + 100.0f + 200.0f < entity->distance_to_player) {
                unkE0->unk7A = 1;
            } else {
                unkE0->unk7A = 0;
            }
            entity->unkA0 = 120;
            unkE0->unk78 = 2;
        }
    }
    turn = func_0018EAC0(entity, direction);
    if (turn != 0) {
        entity->unkC8 = func_00191340(entity, turn);
    } else {
        switch (unkE0->unk78) {
        case 1:
            delta = func_001957B8(entity->model->unkD0[1], entity->unk68);
            if (ABS(delta) > entity->params->turn_limit) {
                if (delta >= 0.0f) {
                    entity->unkC8 |= 0x200;
                } else {
                    entity->unkC8 |= 0x100;
                }
            }
            delta = D_00413F80.position[1] - 190.0f + (f32)unkE0->unk7B - entity->model->position[1];
            if (delta > 10.0f) {
                entity->unkC8 |= 0x20;
            } else if (delta < -10.0f) {
                entity->unkC8 |= 0x10;
            }
            entity->unkC8 |= 1;
            if (entity->radius + 100.0f + 100.0f < entity->distance_to_player) {
                entity->unkC8 |= 0x40;
            }
            break;
        case 2:
            delta = func_001957B8(entity->model->unkD0[1], entity->unk68);
            if (ABS(delta) > entity->params->turn_limit) {
                if (delta >= 0.0f) {
                    entity->unkC8 |= 0x200;
                } else {
                    entity->unkC8 |= 0x100;
                }
            }
            delta = unkE0->unk4C - entity->model->position[1];
            if (delta > 30.0f) {
                entity->unkC8 |= 0x20;
            } else if (delta < -30.0f) {
                entity->unkC8 |= 0x10;
            }
            entity->unkC8 |= 1;
            if (unkE0->unk7A != 0) {
                entity->unkC8 |= 0x40;
            }
            func_00216778(target, &D_00413F80);
            target[1] = unkE0->unk4C;
            // match: the shared prototype has no result; it returns the distance
            if (((f32 (*)(f32 *, f32 *))func_00194F48)(target, entity->model->position) <= 200.0f) {
                unkE0->unk78 = 0;
            }
            break;
        case 3:
            delta = func_001957B8(entity->model->unkD0[1], entity->unk68);
            if (ABS(delta) > entity->params->turn_limit) {
                if (delta >= 0.0f) {
                    entity->unkC8 |= 0x200;
                } else {
                    entity->unkC8 |= 0x100;
                }
            }
            if (entity->radius + 100.0f + 100.0f < entity->distance_to_player) {
                entity->unkC8 |= 0x41;
            }
            unkE0->unk68++;
            if (unkE0->unk68 > 90) {
                unkE0->unk68 = 0;
                unkE0->unk78 = 4;
            }
            break;
        case 4:
            delta = func_001957B8(entity->model->unkD0[1], entity->unk68);
            if (ABS(delta) > entity->params->turn_limit) {
                if (delta >= 0.0f) {
                    entity->unkC8 |= 0x200;
                } else {
                    entity->unkC8 |= 0x100;
                }
            }
            delta = D_00413F80.position[1] - entity->model->position[1];
            entity->unkC8 |= 0x51;
            if (delta > 700.0f) {
                unkE0->unk78 = 0;
            }
            break;
        default:
            unkE0->unk78 = 0;
            break;
        }
    }
    if (entity->handler != (void (*)(Entity *))unkE0->unk18) {
        func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, unknown);
    }
}

/* movement of a flying kind: turn, climb and move as the command bits in Entity.unkC8 say, and pick the animation */
static void func_0012C7B8(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 *vector;
    f32 turn;
    f32 amount;
    f32 climb_step;

    vector = ENTITY_SCRATCH_VECTOR;
    turn = vector[0] = vector[1] = vector[2] = vector[3] = 0.0f;
    climb_step = 0.02f;
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
        unkE0->unk1C = turn;
    }
    turn = unkE0->unk1C;
    if (entity->unkC8 & 0x400) {
        turn += turn;
    }
    entity->unk94 += turn;
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
    if (entity->unkC8 & 0x30) {
        amount = 0.0f;
        if (entity->unkC8 & 0x10) {
            unkE0->unk48 -= climb_step;
            if (unkE0->unk48 < -1.0f) {
                unkE0->unk48 = -1.0f;
            }
            amount = unkE0->unk48;
        } else if (entity->unkC8 & 0x20) {
            unkE0->unk48 += climb_step;
            if (1.0f < unkE0->unk48) {
                unkE0->unk48 = 1.0f;
            }
            amount = unkE0->unk48;
        }
        vector[1] += amount;
    }
    if (entity->unkC8 & 0x3F) {
        unkE0->unk44 = func_00199E68(entity, unkE0->unk44);
        amount = unkE0->unk44;
        if (entity->unkC8 & 0x40) {
            amount += amount;
        }
        sceVu0RotMatrixZ(ENTITY_SCRATCH_MATRIX, D_00431AF0, entity->model->unkD0[2]);
        sceVu0RotMatrixX(ENTITY_SCRATCH_MATRIX, ENTITY_SCRATCH_MATRIX, entity->model->unkD0[0]);
        sceVu0RotMatrixY(ENTITY_SCRATCH_MATRIX, ENTITY_SCRATCH_MATRIX, entity->model->unkD0[1]);
        func_00216BC0(ENTITY_SCRATCH_VECTOR, ENTITY_SCRATCH_MATRIX, ENTITY_SCRATCH_VECTOR);
        func_00216D20(ENTITY_SCRATCH_VECTOR, ENTITY_SCRATCH_VECTOR);
        Vector_Scale(ENTITY_SCRATCH_VECTOR, ENTITY_SCRATCH_VECTOR, amount);
        entity->unk84 += ENTITY_SCRATCH_VECTOR[0];
        entity->unk88 += ENTITY_SCRATCH_VECTOR[1];
        entity->unk8C += ENTITY_SCRATCH_VECTOR[2];
    } else {
        unkE0->unk44 = 0.0f;
        unkE0->unk48 = 0.0f;
    }
    if (entity->unkC8 & 1) {
        if (entity->unkC8 & 0x40) {
            func_00192C98(entity, 4, 0, (s32)((f32)entity->params->unkC4 * 0.5f), 0);
        } else {
            func_00192C98(entity, 3, 0, entity->params->unkC4, 0);
        }
    } else if (entity->unkC8 & 2) {
        func_00192C98(entity, 5, 0, entity->params->unkC4, 0);
    } else if (entity->unkC8 & 0x10) {
        func_00192C98(entity, 8, 0, entity->params->unkC4, 0);
    } else if (entity->unkC8 & 0x20) {
        func_00192C98(entity, 9, 0, entity->params->unkC4, 0);
    } else if (entity->unkC8 & 4) {
        func_00192C98(entity, 6, 0, entity->params->unkC4, 0);
    } else if (entity->unkC8 & 8) {
        func_00192C98(entity, 7, 0, entity->params->unkC4, 0);
    } else if (entity->unkC8 & 0x300) {
        func_00192C98(entity, 3, 0, entity->params->unkC4, 0);
    } else {
        func_00192C98(entity, 0, 0, entity->params->unkC4, 0);
    }
}

static void func_0012CCA0(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityParamsUnk240 *unk240 = &entity->params->unk240[ENTITY_FLAG_BITS(entity)->bits58];
    s32 frame;

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, ENTITY_FLAG_BITS(entity)->bits58 | 0x14, 0, entity->params->unkC4 >> 1, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
        ENTITY_FLAG_BITS(entity)->bit54 = 0;
        ENTITY_FLAG_BITS(entity)->bit55 = 0;
        ENTITY_FLAG_BITS(entity)->bit56 = 0;
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
            func_00199EB8(entity, &unkE0->unk1C, entity->unk68);
        } else {
            unkE0->unk1C = 0.0f;
        }
        frame = (entity->model->unkFC->unk84 - entity->params->unk80[entity->unkD1]) >> 8;
        if (!ENTITY_FLAG_BITS(entity)->bit54) {
            ENTITY_FLAG_BITS(entity)->bit54 = func_00195AC0(entity, unk240, 0, frame, entity->model->unkFC->unk82);
        }
        if (!ENTITY_FLAG_BITS(entity)->bit55) {
            ENTITY_FLAG_BITS(entity)->bit55 = func_00195AC0(entity, unk240, 1, frame, entity->model->unkFC->unk82);
        }
        if (!ENTITY_FLAG_BITS(entity)->bit56) {
            ENTITY_FLAG_BITS(entity)->bit56 = func_00195AC0(entity, unk240, 2, frame, entity->model->unkFC->unk82);
        }
        if (entity->model->unkFC->unk82 > 0 && unk240->unk03 >= entity->model->unkFC->unk82) {
            ENTITY_FLAG_BITS(entity)->bit19 = 1;
        } else {
            ENTITY_FLAG_BITS(entity)->bit19 = 0;
        }
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            func_0019A290(entity, unk240->unk02);
            if (ENTITY_FLAG_BITS(entity)->bit5) {
                unkE0->unk00(entity, 1);
            } else {
                func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, 1);
            }
        }
        break;
    }
}

static void func_0012D040(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    s32 kind = (s32)(entity->flags >> 58) & 3;
    EntityParamsUnk3D8 *unk3D8 = ENTITY_PARAMS_UNK3D8(entity->params, kind);

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, kind + 0x17, 0, entity->params->unkC4 >> 1, 1);
        ENTITY_FLAGS(entity) &= ~0x3800;
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
            func_00199EB8(entity, &unkE0->unk1C, entity->unk68);
        } else {
            unkE0->unk1C = 0.0f;
        }
        func_00195ED0(entity, unk3D8,
                      (entity->model->unkFC->unk84 - entity->params->unk80[entity->unkD1]) >> 8,
                      entity->model->unkFC->unk82);
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
