/* The behaviour of entity type 54 (Entity.params->unk02, the index into D_003E6E40): its set-up
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
void func_00181038(Entity *);
extern s32 D_003E6E40[][2];
void func_00190A30(char *);
int func_0019A310(char *, void (*)(void), int);
void func_00195898(char *, u8);
void func_00181378(Entity *);
int func_00192EB0(void *, int);
// match: the callee is void, but retail keeps what it leaves in $2
int func_00194368(EntityModel *, f32);
int func_00196120(Entity *, void *, s32, void *, f32);
extern u32 D_00446ED4;
extern PlayerData D_00413F80;
extern u8 D_0034A018[];
// match: the size is unknown; a complete small type would be read through $gp
extern f32 D_0044681C[];
int rand(void);
void func_00196870(void *, int);
void func_00196CC0(void *);
void func_0018E9C0(void *);
s32 func_00190F90(Entity *, s32, f32);
int func_00182DE0(void *, int);
int func_00182FD8(void *, int);
void func_00183150(Entity *, s32);
f32 func_0018A9F0(void *, void *);
void func_0018DA38(Entity *, s32, s32, s32);
s32 func_0018EEE0(Entity *, f32, f32);
float func_00195478(float *, char *);
float func_001957B8(float angle_from, float angle_to);
s32 func_00197F00(Entity *);
s32 func_0018F648(Entity *);
s32 func_0018FD50(Entity *, s32);
void func_00190B88(Entity *, s32);
int func_0018EAC0(void *, void *);
float func_00194FB8(float);
s32 func_0018EC78(Entity *, f32);
int func_00190BE0(Entity *);
int func_0018F030(char *);
int func_00192C98(void *, int, int, u32, int);
extern f32 D_00431AF0[16];
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0RotMatrixZ(float *, float *, float);
void func_00216BC0(void *, void *, void *);
void Vector_Scale(float *, float *, float);
void func_0018D8B8(Entity *, s32, s32, s32);
void func_0019A290(Entity *, s32);
void func_0018DCD8(Entity *);
s32 func_00195AC0(Entity *, void *, s32, s32, s32);
int func_0018CF30(Entity *, EntityParamsUnk3D8 *, s32, s32, f32 *, f32, f32);
// match: the size is unknown (a complete small type would be read through $gp) and it is
// const: the store to unk54 may then be scheduled after the lui of this address
extern const f32 D_00446820[];
f32 func_00195018(f32, f32, f32);
int func_0019A3F0(void *, int);

static void func_0015FAC8(Entity *entity);
static void func_0015FB98(Entity *entity);
static void func_0015FD38(Entity *entity, s32 unknown);
static void func_001601B0(Entity *entity, s32 unknown);
static void func_00160630(Entity *entity, s32 unknown);
static void func_00160B30(Entity *entity, s32 unknown);
static void func_001610C0(Entity *entity);
static void func_00161170(Entity *entity);
static void func_00161550(Entity *entity);
static void func_001616D8(Entity *entity);
static void func_00161860(Entity *entity);
static void func_00161A48(Entity *entity);
static void func_00161E20(Entity *entity);
static void func_001620D8(Entity *entity);
static void func_00162410(Entity *entity);
static void func_001625A0(Entity *entity);
static void func_00162738(Entity *entity);
static void func_00162898(Entity *entity);
static s32 func_00162A48(Entity *entity, f32 reach, f32 angle_limit);
static void func_00162B48(Entity *entity);

/* Set-up of a type 54 entity: clears its work area and installs the type's handlers. */
void EntityType54_Init(Entity *entity) {
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
    unkE0->unk00 = func_0015FD38;
    unkE0->unk04 = (void (*)(void))func_0015FB98;
    unkE0->unk08 = (void (*)(void))func_0015FAC8;
    unkE0->unk18 = (void (*)(void))func_001610C0;
    unkE0->unk10 = (void (*)(void))func_00181038;
    unkE0->unk14 = (void (*)(void))func_00161860;
    for (i = 3; i != -1; i--) {
        entity->unkBC[i] = 0x80;
    }
    for (i = 5; i != -1; i--) {
        entity->unkC0[i] = 0x80;
    }
    entity->target = NULL;
    entity->flags |= 0x10000000;
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
    // match: called as returning an int, which keeps $2 busy after the call
    ((int (*)(EntityModel *, s32, s32))func_00193378)(entity->model, 0, 0);
    entity->flags |= 0x40000;
}

