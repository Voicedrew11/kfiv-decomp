/* The behaviour of entity type 1 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers that function installs. */
#include "kf_entity.h"
#include "common.h"
#include "kf_player.h"

void func_001958A8(char *, s32);
int func_001958B8(Entity *, void (*)(Entity *), int);
void func_00191478(EntityModel *);
int func_00193378(EntityModel *, s32, s32);
void func_00181038(Entity *);
void func_001818B8(Entity *);
extern s32 D_003E6E40[][2];
int func_00192EB0(void *, int);
// match: the callee is void, but retail keeps what it leaves in $2
int func_00194368(EntityModel *, f32);
void func_00190A30(char *);
int func_0019A310(char *, void (*)(void), int);
void func_00195898(char *, u8);
void func_001816A0(Entity *);
extern u32 D_00446ED4;
void func_0018E9C0(void *);
void func_00196870(void *, int);
void func_00196CC0(void *);
int rand(void);
void func_0018B778(float, void *, void *, void *);
float func_00194F80(void *, void *);
float func_00195818(void);
float func_001957B8(float, float);
extern PlayerState D_00413F80;
s32 func_0018ED98(Entity *, f32, f32);
s32 func_0018FD50(Entity *, s32);
void func_00190B88(Entity *, s32);
int func_0018F030(char *);
void func_00181C60(Entity *);
void func_00182240(Entity *);
extern f32 D_00431AF0[16];
void sceVu0RotMatrixY(float *, float *, float);
void func_00216BC0(void *, void *, void *);
void func_00216D20(void *, void *);
void Vector_Scale(float *, float *, float);
float func_00199E68(Entity *, float);
float func_001954A8(void *, void *);
int func_00192C98(void *, int, int, u32, int);
s32 func_00199EB8(Entity *, f32 *, f32);
s32 func_00195AC0(Entity *, void *, s32, s32, s32);
void func_0019A290(Entity *, s32);

static void func_00126A08(Entity *entity);
static void func_00126AF8(Entity *entity);
static void func_00126B38(Entity *entity, s32 unknown);
static void func_00126C28(Entity *entity, s32 unknown);
static void func_00126C70(Entity *entity, s32 unknown);
static void func_00126E58(Entity *entity, s32 unknown);
static void func_00127158(Entity *entity);
static void func_001273D8(Entity *entity);

/* Init of the default enemy kind: clears the work area, fills the handler table, starts walking or its action. */
/* Set-up of a type 1 entity: clears its work area and installs the type's handlers. */
void EntityType1_Init(Entity *entity) {
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
    unkE0->unk00 = (void (*)(Entity *, s32))func_00126B38;
    unkE0->unk04 = (void (*)(void))func_00126A08;
    unkE0->unk08 = (void (*)(void))func_00126AF8;
    unkE0->unk18 = (void (*)(void))func_00127158;
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
    unkE0->unk40 = 0.5f;
    entity->unkD6 = (entity->params->flags & 0x4000) ? 0x40 : 0x80;
    entity->model->unk15C = entity->unkD6;
    func_00191478(entity->model);
    func_00193378(entity->model, 0, 0);
}

