/* The behaviour of entity type 24 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers that function installs. */
#include "kf_entity.h"
#include "common.h"
#include "kf_effect.h"

void func_001958A8(char *, s32);
int func_001958B8(Entity *entity, void (*handler)(Entity *), int run_now);
void func_00191478(EntityModel *);
int func_00193378(EntityModel *, s32, s32);
void func_00216778(void *, void *);
void func_00182BD0(char *);
void func_00181038(Entity *);
void func_001818B8(Entity *);
extern ModelInstanceBlock D_003E7140[2];
// match: D_003E7140[1], which the set-up function reaches by its own address
extern ModelInstanceBlock D_003E7300;
extern s32 D_003E6E40[][2];
int func_00192EB0(void *, int);
// match: the callee is void, but retail keeps what it leaves in $2
int func_00194368(EntityModel *, f32);
int func_00196120(Entity *, void *, s32, void *, f32);
extern u32 D_00446ED4;
extern PlayerData D_00413F80;
void func_00196870(void *, int);
void func_00196CC0(char *);
void func_0018E9C0(void *);
int func_00182DE0(void *, int);
s32 func_00190F90(Entity *, s32, f32);
s32 func_00198D10(Entity *);
s32 func_0018F9C0(Entity *);
void func_00182240(Entity *);
float func_001957B8(float, float);
int rand(void);
s32 func_0018EEE0(Entity *, f32, f32);
s32 func_0018FD50(Entity *, s32);
void func_00190B88(Entity *, s32);
int func_0018F030(char *);
void func_00196D48(Entity *);
int func_00190BE0(Entity *);
int func_0018EAC0(void *, void *);
void func_00181AE0(Entity *);
void func_00181E48(Entity *);
float func_001954A8(float *, float *);
int func_00192C98(void *, int, int, u32, int);
s32 func_00199EB8(Entity *, f32 *, f32);
float sqrtf(float);
float atan2f(float, float);
char *func_0010CAB8(char *, u32);
float func_00194FB8(float);
void func_00195588(Vector4 *, float);
void Vector_Scale(float *, float *, float);
int func_00195AC0(Entity *, void *, s32, s32, s32);
void func_0019A290(Entity *, s32);
float func_00195478(float *, char *);
s32 func_001984B0(Entity *, s32);
extern u8 D_004141D8[];
extern EntityModel *D_00446978;
void func_0010B018(ModelInstance *, s32, s32);

static void func_00131630(Entity *entity);
static void func_001317D0(Entity *entity, s32 unknown);
static void func_001319B8(Entity *entity, s32 unknown);
static void func_001319F0(Entity *entity, s32 unknown);
static void func_00131B38(Entity *entity, int invoke_now);
static void func_00131BB8(Entity *entity, s32 unknown);
static void func_00131C78(Entity *entity, s32 unknown);
static void func_00131D70(Entity *entity, s32 unknown);
static void func_00132090(Entity *entity, s32 unknown);
static void func_00132428(Entity *entity);
static void func_001325B8(Entity *entity);
static void func_00132C70(Entity *entity);
static void func_00133228(Entity *entity);
static void func_00133428(Entity *entity);
static s32 func_001335D8(Entity *entity, f32 max_distance, f32 max_angle);
void func_00133708(Entity *entity, s32 unknown);

/* Set-up of a type 24 entity: clears its work area and installs the type's handlers. */
void EntityType24_Init(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    char *unk138 = entity->model->unk138;
    s32 i;

    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        ENTITY_WORK_BYTES(entity)[i] = 0;
    }
    func_001958A8((char *)entity, D_003E6E40[entity->params->unk02][1]);
    unkE0->unk00 = func_001317D0;
    unkE0->unk04 = (void (*)(void))func_00131630;
    unkE0->unk08 = (void (*)(void))func_00182BD0;
    unkE0->unk18 = (void (*)(void))func_00133428;
    unkE0->unk10 = (void (*)(void))func_00181038;
    unkE0->unk14 = (void (*)(void))func_001818B8;
    for (i = 3; i != -1; i--) {
        entity->unkBC[i] = 0x80;
    }
    for (i = 5; i != -1; i--) {
        entity->unkC0[i] = 0x80;
    }
    ENTITY_FLAG_BITS(entity)->bit28 = 0;
    entity->target = NULL;
    if (ENTITY_SPAWN_FLAGS(entity) & (0x8000L << 43)) {
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
    D_003E7300 = *(ModelInstanceBlock *)entity->model;
    D_003E7140[0] = *(ModelInstanceBlock *)entity->model;
}