static void func_0015FAC8(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    func_00190A30((char *)entity);
    entity->flags &= ~0x200;
    if (((s32)(entity->flags >> 36) & 3) == 1) {
        if ((s32)(entity->flags >> 19) & 1) {
            func_0019A310((char *)entity, (void (*)(void))func_00181378, 0);
        } else {
            func_0019A310((char *)entity, (void (*)(void))func_00161550, 0);
        }
    } else if (((s32)(entity->flags >> 36) & 3) == 2) {
        func_0019A310((char *)entity, (void (*)(void))func_001616D8, 0);
    }
    func_00195898((char *)entity, 2);
    unkE0->unk68 = 0;
    unkE0->unk6A = entity->params->unk438;
    entity->unkD6 = 0x80;
}

static void func_0015FB98(Entity *entity) {
    f32 limits[4];
    s32 look_at_target;
    u32 blend_frames; // match: converted to float as unsigned
    s64 flags;
    EntityUnkE0 *unkE0;

    func_00161170(entity);
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
            func_00196120(entity, unkE0->unk20, -1, NULL, 8.0f);
        }
    }
}

static void func_0015FD38(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityUnkE0Kind15F8C0 *timers = (EntityUnkE0Kind15F8C0 *)unkE0;
    f32 height;

    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        func_00196870(entity, 1);
    }
    if (D_0034A018[0] == 0) {
        entity->model->unk102 |= 0x800;
        ((EntityFlagWord *)&entity->spawn_info->unk38)->bytes[3] = 0x1E;
        entity->unkD6 = 0x80;
        ENTITY_FLAGS(entity) |= 0x200000;
        ENTITY_FLAGS(entity) |= 0x40000;
        return;
    }
    ENTITY_FLAGS(entity) &= ~0x40000;
    if (timers->unk6C > 0) {
        timers->unk6C--;
    }
    if (D_00413F80.unk259 != 0) {
        timers->unk6C = entity->params->unk43A;
    }
    func_00162B48(entity);
    if (entity->unkC6 == 0) {
        func_00196CC0(entity);
        func_0018E9C0(entity);
        if (D_00413F80.position[0] >= 6600.0f) {
            ENTITY_FLAGS(entity) |= 0x40000;
            ENTITY_FLAGS(entity) &= ~0x3800;
            entity->target = entity->spawn_info;
            entity->unkC8 = func_00190F90(entity, 0, 500.0f) | 0x440;
            func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, unknown);
        } else if ((s32)(ENTITY_FLAGS(entity) >> 53) & 1) {
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
                if (timers->unk7C == 0) {
                    func_001601B0(entity, unknown);
                } else if (timers->unk7C == 1) {
                    func_00160630(entity, unknown);
                } else {
                    func_00160B30(entity, unknown);
                }
                break;
            default:
                func_00182FD8(entity, unknown);
                break;
            }
        }
    }
    if (((s32)(ENTITY_FLAGS(entity) >> 28) & 1) && !(entity->unkC6 & 0x1000)) {
        height = func_0018A9F0(entity->unk14, entity->model->position);
        if (height != D_0044681C[0]) {
            if (height > 45.0f) {
                timers->unk7A = 1;
            } else if (height < 35.0f) {
                timers->unk7A = 0;
                timers->unk7B = 1;
            }
            if (timers->unk7A != 0) {
                entity->unkC8 |= 0x20;
            } else {
                entity->unkC8 |= 0x10;
            }
        }
    }
    if (timers->unk7C == 0) {
        if (timers->unk68 == 0 && timers->unk6A == 0 && timers->unk7D == 0 && rand() % 100 < 60) {
            timers->unk7D = 1;
            timers->unk7E = 20;
        }
        if (timers->unk7D != 0) {
            if (timers->unk7E != 0) {
                entity->unkD6 = (timers->unk7E << 7) / 20;
                timers->unk7E--;
            } else {
                timers->unk68 = entity->params->unk434;
                timers->unk7D = 0;
                func_0018DA38(entity, 7, 0, 0);
            }
        }
    }
    if (timers->unk68 != 0) {
        entity->model->unk102 |= 0x800;
        ((EntityFlagWord *)&entity->spawn_info->unk38)->bytes[3] = 0x1E;
        entity->unkD6 = 0x80;
        ENTITY_FLAGS(entity) |= 0x200000;
        if (--timers->unk68 <= 0) {
            timers->unk6A = entity->params->unk438;
        }
    } else {
        entity->model->unk102 &= ~0x800;
        ((EntityFlagWord *)&entity->spawn_info->unk38)->bytes[3] = 0x1F;
        ENTITY_FLAGS(entity) &= ~0x200000;
        if (timers->unk6A != 0) {
            timers->unk6A--;
        }
    }
}

