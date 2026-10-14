/* The behaviour of entity type 29 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers that function installs. */
#include "kf_entity.h"
#include "common.h"
#include "kf_entity_world.h"

void func_001958A8(char *, s32);
int func_001958B8(Entity *entity, void (*handler)(Entity *), int run_now);
void func_00191478(EntityModel *);
int func_00193378(EntityModel *, s32, s32);
void func_00216778(void *, void *);
void func_00182C80(Entity *, s32);
void func_00181038(Entity *);
void func_001818B8(Entity *);
extern s32 D_003E6E40[][2];
void func_00190A30(char *);
void func_00195898(char *, u8);
int func_0019A310(char *, void (*)(void), int);
extern void func_001816A0(Entity *);
extern u32 D_00446ED4;
void func_00196870(void *, int);
extern float D_00445194;
extern float D_00445198;
s32 func_00190F90(Entity *, s32, f32);
float func_00194F80(void *, void *);
s32 func_00197078(Entity *);
int func_0018CA50(int *);
int func_0018F030(char *);
float func_001957B8(float, float);
void func_00181E48(Entity *);
void func_00182240(Entity *);
int func_00182DE0(void *, int);
void func_0018E9C0(void *);
void func_00196CC0(void *);
extern PlayerData D_00413F80;
extern char D_0043E8C0[];
int printf(const char *, ...);
s32 func_0018ED98(Entity *, f32, f32);
s32 func_0018EEE0(Entity *, f32, f32);
s32 func_00198D10(Entity *);
s32 func_00197F00(Entity *);
s32 func_0018F9C0(Entity *);
s32 func_0018F648(Entity *);
void func_00196D48(Entity *);
int func_00190BE0(Entity *);
int func_0018EAC0(void *, void *);
void func_00181AE0(Entity *);
void func_00181C60(Entity *);
extern int D_00414184[];
int func_00192C98(void *, int, int, u32, int);
double atan2(double, double);
float func_00194FB8(float);
float func_00195388(float *, float, float);
int func_00192EB0(void *, int);
// match: the callee is void, but retail keeps what it leaves in $2
int func_00194368(EntityModel *, f32);
int func_00196120(Entity *, void *, s32, void *, f32);

static void func_0013EBD0(Entity *entity);
static void func_0013EC40(Entity *entity, s32 unknown);
static void func_0013ED58(Entity *entity, s32 unknown);
static void func_0013EE40(Entity *entity, s32 unknown);
static void func_0013EFB8(Entity *entity, s32 unknown);
static void func_0013F0D8(Entity *entity, s32 unknown);
static void func_0013F1B8(Entity *entity, s32 unknown);
static void func_0013F6A0(Entity *entity);
static void func_0013F7D8(Entity *entity);
static void func_0013F8F0(Entity *entity);

/* Set-up of a type 29 entity: clears its work area and installs the type's handlers. */
void EntityType29_Init(Entity *entity) {
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
    unkE0->unk04 = (void (*)(void))func_0013F8F0;
    unkE0->unk08 = (void (*)(void))func_0013EBD0;
    unkE0->unk18 = (void (*)(void))func_0013F6A0;
    unkE0->unk10 = (void (*)(void))func_00181038;
    unkE0->unk14 = (void (*)(void))func_001818B8;
    if (entity->spawn_info->unk40 == 1) {
        unkE0->unk00 = (void (*)(Entity *, s32))func_0013EC40;
    } else if (entity->spawn_info->unk40 == 2) {
        unkE0->unk00 = (void (*)(Entity *, s32))func_0013F0D8;
    } else {
        unkE0->unk00 = func_00182C80;
    }
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

static void func_0013EBD0(Entity *entity) {
    u64 flags;

    func_00190A30((char *)entity);
    entity->target = NULL;
    entity->flags &= ~0x400;
    entity->flags &= ~0x200;
    flags = entity->flags;
    if (((flags >> 36) & 3) == 2) {
        func_0019A310((char *)entity, (void (*)(void))func_001816A0, 0);
    }
    func_00195898((char *)entity, 2);
}

static void func_0013EC40(Entity *entity, s32 unknown) {
    Entity *other;
    EntityUnkE0 *unkE0 = &entity->unkE0;
    u32 phase;

    other = Entity_GetBySpawnIdForMap(entity->group_id, 0x29);
    if (other != NULL && (s32)other->flags & 1) {
        if ((other->flags & 0xE) != 2) {
            void (*handler)(Entity *, s32) = func_0013F0D8;

            unkE0->unk00 = handler;
            handler(entity, unknown);
            return;
        }
    }
    phase = D_00446ED4 % 10U;
    if (phase == entity->unkD7 % 10U) {
        func_00196870(entity, 1);
    }
    if (entity->unkC6 == 0) {
        if (unkE0->unk78 == 0) {
            func_0013ED58(entity, unknown);
        } else {
            if (unkE0->unk79 != 0) {
                func_0013EE40(entity, unknown);
            } else {
                func_0013EFB8(entity, unknown);
            }
        }
    }
}

static void func_0013ED58(Entity *entity, s32 unknown) {
    f32 point[4];
    f32 limit;
    EntityUnkE0 *unkE0;
    f32 *target;

    limit = 50.0f;
    target = entity->unkE0.unk30;
    unkE0 = &entity->unkE0;
    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    point[0] = D_00445194;
    point[1] = entity->spawn_info->spawn_y;
    point[2] = D_00445198;
    point[3] = 0;
    func_00216778(target, point);
    entity->target = target;
    entity->unkC8 = func_00190F90(entity, 0, 25.0f);
    if (func_00194F80(entity->model->position, point) < limit) {
        unkE0->unk78 = 1;
    }
    func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, unknown);
}