static void func_00131630(Entity *entity) {
    f32 limits[4];
    s32 look_at_target;
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
        look_at_target = ENTITY_FLAG_BITS(entity)->bit9;
        limits[1] = entity->params->unk430 * 0.017453292f;
        limits[0] = -entity->params->unk430 * 0.017453292f;
        limits[3] = entity->params->unk432 * 0.017453292f;
        limits[2] = -entity->params->unk432 * 0.017453292f;
        if (look_at_target) {
            func_00196120(entity, unkE0->unk20, look_at_target, limits, 12.0f);
        } else {
            func_00196120(entity, unkE0->unk20, 0, NULL, 8.0f);
        }
    }
}

static void func_001317D0(Entity *entity, s32 unknown) {
    EntityUnkE0Kind131390 *unkE0 = (EntityUnkE0Kind131390 *)&entity->unkE0;
    ModelInstanceBlock *block;
    ModelInstanceBlock *previous;

    block = &D_003E7140[1];
    previous = block - 1;
    *block = *previous;
    D_003E7140[0] = *(ModelInstanceBlock *)entity->model;
    D_003E7140[0].model.alpha *= 0.5f;
    D_003E7140[1].model.alpha *= 0.5f;
    unkE0->unk7E = 0;
    if (unkE0->unk6A > 0) {
        unkE0->unk6A--;
    }
    if (D_00413F80.unk259 != 0) {
        unkE0->unk6A = entity->params->unk434;
    }
    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        func_00196870(entity, 1);
    }
    if (entity->unkC6 == 0) {
        func_00196CC0((char *)entity);
        func_0018E9C0(entity);
        if (ENTITY_FLAG_BITS(entity)->bit53) {
            func_00182DE0(entity, unknown);
        } else {
            switch (entity->state) {
            case 1:
            case 2:
                func_001319F0(entity, unknown);
                break;
            case 0:
            default:
                func_001319B8(entity, unknown);
                break;
            }
        }
    }
    if (unkE0->unk7D > 0) {
        unkE0->unk7D--;
    }
}

static void func_001319B8(Entity *entity, s32 unknown) {
    entity->unkC8 = 0;
    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
    func_001958B8(entity, (void (*)(Entity *))entity->unkE0.unk18, unknown);
}

static void func_001319F0(Entity *entity, s32 unknown) {
    EntityUnkE0Kind131390 *unkE0 = (EntityUnkE0Kind131390 *)&entity->unkE0;
    f32 distance;

    if (unkE0->unk7D <= 0) {
        distance = entity->distance_to_player - entity->radius - 100.0f;
        if (distance > 1200.0f) {
            unkE0->unk7C = 0;
            unkE0->unk7D = 10;
        } else if (distance <= 1200.0f && distance > 800.0f) {
            unkE0->unk7C = 1;
            unkE0->unk7D = 10;
        } else if (distance <= 800.0f && distance > 500.0f) {
            unkE0->unk7C = 2;
            unkE0->unk7D = 10;
        } else if (distance <= 500.0f && distance > 100.0f) {
            unkE0->unk7C = 3;
            unkE0->unk7D = 10;
        } else {
            unkE0->unk7C = 4;
            unkE0->unk7D = 10;
        }
    }
    switch (unkE0->unk7C) {
    case 0:
        func_00131B38(entity, unknown);
        break;
    case 1:
        func_00131BB8(entity, unknown);
        break;
    case 2:
        func_00131C78(entity, unknown);
        break;
    case 3:
        func_00131D70(entity, unknown);
        break;
    case 4:
        func_00132090(entity, unknown);
        break;
    default:
        func_00131B38(entity, unknown);
        break;
    }
}

static void func_00131B38(Entity *entity, int invoke_now) {
    EntityUnkE0 *sub = &entity->unkE0;

    entity->target = &D_00413F80;
    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    ENTITY_FLAG_BITS(entity)->bit9 = 1;
    entity->unkC8 = func_00190F90(entity, 1, 50.0f);
    func_001958B8(entity, (void (*)(Entity *))sub->unk18, invoke_now);
}

static void func_00131BB8(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    if (func_00198D10(entity)) {
        func_0018F9C0(entity);
        func_001958B8(entity, func_00182240, unknown);
        return;
    }
    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    ENTITY_FLAG_BITS(entity)->bit9 = TRUE;
    entity->target = &D_00413F80;
    entity->unkC8 = func_00190F90(entity, 1, 50.0f);
    func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, unknown);
}