static void func_001601B0(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    f32 angle;
    s32 mode;
    s32 side;

    entity->flags &= ~0x3800;
    if (func_00162A48(entity, 600.0f, 0.5235988f)) {
        func_001958B8(entity, func_001625A0, 1);
        return;
    }
    if (func_0018EEE0(entity, 300.0f, 1.0471976f)) {
        func_001958B8(entity, func_00162410, 1);
        return;
    }
    entity->flags |= 0x200;
    func_001957B8(D_00413F80.unk14, func_00195478(D_00413F80.position, (char *)entity->model->position));
    unkE0->unk5C = 100.0f;
    unkE0->unk60 = 250.0f;
    if (entity->unkA2 == 0 && func_00197F00(entity)) {
        func_0018F648(entity);
        func_001958B8(entity, func_00161A48, unknown);
        return;
    }
    mode = func_0018FD50(entity, 0);
    if (entity->unkA0 == 0) {
        entity->unkCF = entity->unkCD;
        entity->unkCD = mode;
        if (entity->unkCD != entity->unkCF) {
            entity->unk9E = D_00446ED4;
        }
        func_00190B88(entity, mode);
        if (entity->unkCD == 1) {
            entity->target = &D_00413F80;
            angle = func_001957B8(D_00413F80.unk14, entity->unk68);
            if (entity->distance_to_player < 350.0f && ABS(angle) < 0.6981317f) {
                func_001958B8(entity, func_001620D8, unknown);
                return;
            }
            if (func_001957B8(D_00413F80.unk14, func_00195478(D_00413F80.position, (char *)entity->model->position)) > 0.0f) {
                unkE0->unk78 = 2;
            } else {
                unkE0->unk78 = 3;
            }
            entity->unkA0 = 30;
        } else {
            unkE0->unk78 = -1;
        }
    }
    switch (unkE0->unk78) {
    case 0:
        side = func_0018EAC0(entity, direction);
        if (side > 0) {
            entity->unkC8 |= 0x201;
        } else if (side < 0) {
            entity->unkC8 |= 0x101;
        } else {
            entity->unkC8 |= func_00190F90(entity, 0, entity->radius + 100.0f + 30.0f);
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
            if (entity->distance_to_player > unkE0->unk60) {
                entity->unkC8 |= 1;
            } else if (entity->distance_to_player < unkE0->unk5C) {
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
            if (entity->distance_to_player > unkE0->unk60) {
                entity->unkC8 |= 1;
            } else if (entity->distance_to_player < unkE0->unk5C) {
                entity->unkC8 |= 2;
            }
        }
        break;
    case 4:
        entity->unkC8 |= func_00190BE0(entity);
        break;
    }
    func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, unknown);
}

