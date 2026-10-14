/* The behaviour of entity type 13 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers that function installs. */
#include "kf_entity.h"
#include "common.h"
#include "kf_entity_world.h"

#define ENTITY_WORK_AREA_SIZE 0xA0
void func_001958A8(char *, s32);
int func_001958B8(Entity *, void (*)(Entity *), int);
void func_00191478(EntityModel *);
int func_00193378(EntityModel *, s32, s32);
void func_00216778(void *, void *);
void func_00182BD0(char *);
void func_00183570(Entity *);
void func_00181038(Entity *);
extern s32 D_003E6E40[][2];
int func_00192EB0(void *, int);
// match: the callee is void, but retail keeps what it leaves in $2
int func_00194368(EntityModel *, f32);
int func_00196120(Entity *, void *, s32, void *, f32);
double atan2(double, double);
float func_00194FB8(float);
float func_00195388(float *, float, float);
extern u32 D_00446ED4;
void func_00196870(void *, int);
void func_00196CC0(char *);
void func_0018E9C0(void *);
int func_00182DE0(void *, int);
int func_00182FD8(void *, int);
void func_00183150(Entity *, s32);
extern PlayerData D_00413F80;
int rand(void);
s32 func_0018ED98(Entity *, f32, f32);
s32 func_0018EEE0(Entity *, f32, f32);
s32 func_0018FD50(Entity *, s32);
void func_00190B88(Entity *, s32);
int func_0018F030(char *);
void func_00196D48(Entity *);
float func_001957B8(float, float);
int func_00190BE0(Entity *);
int func_0018EAC0(void *, void *);
s32 func_00190F90(Entity *, s32, f32);
void func_00181AE0(Entity *);
void func_00181C60(Entity *);
void func_00181E48(Entity *);
int func_00192C98(void *, int, int, u32, int);
s32 func_00199EB8(Entity *, f32 *, f32);
int func_0018CF30(Entity *, EntityParamsUnk3D8 *, s32, s32, f32 *, f32, f32);
void func_0019A290(Entity *, s32);
void func_0018DCD8(Entity *);
void func_0018DC40(char *);

static void func_00177318(Entity *entity);
static void func_00177520(Entity *actor);
static void func_001775F0(Entity *entity, s32 unknown);
static void func_001777B0(Entity *entity, s32 unknown);
static void func_00177AD0(Entity *entity);
static void func_00177D90(Entity *entity);
static void func_00178108(Entity *entity);