static void func_00131C78(Entity *entity, s32 unknown) {
    EntityUnkE0Kind131390 *unkE0 = (EntityUnkE0Kind131390 *)&entity->unkE0;
    f32 angle;

    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    ENTITY_FLAG_BITS(entity)->bit9 = TRUE;
    angle = func_001957B8(entity->model->unkD0[1], entity->unk68);
    if (!(ABS(angle) > 0.34906584f)) {
        unkE0->unk7B = unkE0->unk7B == 0;
        func_001958B8(entity, func_00132428, unknown);
        return;
    }
    entity->target = &D_00413F80;
    entity->unkC8 = func_00190F90(entity, 1, 50.0f);
    func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, unknown);
}

static void func_00131D70(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    f32 angle;
    s32 side;
    s32 turn;

    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    if (func_001335D8(entity, 600.0f, 0.5235988f)) {
        func_001958B8(entity, func_00133228, 1);
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
                func_001958B8(entity, func_00182240, unknown);
            } else if (ENTITY_FLAG_BITS(entity)->bits58 == 0) {
                func_001958B8(entity, func_001325B8, unknown);
            } else if (ENTITY_FLAG_BITS(entity)->bits58 == 1) {
                func_001958B8(entity, func_00132C70, unknown);
            } else {
                func_001958B8(entity, func_00181E48, unknown);
            }
            return;
        case 1:
            entity->target = &D_00413F80;
            entity->unkA0 = rand() % 60 + 120;
            unkE0->unk78 = 0;
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
    if (unkE0->unk78 == 0) {
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
            if (turn > 0) {
                entity->unkC8 = 0x201;
            } else if (turn < 0) {
                entity->unkC8 = 0x101;
            } else {
                entity->unkC8 = func_00190F90(entity, 0, entity->radius + 100.0f + 30.0f);
            }
        }
    }
done:
    func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, unknown);
}

static void func_00132090(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    f32 angle;
    s32 side;
    s32 turn;

    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    if (func_001335D8(entity, 600.0f, 0.5235988f)) {
        func_001958B8(entity, func_00133228, 1);
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
                func_001958B8(entity, func_00182240, unknown);
            } else if (ENTITY_FLAG_BITS(entity)->bits58 == 0) {
                func_001958B8(entity, func_001325B8, unknown);
            } else if (ENTITY_FLAG_BITS(entity)->bits58 == 1) {
                func_001958B8(entity, func_00132C70, unknown);
            } else {
                func_001958B8(entity, func_00181E48, unknown);
            }
            return;
        case 1:
            entity->target = &D_00413F80;
            entity->unkA0 = rand() % 60 + 120;
            unkE0->unk78 = 0;
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
    if (unkE0->unk78 == 0) {
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
            if (turn > 0) {
                entity->unkC8 = 0x201;
            } else if (turn < 0) {
                entity->unkC8 = 0x101;
            } else {
                direction[0] = func_001954A8(entity->model->position, entity->target);
                angle = func_001957B8(entity->model->unkD0[1], direction[0]);
                if (ABS(angle) > entity->params->turn_limit) {
                    if (angle >= 0.0f) {
                        entity->unkC8 |= 0x200;
                    } else {
                        entity->unkC8 |= 0x100;
                    }
                }
                entity->unkC8 |= 2;
            }
        }
    }
done:
    func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, unknown);
}