static void func_00160630(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityUnkE0Kind15F8C0 *timers = (EntityUnkE0Kind15F8C0 *)unkE0;
    f32 direction[4];
    f32 angle;
    s32 mode;
    s32 side;
    s32 may_strike;

    entity->flags &= ~0x3800;
    if (func_00162A48(entity, 600.0f, 0.5235988f)) {
        func_001958B8(entity, func_001625A0, 1);
        return;
    }
    if (func_0018EEE0(entity, 300.0f, 1.0471976f)) {
        func_001958B8(entity, func_00162410, 1);
        return;
    }
    entity->flags |= 0x200;
    func_001957B8(D_00413F80.unk14, func_00195478(D_00413F80.position, (char *)entity->model->position));
    may_strike = TRUE;
    unkE0->unk5C = 100.0f;
    unkE0->unk60 = 250.0f;
    mode = func_0018FD50(entity, 0);
    if (entity->unkA0 == 0 || mode == 0) {
        entity->unkCF = entity->unkCD;
        entity->unkCD = mode;
        if (entity->unkCD != entity->unkCF) {
            entity->unk9E = D_00446ED4;
        }
        func_00190B88(entity, mode);
        switch (entity->unkCD) {
        case 0:
            if (func_0018F030((char *)entity)) {
                func_001958B8(entity, func_00161E20, unknown);
            } else {
                func_001958B8(entity, func_00161A48, unknown);
            }
            return;
        case 1:
            entity->target = &D_00413F80;
            angle = func_001957B8(D_00413F80.unk14, entity->unk68);
            if (may_strike && entity->distance_to_player < 350.0f && ABS(angle) < 0.6981317f) {
                func_001958B8(entity, func_001620D8, unknown);
                return;
            }
            if (timers->unk68 == 0 && timers->unk6A == 0 && rand() % 100 < 60) {
                func_001958B8(entity, func_00162898, unknown);
                return;
            }
            if (func_001957B8(D_00413F80.unk14, func_00195478(D_00413F80.position, (char *)entity->model->position)) > 0.0f) {
                unkE0->unk78 = 2;
            } else {
                unkE0->unk78 = 3;
            }
            entity->unkA0 = 30;
            break;
        case 2:
            entity->target = &D_00413F80;
            entity->unkA0 = 30;
            unkE0->unk78 = 4;
            break;
        default:
            unkE0->unk78 = -1;
            break;
        }
    }
    switch (unkE0->unk78) {
    case 0:
        side = func_0018EAC0(entity, direction);
        if (side > 0) {
            entity->unkC8 |= 0x201;
        } else if (side < 0) {
            entity->unkC8 |= 0x101;
        } else {
            entity->unkC8 |= func_00190F90(entity, 0, entity->radius + 100.0f + 30.0f);
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
            if (entity->distance_to_player > unkE0->unk60) {
                entity->unkC8 |= 1;
            } else if (entity->distance_to_player < unkE0->unk5C) {
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
            if (entity->distance_to_player > unkE0->unk60) {
                entity->unkC8 |= 1;
            } else if (entity->distance_to_player < unkE0->unk5C) {
                entity->unkC8 |= 2;
            }
        }
        break;
    case 4:
        entity->unkC8 |= func_00190BE0(entity);
        break;
    }
    func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, unknown);
}

