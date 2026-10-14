/* The behaviour of entity type 22 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers that function installs. */
#include "kf_entity.h"
#include "common.h"

void func_001958A8(char *, s32);
int func_001958B8(Entity *, void (*)(Entity *), int);
void func_00191478(EntityModel *);
int func_00193378(EntityModel *, s32, s32);
void func_00216778(void *, void *);
void func_001914B0(EntityModel *, s32, s32, s32);
extern s32 D_003E6E40[][2];
int func_0019A538(void *, void *, int);
s32 func_00191600(Entity *, EntityUnkE0SlotBits *, s32, s32);
void func_0018D670(Entity *, s32, s32, s32);
void func_0018D8B8(Entity *, s32, s32, s32);
int func_001960C0(void *, int, int, int, int, float);
int func_00196120(Entity *, void *, s32, void *, f32);
void func_00190A30(char *);
int func_0019A310(char *entity, void (*callback)(void), int run);
void func_00195898(char *, u8);
int func_0019A448(void *, void *, int, int, int, int, int);
void func_0019A290(Entity *, s32);
float func_00195388(float *, float, float);
void func_0018DCD8(Entity *);
s32 func_00199EB8(Entity *, f32 *, f32);
int func_00195AC0(Entity *, void *, s32, s32, s32);
extern u32 D_00446ED4;
int func_00195ED0(Entity *, EntityParamsUnk3D8 *, s32, s32);
void func_00196870(void *, int);
void func_00196C28(char *);
void func_0018E9C0(void *);
int func_0018EAC0(void *, void *);
s32 func_00190D50(Entity *, s32, f32);
void func_00216D20(void *, void *);
void func_00216CD8(void *, void *, void *);
void Vector_Subtract(void *, void *, void *);
extern float D_00413F80[];
int rand(void);
s32 func_00196D70(Entity *, void *, f32, f32, f32);
s32 func_0018F9C0(Entity *);
void func_0018B6F8(f32 *, f32, f32, void *, void *);
int func_00191340(Entity *, int);
int func_0018F030(char *);
float func_00194FB8(float);
void func_00195588(Vector4 *, float);
void Vector_Scale(float *, float *, float);
void func_00216C48(void *, void *, void *);

static void func_001339D8(Entity *entity);
static void func_00133CA0(Entity *entity);
static void func_00133D60(Entity *entity);
static void func_00133E40(Entity *entity);
static void func_00133FC8(Entity *entity);
static void func_00134160(Entity *entity);
static void func_00134398(Entity *entity);
static void func_00134750(Entity *entity);
static void func_001349E8(Entity *entity, s32 unknown);
static void func_00134B50(Entity *entity, s32 unknown);
static void func_00134DC8(Entity *entity, s32 unknown);
static void func_00134F68(Entity *entity, s32 unknown);
static void func_00135280(Entity *entity);

/* Set-up of a type 22 entity: clears its work area and installs the type's handlers. */
void EntityType22_Init(Entity *entity) {
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
    unkE0->unk00 = func_001349E8;
    unkE0->unk04 = (void (*)(void))func_001339D8;
    unkE0->unk08 = (void (*)(void))func_00133CA0;
    unkE0->unk18 = (void (*)(void))func_00135280;
    unkE0->unk14 = (void (*)(void))func_00134160;
    unkE0->unk10 = NULL;
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
    if (unk138 != NULL) {
        func_00216778(unkE0->unk20, unk138 + 0x50);
    }
    unkE0->unk40 = 0.5f;
    entity->unkD6 = (entity->params->flags & 0x4000) ? 0x40 : 0x80;
    entity->model->unk15C = entity->unkD6;
    func_00191478(entity->model);
    func_001914B0(entity->model, 10, 0x13, 1);
    ((EntityUnkE0Kind151D30 *)unkE0)->unk80.bit5 = 1;
    ((EntityUnkE0Kind151D30 *)unkE0)->unk80.bit6 = 1;
    ((EntityUnkE0Kind151D30 *)unkE0)->unk88.bit6 = 1;
    func_00193378(entity->model, 0, 0);
}

