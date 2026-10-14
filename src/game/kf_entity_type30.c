/* The behaviour of entity type 30 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers that function installs. */
#include "kf_entity.h"
#include "common.h"
#include "kf_entity_world.h"
#include "kf_effect.h"

#define ENTITY_WORK_AREA_SIZE 0xA0
void func_001958A8(char *, s32);
int func_001958B8(Entity *, void (*)(Entity *), int);
void func_00191478(EntityModel *);
int func_00193378(EntityModel *, s32, s32);
void func_00216778(void *, void *);
// match: the definition takes an Actor, a struct local to its file
void func_00183570(Entity *);
extern s32 D_003E6E40[][2];
int func_00192EB0(void *, int);
// match: the callee is void, but retail keeps what it leaves in $2
int func_00194368(EntityModel *, f32);
void func_00190A30(char *);
int func_0019A310(char *, void (*)(void), int);
void func_00195898(char *, u8);
void func_001816A0(Entity *);
extern PlayerData D_00413F80;
int func_00192C98(void *, int, int, u32, int);
char *func_0010CAB8(char *, u32);
void func_0018DCD8(Entity *);
int func_0019A3F0(void *, int);
extern u32 D_00446ED4;
void func_00196870(void *, int);
void func_0018E9C0(void *);
int rand(void);
s32 func_0018ED98(Entity *, f32, f32);
s32 func_0018EEE0(Entity *, f32, f32);
s32 func_0018FD50(Entity *, s32);
void func_00190B88(Entity *, s32);
int func_0018F030(char *);
int func_00190BE0(Entity *);
void func_00181AE0(Entity *);
void func_00181C60(Entity *);
void func_00181E48(Entity *);
s32 func_00199EB8(Entity *, f32 *, f32);
int func_0018CF30(Entity *, EntityParamsUnk3D8 *, s32, s32, f32 *, f32, f32);
void func_0019A290(Entity *, s32);

static void func_00135640(Entity *entity);
static void func_00135730(Entity *entity);
static void func_00135798(Entity *entity);
static void func_00135A40(Entity *entity);
static void func_00135DA0(Entity *entity);
static void func_00135EF0(Entity *entity, s32 unknown);
static void func_00136040(Entity *entity, s32 unknown);
static void func_001362A0(Entity *entity);

/* Set-up of a type 30 entity: clears its work area and installs the type's handlers. */
void EntityType30_Init(Entity *entity) {
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
    unkE0->unk00 = func_00135EF0;
    unkE0->unk04 = (void (*)(void))func_00135640;
    unkE0->unk08 = (void (*)(void))func_00135730;
    unkE0->unk18 = (void (*)(void))func_00183570;
    unkE0->unk10 = (void (*)(void))func_00135A40;
    unkE0->unk14 = (void (*)(void))func_00135798;
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
    func_00193378(entity->model, 0, 0);
}

static void func_00135640(Entity *entity) {
    u32 blend_frames; // match: converted to float as unsigned
    EntityUnkE0 *unkE0;

    unkE0 = &entity->unkE0;
    if (ENTITY_FLAG_BITS(entity)->bit14) {
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
        blend_frames = entity->model->unkFC->unk88;
        if (blend_frames != 0) {
            ENTITY_FLAG_BITS(entity)->bit34 = func_00194368(entity->model, (entity->unkC6 & 0x12) ? unkE0->unk40 : 1.0f / (f32)blend_frames);
        } else {
            ENTITY_FLAG_BITS(entity)->bit34 = func_00192EB0(entity, ENTITY_FLAG_BITS(entity)->bits11);
        }
    }
}

static void func_00135730(Entity *entity) {
    func_00190A30((char *)entity);
    entity->target = NULL;
    entity->flags &= ~0x200;
    if (((s32)(entity->flags >> 36) & 3) == 2) {
        func_0019A310((char *)entity, (void (*)(void))func_001816A0, 0);
    }
    func_00195898((char *)entity, 2);
}