static void func_00160B30(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityUnkE0Kind15F8C0 *timers = (EntityUnkE0Kind15F8C0 *)unkE0;
    f32 direction[4];
    f32 angle;
    s32 mode;
    s32 side;
    s32 may_strike;

    entity->flags &= ~0x3800;
    if (func_00162A48(entity, 600.0f, 0.5235988f)) {
        func_001958B8(entity, func_001625A0, 1);
        return;
    }
    if (func_0018EEE0(entity, 300.0f, 1.0471976f)) {
        func_001958B8(entity, func_00162410, 1);
        return;
    }
    entity->flags |= 0x200;
    angle = func_001957B8(D_00413F80.unk14, func_00195478(D_00413F80.position, (char *)entity->model->position));
    if (ABS(angle) > 0.87266463f || unkE0->unk68 != 0) {
        unkE0->unk5C = 100.0f;
        unkE0->unk60 = 250.0f;
        may_strike = FALSE;
    } else {
        unkE0->unk5C = 500.0f;
        unkE0->unk60 = 800.0f;
        may_strike = TRUE;
    }
    mode = func_0018FD50(entity, 0);
    if (entity->unkA0 == 0 || mode == 0) {
        entity->unkCF = entity->unkCD;
        entity->unkCD = mode;
        if (entity->unkCD != entity->unkCF) {
            entity->unk9E = D_00446ED4;
        }
        func_00190B88(entity, mode);
        switch (entity->unkCD) {
        case 0:
            if (func_0018F030((char *)entity)) {
                func_001958B8(entity, func_00161E20, unknown);
            } else {
                func_001958B8(entity, func_00161A48, unknown);
            }
            return;
        case 1:
            entity->target = &D_00413F80;
            angle = func_001957B8(D_00413F80.unk14, entity->unk68);
            if (may_strike && entity->distance_to_player < 350.0f && ABS(angle) < 0.6981317f) {
                func_001958B8(entity, func_001620D8, unknown);
                return;
            }
            if (entity->distance_to_player > unkE0->unk60) {
                unkE0->unk78 = 0;
            } else if (entity->distance_to_player < unkE0->unk5C) {
                unkE0->unk78 = 1;
            } else {
                if (timers->unk68 == 0 && timers->unk6A == 0 && rand() % 100 < 60) {
                    func_001958B8(entity, func_00162898, unknown);
                    return;
                }
                if (func_001957B8(D_00413F80.unk14, func_00195478(D_00413F80.position, (char *)entity->model->position)) > 0.0f) {
                    unkE0->unk78 = 2;
                } else {
                    unkE0->unk78 = 3;
                }
            }
            entity->unkA0 = 30;
            break;
        case 2:
            entity->target = &D_00413F80;
            entity->unkA0 = 30;
            unkE0->unk78 = 4;
            break;
        default:
            unkE0->unk78 = -1;
            break;
        }
    }
    switch (unkE0->unk78) {
    case 0:
        side = func_0018EAC0(entity, direction);
        if (side > 0) {
            entity->unkC8 |= 0x201;
        } else if (side < 0) {
            entity->unkC8 |= 0x101;
        } else {
            entity->unkC8 |= func_00190F90(entity, 0, entity->radius + 100.0f + 30.0f);
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
            if (entity->distance_to_player > unkE0->unk60) {
                entity->unkC8 |= 1;
            } else if (entity->distance_to_player < unkE0->unk5C) {
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
            if (entity->distance_to_player > unkE0->unk60) {
                entity->unkC8 |= 1;
            } else if (entity->distance_to_player < unkE0->unk5C) {
                entity->unkC8 |= 2;
            }
        }
        break;
    case 4:
        entity->unkC8 |= func_00190BE0(entity);
        break;
    }
    func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, unknown);
}