static void func_00126A08(Entity *entity) {
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

static void func_00126AF8(Entity *entity) {
    func_00190A30((char *)entity);
    func_0019A310((char *)entity, (void (*)(void))func_001816A0, 0);
    func_00195898((char *)entity, 2);
}

static void func_00126B38(Entity *entity, s32 unknown) {
    u32 phase;

    phase = D_00446ED4 % 10U;
    if (phase == entity->unkD7 % 10U) {
        func_00196870(entity, 0);
    }
    if (entity->unkC6 != 0) {
        return;
    }
    func_00196CC0(entity);
    func_0018E9C0(entity);
    if (ENTITY_FLAG_BITS(entity)->bit53) {
        func_00126C28(entity, unknown);
        return;
    }
    if (entity->state == 2) {
        func_00126E58(entity, unknown);
        return;
    }
    func_00126C70(entity, unknown);
}

/* Sends the entity back to its spawn point with the walk handler of its kind. */
static void func_00126C28(Entity *entity, s32 unknown) {
    f32 unused[8]; // match: retail reserves 0x20 bytes, which also keeps the call from being a jump

    ENTITY_FLAG_BITS(entity)->bits11 = 0;
    entity->target = entity->spawn_info;
    entity->unkC8 = 1;
    entity->unkA0 = 0;
    func_001958B8(entity, (void (*)(Entity *))entity->unkE0.unk18, unknown);
}

/* wander: every so often walk to a point near the spawn point, turn to a random heading, or stand still */
static void func_00126C70(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    s32 roll;
    f32 turn;

    ENTITY_FLAG_BITS(entity)->bits11 = 0;
    if (entity->unkA0 == 0) {
        roll = rand() % 100;
        if (roll > 60) {
            func_0018B778(entity->spawn_info->unk4C, entity->unkE0.unk30, entity->spawn_info, entity->model->position);
            entity->target = entity->unkE0.unk30;
            entity->unkA0 = rand() % 120 + 240;
            unkE0->unk78 = 0;
        } else if (roll > 40) {
            unkE0->unk48 = func_00195818();
            entity->unkA0 = rand() % 30 + 60;
            unkE0->unk78 = 1;
        } else {
            entity->target = NULL;
            entity->unkA0 = rand() % 30 + 60;
            unkE0->unk78 = 2;
        }
    }
    if (unkE0->unk78 == 0) {
        if (entity->target != NULL) {
            if (func_00194F80(entity->model->position, entity->target) > 50.0f) {
                entity->unkC8 = 1;
            }
        }
    } else if (unkE0->unk78 == 1) {
        turn = func_001957B8(entity->model->unkD0[1], unkE0->unk48);
        if (ABS(turn) > entity->params->turn_limit) {
            entity->unkC8 = (turn >= 0.0f) ? 0x200 : 0x100;
        }
    }
    func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, unknown);
}

/* chase: attack when in reach, else go for the player or a point near the spawn point for a while */
static void func_00126E58(Entity *entity, s32 unknown) {
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
                func_001958B8(entity, func_00182240, unknown);
                return;
            }
            func_001958B8(entity, func_001273D8, unknown);
            return;
        case 1:
            if (rand() % 100 < entity->params->unk430) {
                entity->target = &D_00413F80;
                entity->unkA0 = rand() % 120 + 480;
                unkE0->unk78 = 0;
            } else {
                func_0018B778(entity->spawn_info->unk4C, entity->unkE0.unk30, entity->spawn_info, entity->model->position);
                entity->target = entity->unkE0.unk30;
                entity->unkA0 = rand() % 120 + 480;
                unkE0->unk78 = 1;
            }
            break;
        case 2:
            entity->unkA0 = rand() % 60 + 120;
            unkE0->unk78 = -1;
            break;
        default:
            unkE0->unk78 = -1;
            break;
        }
    }
    switch (unkE0->unk78) {
    case 0:
        if (entity->target != NULL) {
            if (func_00194F80(entity->model->position, entity->target) > entity->radius + 100.0f + 30.0f) {
                entity->unkC8 = 1;
            }
        }
        break;
    case 1:
        if (entity->target != NULL) {
            if (func_00194F80(entity->model->position, entity->target) > 50.0f) {
                entity->unkC8 = 1;
            }
        }
        break;
    }
    func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, unknown);
}