static void func_001339D8(Entity *entity) {
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

static void func_00133CA0(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    func_00190A30((char *)entity);
    entity->target = NULL;
    entity->flags &= ~0x200;
    if (((s32)(entity->flags >> 36) & 3) == 1) {
        if ((s32)(entity->flags >> 19) & 1) {
            func_0019A310((char *)entity, (void (*)(void))func_00133D60, 0);
        } else {
            func_0019A310((char *)entity, (void (*)(void))func_00133E40, 0);
        }
    } else if (((s32)(entity->flags >> 36) & 3) == 2) {
        func_0019A310((char *)entity, (void (*)(void))func_00133FC8, 0);
    }
    func_00195898((char *)entity, 2);
    unkE0->unk7A = 1;
}

static void func_00133D60(Entity *entity) {
    EntityUnkE0Kind1740A8 *unkE0 = (EntityUnkE0Kind1740A8 *)&entity->unkE0;
    s32 frame;

    entity->unkC6 |= 2;
    switch (entity->unkCB) {
    case 0:
        frame = unkE0->unk80.unk02 - 3;
        if (frame < 0) {
            frame = 0;
        }
        func_0019A448(entity, &unkE0->unk80, 0, unkE0->unk80.unk00 & 0x1F, frame, 5, 1);
        entity->unkCB++;
        entity->flags = ((entity->flags & ~0x3800ULL) | 0x800) & ~0x200ULL;
        break;
    case 1:
        if (unkE0->unk80.unk01 == 0) {
            entity->unkC6 &= ~2;
            func_0019A310((char *)entity, NULL, 0);
        }
        break;
    }
}

static void func_00133E40(Entity *entity) {
    EntityUnkE0Kind1740A8 *unkE0 = (EntityUnkE0Kind1740A8 *)&entity->unkE0;

    entity->unkC6 = 2;
    switch (entity->unkCB) {
    case 0:
        func_0019A448(entity, &((EntityUnkE0Kind1740A8 *)&entity->unkE0)->unk80, 0, 0xE, 0, 5, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        unkE0->unk1C = 0;
        ENTITY_FLAG_BITS(entity)->bit14 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit19 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit35 = FALSE;
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

static void func_00133FC8(Entity *entity) {
    EntityUnkE0Kind1740A8 *unkE0 = (EntityUnkE0Kind1740A8 *)&entity->unkE0;

    entity->unkC6 = 4;
    switch (entity->unkCB) {
    case 0:
        func_0019A448(entity, &((EntityUnkE0Kind1740A8 *)&entity->unkE0)->unk80, 0, 0xF, 0, 5, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        unkE0->unk1C = 0;
        ENTITY_FLAG_BITS(entity)->bit14 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit19 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit35 = FALSE;
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

static void func_00134160(Entity *entity) {
    EntityUnkE0Kind1740A8 *unkE0 = (EntityUnkE0Kind1740A8 *)&entity->unkE0;

    entity->unkC6 = 0x40;
    switch (entity->action_step) {
    case 0:
        func_0019A448(entity, &unkE0->unk80, 0, 0x10, 0, 5, 1);
        func_0019A448(entity, &unkE0->unk88, 1, 0x10, 0, 5, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        ENTITY_FLAG_BITS(entity)->bit14 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit20 = TRUE;
        ENTITY_FLAG_BITS(entity)->bits2 = 1;
        entity->unk30 = 20.0f;
        ENTITY_FLAG_BITS(entity)->bit27 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit26 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
        entity->action_step++;
    case 1:
        if (unkE0->unk80.unk01 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (unkE0->unk80.unk02 >= 30) {
            ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
            entity->action_step++;
        }
        break;
    case 3:
        func_00195388(&entity->model->unkD0[0], 0.0f, 10.0f);
        func_00195388(&entity->model->unkD0[2], 0.0f, 10.0f);
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            ENTITY_FLAG_BITS(entity)->bit14 = FALSE;
            ENTITY_FLAG_BITS(entity)->bits2 = 2;
            ENTITY_FLAG_BITS(entity)->bit4 = TRUE;
            entity->unkA0 = 20;
            func_0018DCD8(entity);
            entity->action_step++;
        }
        break;
    case 4:
        if (entity->unkA0 != 0) {
            entity->model->unk15C = (f32)entity->unkD6 * (f32)entity->unkA0 / 20.0f;
        } else {
            ENTITY_FLAG_BITS(entity)->bits2 = 3;
            ENTITY_FLAG_BITS(entity)->bit1 = FALSE;
        }
        break;
    }
}

static void func_00134398(Entity *entity) {
    EntityUnkE0Kind1740A8 *unkE0 = (EntityUnkE0Kind1740A8 *)&entity->unkE0;
    s32 kind = (s32)(ENTITY_FLAG_WORD(entity->flags) >> 58) & 3;
    EntityParamsUnk240 *unk240 = &entity->params->unk240[kind];
    s32 elapsed;

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        func_0019A448(entity, &unkE0->unk80, 0, kind | 0x14, 0, entity->params->unkC4 >> 1, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
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
        elapsed = (unkE0->unk80.unk04 - entity->params->unk80[((EntityUnkE0SlotBits *)&unkE0->unk80)->index]) >> 8;
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

static void func_00134750(Entity *entity) {
    EntityUnkE0Kind1740A8 *unkE0 = (EntityUnkE0Kind1740A8 *)&entity->unkE0;
    s32 kind = (s32)(ENTITY_FLAG_WORD(entity->flags) >> 58) & 3;
    EntityParamsUnk3D8 *unk3D8 = ENTITY_PARAMS_UNK3D8(entity->params, kind);

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        func_0019A448(entity, &unkE0->unk80, 0, kind + 0x17, 0, entity->params->unkC4 >> 1, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
        entity->action_step++;
    case 1:
        if (unkE0->unk80.unk01 == 0) {
            entity->action_step++;
        }
        if (unk3D8->unk01 != 0) {
            func_00199EB8(entity, (f32 *)&unkE0->unk1C, entity->unk68);
        }
        break;
    case 2:
        if (unkE0->unk80.unk02 <= unk3D8->unk01) {
            func_00199EB8(entity, (f32 *)&unkE0->unk1C, entity->unk68);
        } else {
            unkE0->unk1C = 0;
        }
        if (((EntityUnkE0SlotBits *)&unkE0->unk80)->index == 0x17) {
            if (unkE0->unk80.unk02 >= 20 && unkE0->unk80.unk02 <= 44) {
                if (D_00446ED4 & 1) {
                    func_00195ED0(entity, unk3D8, 20, 44);
                }
                ENTITY_FLAG_BITS(entity)->bit9 = TRUE;
            }
        } else {
            func_00195ED0(entity, unk3D8,
                          (unkE0->unk80.unk04 - entity->params->unk80[((EntityUnkE0SlotBits *)&unkE0->unk80)->index]) >> 8,
                          unkE0->unk80.unk02);
        }
        if (unkE0->unk80.unk02 > 0 && unk3D8->unk03 >= unkE0->unk80.unk02) {
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

static void func_001349E8(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    if (!(entity->unkC6 & 0x40)) {
        func_0019A448(entity, unkE0->unk88, 1, 3, 0, entity->params->unkC4, 0);
    }
    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        func_00196870(entity, 1);
    }
    if (entity->unkC6 != 0) {
        return;
    }
    func_00196C28((char *)entity);
    func_0018E9C0(entity);
    if (ENTITY_FLAG_BITS(entity)->bit53) {
        func_00134B50(entity, unknown);
        return;
    }
    if (unkE0->unk7A == 0) {
        func_00134DC8(entity, unknown);
        return;
    }
    switch (entity->state) {
    case 0:
        break;
    case 1:
    case 2:
        func_00134F68(entity, unknown);
        return;
    }
    func_00134DC8(entity, unknown);
}

/* walk back to the spawn point, turning away from whatever is pushing the entity */
static void func_00134B50(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 to_spawn[4];
    f32 push[4];
    f32 direction[4];
    s32 turn;

    if (entity->params->flags & 0x100) {
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
    } else {
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
    }
    entity->unkA0 = 0;
    ENTITY_FLAG_BITS(entity)->bit10 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
    turn = func_0018EAC0(entity, direction);
    if (turn == 0) {
        entity->target = entity->spawn_info;
        entity->unkC8 = func_00190D50(entity, 0, 50.0f);
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
            entity->unkC8 = 0x201;
            if (push[1] > 0.0f) {
                entity->unkC8 = 0x221;
            } else if (push[1] < 0.0f) {
                entity->unkC8 = 0x211;
            }
        } else if (turn < 0) {
            entity->unkC8 = 0x101;
            if (push[1] > 0.0f) {
                entity->unkC8 = 0x121;
            } else if (push[1] < 0.0f) {
                entity->unkC8 = 0x111;
            }
        } else {
            entity->unkC8 = func_00190D50(entity, 0, 50.0f);
        }
    }
    func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, unknown);
}

static void func_00134DC8(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    f32 *point;
    s32 turn;

    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
    if (func_00196D70(entity, D_00413F80, entity->params->unk43A * 0.017453292f,
                      entity->params->unk43C * 0.017453292f, entity->params->unk43E) &&
        unkE0->unk6A <= 0) {
        func_0018F9C0(entity);
        // match: an int result keeps $2 busy after the call
        ((int (*)(char *, void (*)(void), int))func_001958B8)((char *)entity, (void (*)(void))func_00134750, unknown);
        unkE0->unk6A = entity->params->unk438;
        return;
    }
    if (unkE0->unk6A > 0) {
        unkE0->unk6A--;
    }
    if (entity->unkA0 == 0) {
        point = unkE0->unk30;
        func_0018B6F8(point, entity->spawn_info->unk4C, entity->spawn_info->unk56, entity->spawn_info,
                      entity->model->position);
        entity->target = point;
        entity->unkA0 = rand() % 120 + 240;
    }
    turn = func_0018EAC0(entity, direction);
    if (turn != 0) {
        entity->unkC8 = func_00191340(entity, turn);
    } else {
        entity->unkC8 = func_00190D50(entity, 0, 50.0f);
    }
    func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, unknown);
}

static void func_00134F68(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    s32 turn;

    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    ENTITY_FLAG_BITS(entity)->bit9 = TRUE;
    turn = func_0018EAC0(entity, direction);
    if (turn != 0) {
        entity->unkC8 = func_00191340(entity, turn);
    } else {
        switch (unkE0->unk78) {
        case 0: {
            f32 *point = unkE0->unk30;

            func_00216778(point, D_00413F80);
            unkE0->unk30[1] -= 220.0f;
            entity->target = point;
            entity->unkC8 = func_00190D50(entity, 0, entity->radius + 100.0f + 30.0f);
            if (entity->distance_to_player < (f32)ENTITY_CONTROLLER(entity)->unk434) {
                unkE0->unk78++;
            }
            break;
        }
        case 1: {
            f32 *point = unkE0->unk30;

            func_00216778(point, D_00413F80);
            unkE0->unk30[1] -= 190.0f;
            entity->target = point;
            if (func_00196D70(entity, point, entity->params->unk43A * 0.017453292f,
                              entity->params->unk43C * 0.017453292f, entity->params->unk43E)) {
                if (func_0018F030((char *)entity)) {
                    func_001958B8(entity, func_00134750, unknown);
                } else {
                    func_001958B8(entity, func_00134398, unknown);
                }
                unkE0->unk68 = entity->params->unk436;
                unkE0->unk78++;
                return;
            }
            entity->unkC8 = func_00190D50(entity, 0, entity->radius + 100.0f + 30.0f);
            break;
        }
        case 2: {
            f32 *point = unkE0->unk30;

            direction[0] = func_00194FB8(entity->unk68 + 3.1415927f);
            func_00195588((Vector4 *)point, direction[0]);
            Vector_Scale(point, point, 800.0f);
            func_00216C48(point, point, D_00413F80);
            entity->target = point;
            unkE0->unk30[1] = entity->spawn_info->spawn_y;
            entity->unkC8 = func_00190D50(entity, 0, 50.0f);
            if (entity->distance_to_player >= (f32)ENTITY_CONTROLLER(entity)->unk434 || unkE0->unk68 <= 0) {
                unkE0->unk78 = 0;
            }
            break;
        }
        }
    }
    if (unkE0->unk68 > 0) {
        unkE0->unk68--;
    }
    func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, unknown);
}

static void func_00135280(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 value;
    f32 roll;
    f32 pitch;
    f32 pitch_change;
    f32 pitch_rate;

    if (entity->unkC8 & 0x100) {
        unkE0->unk1C -= entity->params->unk58;
        if (unkE0->unk1C < -entity->params->turn_limit) {
            unkE0->unk1C = -entity->params->turn_limit;
        }
        roll = 0.34906584f;
    } else if (entity->unkC8 & 0x200) {
        unkE0->unk1C += entity->params->unk58;
        if (unkE0->unk1C > entity->params->turn_limit) {
            unkE0->unk1C = entity->params->turn_limit;
        }
        roll = -0.34906584f;
    } else {
        unkE0->unk1C = 0.0f;
        roll = unkE0->unk1C;
    }
    value = unkE0->unk1C;
    if (entity->unkC8 & 0x400) {
        value += value;
    }
    entity->unk94 += value;
    func_00195388(&entity->model->unkD0[2], roll, 30.0f);
    if (entity->unkC8 & 0x10) {
        pitch = -0.5235988f;
    } else if (entity->unkC8 & 0x20) {
        pitch = 0.5235988f;
    } else {
        pitch = 0.0f;
    }
    pitch_change = pitch - entity->model->unkD0[0];
    if (ABS(pitch_change) > 0.5235988f) {
        pitch_rate = 60.0f;
    } else {
        pitch_rate = 30.0f;
    }
    value = entity->model->unkD0[0];
    func_00195388(&value, pitch, pitch_rate);
    entity->unk90 += value - entity->model->unkD0[0];
    func_0019A448(entity, unkE0->unk80, 0, 3, 0, entity->params->unkC4, 0);
}