static void func_00135798(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    Entity *other;
    char *part;

    entity->unkC6 = 0x40;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 0x10, 0, 5, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
        if ((((EntityParamsWord *)entity->params)->unk00 & 0x6000000) == 0x4000000) {
            entity->unk30 = 20.0f;
        }
        ENTITY_FLAG_BITS(entity)->bit14 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit21 = TRUE;
        ENTITY_FLAG_BITS(entity)->bits2 = 1;
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
            part = func_0010CAB8((char *)entity->model, 0x14);
            if (part != NULL) {
                Effect_StartSpawner(0x3BC, part, entity->model->unkD0, D_00413F80.position);
            }
            part = func_0010CAB8((char *)entity->model, 0x15);
            if (part != NULL) {
                Effect_StartSpawner(0x3BC, part, entity->model->unkD0, D_00413F80.position);
            }
            part = func_0010CAB8((char *)entity->model, 0x16);
            if (part != NULL) {
                Effect_StartSpawner(0x3BC, part, entity->model->unkD0, D_00413F80.position);
            }
            ENTITY_FLAG_BITS(entity)->bit14 = FALSE;
            unkE0->unk7A = 1;
            entity->action_step++;
        }
        break;
    case 3:
        other = Entity_GetBySpawnIdForMap(entity->group_id, entity->spawn_info->unk40);
        if (other != NULL && ((s32)other->flags & 1) && other->unkE0.unk7A != 0) {
            ENTITY_FLAG_BITS(entity)->bits2 = 2;
            ENTITY_FLAG_BITS(entity)->bit4 = TRUE;
            ENTITY_FLAG_BITS(entity)->bit20 = TRUE;
            ENTITY_FLAG_BITS(entity)->bit27 = TRUE;
            ENTITY_FLAG_BITS(entity)->bit26 = TRUE;
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

static void func_00135A40(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 0x20;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 1, 0, 0, 1);
        if (entity->params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->bits11 = (((EntityParamsWord *)entity->params)->unk00 & 0x6000000) == 0x2000000 ? 2 : 1;
        }
        unkE0->unk1C = 0.0f;
        ENTITY_FLAG_BITS(entity)->bit21 = 1;
        ENTITY_FLAG_BITS(entity)->bit22 = 1;
        ENTITY_FLAG_BITS(entity)->bit23 = 1;
        ENTITY_FLAG_BITS(entity)->bit24 = 1;
        ENTITY_FLAG_BITS(entity)->bit18 = 1;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->action_step++;
        break;
    case 1:
        if (D_00413F80.position[0] > -4900.0f && D_00413F80.position[0] < -2500.0f &&
            D_00413F80.position[2] > -5350.0f && D_00413F80.position[2] < -3350.0f) {
            if (entity->distance_to_player < entity->spawn_info->unk54 || ENTITY_FLAG_BITS(entity)->bit43) {
                func_00192C98(entity, 2, 0, entity->params->unkC4, 0);
                if (entity->params->flags & 0x100) {
                    ENTITY_FLAG_BITS(entity)->bits11 = 0;
                } else {
                    ENTITY_FLAG_BITS(entity)->bits11 = (((EntityParamsWord *)entity->params)->unk00 & 0x6000000) == 0x2000000 ? 2 : 1;
                }
                ENTITY_FLAG_BITS(entity)->bits36 = 0;
                entity->action_step++;
            }
        }
        break;
    case 2:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 3:
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            func_00195898((char *)entity, 2);
            ENTITY_FLAG_BITS(entity)->bit21 = 0;
            ENTITY_FLAG_BITS(entity)->bit22 = 0;
            ENTITY_FLAG_BITS(entity)->bit23 = 0;
            ENTITY_FLAG_BITS(entity)->bit24 = 0;
            ENTITY_FLAG_BITS(entity)->bit18 = 0;
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

static void func_00135DA0(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 0x1000;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 2, func_0019A3F0(entity, 2) - 1, entity->params->unkC4, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
        ENTITY_FLAG_BITS(entity)->bit15 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
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
            ENTITY_FLAG_BITS(entity)->bit15 = FALSE;
            func_00216778(entity->model->unkD0, &entity->spawn_info->unk10);
            func_001958B8(entity, (void (*)(Entity *))unkE0->unk10, 1);
        }
        break;
    }
}

static void func_00135EF0(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        func_00196870(entity, 2);
    }
    if (entity->unkC6 != 0) {
        return;
    }
    func_0018E9C0(entity);
    if (D_00413F80.position[0] < -4900.0f || D_00413F80.position[0] > -2500.0f ||
        D_00413F80.position[2] < -5350.0f || D_00413F80.position[2] > -3350.0f) {
        func_001958B8(entity, func_00135DA0, unknown);
        return;
    }
    if (entity->state == 2) {
        func_00136040(entity, unknown);
        return;
    }
    func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, unknown);
}

static void func_00136040(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    s32 side;

    ENTITY_FLAG_BITS(entity)->bits11 = 0;
    if (func_0018ED98(entity, 300.0f, 0.5235988f)) {
        func_001958B8(entity, func_00181C60, 1);
        return;
    }
    if (func_0018EEE0(entity, 300.0f, 0.5235988f)) {
        func_001958B8(entity, func_00181AE0, 1);
        return;
    }
    side = func_0018FD50(entity, 3);
    ENTITY_FLAG_BITS(entity)->bit9 = 0;
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
                func_001958B8(entity, func_001362A0, unknown);
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
        entity->unkC8 = func_00190BE0(entity);
    }
    func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, unknown);
}

static void func_001362A0(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityParamsUnk3D8 *unk3D8 = &entity->params->unk3D8[0].record;
    f32 direction[4];

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 0x17, 0, entity->params->unkC4 >> 1, 1);
        ENTITY_FLAGS(entity) &= ~0x3800;
        ENTITY_FLAGS(entity) &= ~0x200;
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
        if (entity->model->unkFC->unk82 >= 12 && entity->model->unkFC->unk82 <= 76 && D_00446ED4 % 3 != 0) {
            func_00216778(direction, &D_00413F80);
            direction[1] -= 100.0f;
            func_0018CF30(entity, unk3D8, 12, 76, direction, 0.3926991f, 0.17453294f);
        }
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
