/* The behaviour of entity type 73 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers that function installs. */
#include "kf_entity.h"

#define ENTITY_WORK_AREA_SIZE 0xA0
void func_001958A8(char *, s32);
void func_00191478(EntityModel *);
int func_00193378(EntityModel *, s32, s32);
void func_00216778(void *, void *);
int func_001958B8(Entity *, void (*)(Entity *), int);
// match: the definition takes an Actor, a struct local to func_00182A08.c
void func_00182A08(void);
extern s32 D_003E6E40[][2];
extern u32 D_00446ED4;
void func_00196870(void *, int);
s32 func_001992F0(Entity *, s32);
void func_001B4EC8(s32);
int func_00192C98(void *, int, int, u32, int);
void func_0018DB58(void *, int, int);
void func_0018DCD8(Entity *);

static void func_00140140(Entity *entity, s32 unknown);
static void func_00140280(Entity *entity);
static void func_00140328(Entity *entity);

/* Set-up of a type 73 entity: clears its work area and installs the type's handlers. */
void EntityType73_Init(Entity *entity) {
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
    unkE0->unk00 = func_00140140;
    unkE0->unk04 = func_00182A08;
    unkE0->unk08 = NULL;
    unkE0->unk18 = (void (*)(void))func_00140280;
    unkE0->unk10 = NULL;
    unkE0->unk14 = (void (*)(void))func_00140328;
    for (i = 4 - 1; i != -1; i--) {
        entity->unkBC[i] = 0x80;
    }
    for (i = 6 - 1; i != -1; i--) {
        entity->unkC0[i] = 0x80;
    }
    ENTITY_UNK5C(entity)->as_s32 = 0;
    ENTITY_FLAG_BITS(entity)->bit28 = TRUE;
    func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, 0);
    if (unk138 != NULL) {
        func_00216778(unkE0->unk20, unk138 + 0x50);
    }
    unkE0->unk40 = 0.5f;
    entity->unkD6 = (entity->params->flags & 0x4000) ? 0x40 : 0x80;
    entity->model->unk15C = entity->unkD6;
    func_00191478(entity->model);
    func_00193378(entity->model, 0, 0);
}

static void func_00140140(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    s8 previous;

    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        func_00196870(entity, 1);
    }
    if (entity->unkC6 == 0) {
        previous = unkE0->unk7A;
        if (!(s32)(entity->flags >> 63) && func_001992F0(entity, 0)) {
            func_001B4EC8((s16)entity->params->unk434);
            unkE0->unk7A = 1;
            ENTITY_FLAG_BITS(entity)->bit9 = TRUE;
            func_00192C98(entity, 2, 0, entity->params->unkC4, 0);
        } else {
            unkE0->unk7A = 0;
            ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
            func_00192C98(entity, 0, 0, entity->params->unkC4, 0);
        }
        if (previous != unkE0->unk7A) {
            func_0018DB58(entity, 1, 0);
        }
    }
}

static void func_00140280(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    if (unkE0->unk7A != 0) {
        func_00192C98(entity, 2, 0, entity->params->unkC4, 0);
        if (unkE0->unk68 <= 0) {
            func_0018DB58(entity, 0, 0);
            unkE0->unk68 = entity->params->unk438;
        }
    } else {
        func_00192C98(entity, 0, 0, entity->params->unkC4, 0);
        unkE0->unk68 = 0;
    }
    unkE0->unk68--;
}

static void func_00140328(Entity *entity) {
    entity->unkC6 = 0x40;
    switch (entity->action_step) {
    case 0:
        func_0018DB58(entity, 2, 0);
        ENTITY_FLAG_BITS(entity)->bit14 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit20 = TRUE;
        ENTITY_FLAG_BITS(entity)->bits2 = 1;
        ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit14 = FALSE;
        ENTITY_FLAG_BITS(entity)->bits2 = 2;
        ENTITY_FLAG_BITS(entity)->bit4 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit27 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit26 = TRUE;
        entity->unkA0 = 0x14;
        func_0018DCD8(entity);
        entity->action_step++;
        break;
    case 1:
        if (entity->unkA0 != 0) {
            entity->model->unk15C = (f32)entity->unkD6 * (f32)entity->unkA0 / 20.0f;
        } else {
            ENTITY_FLAG_BITS(entity)->bits2 = 3;
            ENTITY_FLAG_BITS(entity)->bit1 = FALSE;
        }
        break;
    }
}
