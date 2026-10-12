#include "common.h"
#include "kf_entity.h"
#include "kf_map.h"

extern u32 D_00446ED4;
extern s32 D_00446770;
extern PlayerData D_00413F80;
extern MapSlot D_0038CD20[];
extern KfVector D_004FF390[];
extern f32 D_00446824[];
extern f32 D_00446828[];

void func_00196870(void *, int);
void func_0018B778(float, void *, void *, void *);
f32 func_0018A9F0(void *, void *);
void func_00196CC0(void *);
void func_0018E9C0(void *);
int func_00182DE0(void *, int);
int func_00182FD8(void *, int);
void func_00183150(Entity *, s32);
static void func_001686B0(Entity *, s32);
int rand(void);
static s32 func_00169DF8(Entity *, f32, f32);
s32 func_0018EEE0(Entity *, f32, f32);
int func_0016A450(char *);
s32 func_0018FD50(Entity *, s32);
void func_00190B88(Entity *, s32);
int func_0018F030(char *);
int func_0018CA50(int *);
void func_00196D48(Entity *);
float func_001957B8(float, float);
int func_00190BE0(Entity *);
int func_0018EAC0(void *, void *);
s16 func_00190F90(float, void *, int); // match: this file only matches with this type; func_00190F90.c defines it differently
int func_001958B8(Entity *, void (*)(Entity *), int);
void func_00181AE0(Entity *);
void func_00181E48(Entity *);
static void func_00168DB8(Entity *);
static void func_00169500(Entity *);
static void func_00169738(Entity *);
static void func_00169BB0(Entity *);
static void func_00169F58(Entity *);
void func_0016A530(Entity *);
int func_00192C98(void *, int, int, u32, int);
int func_00199EB8(void *, void *, float);
static void func_00168F48(Entity *);
int func_00195AC0(Entity *, void *, s32, s32, s32);
float func_00194FB8(float);
float sinf(float);
float cosf(float);
float func_00195478(float *, char *);
void func_00190A30(char *);
void func_00195898(char *, u8);
int func_0019A310(char *, void (*)(void), int); // match: an int result keeps $2 busy after the call
static void func_00169990(Entity *);
void func_0019A290(Entity *, s32);
void func_0018D600(char **, int, u32, int);
int func_00195ED0(Entity *, EntityParamsUnk3D8 *, s32, s32);
void *func_001267B0(int);
float func_00194F80(void *, void *);
void func_0018DC40(char *);

void func_00168368(Entity *entity, s32 unknown) {
    EntityUnkE0Kind168160 *unkE0 = (EntityUnkE0Kind168160 *)&entity->unkE0;
    MapSlot *slot;
    KfVector *point;
    f32 height;
    f32 no_floor;

    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        func_00196870(entity, 1);
    }
    if (D_00413F80.unk259 != 0) {
        unkE0->unk7C = 30;
    }
    if (unkE0->unk7C > 0) {
        unkE0->unk7C--;
    }
    if (entity->distance_to_player < entity->radius + 500.0f + 100.0f && unkE0->unk7A == 1 && unkE0->unk7B == 0) {
        unkE0->unk7A = 0;
    } else if (entity->radius + 1000.0f + 100.0f < entity->distance_to_player && unkE0->unk7A == 0) {
        unkE0->unk7A = 1;
    }
    if (unkE0->unk6A < 5) {
        func_0018B778(500.0f, &D_004FF390[unkE0->unk6A], &D_00413F80, &D_00413F80);
        slot = &D_0038CD20[D_00446770];
        height = func_0018A9F0((MapRecord *)slot->records + D_00413F80.unk252, &D_004FF390[unkE0->unk6A]);
        no_floor = D_00446824[0];
        if (height == no_floor) {
            height = func_0018A9F0(entity->unk14, &D_004FF390[unkE0->unk6A]);
            if (height != no_floor) {
                point = &D_004FF390[unkE0->unk6A];
                point->y += height;
                unkE0->unk6A++;
            }
        } else {
            point = &D_004FF390[unkE0->unk6A];
            point->y += height;
            unkE0->unk6A++;
        }
    }
    if (entity->unkC6 == 0) {
        func_00196CC0(entity);
        func_0018E9C0(entity);
        if ((s32)(entity->flags >> 53) & 1) {
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
                func_001686B0(entity, unknown);
                break;
            default:
                func_00182FD8(entity, unknown);
                break;
            }
        }
    }
}

