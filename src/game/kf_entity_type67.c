/* The behaviour of entity type 67 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers that function installs. */
#include "kf_entity.h"
#include "kf_map.h"
#include "kf_effect.h"
#include "kf_entity_world.h"

#define ENTITY_WORK_AREA_SIZE 0xA0
extern MapSlot D_0038CD20[];
extern s32 D_003E6E40[][2];
void func_001958A8(char *, s32);
int func_001958B8(Entity *, void (*)(Entity *), int);
void func_00191478(EntityModel *);
int func_00193378(EntityModel *, s32, s32);
void func_00216778(void *, void *);
// match: the definition takes an Actor, a struct local to func_00182A08.c
void func_00182A08(void);
void func_00182BD0(char *);
void func_00183570(Entity *);
void func_001818B8(Entity *);
extern u32 D_00446ED4;
void func_00196870(void *, int);
void func_0018E9C0(void *);
int func_00182FD8(void *, int);
void func_00183240(Entity *, s32);
void func_0018D600(char **, int, u32, int);
int func_00192C98(void *, int, int, u32, int);
void func_00195898(char *, u8);
void func_0018DCD8(Entity *);
char *func_0010CAB8(char *, u32);
int rand(void);
float sinf(float);
float cosf(float);
float atan2f(float, float);
s32 func_00195AC0(Entity *, void *, s32, s32, s32);
void func_00195588(Vector4 *, float);
void Vector_Scale(float *, float *, float);

static void func_0016F140(Entity *entity, s32 unknown);
static void func_0016F348(Entity *entity);
static void func_0016F5E8(Entity *entity);
static void func_0016F7B8(Entity *entity);
static void func_0016F930(Entity *entity);
static void func_0016FE10(Entity *entity);

/* Set-up of a type 67 entity: clears its work area and installs the type's handlers. */
void EntityType67_Init(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    u8 *unkBC = entity->unkBC;
    u8 *unkC0 = entity->unkC0;
    char *unk138 = entity->model->unk138;
    MapSlotUnk10 *record;
    s32 i;

    // match: the three fills name their target inside the loop; unkBC and unkC0 above are
    // otherwise unused but retail computes them up front and copies them before each loop
    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        u8 *work_area = (u8 *)&entity->unkE0;

        work_area[i] = 0;
    }
    func_001958A8((char *)entity, D_003E6E40[entity->params->unk02][1]);
    unkE0->unk00 = func_0016F140;
    unkE0->unk04 = func_00182A08;
    unkE0->unk08 = (void (*)(void))func_00182BD0;
    unkE0->unk18 = (void (*)(void))func_00183570;
    if (entity->spawn_info->unk40 != 0) {
        unkE0->unk10 = (void (*)(void))func_0016F7B8;
    } else {
        unkE0->unk10 = (void (*)(void))func_0016F348;
    }
    unkE0->unk14 = (void (*)(void))func_001818B8;
    for (i = 3; i != -1; i--) {
        entity->unkBC[i] = 0x80;
    }
    for (i = 5; i != -1; i--) {
        entity->unkC0[i] = 0x80;
    }
    entity->flags &= ~0x10000000;
    entity->target = NULL;
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
    record = &((MapSlotUnk10 *)D_0038CD20[entity->group_id].unk10)[(s8)((EntityFlagWord *)&entity->spawn_info->unk38)->bytes[3]];
    record->unk74 = 0.5f;
    record->unk70 = 1.0f;
}

static void func_0016F140(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    MapSlotUnk10 *record;
    f32 scale;

    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        func_00196870(entity, 1);
    }
    if (entity->unkC6 == 0) {
        func_0018E9C0(entity);
        switch (entity->state) {
        case 1:
        case 2:
            func_00183240(entity, unknown);
            break;
        case 0:
        default:
            func_00182FD8(entity, unknown);
            break;
        }
    }
    if (D_00446ED4 % 20U == 0) {
        func_0018D600((char **)entity, entity->params->unk43A, 0xFF, (int)entity->model->unkD0);
    }
    if (unkE0->unk6A != 0) {
        unkE0->unk6A--;
        scale = (f32)unkE0->unk6A / 1200.0f + 0.5f;
        entity->model->unkE0[0] = unkE0->unk54 * scale;
        entity->model->unkE0[1] = unkE0->unk58 * scale;
        entity->model->unkE0[2] = unkE0->unk5C * scale;
        scale = (f32)unkE0->unk6A / 600.0f;
        record = &((MapSlotUnk10 *)D_0038CD20[entity->group_id].unk10)[(s8)((EntityFlagWord *)&entity->spawn_info->unk38)->bytes[3]];
        record->unk70 = scale * 0.75f + 0.25f;
        record->unk74 = scale * 0.375f + 0.125f;
        if (unkE0->unk6A <= 0) {
            func_001958B8(entity, func_0016F5E8, 0);
        }
    }
}

