/* The behaviour of entity type 4 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers that function installs. */
#include "kf_entity.h"
#include "common.h"

void func_001958A8(char *, s32);
int func_001958B8(Entity *, void (*)(Entity *), int);
void func_00191478(EntityModel *);
int func_00193378(EntityModel *, s32, s32);
void func_00216778(void *, void *);
void func_00181038(Entity *);
void func_001818B8(Entity *);
extern s32 D_003E6E40[][2];
int func_00192EB0(void *, int);
// match: the callee is void, but retail keeps what it leaves in $2
int func_00194368(EntityModel *, f32);
float func_001957B8(float, float);
float func_00195388(float *, float, float);
void func_00190A30(char *);
void func_00195898(char *, u8);
int func_0019A310(char *, void (*)(void), int);
void func_001816A0(Entity *);
extern u32 D_00446ED4;
void func_00196870(void *, int);
void func_0018E9C0(void *);
int rand(void);
s32 func_0018ED98(Entity *, f32, f32);
s32 func_0018FD50(Entity *, s32);
void func_00190B88(Entity *, s32);
int func_0018F030(char *);
void func_00181C60(Entity *);
void func_00181E48(Entity *);
int func_00192C98(void *, int, int, u32, int);
extern PlayerData D_00413F80;
s32 func_00199EB8(Entity *, f32 *, f32);
int func_0018CF30(Entity *, EntityParamsUnk3D8 *, s32, s32, f32 *, f32, f32);
void func_0019A290(Entity *, s32);

static void func_0012A0A8(Entity *entity);
static void func_0012A228(Entity *entity);
static void func_0012A290(Entity *entity, s32 unknown);
static void func_0012A370(Entity *entity, s32 unknown);
static void func_0012A478(Entity *entity, s32 unknown);
static void func_0012A6D8(Entity *entity);
static void func_0012A778(Entity *entity);
static void func_0012A8E0(Entity *entity);
static void func_0012AA48(Entity *entity);

/* Set-up of a type 4 entity: clears its work area and installs the type's handlers. */
void EntityType4_Init(Entity *entity) {
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
    unkE0->unk00 = func_0012A290;
    unkE0->unk04 = (void (*)(void))func_0012A0A8;
    unkE0->unk08 = (void (*)(void))func_0012A228;
    unkE0->unk18 = (void (*)(void))func_0012A6D8;
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
    if (unk138 != NULL) {
        func_00216778(unkE0->unk20, unk138 + 0x50);
    }
    unkE0->unk40 = 0.5f;
    entity->unkD6 = (entity->params->flags & 0x4000) ? 0x40 : 0x80;
    entity->model->unk15C = entity->unkD6;
    func_00191478(entity->model);
    func_00193378(entity->model, 0, 0);
}

static void func_0012A0A8(Entity *entity) {
    u32 blend_frames; // match: converted to float as unsigned
    s64 flags;
    EntityUnkE0 *unkE0;
    EntityModelPart80 *part;
    f32 angle;

    unkE0 = &entity->unkE0;
    flags = entity->flags;
    part = entity->model->unk138;
    if (ENTITY_FLAG_BITS(entity)->bit14) {
        blend_frames = entity->model->unkFC->unk88;
        if (blend_frames != 0) {
            ENTITY_FLAG_BITS(entity)->bit34 = func_00194368(entity->model, (entity->unkC6 & 2) ? unkE0->unk40 : 1.0f / (f32)blend_frames);
        } else {
            ENTITY_FLAG_BITS(entity)->bit34 = func_00192EB0(entity, (s32)(flags >> 11) & 7);
        }
        if (part != NULL) {
            if ((u8)(entity->unkD1 - 0xE) < 3) {
                unkE0->unk20[1] = -part->unk50[1];
            } else {
                angle = func_001957B8(entity->model->unkD0[1], entity->unk68);
                if (angle < -0.5235988f) {
                    angle = -0.5235988f;
                }
                if (0.5235988f < angle) {
                    angle = 0.5235988f;
                }
                func_00195388(&unkE0->unk20[1], angle, 10.0f);
                part->unk50[1] = -unkE0->unk20[1];
            }
        }
    }
}