/* movement of the default kind: turn as the command bits in Entity.unkC8 say and walk towards the target */
static void func_00127158(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 *vector;
    f32 turn = 0.0f;
    f32 amount;
    f32 heading;

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
    if ((entity->unkC8 & 1) && entity->target != NULL) {
        vector = ENTITY_SCRATCH_VECTOR;
        vector[0] = vector[1] = vector[3] = 0.0f;
        amount = -1.0f;
        vector[2] = amount;
        unkE0->unk44 = func_00199E68(entity, unkE0->unk44);
        amount = unkE0->unk44;
        if (entity->unkC8 & 0x40) {
            amount += amount;
        }
        heading = func_001954A8(entity->model->position, entity->target);
        sceVu0RotMatrixY(ENTITY_SCRATCH_MATRIX, D_00431AF0, heading);
        func_00216BC0(ENTITY_SCRATCH_VECTOR, ENTITY_SCRATCH_MATRIX, ENTITY_SCRATCH_VECTOR);
        func_00216D20(ENTITY_SCRATCH_VECTOR, ENTITY_SCRATCH_VECTOR);
        Vector_Scale(ENTITY_SCRATCH_VECTOR, ENTITY_SCRATCH_VECTOR, amount);
        entity->unk84 += ENTITY_SCRATCH_VECTOR[0];
        entity->unk88 += ENTITY_SCRATCH_VECTOR[1];
        entity->unk8C += ENTITY_SCRATCH_VECTOR[2];
    } else {
        unkE0->unk44 = 0.0f;
    }
    if (entity->unkC8 & 1) {
        if (entity->unkC8 & 0x40) {
            func_00192C98(entity, 4, 0, entity->params->unkC4 >> 1, 0);
        } else {
            func_00192C98(entity, 3, 0, entity->params->unkC4, 0);
        }
    } else if (entity->unkC8 & 0x300) {
        func_00192C98(entity, 0, 0, entity->params->unkC4, 0);
    } else {
        func_00192C98(entity, 0, 0, entity->params->unkC4, 0);
    }
}

/* Action: one of the melee attacks; a hit may also clear the player's first timed flag. */
static void func_001273D8(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityParamsUnk240 *unk240 = &entity->params->unk240[ENTITY_FLAG_BITS(entity)->bits58];
    s32 frame;

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, ENTITY_FLAG_BITS(entity)->bits58 | 0x14, 0, entity->params->unkC4 >> 1, 1);
        if (entity->params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->bits11 = (((EntityParamsWord *)entity->params)->unk00 & 0x6000000) == 0x2000000 ? 2 : 1;
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
            func_00199EB8(entity, &unkE0->unk1C, entity->unk68);
        }
        break;
    case 2:
        if (unk240->unk01 >= entity->model->unkFC->unk82) {
            func_00199EB8(entity, &entity->unkE0.unk1C, entity->unk68);
        } else {
            unkE0->unk1C = 0.0f;
        }
        frame = (entity->model->unkFC->unk84 - entity->params->unk80[entity->unkD1]) >> 8;
        if (!ENTITY_FLAG_BITS(entity)->bit54) {
            ENTITY_FLAG_BITS(entity)->bit54 = func_00195AC0(entity, unk240, 0, frame, entity->model->unkFC->unk82);
            if (ENTITY_FLAG_BITS(entity)->bit54 && rand() % 100 < entity->params->unk432) {
                D_00413F80.timed_flags[0] = 0;
            }
        }
        if (!ENTITY_FLAG_BITS(entity)->bit55) {
            ENTITY_FLAG_BITS(entity)->bit55 = func_00195AC0(entity, unk240, 1, frame, entity->model->unkFC->unk82);
            if (ENTITY_FLAG_BITS(entity)->bit55 && rand() % 100 < entity->params->unk432) {
                D_00413F80.timed_flags[0] = 0;
            }
        }
        if (!ENTITY_FLAG_BITS(entity)->bit56) {
            ENTITY_FLAG_BITS(entity)->bit56 = func_00195AC0(entity, unk240, 2, frame, entity->model->unkFC->unk82);
            if (ENTITY_FLAG_BITS(entity)->bit56 && rand() % 100 < entity->params->unk432) {
                D_00413F80.timed_flags[0] = 0;
            }
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