static void func_001686B0(Entity *entity, s32 unknown) {
    EntityUnkE0Kind168160 *unkE0 = (EntityUnkE0Kind168160 *)&entity->unkE0;
    f32 direction[4];
    f32 angle;
    f32 turn_left;
    s32 side;
    s32 turn;

    if (entity->params->flags & 0x100) {
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
    } else {
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
    }
    if (func_00169DF8(entity, 200.0f, 1.0471976f)) {
        func_001958B8(entity, func_00169BB0, 1);
        return;
    }
    if (func_0018EEE0(entity, 300.0f, 0.5235988f)) {
        func_001958B8(entity, func_00181AE0, 1);
        return;
    }
    if (entity->spawn_info->unk40 == 1) {
        func_0016A450((char *)entity);
    } else {
        entity->params->unk3D8[2].record.unk00 = 0;
    }
    // match: angle is read here before anything has set it
    if (unkE0->unk7B == 0 && entity->distance_to_player < entity->radius + 50.0f + 100.0f && entity->unkA0 == 0 &&
        rand() % 100 < (s16)entity->params->unk434 && ABS(angle) < 1.0471976f) {
        turn_left = entity->unk68 - entity->model->unkD0[1];
        if (ABS(turn_left) < 1.0471976f) {
            func_001958B8(entity, func_00169738, unknown);
            return;
        }
    }
    side = func_0018FD50(entity, 2);
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
            if (unkE0->unk7B == 1) {
                func_001958B8(entity, func_00169500, unknown);
            } else if (func_0018F030((char *)entity)) {
                if (((s32)(entity->flags >> 58) & 3) == 2) {
                    func_001958B8(entity, func_00169F58, unknown);
                } else {
                    func_001958B8(entity, func_0016A530, unknown);
                }
            } else {
                func_001958B8(entity, func_00181E48, unknown);
            }
            return;
        case 1:
            if (unkE0->unk7A == 1 && unkE0->unk7B == 0 && func_0018CA50((int *)entity) == 0) {
                entity->target = &D_00413F80;
                entity->unkA0 = rand() % 60 + 0xF0;
                unkE0->unk78 = 0;
                func_001958B8(entity, func_00168DB8, unknown);
                return;
            }
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
    if (entity->unkCD == 1) {
        turn = func_0018EAC0(entity, direction);
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
        } else if (unkE0->unk7A == 1) {
            if (func_0018CA50((int *)entity) == 0 && unkE0->unk7B == 0) {
                entity->target = &D_00413F80;
                entity->unkA0 = rand() % 60 + 0xF0;
                unkE0->unk78 = 0;
                func_001958B8(entity, func_00168DB8, unknown);
                return;
            }
            if (turn > 0) {
                entity->unkC8 = 0x241;
            } else if (turn < 0) {
                entity->unkC8 = 0x141;
            } else {
                entity->unkC8 = func_00190F90(entity->radius + 100.0f + 30.0f, entity, 1);
            }
        } else {
            if (turn > 0) {
                entity->unkC8 = 0x201;
            } else if (turn < 0) {
                entity->unkC8 = 0x101;
            } else {
                entity->unkC8 = func_00190F90(entity->radius + 100.0f + 30.0f, entity, 0);
            }
        }
    }
done:
    func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, unknown);
}