static void func_0016F348(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 0x20;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 1, 0, entity->params->unkC4, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
        unkE0->unk1C = 0.0f;
        ENTITY_FLAG_BITS(entity)->bit21 = 1;
        ENTITY_FLAG_BITS(entity)->bit20 = 1;
        ENTITY_FLAG_BITS(entity)->bit22 = 1;
        ENTITY_FLAG_BITS(entity)->bit23 = 1;
        ENTITY_FLAG_BITS(entity)->bit24 = 1;
        ENTITY_FLAG_BITS(entity)->bit18 = 1;
        ENTITY_FLAG_BITS(entity)->bit28 = 1;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->action_step++;
        break;
    case 1:
        if (entity->distance_to_player < (f32)entity->spawn_info->unk54 || ENTITY_FLAG_BITS(entity)->bit43) {
            func_00192C98(entity, 2, 0, entity->params->unkC4, 0);
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
            ENTITY_FLAG_BITS(entity)->bits36 = 0;
            entity->action_step++;
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
            ENTITY_FLAG_BITS(entity)->bit20 = 0;
            ENTITY_FLAG_BITS(entity)->bit22 = 0;
            ENTITY_FLAG_BITS(entity)->bit23 = 0;
            ENTITY_FLAG_BITS(entity)->bit24 = 0;
            ENTITY_FLAG_BITS(entity)->bit18 = 0;
            if (!(((EntityParamsWord *)entity->params)->unk00 & 0x06000000)) {
                ENTITY_FLAG_BITS(entity)->bit28 = 0;
            }
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

static void func_0016F5E8(Entity *entity) {
    entity->unkC6 = 0x40;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 0x10, 0, 5, 1);
        entity->flags &= ~0x3800;
        entity->flags |= 0x4000;
        entity->flags |= 0x100000;
        entity->flags = (entity->flags & ~0xC) | 4;
        entity->flags |= 0x8000000;
        entity->flags |= 0x4000000;
        entity->flags &= ~0x10000000;
        entity->flags &= ~0x200;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            if (entity->params->unkC6 != 0 && entity->params->unkC6 != 0xFFFF) {
                Effect_FreeForOwner(entity);
            }
            ENTITY_FLAG_BITS(entity)->bit14 = 0;
            ENTITY_FLAG_BITS(entity)->bits2 = 2;
            ENTITY_FLAG_BITS(entity)->bit4 = 1;
            entity->unkA0 = 20;
            func_0018DCD8(entity);
            entity->action_step++;
        }
        break;
    case 3:
        if (entity->unkA0 != 0) {
            entity->model->unk15C = (f32)entity->unkD6 * (f32)entity->unkA0 / 20.0f;
        } else {
            entity->flags |= 0xC;
            entity->flags &= ~2;
        }
        break;
    }
}

static void func_0016F7B8(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    Entity *other;
    f32 *point;

    entity->unkC6 |= 0x20;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 0x11, 0, 0, 1);
        entity->flags &= ~0x3800;
        entity->flags |= 0x200000;
        entity->flags |= 0x100000;
        entity->flags |= 0x40000;
        entity->flags |= 0x10000000;
        entity->flags |= 0x400000;
        entity->flags |= 0x800000;
        entity->flags |= 0x1000000;
        entity->flags |= 0x8000000;
        entity->action_step++;
    case 1:
        if (unkE0->unk7A > 0) {
            func_001958B8(entity, func_0016F930, 1);
            return;
        }
        if (unkE0->unk7A < 0) {
            func_001958B8(entity, func_0016FE10, 1);
            return;
        }
        other = Entity_GetBySpawnId(entity->spawn_info->variant);
        if (other != NULL && (other->flags & 3) == 3) {
            point = (f32 *)func_0010CAB8((char *)other->model, 8);
            if (point != NULL) {
                entity->model->position[0] = point[0];
                entity->model->position[1] = point[1];
                entity->model->position[2] = point[2];
            }
        }
        break;
    }
}