static void func_00132428(Entity *entity) {
    EntityUnkE0Kind131390 *unkE0 = (EntityUnkE0Kind131390 *)&entity->unkE0;

    entity->unkC6 |= 0x1000;
    unkE0->unk7E = 1;
    ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
    func_00199EB8(entity, &unkE0->unk1C, entity->unk68);
    switch (entity->action_step) {
    case 0:
        if (unkE0->unk7B != 0) {
            func_00192C98(entity, 6, 0, entity->params->unkC4 >> 1, 0);
        } else {
            func_00192C98(entity, 7, 0, entity->params->unkC4 >> 1, 0);
        }
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
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

static void func_001325B8(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityParamsUnk240 *unk240 = &entity->params->unk240[0];
    f32 position[4];
    f32 angles[4];
    f32 *from;
    f32 *tip;
    f32 delta_x;
    f32 delta_y;
    f32 delta_z;
    s32 count;
    s32 i;
    s32 elapsed;

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 0x14, 0, entity->params->unkC4 >> 1, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        ENTITY_FLAG_BITS(entity)->bit54 = 0;
        ENTITY_FLAG_BITS(entity)->bit55 = 0;
        ENTITY_FLAG_BITS(entity)->bit56 = 0;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->action_step++;
    case 1:
        if (unk240->unk01 != 0) {
            func_00199EB8(entity, &unkE0->unk1C, entity->unk68);
        }
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (unk240->unk01 >= entity->model->unkFC->unk82) {
            func_00199EB8(entity, &unkE0->unk1C, entity->unk68);
        } else {
            unkE0->unk1C = 0.0f;
        }
        if (entity->model->unkFC->unk82 >= 8) {
            ENTITY_FLAG_BITS(entity)->bit14 = FALSE;
            unkE0->unk68 = 15;
            entity->action_step++;
        }
        break;
    case 3:
        func_00195588((Vector4 *)position, entity->model->unkD0[1]);
        Vector_Scale(position, position, 30.0f);
        entity->unk84 += position[0];
        entity->unk88 += position[1];
        entity->unk8C += position[2];
        unkE0->unk68--;
        if (entity->distance_to_player - entity->radius - 100.0f < 200.0f || unkE0->unk68 < 0) {
            entity->action_step++;
        }
        break;
    case 4:
        ENTITY_FLAG_BITS(entity)->bit14 = TRUE;
        if (unk240->unk01 >= entity->model->unkFC->unk82) {
            func_00199EB8(entity, &unkE0->unk1C, entity->unk68);
        } else {
            unkE0->unk1C = 0.0f;
        }
        from = (f32 *)func_0010CAB8((char *)entity->model, 8);
        tip = (f32 *)func_0010CAB8((char *)entity->model, 9);
        if (from != NULL && tip != NULL) {
            if (entity->model->unkFC->unk82 >= 0x14 && entity->model->unkFC->unk82 <= 0x1A) {
                delta_x = tip[0] - from[0];
                delta_y = tip[1] - from[1];
                delta_z = tip[2] - from[2];
                angles[0] = -atan2f(delta_y, sqrtf(delta_x * delta_x + delta_z * delta_z));
                angles[1] = func_00194FB8(-atan2f(delta_z, delta_x) + 1.5707964f);
                angles[2] = angles[3] = 0.0f;
                delta_x = from[0] - unkE0->unk48;
                delta_y = from[1] - unkE0->unk4C;
                delta_z = from[2] - unkE0->unk50;
                count = sqrtf(delta_x * delta_x + delta_y * delta_y + delta_z * delta_z) / 30.0f + 1.0f;
                for (i = 0; i < count; i++) {
                    position[0] = unkE0->unk48 + delta_x * i / count;
                    position[1] = unkE0->unk4C + delta_y * i / count;
                    position[2] = unkE0->unk50 + delta_z * i / count;
                    Effect_StartSpawner(0x3A0, position, angles, entity);
                }
            }
            unkE0->unk48 = from[0];
            unkE0->unk4C = from[1];
            unkE0->unk50 = from[2];
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
            } else {
                func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, 1);
            }
        }
        break;
    }
}