static void func_0012A228(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    func_00190A30((char *)entity);
    entity->flags &= ~0x200;
    func_0019A310((char *)entity, (void (*)(void))func_001816A0, 0);
    func_00195898((char *)entity, 2);
    unkE0->unk7A = 1;
}

static void func_0012A290(Entity *entity, s32 unknown) {
    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        func_00196870(entity, 0);
    }
    if (entity->unkC6 != 0) {
        return;
    }
    entity->flags |= 0x200;
    func_0018E9C0(entity);
    switch (entity->state) {
    case 0:
    case 1:
        break;
    case 2:
        func_0012A478(entity, unknown);
        return;
    }
    func_0012A370(entity, unknown);
}

static void func_0012A370(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 angle;

    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    angle = func_001957B8(entity->model->unkD0[1], entity->unk68);
    if (ABS(angle) > 0.43633232f) {
        if (angle >= 0.0f) {
            func_001958B8(entity, func_0012A8E0, unknown);
        } else {
            func_001958B8(entity, func_0012A778, unknown);
        }
    } else {
        func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, unknown);
    }
}

static void func_0012A478(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    s32 side;
    f32 angle;

    ENTITY_FLAG_BITS(entity)->bits11 = 1;
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
                func_001958B8(entity, func_0012AA48, unknown);
            } else {
                func_001958B8(entity, func_00181E48, unknown);
            }
            unkE0->unk7A = 0;
            return;
        case 1:
            angle = func_001957B8(entity->model->unkD0[1], entity->unk68);
            if (ABS(angle) > 0.43633232f) {
                if (angle >= 0.0f) {
                    func_001958B8(entity, func_0012A8E0, unknown);
                    return;
                }
                func_001958B8(entity, func_0012A778, unknown);
                return;
            }
            unkE0->unk7A = 0;
            unkE0->unk78 = -1;
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
    func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, unknown);
}

static void func_0012A6D8(Entity *entity) {
    u16 command;

    command = entity->unkC8;
    if (command & 0x100) {
        if (command & 0x400) {
            func_00192C98(entity, 7, 0, entity->params->unkC4 >> 1, 0);
            return;
        }
        func_00192C98(entity, 0xC, 0, entity->params->unkC4, 0);
        return;
    }
    if (command & 0x200) {
        if (command & 0x400) {
            func_00192C98(entity, 6, 0, entity->params->unkC4 >> 1, 0);
            return;
        }
        func_00192C98(entity, 0xB, 0, entity->params->unkC4, 0);
        return;
    }
    func_00192C98(entity, 0, 0, entity->params->unkC4, 0);
}

static void func_0012A778(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 0x1000;
    switch (entity->action_step) {
    case 0:
        if (unkE0->unk7A != 0) {
            func_00192C98(entity, 6, 0, entity->params->unkC4 >> 1, 1);
        } else {
            func_00192C98(entity, 0xB, 0, entity->params->unkC4, 1);
        }
        ENTITY_FLAG_BITS(entity)->bits11 = 3;
        ENTITY_UNK5C(entity)->as_s32 = 0;
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

static void func_0012A8E0(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 0x2000;
    switch (entity->action_step) {
    case 0:
        if (unkE0->unk7A != 0) {
            func_00192C98(entity, 7, 0, entity->params->unkC4 >> 1, 1);
        } else {
            func_00192C98(entity, 0xC, 0, entity->params->unkC4, 1);
        }
        ENTITY_FLAG_BITS(entity)->bits11 = 3;
        ENTITY_UNK5C(entity)->as_s32 = 0;
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

static void func_0012AA48(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    s32 kind = (s32)(entity->flags >> 58) & 3;
    EntityParamsUnk3D8 *unk3D8 = ENTITY_PARAMS_UNK3D8(entity->params, kind);
    f32 direction[4];
    s32 elapsed;

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, kind + 0x17, 0, entity->params->unkC4 >> 1, 1);
        entity->flags &= ~0x3800;
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
        elapsed = (entity->model->unkFC->unk84 - entity->params->unk80[entity->unkD1]) >> 8;
        func_00216778(direction, &D_00413F80);
        direction[1] -= 160.0f;
        func_0018CF30(entity, unk3D8, elapsed, entity->model->unkFC->unk82, direction, 0.2617994f, 0.2617994f);
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
