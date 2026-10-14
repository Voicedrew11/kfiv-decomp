/* The behaviour of entity type 12 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers that function installs. */
#include "kf_entity.h"
#include "common.h"

#define ENTITY_WORK_AREA_SIZE 0xA0
void func_001958A8(char *, s32);
int func_001958B8(Entity *, void (*)(Entity *), int);
void func_00191478(EntityModel *);
int func_00193378(EntityModel *, s32, s32);
void func_00183570(Entity *);
void func_00181038(Entity *);
extern s32 D_003E6E40[][2];
int func_00192EB0(void *, int);
// match: the callee is void, but retail keeps what it leaves in $2
int func_00194368(EntityModel *, f32);
int rand(void);
void func_0018D600(char **, int, u32, int);
float func_00195388(float *, float, float);
float func_00194F80(void *, void *);
int func_00192C98(void *, int, int, u32, int);
void func_0018DCD8(Entity *);

static void func_00175F60(Entity *entity);
static void func_00176050(Entity *entity, int frame);
static void func_001760B8(Entity *entity, s32 unknown);
static void func_00176238(Entity *entity, s32 unknown);
static void func_00176360(Entity *entity);
static void func_001765C0(Entity *entity);
static void func_001766F8(Entity *entity);

/* Set-up of a type 12 entity: clears its work area and installs the type's handlers. */
void EntityType12_Init(Entity *entity) {
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
    unkE0->unk00 = func_00176050;
    unkE0->unk04 = (void (*)(void))func_00175F60;
    unkE0->unk08 = NULL;
    unkE0->unk18 = (void (*)(void))func_00183570;
    unkE0->unk10 = (void (*)(void))func_00181038;
    unkE0->unk14 = (void (*)(void))func_00176360;
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
    unkE0->unk40 = 0.5f;
    entity->unkD6 = (entity->params->flags & 0x4000) ? 0x40 : 0x80;
    entity->model->unk15C = entity->unkD6;
    func_00191478(entity->model);
    func_00193378(entity->model, 0, 0);
}

static void func_00175F60(Entity *entity) {
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

static void func_00176050(Entity *entity, int frame) {
    if (entity->unkC6 != 0) {
        return;
    }
    if (entity->distance_to_player < (float)ENTITY_CONTROLLER(entity)->unk430) {
        // match: retail reads the u8 state signed (lb)
        if ((s8)entity->state != 3) {
            entity->state = 3;
        }
    }
    if ((s8)entity->state == 3) {
        func_00176238(entity, frame);
        return;
    }
    func_001760B8(entity, frame);
}

static void func_001760B8(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    s32 roll;

    if (entity->params->flags & 0x100) {
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
    } else {
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
    }
    if (entity->unkA0 == 0) {
        roll = entity->params->unk432 + (s16)entity->params->unk434 + entity->params->unk436;
        if (roll != 0) {
            roll = rand() % roll;
        }
        if (roll < entity->params->unk432) {
            func_001958B8(entity, func_001765C0, unknown);
            return;
        }
        if (roll < entity->params->unk432 + (s16)entity->params->unk434) {
            func_001958B8(entity, func_001766F8, unknown);
            return;
        }
        if (roll < entity->params->unk432 + (s16)entity->params->unk434 + entity->params->unk436) {
            entity->unkA0 = rand() % 60 + 0x78;
        }
    }
    func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, unknown);
    return;
}

static void func_00176238(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 turn;

    ENTITY_FLAG_BITS(entity)->bits11 = 0;
    entity->unkC8 |= 1;
    if (unkE0->unk68 < entity->params->unk43A) {
        func_0018D600((char **)entity, entity->params->unk438, 0xFF, (int)entity->model->unkD0);
        unkE0->unk68++;
    }
    // match: the whole step is written out in both arms (only the call is shared in retail)
    if (-2000.0f < entity->model->position[1]) {
        turn = entity->model->unkD0[0];
        func_00195388(&turn, -0.6981317f, 20.0f);
        entity->unk90 += turn - entity->model->unkD0[0];
    } else {
        turn = entity->model->unkD0[0];
        func_00195388(&turn, 0.0f, 20.0f);
        entity->unk90 += turn - entity->model->unkD0[0];
    }
    if (func_00194F80(entity->model->position, entity->spawn_info) > 10000.0f) {
        ENTITY_FLAG_BITS(entity)->bit1 = FALSE;
    }
    func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, unknown);
}

static void func_00176360(Entity *entity) {
    f32 turn;

    entity->unkC6 = 0x40;
    // match: the whole step is written out in both arms (only the call is shared in retail)
    if (!ENTITY_FLAG_BITS(entity)->bit44) {
        turn = entity->model->unkD0[0];
        func_00195388(&turn, 1.5707964f, 3.0f);
        entity->unk90 += turn - entity->model->unkD0[0];
    } else {
        turn = entity->model->unkD0[0];
        func_00195388(&turn, 0.0f, 2.0f);
        entity->unk90 += turn - entity->model->unkD0[0];
    }
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

static void func_001765C0(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 0x1000;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 0x14, 0, entity->params->unkC4, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
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

static void func_001766F8(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 0x2000;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 0x15, 0, entity->params->unkC4, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
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