static void func_00132C70(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityParamsUnk240 *unk240 = &entity->params->unk240[1];
    f32 position[4];
    f32 angles[4];
    f32 *from;
    f32 *tip;
    f32 delta_x;
    f32 delta_y;
    f32 delta_z;
    s32 count;
    s32 i;
    s32 elapsed;

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 0x15, 0, entity->params->unkC4 >> 1, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        ENTITY_FLAG_BITS(entity)->bit54 = 0;
        ENTITY_FLAG_BITS(entity)->bit55 = 0;
        ENTITY_FLAG_BITS(entity)->bit56 = 0;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->action_step++;
    case 1:
        if (unk240->unk01 != 0) {
            func_00199EB8(entity, &unkE0->unk1C, entity->unk68);
        }
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (unk240->unk01 >= entity->model->unkFC->unk82) {
            func_00199EB8(entity, &unkE0->unk1C, entity->unk68);
        } else {
            unkE0->unk1C = 0.0f;
        }
        from = (f32 *)func_0010CAB8((char *)entity->model, 0x10);
        tip = (f32 *)func_0010CAB8((char *)entity->model, 0x11);
        if (from != NULL && tip != NULL) {
            if (entity->model->unkFC->unk82 >= 0x17 && entity->model->unkFC->unk82 <= 0x1B) {
                delta_x = tip[0] - from[0];
                delta_y = tip[1] - from[1];
                delta_z = tip[2] - from[2];
                angles[0] = -atan2f(delta_y, sqrtf(delta_x * delta_x + delta_z * delta_z));
                angles[1] = func_00194FB8(-atan2f(delta_z, delta_x) + 1.5707964f);
                angles[2] = angles[3] = 0.0f;
                delta_x = from[0] - unkE0->unk48;
                delta_y = from[1] - unkE0->unk4C;
                delta_z = from[2] - unkE0->unk50;
                count = sqrtf(delta_x * delta_x + delta_y * delta_y + delta_z * delta_z) / 30.0f + 1.0f;
                for (i = 0; i < count; i++) {
                    position[0] = unkE0->unk48 + delta_x * i / count;
                    position[1] = unkE0->unk4C + delta_y * i / count;
                    position[2] = unkE0->unk50 + delta_z * i / count;
                    Effect_StartSpawner(0x3A0, position, angles, entity);
                }
            }
            unkE0->unk48 = from[0];
            unkE0->unk4C = from[1];
            unkE0->unk50 = from[2];
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
            } else {
                func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, 1);
            }
        }
        break;
    }
}

static void func_00133228(Entity *entity) {
    EntityUnkE0Kind131390 *unkE0 = (EntityUnkE0Kind131390 *)&entity->unkE0;
    PlayerData *player;

    entity->unkC6 |= 0x1000;
    unkE0->unk7E = 1;
    ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
    func_00199EB8(entity, &unkE0->unk1C, entity->unk68);
    switch (entity->action_step) {
    case 0:
        func_001957B8(entity->model->unkD0[1], entity->unk68);
        player = &D_00413F80;
        if (func_001957B8(player->unk14, func_00195478(player->position, (char *)entity->model->position)) >= 0.0f) {
            func_00192C98(entity, 6, 0, entity->params->unkC4 >> 1, 0);
        } else {
            func_00192C98(entity, 7, 0, entity->params->unkC4 >> 1, 0);
        }
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if (func_001984B0(entity, 0)) {
                func_001958B8(entity, func_001325B8, 1);
            } else if (ENTITY_FLAG_BITS(entity)->bit5) {
                unkE0->unk00(entity, 1);
            } else {
                func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, 1);
            }
        }
        break;
    }
}

static void func_00133428(Entity *entity) {
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
        func_00192C98(entity, 6, 0, params->unkC4 >> 1, 0);
    } else if (entity->unkC8 & 8) {
        func_00192C98(entity, 7, 0, params->unkC4 >> 1, 0);
    } else if (entity->unkC8 & 0x300) {
        func_00192C98(entity, 3, 0, params->unkC4, 0);
    } else {
        func_00192C98(entity, 0, 0, params->unkC4, 0);
    }
}

static s32 func_001335D8(Entity *entity, f32 max_distance, f32 max_angle) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 distance;
    f32 angle;

    if (D_004141D8[0] == 0 || D_00446978->unkFC->unk82 >= 6) {
        if (unkE0->unk6A <= 0) {
            return FALSE;
        }
    }
    distance = entity->distance_to_player - (entity->radius + 100.0f);
    angle = func_001957B8(entity->model->unkD0[1], entity->unk68);
    if (entity->unkC6 != 0) {
        return FALSE;
    }
    if (max_distance < distance) {
        return FALSE;
    }
    if (ABS(angle) > max_angle) {
        return FALSE;
    }
    return rand() % 100 < entity->params->unkCA / 5;
}

void func_00133708(Entity *entity, s32 unknown) {
    EntityUnkE0Kind131390 *unkE0 = (EntityUnkE0Kind131390 *)&entity->unkE0;

    if (unkE0->unk7E != 0 && ENTITY_FLAG_BITS(entity)->bits2 == 0) {
        D_003E7140[0].model.unk14C = ENTITY_MODEL(entity)->unk14C;
        D_003E7140[0].model.unk148 = ENTITY_MODEL(entity)->unk148;
        D_003E7140[1].model.unk14C = ENTITY_MODEL(entity)->unk14C;
        D_003E7140[1].model.unk148 = ENTITY_MODEL(entity)->unk148;
        func_0010B018(&D_003E7140[0].model, 0, unknown);
        func_0010B018(&D_003E7140[1].model, 0, unknown);
    }
}