void func_00168C10(Entity *entity) {
    EntityUnkE0Kind168160 *unkE0 = (EntityUnkE0Kind168160 *)&entity->unkE0;
    EntityParams *params;
    f32 turn;

    if (entity == D_00413F80.unk204 && (entity->params->flags & 0x800)) {
        entity->unkC8 = 0;
    }
    params = entity->params;
    if (entity->unkC8 & 0x100) {
        unkE0->unk1C -= params->unk58;
        if (unkE0->unk1C < -params->turn_limit) {
            unkE0->unk1C = -params->turn_limit;
        }
    } else if (entity->unkC8 & 0x200) {
        unkE0->unk1C += params->unk58;
        if (params->turn_limit < unkE0->unk1C) {
            unkE0->unk1C = params->turn_limit;
        }
    } else {
        unkE0->unk1C = 0.0f;
    }
    turn = unkE0->unk1C;
    if (entity->unkC8 & 0x440) {
        turn += turn;
    }
    entity->unk94 += turn;
    if (entity->unkC8 & 1) {
        if (entity->unkC8 & 0x40) {
            func_00192C98(entity, 4, 0, params->unkC4, 0);
        } else {
            func_00192C98(entity, 3, 0, params->unkC4, 0);
        }
    } else if (entity->unkC8 & 2) {
        func_00192C98(entity, 5, 0, params->unkC4, 0);
    } else if (entity->unkC8 & 0x10) {
        func_00192C98(entity, 8, 0, params->unkC4, 0);
    } else if (entity->unkC8 & 0x20) {
        func_00192C98(entity, 9, 0, params->unkC4, 0);
    } else if (entity->unkC8 & 4) {
        func_00192C98(entity, 6, 0, params->unkC4, 0);
    } else if (entity->unkC8 & 8) {
        func_00192C98(entity, 7, 0, params->unkC4, 0);
    } else if (entity->unkC8 & 0x300) {
        func_00192C98(entity, 3, 0, params->unkC4, 0);
    } else {
        func_00192C98(entity, 0, 0, params->unkC4, 0);
    }
}

static void func_00168DB8(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 0x3000;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 8, 0, entity->params->unkC4 >> 1, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        ENTITY_FLAG_BITS(entity)->bit28 = 1;
        ENTITY_FLAG_BITS(entity)->bit17 = 0;
        unkE0->unk48 = 0;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        func_00199EB8(entity, &unkE0->unk1C, entity->unk68);
        return;
    case 2:
        if ((int)(entity->flags >> 34) & 1) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            unkE0->unk7B = 1;
            func_001958B8(entity, func_00168F48, 1);
        }
        if (entity->model->unkFC->unk82 >= 0x29) {
            float next_unk48;
            entity->unk88 = -unkE0->unk48;
            next_unk48 = unkE0->unk48 + 0.8f;
            unkE0->unk48 = next_unk48;
            if (next_unk48 > 12.0f) unkE0->unk48 = 12.0f;
        }
        break;
    }
}