static void func_001610C0(Entity *entity) {
    if (entity == D_00413F80.unk204) {
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

static void func_00161170(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 turn;
    f32 step;
    f32 limit;

    if (entity == D_00413F80.unk204) {
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
    ENTITY_SCRATCH_VECTOR[0] = ENTITY_SCRATCH_VECTOR[1] = ENTITY_SCRATCH_VECTOR[2] = ENTITY_SCRATCH_VECTOR[3] = 0.0f;
    step = entity->params->unk50 * 0.5f;
    limit = entity->params->unk54 * 0.5f;
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

static void func_00161550(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 = 2;
    switch (entity->unkCB) {
    case 0:
        func_0018D8B8(entity, 0xE, 0, 0);
        func_00192C98(entity, 0xE, 0, 5, 1);
        entity->flags &= ~0x3800;
        entity->flags |= 0x4000;
        entity->flags &= ~0x80000;
        entity->flags &= ~0x800000000L;
        entity->flags &= ~0x200;
        entity->unkCB++;
        break;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->unkCB++;
        }
        break;
    case 2:
        func_0019A290(entity, entity->params->unkC8);
        func_0019A310((char *)entity, NULL, 0);
        entity->unkC6 = 0;
        entity->unkC8 = 0;
        entity->unkA0 = 0;
        if ((s32)(entity->flags >> 5) & 1) {
            unkE0->unk00(entity, 1);
        } else {
            func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, 1);
        }
        break;
    }
}

static void func_001616D8(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 = 4;
    switch (entity->unkCB) {
    case 0:
        func_00192C98(entity, 0xF, 0, 5, 1);
        entity->flags &= ~0x3800;
        entity->flags |= 0x4000;
        entity->flags &= ~0x80000;
        entity->flags &= ~0x800000000L;
        entity->flags &= ~0x200;
        entity->unkCB++;
        break;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->unkCB++;
        }
        break;
    case 2:
        if ((s32)(entity->flags >> 34) & 1) {
            func_0019A290(entity, entity->params->unkC8);
            func_0019A310((char *)entity, NULL, 0);
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

static void func_00161860(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 = 0x40;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 0x10, 0, 5, 1);
        ENTITY_FLAGS(entity) &= ~0x3800;
        entity->model->unk102 &= ~0x800;
        unkE0->unk68 = 0;
        ((EntityFlagWord *)&entity->spawn_info->unk38)->bytes[3] = 0x1F;
        entity->unkD6 = 0x80;
        ENTITY_FLAGS(entity) |= 0x4000;
        ENTITY_FLAGS(entity) |= 0x100000;
        ENTITY_FLAGS(entity) = (ENTITY_FLAGS(entity) & ~0xC) | 4;
        ENTITY_FLAGS(entity) |= 0x8000000;
        ENTITY_FLAGS(entity) |= 0x4000000;
        ENTITY_FLAGS(entity) &= ~0x10000000;
        ENTITY_FLAGS(entity) &= ~0x200;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if ((s32)(ENTITY_FLAGS(entity) >> 34) & 1) {
            ENTITY_FLAGS(entity) &= ~0x4000;
            ENTITY_FLAGS(entity) = (ENTITY_FLAGS(entity) & ~0xC) | 8;
            ENTITY_FLAGS(entity) |= 0x10;
            entity->unkA0 = 20;
            func_0018DCD8(entity);
            entity->action_step++;
        }
        break;
    case 3:
        if (entity->unkA0 != 0) {
            entity->model->unk15C = (f32)entity->unkD6 * (f32)entity->unkA0 / 20.0f;
        } else {
            ENTITY_FLAGS(entity) |= 0xC;
            ENTITY_FLAGS(entity) &= ~2;
        }
        break;
    }
}

static void func_00161A48(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityParamsUnk240 *unk240 = &entity->params->unk240[ENTITY_FLAG_BITS(entity)->bits58];
    s32 frame;

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, ENTITY_FLAG_BITS(entity)->bits58 | 0x14, 0, entity->params->unkC4 >> 2, 1);
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
            entity->unkC8 |= func_00190BE0(entity);
        }
        return;
    case 2:
        if (unk240->unk01 >= entity->model->unkFC->unk82) {
            entity->unkC8 |= func_00190BE0(entity);
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

static void func_00161E20(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    s32 attack_index = (s32)(entity->flags >> 58) & 3;
    EntityParamsUnk3D8 *unk3D8 = ENTITY_PARAMS_UNK3D8(entity->params, attack_index);
    f32 target[4];
    s32 elapsed;

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, attack_index + 0x17, 0, entity->params->unkC4 >> 2, 1);
        if (entity->params->flags & 0x100) {
            ENTITY_FLAGS(entity) &= ~0x3800;
        } else {
            ENTITY_FLAG_BITS(entity)->bits11 = ((((EntityParamsWord *)entity->params)->unk00 & 0x06000000) == 0x02000000) ? 2 : 1;
        }
        ENTITY_FLAGS(entity) &= ~0x200;
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
        elapsed = (entity->model->unkFC->unk84 - entity->params->unk80[entity->unkD1]) >> 8;
        func_00216778(target, D_00413F80.position);
        target[1] -= 190.0f;
        func_0018CF30(entity, unk3D8, elapsed, entity->model->unkFC->unk82, target, 0.7853982f, 0.17453292f);
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

static void func_001620D8(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 height;
    s32 run = FALSE;

    entity->unkC6 |= 0x1000;
    entity->flags &= ~0x3800;
    entity->flags &= ~0x200;
    height = func_0018A9F0(entity->unk14, entity->model->position);
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 8, 0, entity->params->unkC4, 1);
        unkE0->unk50 = func_00194FB8(entity->model->unkD0[1] + 3.1415927f);
        unkE0->unk54 = unkE0->unk48;
        unkE0->unk58 = 0.0f;
        unkE0->unk44 = 0.0f;
        unkE0->unk48 = 0.0f;
        unkE0->unk4C = 0.0f;
        unkE0->unk1C = 0.0f;
        entity->action_step++;
    case 1:
        func_00192C98(entity, 8, 0, entity->params->unkC4, 0);
        unkE0->unk58 = func_00195018(unkE0->unk58, entity->params->unk54, entity->params->unk50);
        unkE0->unk54 = func_00195018(unkE0->unk54, -20.0f, 2.0f);
        if (height == D_00446820[0] || height >= 400.0f) {
            entity->action_step++;
        }
        break;
    case 2:
        func_00192C98(entity, 4, 0, entity->params->unkC4, 0);
        unkE0->unk58 = func_00195018(unkE0->unk58, entity->params->unk54, entity->params->unk50);
        unkE0->unk54 = func_00195018(unkE0->unk54, 20.0f, 2.0f);
        if (unkE0->unk54 > 0.0f) {
            entity->unkC8 |= ((u16 (*)(Entity *))func_00190BE0)(entity); // match: read as unsigned here
            entity->unkC8 |= 0x400;
        }
        if (entity->flags & (0x8200L << 35)) {
            func_00192C98(entity, 9, 0, 0, 1);
            unkE0->unk58 = unkE0->unk54 = 0.0f;
            entity->action_step++;
        }
        break;
    case 3:
        if ((s32)(entity->flags >> 34) & 1) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if ((s32)(entity->flags >> 5) & 1) {
                entity->unkE0.unk00(entity, 1);
            } else {
                func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, 1);
            }
        }
        break;
    }
    ENTITY_SCRATCH_VECTOR[1] = unkE0->unk54;
    ENTITY_SCRATCH_VECTOR[2] = unkE0->unk58 + unkE0->unk58;
    ENTITY_SCRATCH_VECTOR[0] = ENTITY_SCRATCH_VECTOR[3] = 0.0f;
    sceVu0RotMatrixY(ENTITY_SCRATCH_MATRIX, D_00431AF0, unkE0->unk50);
    func_00216BC0(ENTITY_SCRATCH_VECTOR, ENTITY_SCRATCH_MATRIX, ENTITY_SCRATCH_VECTOR);
    // match: never taken, but until the compiler works that out the branch separates the
    // scratchpad stores above from the loads below, so their addresses are not kept in registers
    if (run) {
        Vector_Scale(ENTITY_SCRATCH_VECTOR, ENTITY_SCRATCH_VECTOR, 2.0f);
    }
    entity->unk84 += ENTITY_SCRATCH_VECTOR[0];
    entity->unk88 += ENTITY_SCRATCH_VECTOR[1];
    entity->unk8C += ENTITY_SCRATCH_VECTOR[2];
}

