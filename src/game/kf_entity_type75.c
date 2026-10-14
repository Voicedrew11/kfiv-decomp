/* The behaviour of entity type 75 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers that function installs. */
#include "kf_entity.h"

#define ENTITY_WORK_AREA_SIZE 0xA0
void func_001958A8(char *, s32);
int func_001958B8(Entity *, void (*)(Entity *), int);
void func_00191478(EntityModel *);
int func_00193378(EntityModel *, s32, s32); // match: an int result keeps $2 busy after the call
void func_00216778(void *, void *);
// match: the definition takes an Actor, a struct local to func_00182A08.c
void func_00182A08(void);
void func_00182BD0(char *);
void func_00183570(Entity *);
void func_00181038(Entity *);
extern s32 D_003E6E40[][2];
extern s32 D_00446928;
extern u32 D_00446ED4;
void func_0018D600(char **, int, u32, int);
void func_0018DA38(Entity *, s32, s32, s32);
int func_0019D490(u8, u8, float *, float);
void func_00196870(void *, int);
void func_0018E9C0(void *);
int func_00182FD8(void *, int);
void func_00183240(Entity *, s32);
extern u8 D_00413FBC[];
int func_00192C98(void *, int, int, u32, int);
void func_0018DCD8(Entity *);

static void func_001799A0(Entity *entity, s32 unknown);
static void func_00179BB0(Entity *entity);
static void func_00179C10(Entity *entity);
static void func_00179DD8(Entity *entity);

/* Set-up of a type 75 entity: clears its work area and installs the type's handlers. */
void EntityType75_Init(Entity *entity) {
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
    unkE0->unk00 = func_001799A0;
    unkE0->unk04 = func_00182A08;
    unkE0->unk08 = (void (*)(void))func_00182BD0;
    unkE0->unk18 = (void (*)(void))func_00183570;
    unkE0->unk10 = (void (*)(void))func_00181038;
    unkE0->unk14 = (void (*)(void))func_00179BB0;
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
    ENTITY_FLAG_BITS(entity)->bit26 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit16 = TRUE;
    unkE0->unk7A = D_00446928;
}

static void func_001799A0(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    if (D_00446ED4 % 3U == 0) {
        switch (D_00446928) {
        case 3:
            func_0018D600((char **)entity, entity->params->unk430, 0xFF, (int)entity->model->unkD0);
            break;
        case 2:
            func_0018D600((char **)entity, entity->params->unk432, 0xFF, (int)entity->model->unkD0);
            break;
        default:
            func_0018D600((char **)entity, (s16)entity->params->unk434, 0xFF, (int)entity->model->unkD0);
            break;
        }
    }
    if (ENTITY_FLAG_BITS(entity)->bit43) {
        func_0018D600((char **)entity, 0x13D, 0xFF, (int)entity->model->unkD0);
        ENTITY_FLAG_BITS(entity)->bit43 = FALSE;
        if (D_00446928 <= 0) {
            entity->unkA3 = 6;
            func_001958B8(entity, func_00179BB0, unknown);
            return;
        }
        if (unkE0->unk7A != D_00446928) {
            func_0018DA38(entity, entity->params->unk436, entity->params->unk438, 0);
            // match: an int result keeps $2 busy after the call
            ((int (*)(s32, s32, f32 *, f32))func_0019D490)((u8)entity->params->unk43A, (u8)entity->params->unk43C,
                                                           entity->model->position, entity->params->unk42E);
            unkE0->unk7A = D_00446928;
        }
    }
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
}

static void func_00179BB0(Entity *entity) {
    if (D_00413FBC[0] == 0x2E) {
        ENTITY_FLAG_BITS(entity)->bit29 = TRUE;
        func_001958B8(entity, func_00179DD8, 1);
    } else {
        ENTITY_FLAG_BITS(entity)->bit29 = FALSE;
        func_001958B8(entity, func_00179C10, 1);
    }
}

static void func_00179C10(Entity *entity) {
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
        if (!ENTITY_PARAMS_BITS(entity)->bit25 && ENTITY_PARAMS_BITS(entity)->bit26) {
            entity->unk30 = 20.0f;
        }
        ENTITY_FLAG_BITS(entity)->bit14 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit20 = TRUE;
        ENTITY_FLAG_BITS(entity)->bits2 = 1;
        ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
        func_0018DCD8(entity);
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
            ENTITY_FLAG_BITS(entity)->bit27 = TRUE;
            ENTITY_FLAG_BITS(entity)->bit26 = TRUE;
            entity->action_step++;
        }
        break;
    }
}

static void func_00179DD8(Entity *entity) {
    entity->unkC6 = 0x40;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 0, 0, 5, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
        ENTITY_FLAG_BITS(entity)->bit14 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit20 = TRUE;
        ENTITY_FLAG_BITS(entity)->bits2 = 1;
        ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
        func_0018DCD8(entity);
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
            ENTITY_FLAG_BITS(entity)->bit27 = TRUE;
            ENTITY_FLAG_BITS(entity)->bit26 = TRUE;
            entity->action_step++;
        }
        break;
    }
}