static void func_0016F930(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    f32 max_angle;
    f32 min_angle;
    f32 half;
    f32 degree;
    f32 launch_angle;
    f32 angle;

    entity->unkC6 |= 0x20;
    switch (entity->action_step) {
    case 0:
        half = 0.5f;
        func_00192C98(entity, 0x12, 0, entity->params->unkC4, 1);
        degree = 0.017453292f;
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
        ENTITY_FLAG_BITS(entity)->bit21 = 1;
        ENTITY_FLAG_BITS(entity)->bit20 = 1;
        ENTITY_FLAG_BITS(entity)->bit18 = 1;
        ENTITY_FLAG_BITS(entity)->bit28 = 1;
        ENTITY_FLAG_BITS(entity)->bit22 = 0;
        ENTITY_FLAG_BITS(entity)->bit23 = 0;
        ENTITY_FLAG_BITS(entity)->bit24 = 0;
        unkE0->unk54 = entity->model->unkE0[0];
        unkE0->unk58 = entity->model->unkE0[1];
        unkE0->unk5C = entity->model->unkE0[2];
        entity->model->unkE0[0] = 0.75f;
        entity->model->unkE0[1] = 0.75f;
        entity->model->unkE0[2] = half;
        min_angle = 105.0f - (f32)(s16)entity->params->unk434 * half;
        max_angle = (f32)(s16)entity->params->unk434 * half + -130.0f;
        angle =
            entity->unk68 + ((f32)(rand() % ((s16)entity->params->unk434 + 1)) - (f32)(s16)entity->params->unk434 * half) * degree;
        entity->model->unkD0[1] = angle;
        max_angle *= degree;
        min_angle *= degree;
        if (entity->model->unkD0[1] < min_angle && 0.0f <= entity->model->unkD0[1]) {
            entity->model->unkD0[1] = min_angle;
        }
        if (max_angle < entity->model->unkD0[1] && entity->model->unkD0[1] < 0.0f) {
            entity->model->unkD0[1] = max_angle;
        }
        unkE0->unk50 =
            (entity->distance_to_player + (f32)(rand() % (entity->params->unk436 + 1)) - (f32)entity->params->unk436 * 0.5f - 500.0f) /
                1300.0f * 24.0f + 24.0f;
        if (unkE0->unk50 < 24.0f) {
            unkE0->unk50 = 24.0f;
        }
        if (48.0f < unkE0->unk50) {
            unkE0->unk50 = 48.0f;
        }
        launch_angle = 0.6981317f;
        unkE0->unk48 = -unkE0->unk50 * sinf(launch_angle);
        unkE0->unk4C = unkE0->unk50 * cosf(launch_angle);
        unkE0->unk68 = 0;
        unkE0->unk7C = 0;
        entity->action_step++;
        break;
    case 1:
        unkE0->unk48 += (f32)entity->params->unk430 * ((f32)unkE0->unk68 / 30.0f);
        if ((f32)entity->params->unk432 < unkE0->unk48) {
            unkE0->unk48 = (f32)entity->params->unk432;
        }
        if (D_00446ED4 % 3U == 0) {
            func_0018D600((char **)entity, entity->params->unk43C, 0xFF, (int)entity->model->unkD0);
        }
        if (unkE0->unk7C == 0) {
            unkE0->unk7C = func_00195AC0(entity, entity->params->unk240, 0, 1, 2);
        }
        if ((s32)(entity->flags >> 44) & 1) {
            ENTITY_FLAG_BITS(entity)->bit27 = 0;
            entity->model->unkD0[0] = 0.0f;
            unkE0->unk6A = 600;
            entity->model->unkE0[0] = unkE0->unk54;
            entity->model->unkE0[1] = unkE0->unk58;
            entity->model->unkE0[2] = unkE0->unk5C;
            unkE0->unk7B = 1;
            func_0018D600((char **)entity, entity->params->unk43E, 0xFF, (int)entity->model->unkD0);
            func_00195898((char *)entity, 2);
            entity->flags &= ~0x200000;
            entity->flags &= ~0x100000;
            entity->flags &= ~0x40000;
            entity->flags &= ~0x10000000;
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            func_001958B8(entity, func_0016F348, 1);
            return;
        }
        break;
    }
    unkE0->unk68++;
    func_00195588((Vector4 *)direction, entity->model->unkD0[1]);
    Vector_Scale(direction, direction, unkE0->unk4C);
    entity->unk84 += direction[0];
    entity->unk88 += unkE0->unk48;
    entity->unk8C += direction[2];
    entity->model->unkD0[0] = atan2f(unkE0->unk48, unkE0->unk4C);
}

static void func_0016FE10(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 = 0x40;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 0x11, 0, entity->params->unkC4, 1);
        ENTITY_FLAG_BITS(entity)->bit22 = 0;
        ENTITY_FLAG_BITS(entity)->bit20 = 1;
        ENTITY_FLAG_BITS(entity)->bit27 = 1;
        ENTITY_FLAG_BITS(entity)->bit26 = 1;
        ENTITY_FLAG_BITS(entity)->bits2 = 1;
        ENTITY_FLAG_BITS(entity)->bits2 = 2;
        ENTITY_FLAG_BITS(entity)->bit4 = 1;
        entity->unkA0 = 20;
        unkE0->unk68 = 0;
        entity->action_step++;
    case 1:
        unkE0->unk48 += (f32)entity->params->unk430 * ((f32)unkE0->unk68 / 30.0f);
        if ((f32)entity->params->unk432 < unkE0->unk48) {
            unkE0->unk48 = (f32)entity->params->unk432;
        }
        if (entity->unkA0 != 0) {
            entity->model->unk15C = (f32)entity->unkD6 * (f32)entity->unkA0 / 20.0f;
        } else {
            if (entity->params->unkC6 != 0 && entity->params->unkC6 != 0xFFFF) {
                Effect_FreeForOwner(entity);
            }
            ENTITY_FLAG_BITS(entity)->bits2 = 3;
            ENTITY_FLAG_BITS(entity)->bit1 = 0;
        }
        break;
    }
    entity->unk88 += unkE0->unk48 + unkE0->unk48;
    unkE0->unk68++;
}