static void func_00162410(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 8;
    entity->flags &= ~0x200;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 0xA, 0, 5, 1);
        entity->flags = (entity->flags & ~0x3800) | 0x800;
        entity->action_step++;
        break;
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
            if (rand() % 100 < 50) {
                func_001958B8(entity, func_00162738, 1);
            } else if ((s32)(entity->flags >> 5) & 1) {
                unkE0->unk00(entity, 1);
            } else {
                func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, 1);
            }
        }
        break;
    }
}

static void func_001625A0(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 8;
    entity->flags &= ~0x200;
    switch (entity->action_step) {
    case 0:
        if (func_001957B8(D_00413F80.unk14, func_00195478(D_00413F80.position, (char *)entity->model->position)) > 0.0f) {
            func_00192C98(entity, 0x1D, 0, 5, 1);
        } else {
            func_00192C98(entity, 0x1E, 0, 5, 1);
        }
        entity->flags = (entity->flags & ~0x3800) | 0x800;
        entity->action_step++;
        break;
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

static void func_00162738(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 0x2000;
    entity->flags |= 0x200;
    entity->unkC8 |= func_00190BE0(entity);
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 0x12, 0, entity->params->unkC4 >> 2, 0);
        entity->flags &= ~0x3800;
        entity->action_step++;
        break;
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

static void func_00162898(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 0x4000;
    entity->unkC8 |= func_00190BE0(entity);
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 0x11, 0, entity->params->unkC4 >> 2, 0);
        entity->flags &= ~0x3800;
        entity->action_step++;
        break;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (entity->params->unk436 >= entity->model->unkFC->unk82) {
            func_0019A3F0(entity, 0x11);
            entity->unkD6 = ((entity->params->unk436 - entity->model->unkFC->unk82) << 7) / entity->params->unk436;
        } else {
            unkE0->unk68 = entity->params->unk434;
        }
        if ((s32)(ENTITY_FLAGS(entity) >> 34) & 1) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if ((s32)(ENTITY_FLAGS(entity) >> 5) & 1) {
                unkE0->unk00(entity, 1);
            } else {
                func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, 1);
            }
        }
        break;
    }
}