static void func_0013EE40(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 home[4];
    f32 angle;
    f32 range = 100.0f;

    home[0] = -7900.0f;
    home[1] = entity->spawn_info->spawn_y;
    home[2] = 3050.0f;
    home[3] = 0.0f;
    if (func_00194F80(entity->model->position, home) > range) {
        unkE0->unk78 = 0;
    }
    // match: func_00197078 takes the entity
    if (((int (*)(Entity *))func_00197078)(entity) && !func_0018CA50((int *)entity)) {
        if (func_0018F030((char *)entity)) {
            func_001958B8(entity, func_00182240, 1);
        } else {
            func_001958B8(entity, func_00181E48, 1);
        }
        return;
    }
    ENTITY_FLAG_BITS(entity)->bits11 = 0;
    angle = func_001957B8(entity->model->unkD0[1], unkE0->unk48);
    if (ABS(angle) > entity->params->turn_limit) {
        if (angle >= 0.0f) {
            entity->unkC8 |= 0x200;
        } else {
            entity->unkC8 |= 0x100;
        }
    } else {
        unkE0->unk79 = 0;
    }
    func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, unknown);
}

static void func_0013EFB8(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 angle;

    // match: func_00197078 takes the entity
    if (((int (*)(Entity *))func_00197078)(entity) && !func_0018CA50((int *)entity)) {
        if (func_0018F030((char *)entity)) {
            func_001958B8(entity, func_00182240, 1);
        } else {
            func_001958B8(entity, func_00181E48, 1);
        }
        return;
    }
    angle = func_001957B8(entity->model->unkD0[1], entity->unk68);
    if (ABS(angle) > 0.5235988f) {
        unkE0->unk48 = entity->unk68;
        unkE0->unk79 = 1;
    }
    func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, unknown);
}

static void func_0013F0D8(Entity *entity, s32 unknown) {
    u32 phase;

    phase = D_00446ED4 % 10U;
    if (phase == entity->unkD7 % 10U) {
        func_00196870(entity, 1);
    }
    if (entity->unkC6 != 0) {
        return;
    }
    if ((((EntityParamsWord *)entity->params)->unk00 & 0x06000000) != 0x06000000) {
        func_00196CC0(entity);
    }
    func_0018E9C0(entity);
    if ((s32)(entity->flags >> 53) & 1) {
        func_00182DE0(entity, unknown);
        return;
    }
    func_0013F1B8(entity, unknown);
}

static void func_0013F1B8(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 post[4];
    f32 direction[4];
    f32 angle;
    f32 chase_angle;
    f32 margin = 0.0f;
    Entity *other;
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
    if (func_00194F80(entity->spawn_info, &D_00413F80) > entity->spawn_info->unk4C) {
        if (ENTITY_FLAG_BITS(entity)->bit32) {
            if (func_00198D10(entity) && !func_0018CA50((int *)entity)) {
                func_0018F9C0(entity);
                func_001958B8(entity, func_00182240, unknown);
                return;
            }
        }
    } else if (ENTITY_FLAG_BITS(entity)->bit32) {
        if (func_00197F00(entity) && !func_0018CA50((int *)entity)) {
            func_0018F648(entity);
            func_001958B8(entity, func_00181E48, unknown);
            return;
        }
    }
    ENTITY_FLAG_BITS(entity)->bit9 = TRUE;
    if (D_00413F80.position[0] < -7900.0f) {
        func_00216778(post, &D_00413F80);
        post[0] = -8000.0f;
        if (post[2] > 3350.0f) {
            post[2] = 3550.0f;
        } else if (post[2] < 2750.0f) {
            post[2] = 2550.0f;
        } else {
            post[2] = 3050.0f;
            other = Entity_GetBySpawnId(0x28);
            if (ENTITY_FLAG_BITS(other)->bits2 == 0) {
                other = Entity_GetBySpawnId(0x28);
                if (ENTITY_FLAG_BITS(other)->bit1) {
                    margin = entity->radius + entity->radius;
                }
            }
        }
        if (unkE0->unk78 == 0) {
            if (func_00194F80(entity->model->position, post) < margin + 60.0f) {
                unkE0->unk78 = 1;
                printf(D_0043E8C0);
            } else {
                entity->target = post;
                turn = func_0018EAC0(entity, direction);
                if (turn > 0) {
                    entity->unkC8 = 0x201;
                } else if (turn < 0) {
                    entity->unkC8 = 0x101;
                } else {
                    entity->unkC8 = func_00190F90(entity, 0, 50.0f);
                }
                func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, unknown);
                return;
            }
        } else {
            angle = func_001957B8(entity->model->unkD0[1], entity->unk68);
            if (func_00194F80(entity->model->position, post) > margin + 150.0f) {
                entity->target = post;
                unkE0->unk78 = 0;
                return;
            }
            if (ENTITY_FLAG_BITS(entity)->bit32 && func_00198D10(entity) && !func_0018CA50((int *)entity)) {
                func_0018F9C0(entity);
                func_001958B8(entity, func_00182240, unknown);
                return;
            }
            if (ABS(angle) > 0.5235988f) {
                if (angle >= 0.0f) {
                    entity->unkC8 = 0x201;
                } else {
                    entity->unkC8 = 0x101;
                }
                func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, unknown);
                return;
            }
        }
    } else {
        entity->target = &D_00413F80;
    }
    func_00196D48(entity);
    if (ENTITY_FLAG_BITS(entity)->bit31) {
        chase_angle = func_001957B8(entity->model->unkD0[1], entity->unk68);
        if (unkE0->unk79 == 0) {
            unkE0->unk79 = ABS(chase_angle) > 0.5235988f;
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
done:
    func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, unknown);
}