/* Set-up of a type 13 entity: clears its work area and installs the type's handlers. */
void EntityType13_Init(Entity *entity) {
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
    unkE0->unk00 = func_001775F0;
    unkE0->unk04 = (void (*)(void))func_00177318;
    unkE0->unk08 = (void (*)(void))func_00182BD0;
    unkE0->unk18 = (void (*)(void))func_00183570;
    unkE0->unk10 = (void (*)(void))func_00181038;
    unkE0->unk14 = (void (*)(void))func_00178108;
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

static void func_00177318(Entity *entity) {
    f32 limits[4];
    u32 blend_frames; // match: converted to float as unsigned
    s64 flags;
    s64 flags_now;
    EntityUnkE0 *unkE0;
    s32 look_at_target;
    EntityParams *params;

    flags = entity->flags;
    unkE0 = &entity->unkE0;
    if ((s32)((s64)(flags << 18) >> 32) & 1) {
        blend_frames = entity->model->unkFC->unk88;
        if (blend_frames != 0) {
            ENTITY_FLAG_BITS(entity)->bit34 = func_00194368(entity->model, (entity->unkC6 & 2) ? unkE0->unk40 : 1.0f / (f32)blend_frames);
            flags = entity->flags;
        } else {
            ENTITY_FLAG_BITS(entity)->bit34 = func_00192EB0(entity, (s32)(flags >> 11) & 7);
            flags = entity->flags;
        }
    }
    if ((s32)((s64)(flags << 22) >> 32) & 1) {
        func_00177520(entity);
    } else {
        unkE0->unk48 = 0.0f;
    }
    flags_now = entity->flags;
    if ((s32)((s64)(flags_now << 18) >> 32) & 1) {
        params = entity->params;
        look_at_target = (s32)(flags_now >> 9) & 1;
        limits[1] = params->unk430 * 0.017453292f;
        limits[0] = -params->unk430 * 0.017453292f;
        limits[3] = params->unk432 * 0.017453292f;
        limits[2] = -params->unk432 * 0.017453292f;
        if (look_at_target) {
            func_00196120(entity, unkE0->unk20, look_at_target, limits, 12.0f);
        } else {
            func_00196120(entity, unkE0->unk20, 0, NULL, 8.0f);
        }
    }
}

/* Aim: turn the angle toward the target into a clamped value, smooth it, and copy it to the part. */
static void func_00177520(Entity *actor) {
    ModelInstance *model = ENTITY_MODEL(actor);
    ModelPart *part = &model->parts[ENTITY_CONTROLLER(actor)->unk434];
    EntityUnkE0 *sub = &actor->unkE0;
    float angle;

    if (part != NULL) {
        angle = func_00194FB8(atan2(D_00413F80.position[1] - model->position.y, actor->distance_to_player));
        if (0.87266463f < angle) {
            angle = 0.87266463f;
        } else if (angle < -0.87266463f) {
            angle = -0.87266463f;
        }
        func_00195388(&sub->unk48, angle, 10.0f);
        part->unk50 = sub->unk48;
    }
}

static void func_001775F0(Entity *entity, s32 unknown) {
    if (((u8)entity->unkD1 == 0x17 || (u8)entity->unkD1 == 0x18) && entity->model->unkFC->unk88 == 0 &&
        entity->model->unkFC->unk82 >= 9 && entity->model->unkFC->unk82 < 0x2F) {
        entity->model->unkF8[5].unk68 &= ~(0x8000L << 25);
    } else {
        entity->model->unkF8[5].unk68 |= 0x8000L << 25;
    }
    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        func_00196870(entity, 1);
    }
    if (entity->unkC6 != 0) {
        return;
    }
    func_00196CC0((char *)entity);
    func_0018E9C0(entity);
    if (ENTITY_FLAG_BITS(entity)->bit53) {
        func_00182DE0(entity, unknown);
        return;
    }
    switch (entity->state) {
    case 0:
        func_00182FD8(entity, unknown);
        return;
    case 1:
        func_00183150(entity, unknown);
        return;
    case 2:
        func_001777B0(entity, unknown);
        return;
    }
    func_00182FD8(entity, unknown);
}

static void func_001777B0(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    f32 angle;
    s32 side;
    s32 turn;

    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    if (func_0018ED98(entity, 300.0f, 0.5235988f)) {
        func_001958B8(entity, func_00181C60, 1);
        return;
    }
    if (func_0018EEE0(entity, 300.0f, 0.5235988f)) {
        func_001958B8(entity, func_00181AE0, 1);
        return;
    }
    side = func_0018FD50(entity, 0);
    ENTITY_FLAG_BITS(entity)->bit10 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit9 = TRUE;
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
                if (ENTITY_FLAG_BITS(entity)->bits58 == 1) {
                    func_001958B8(entity, func_00177D90, unknown);
                } else {
                    func_001958B8(entity, func_00177AD0, unknown);
                }
            } else {
                func_001958B8(entity, func_00181E48, unknown);
            }
            return;
        case 1:
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