/* Whether to start an attack now: only while the counter unk6C runs, the entity is free, the player is within reach
   and angle_limit of its heading, and then by a chance of a fifth of EntityParams.unkCA percent. */
static s32 func_00162A48(Entity *entity, f32 reach, f32 angle_limit) {
    f32 distance;
    f32 turn;

    if (entity->unkE0.unk6C <= 0) {
        return 0;
    }
    distance = entity->distance_to_player - (entity->radius + 100.0f);
    turn = func_001957B8(entity->model->unkD0[1], entity->unk68);
    if (entity->unkC6 != 0) {
        return 0;
    }
    if (reach < distance) {
        return 0;
    }
    if (angle_limit < (0.0f <= turn ? turn : -turn)) {
        return 0;
    }
    return rand() % 100 < (s32)(entity->params->unkCA / 5U);
}

/* Counts the first six objects whose flags low nibble is 3, then stores (count + 2) / 3. */
static void func_00162B48(Entity *entity) {
    EntityUnkE0 *sub;
    Entity *object;
    int count;
    int next;
    int i;

    sub = &entity->unkE0;
    count = 0;
    i = 0;
    do {
        object = Entity_GetBySpawnId(i + 0x65);
        if (object != NULL) {
            next = count + 1;
            count = ((object->flags & 0xF) ^ 3) ? count : next;
        }
        i += 1;
    } while (i < 6);
    sub->unk7C = (char)((count + 2) / 3);
}