static void func_0013F6A0(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 turn;

    if (entity == (Entity *)D_00414184[0] && (entity->params->flags & 0x800)) {
        entity->unkC8 = 0;
    }
    if (entity->unkC8 & 0x100) {
        unkE0->unk1C -= entity->params->unk58;
        if (unkE0->unk1C < -entity->params->turn_limit) {
            unkE0->unk1C = -entity->params->turn_limit;
        }
    } else if (entity->unkC8 & 0x200) {
        unkE0->unk1C += entity->params->unk58;
        if (unkE0->unk1C > entity->params->turn_limit) {
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
    if (entity->unkC8 & 1) {
        if (entity->unkC8 & 0x40) {
            func_00192C98(entity, 4, 0, entity->params->unkC4 >> 1, 0);
        } else {
            func_00192C98(entity, 3, 0, entity->params->unkC4, 0);
        }
    } else if (entity->unkC8 & 0x100) {
        func_00192C98(entity, 0xB, 0, entity->params->unkC4, 0);
    } else if (entity->unkC8 & 0x200) {
        func_00192C98(entity, 0xC, 0, entity->params->unkC4, 0);
    } else {
        func_00192C98(entity, 0, 0, entity->params->unkC4, 0);
    }
}

static void func_0013F7D8(Entity *entity) {
    EntityModel *model;
    EntityUnkE0 *unkE0;
    EntityModelPart80 *part;
    s16 frame;
    f32 point[2];
    f32 pitch;

    model = entity->model;
    part = &((EntityModelPart80 *)model->unkF8)[15];
    unkE0 = &entity->unkE0;
    if (part != NULL) {
        frame = model->unkFC->unk82;
        if (frame < 43) {
            func_00216778(point, D_00413F80.position);
            point[1] += 400.0f;
            pitch = func_00194FB8(atan2(point[1] - model->position[1], entity->distance_to_player));
            func_00195388(&unkE0->unk4C, pitch, 10.0f);
        } else if (frame >= 54) {
            unkE0->unk4C *= (f32)(67 - frame) / 14.0f;
        }
        part->unk50[0] += unkE0->unk4C;
    }
}

static void func_0013F8F0(Entity *entity) {
    f32 limits[4];
    s32 look_at_target;
    u32 blend_frames; // match: converted to float as unsigned
    s64 flags;
    EntityUnkE0 *unkE0;

    flags = entity->flags;
    unkE0 = &entity->unkE0;
    if (ENTITY_FLAG_BITS(entity)->bit14) {
        if ((((EntityParamsWord *)entity->params)->unk00 & 0x6000000) == 0x6000000) {
            entity->flags = flags & ~0x3800L;
        }
        blend_frames = entity->model->unkFC->unk88;
        if (blend_frames != 0) {
            ENTITY_FLAG_BITS(entity)->bit34 = func_00194368(entity->model, (entity->unkC6 & 0x12) ? unkE0->unk40 : 1.0f / (f32)blend_frames);
        } else {
            ENTITY_FLAG_BITS(entity)->bit34 = func_00192EB0(entity, ENTITY_FLAG_BITS(entity)->bits11);
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
        if ((entity->unkC6 & 1) && entity->unkD1 == 0x17) {
            func_0013F7D8(entity);
        } else {
            unkE0->unk4C = 0.0f;
        }
    }
}