static void func_00168F48(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    s32 flag;

    entity->unkC6 |= 0x4000;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 0x11, 0, 10, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
        unkE0->unk6E = 0;
        ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        entity->unk88 = -unkE0->unk48;
        unkE0->unk48 += 0.8f;
        if (unkE0->unk48 > 12.0f) {
            unkE0->unk48 = 12.0f;
        }
        break;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            if (ABS(func_00194FB8(entity->unk68 - entity->model->unkD0[1])) < 0.34906584f) {
                func_00192C98(entity, 0x12, 0, 1, 1);
                entity->action_step++;
                unkE0->unk48 = 20.0f;
                unkE0->unk44 = 12.0f;
            }
        }
        func_00199EB8(entity, &unkE0->unk1C, entity->unk68);
        if (func_0018A9F0(entity->unk14, entity->model->position) < 220.0f) {
            entity->unk88 = -unkE0->unk48;
            unkE0->unk48 += 0.8f;
            if (unkE0->unk48 > 12.0f) {
                unkE0->unk48 = 12.0f;
            }
        }
        break;
    case 3:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 4:
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            func_00192C98(entity, 0x13, 0, 10, 1);
            entity->action_step++;
        }
        if (entity->model->unkFC->unk82 >= 0x10) {
            entity->unk84 = -sinf(func_00194FB8(entity->model->unkD0[1])) * unkE0->unk44;
            entity->unk88 = unkE0->unk48;
            entity->unk8C = -cosf(func_00194FB8(entity->model->unkD0[1])) * unkE0->unk44;
            unkE0->unk44 *= 1.09f;
            unkE0->unk48 -= 0.8f;
            if (unkE0->unk44 > 80.0f) {
                unkE0->unk44 = 80.0f;
            }
            if (unkE0->unk48 < 3.0f) {
                unkE0->unk48 = 3.0f;
            }
        }
        break;
    case 5:
        if (ENTITY_FLAG_BITS(entity)->bit44 == 1 || ENTITY_FLAG_BITS(entity)->bit46 == 1
            || ENTITY_FLAG_BITS(entity)->bit45 == 1 || ENTITY_FLAG_BITS(entity)->bit49 == 1
            || ENTITY_FLAG_BITS(entity)->bit48 == 1) {
            char *unk350;

            ENTITY_FLAG_BITS(entity)->bit14 = TRUE;
            ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
            entity->unkC6 = 0;
            unk350 = entity->params->unk350;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            flag = ENTITY_FLAG_BITS(entity)->bit49;
            if (flag == 1) {
                func_00195AC0(entity, unk350, 0, 0x1E, 0x1F);
                unkE0->unk6E = flag;
            }
            func_001958B8(entity, func_00169500, 1);
            return;
        }
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            entity->action_step++;
            ENTITY_FLAG_BITS(entity)->bit14 = FALSE;
        }
        entity->unk84 = -sinf(func_00194FB8(entity->model->unkD0[1])) * unkE0->unk44;
        entity->unk88 = unkE0->unk48;
        entity->unk8C = -cosf(func_00194FB8(entity->model->unkD0[1])) * unkE0->unk44;
        unkE0->unk44 *= 1.09f;
        unkE0->unk48 -= 0.8f;
        if (unkE0->unk44 > 80.0f) {
            unkE0->unk44 = 80.0f;
        }
        if (unkE0->unk48 < 3.0f) {
            unkE0->unk48 = 3.0f;
        }
        break;
    case 6:
        if (ENTITY_FLAG_BITS(entity)->bit44 == 1 || ENTITY_FLAG_BITS(entity)->bit46 == 1
            || ENTITY_FLAG_BITS(entity)->bit45 == 1 || ENTITY_FLAG_BITS(entity)->bit49 == 1
            || ENTITY_FLAG_BITS(entity)->bit48 == 1) {
            char *unk350;

            ENTITY_FLAG_BITS(entity)->bit14 = TRUE;
            ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
            entity->unkC6 = 0;
            unk350 = entity->params->unk350;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            flag = ENTITY_FLAG_BITS(entity)->bit49;
            if (flag == 1) {
                func_00195AC0(entity, unk350, 0, 0x1E, 0x1F);
                unkE0->unk6E = flag;
            }
            func_001958B8(entity, func_00169500, 1);
            return;
        }
        entity->unk84 = -sinf(func_00194FB8(entity->model->unkD0[1])) * unkE0->unk44;
        entity->unk88 = unkE0->unk48;
        entity->unk8C = -cosf(func_00194FB8(entity->model->unkD0[1])) * unkE0->unk44;
        unkE0->unk44 *= 1.09f;
        unkE0->unk48 -= 0.8f;
        if (unkE0->unk44 > 80.0f) {
            unkE0->unk44 = 80.0f;
        }
        if (unkE0->unk48 < 3.0f) {
            unkE0->unk48 = 3.0f;
        }
        func_00199EB8(entity, &unkE0->unk1C, entity->unk68);
        return;
    }
    return; // match: with this the call above becomes a tail call
}

static void func_00169500(Entity *entity) {
    EntityUnkE0Kind168160 *unkE0 = (EntityUnkE0Kind168160 *)&entity->unkE0;
    char *unk350 = entity->params->unk350;

    entity->unkC6 |= 0x5000;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 0x1D, 0, 1, 1);
        entity->flags &= ~0x3800;
        entity->flags &= ~0x200;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->flags |= 0x20000;
            entity->action_step++;
        }
        if (unkE0->unk6E == 0) {
            s32 flag = (s32)(entity->flags >> 49) & 1;

            if (flag == 1) {
                func_00195AC0(entity, unk350, 0, 0x1E, 0x1F);
                unkE0->unk6E = flag;
            }
        }
        break;
    case 2:
        if (unkE0->unk6E == 0) {
            s32 flag = (s32)(entity->flags >> 49) & 1;

            if (flag == 1) {
                func_00195AC0(entity, unk350, 0, 0x1E, 0x1F);
                unkE0->unk6E = flag;
            }
        }
        if ((s32)(entity->flags >> 34) & 1) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            unkE0->unk7B = 0;
            if ((s32)(entity->flags >> 5) & 1) {
                unkE0->unk00(entity, 1);
            } else {
                func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, 1);
            }
        }
        entity->unk84 = -sinf(func_00194FB8(entity->model->unkD0[1])) * unkE0->unk44;
        entity->unk8C = -cosf(func_00194FB8(entity->model->unkD0[1])) * unkE0->unk44;
        unkE0->unk44 -= 2.0f;
        if (unkE0->unk44 < 0.0f) {
            unkE0->unk44 = 0.0f;
        }
        break;
    }
}