static void func_00177AD0(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    s32 attack_index = (s32)(entity->flags >> 58) & 3;
    EntityParamsUnk3D8 *unk3D8 = ENTITY_PARAMS_UNK3D8(entity->params, attack_index);
    f32 target[4];
    s32 elapsed;

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, attack_index + 0x17, 0, entity->params->unkC4 >> 1, 1);
        if (entity->params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            s32 kind = 2;

            if (!(ENTITY_PARAMS_BITS(entity)->bit25 && !ENTITY_PARAMS_BITS(entity)->bit26)) {
                kind = 1;
            }
            ENTITY_FLAG_BITS(entity)->bits11 = kind;
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
        ENTITY_FLAG_BITS(entity)->bit10 = TRUE;
        if (unk3D8->unk01 >= entity->model->unkFC->unk82) {
            func_00199EB8(entity, &entity->unkE0.unk1C, entity->unk68);
        } else {
            unkE0->unk1C = 0.0f;
        }
        elapsed = (entity->model->unkFC->unk84 - entity->params->unk80[entity->unkD1]) >> 8;
        func_00216778(target, D_00413F80.position);
        target[1] -= 170.0f;
        func_0018CF30(entity, unk3D8, elapsed, entity->model->unkFC->unk82, target, 0.17453294f, 0.17453294f);
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

static void func_00177D90(Entity *entity) {
    EntityUnkE0Kind1740A8 *unkE0 = (EntityUnkE0Kind1740A8 *)&entity->unkE0;
    EntityParamsUnk3D8 *unk3D8 = ENTITY_PARAMS_UNK3D8(entity->params, ENTITY_FLAG_BITS(entity)->bits58);
    f32 target[4];
    f32 angle;
    s32 elapsed;

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, ENTITY_FLAG_BITS(entity)->bits58 + 0x17, 0, entity->params->unkC4 >> 1, 1);
        if (entity->params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->bits11 = ((((EntityParamsWord *)entity->params)->unk00 & 0x06000000) == 0x02000000) ? 2 : 1;
        }
        ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
        entity->action_step++;
        break;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        ENTITY_FLAG_BITS(entity)->bit10 = TRUE;
        func_00199EB8(entity, (f32 *)&unkE0->unk1C, entity->unk68);
        if (entity->model->unkFC->unk82 > 0 && unk3D8->unk03 >= entity->model->unkFC->unk82) {
            ENTITY_FLAGS(entity) |= 0x80000;
        } else {
            ENTITY_FLAGS(entity) &= ~0x80000;
        }
        if (entity->model->unkFC->unk82 >= 0x2A) {
            entity->unkA0 = 0x5A;
            entity->action_step++;
        }
        break;
    case 3:
        func_00199EB8(entity, (f32 *)&unkE0->unk1C, entity->unk68);
        ENTITY_FLAG_BITS(entity)->bit10 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit14 = FALSE;
        func_001957B8(entity->model->unkD0[1], entity->unk68);
        angle = func_001957B8(unkE0->unk4C, entity->unk68);
        if (entity->unkA0 == 0 || entity->distance_to_player < 200.0f || ABS(angle) <= 0.010471975f) {
            entity->action_step++;
        }
        break;
    case 4:
        ENTITY_FLAG_BITS(entity)->bit14 = TRUE;
        elapsed = (entity->model->unkFC->unk84 - entity->params->unk80[entity->unkD1]) >> 8;
        func_00216778(target, D_00413F80.position);
        target[1] -= 170.0f;
        func_0018CF30(entity, unk3D8, elapsed, entity->model->unkFC->unk82, target, 0.17453294f, 0.17453294f);
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
    unkE0->unk4C = entity->unk68;
}

static void func_00178108(Entity *entity) {
    entity->unkC6 = 0x40;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 0x10, 0, 5, 1);
        if (entity->params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            s32 kind = 2;

            if (!(ENTITY_PARAMS_BITS(entity)->bit25 && !ENTITY_PARAMS_BITS(entity)->bit26)) {
                kind = 1;
            }
            ENTITY_FLAG_BITS(entity)->bits11 = kind;
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
        if (entity->model->unkFC->unk88 == 0) {
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
            if (entity->spawn_info->unk40 != 0) {
                Entity *partner = Entity_GetBySpawnId(entity->spawn_info->unk40);

                if (partner != NULL && ((s32)partner->flags & 1)) {
                    func_0018DC40((char *)partner);
                    func_00216778(partner->model->position, entity->model->position);
                }
            }
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
