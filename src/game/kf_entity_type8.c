/* The behaviour of entity type 8 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers that function installs. */
#include "kf_entity.h"
#include "common.h"

#define ENTITY_WORK_AREA_SIZE 0xA0
void func_001958A8(char *, s32);
int func_001958B8(Entity *, void (*)(Entity *), int);
void func_00191478(EntityModel *);
int func_00193378(EntityModel *, s32, s32);
// match: the definition takes an Actor, a struct local to func_00182A08.c
void func_00182A08(void);
void func_00183570(Entity *);
void func_00181038(Entity *);
void func_001818B8(Entity *);
extern s32 D_003E6E40[][2];
void func_00190A30(char *);
void func_00182240(Entity *);
void func_00181378(Entity *);
void func_001814B0(Entity *);
void func_001816A0(Entity *);
int func_0019A310(char *, void (*)(void), int);
s32 func_00197078(Entity *);
int func_0018F030(char *);
void func_00181E48(Entity *);

static void func_00176A08(Entity *entity);
static void func_00176AF8(Entity *entity, s32 unknown);

/* Set-up of a type 8 entity: clears its work area and installs the type's handlers. */
void EntityType8_Init(Entity *entity) {
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
    unkE0->unk00 = func_00176AF8;
    unkE0->unk04 = func_00182A08;
    unkE0->unk08 = (void (*)(void))func_00176A08;
    unkE0->unk18 = (void (*)(void))func_00183570;
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
    unkE0->unk40 = 0.5f;
    entity->unkD6 = (entity->params->flags & 0x4000) ? 0x40 : 0x80;
    entity->model->unk15C = entity->unkD6;
    func_00191478(entity->model);
    func_00193378(entity->model, 0, 0);
}

static void func_00176A08(Entity *entity) {
    u64 flags;

    func_00190A30((char *)entity);
    flags = entity->flags;
    if ((s32)(flags >> 38) & 3) {
        entity->flags = (flags & ~(3UL << 58)) | (1UL << 57);
        func_001958B8(entity, func_00182240, 1);
        return;
    }
    if (((s32)(flags >> 36) & 3) == 1) {
        if ((s32)((s64)(flags << 13) >> 32) & 1) {
            func_0019A310((char *)entity, (void (*)(void))func_00181378, 0);
            return;
        }
        func_0019A310((char *)entity, (void (*)(void))func_001814B0, 0);
        return;
    }
    func_0019A310((char *)entity, (void (*)(void))func_001816A0, 0);
}

static void func_00176AF8(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0;

    unkE0 = &entity->unkE0;
    if (entity->unkC6 == 0) {
        if (func_00197078(entity) != 0 && entity->unkA2 == 0) {
            if (func_0018F030((char *)entity) != 0) {
                func_001958B8(entity, func_00182240, unknown);
            } else {
                func_001958B8(entity, func_00181E48, unknown);
            }
        } else {
            func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, unknown);
        }
    }
}