static void func_00169738(Entity *entity) {
    EntityUnkE0Kind168160 *unkE0 = (EntityUnkE0Kind168160 *)&entity->unkE0;

    entity->unkC6 |= 0x1000;
    switch (entity->action_step) {
    case 0:
        if (0.0f < func_001957B8(D_00413F80.unk14, func_00195478(D_00413F80.position, (char *)entity->model->position)) * 57.29578f) {
            func_00192C98(entity, 6, 0, entity->params->unkC4 >> 1, 1);
        } else {
            func_00192C98(entity, 7, 0, entity->params->unkC4 >> 1, 1);
        }
        entity->flags = (entity->flags & ~0x3800) | 0x800;
        entity->flags &= ~0x200;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        func_00199EB8(entity, &unkE0->unk1C, entity->unk68);
        break;
    case 2:
        func_00199EB8(entity, &unkE0->unk1C, entity->unk68);
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

/* When the state field in flags is 2, resets the entity and registers func_00169990; then sets state 2. */
void func_00169918(Entity *entity) {
    if ((((u64)entity->flags >> 36) & 3) == 2) {
        func_00190A30((char *)entity);
        entity->target = NULL;
        entity->flags &= ~0x400;
        entity->flags &= ~0x200;
        func_0019A310((char *)entity, (void (*)(void))func_00169990, 0);
    }
    func_00195898((char *)entity, 2);
}

int func_0019A310(char *, void (*)(void), int); // match: an int result keeps $2 busy after the call

static void func_00169990(Entity *entity) {
    EntityUnkE0Kind168160 *unkE0 = (EntityUnkE0Kind168160 *)&entity->unkE0;

    entity->unkC6 = 4;
    switch (entity->unkCB) {
    case 0:
        func_00192C98(entity, 0xF, 0, 5, 1);
        if (entity->params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->bits11 = ((((EntityParamsWord *)entity->params)->unk00 & 0x06000000) == 0x02000000) ? 2 : 1;
        }
        unkE0->unk1C = 0.0f;
        ENTITY_FLAG_BITS(entity)->bit14 = 1;
        ENTITY_FLAG_BITS(entity)->bit19 = 0;
        ENTITY_FLAG_BITS(entity)->bit35 = 0;
        if (!(((EntityParamsWord *)entity->params)->unk00 & 0x06000000)) {
            ENTITY_FLAG_BITS(entity)->bit28 = 0;
        }
        ENTITY_FLAG_BITS(entity)->bit10 = 0;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->unkCB++;
        unkE0->unk7B = 0;
        break;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
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

static void func_00169BB0(Entity *entity) {
    EntityUnkE0Kind168160 *unkE0 = (EntityUnkE0Kind168160 *)&entity->unkE0;

    entity->unkC6 |= 0x10;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 0xD, 0, 0xA, 1);
        if (entity->params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->bits11 = ((((EntityParamsWord *)entity->params)->unk00 & 0x06000000) == 0x02000000) ? 2 : 1;
        }
        unkE0->unk1C = 0.0f;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->action_step++;
        break;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->unkA0 = entity->params->unkCE;
            entity->flags |= 0x8000L << 20;
            entity->flags &= ~0x4000;
            entity->action_step++;
        }
        break;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->bit43 && ENTITY_FLAG_BITS(entity)->bit39 && !ENTITY_FLAG_BITS(entity)->bit38) {
            entity->flags &= ~(0x8000L << 28);
            entity->unkA0 = entity->params->unkCE;
        }
        func_0018D600((char **)entity, 0x32F, 0, (int)entity->model->unkD0);
        if (entity->unkA0 == 0) {
            ENTITY_FLAG_BITS(entity)->bit35 = 0;
            ENTITY_FLAG_BITS(entity)->bit14 = 1;
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

static s32 func_00169DF8(Entity *entity, f32 min_gap, f32 max_angle) {
    EntityUnkE0Kind168160 *unkE0 = (EntityUnkE0Kind168160 *)&entity->unkE0;
    f32 gap = entity->distance_to_player - (entity->radius + 100.0f);
    f32 angle = func_001957B8(entity->model->unkD0[1], entity->unk68);
    f32 facing = func_001957B8(D_00413F80.unk14, func_00195478(D_00413F80.position, (char *)entity->model->position));

    if (entity->unkC6 != 0 || gap < min_gap || max_angle < ABS(angle) || 0.7853982f < ABS(facing) || unkE0->unk7C <= 0) {
        return FALSE;
    }
    return rand() % 100 < entity->params->unkCB;
}

static void func_00169F58(Entity *entity) {
    EntityUnkE0Kind168160 *unkE0 = (EntityUnkE0Kind168160 *)&entity->unkE0;
    EntityParams *params = entity->params;
    s32 kind = (s32)(entity->flags >> 58) & 3;
    EntityParamsUnk3D8 *unk3D8 = ENTITY_PARAMS_UNK3D8(params, kind);
    MapSlot *slot;
    KfVector *floor_point;
    KfVector *point;
    Entity *minion;
    EntityModel *minion_model;
    f32 height;
    f32 no_floor;
    f32 spin;
    s32 minion_index;
    s32 point_index;
    s32 count;

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, kind + 0x17, 0, params->unkC4 >> 1, 1);
        if (entity->params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->bits11 = ((((EntityParamsWord *)entity->params)->unk00 & 0x06000000) == 0x02000000) ? 2 : 1;
        }
        unkE0->unk6A = 0;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        if (unkE0->unk6A < 5) {
            func_0018B778(500.0f, &D_004FF390[unkE0->unk6A], &D_00413F80, &D_00413F80);
            slot = &D_0038CD20[D_00446770];
            height = func_0018A9F0((MapRecord *)slot->records + D_00413F80.unk252, &D_004FF390[unkE0->unk6A]);
            no_floor = D_00446828[0];
            if (height == no_floor) {
                height = func_0018A9F0(entity->unk14, &D_004FF390[unkE0->unk6A]);
                if (height == no_floor) {
                    break;
                }
            }
            floor_point = &D_004FF390[unkE0->unk6A];
            floor_point->y += height;
            unkE0->unk6A++;
        }
        break;
    case 2:
        unkE0->unk1C = 0.0f;
        func_00195ED0(entity, unk3D8, (entity->model->unkFC->unk84 - params->unk80[entity->unkD1]) >> 8,
                      entity->model->unkFC->unk82);
        if (entity->model->unkFC->unk82 == 0x11) {
            minion_index = 0;
            count = rand() % 5 + 1;
            if (unkE0->unk6A < count) {
                count = unkE0->unk6A;
            }
            for (point_index = 0; point_index < count; point_index++) {
                for (; minion_index < 8; minion_index++) {
                    minion = func_001267B0(minion_index + 10);
                    if (minion != NULL && (minion->flags & 3) == 1 &&
                        (((s32)(minion->flags >> 2) & 3) == 0 || ((s32)(minion->flags >> 2) & 3) == 3)) {
                        point = &D_004FF390[point_index];
                        if (func_00194F80(point, &D_00413F80) > 180.0f) {
                            func_0018DC40((char *)minion);
                            minion->model->position[0] = point->x;
                            minion->model->position[1] = point->y;
                            minion->model->position[2] = point->z;
                            minion->model->unkD0[1] = func_00194FB8((f32)(rand() % 360) * 0.017453292f);
                            minion_model = minion->model;
                            spin = minion_model->unkE0[0];
                            if (rand() % 3 != 0) {
                                spin = -spin;
                            }
                            minion_model->unkE0[0] = spin;
                            break;
                        }
                    }
                }
            }
        }
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
