#include "kf_entity.h"

void func_001958A8(char *, s32);
void func_001958B8(char *, void (*)(void), int);
void func_00191478(EntityModel *);
void func_00193378(EntityModel *, s32, s32);
void func_00216778(void *, void *);
void func_00157BD0(Entity *, s32);
// match: the definition takes an Actor, a struct local to func_00182A08.c
void func_00182A08(void);
void func_00158E28(Entity *);
// match: the definition takes an Actor, a struct local to func_00158380.c
void func_00158380(void);
void func_00158ED8(Entity *);
void func_001593E8(Entity *);

extern s32 D_003E6E40[][2];

void func_00157A00(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    char *unk138 = entity->model->unk138;
    s32 i;

    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        ENTITY_WORK_BYTES(entity)[i] = 0;
    }
    func_001958A8((char *)entity, D_003E6E40[entity->params->unk02][1]);
    unkE0->unk00 = func_00157BD0;
    unkE0->unk04 = func_00182A08;
    unkE0->unk08 = (void (*)(void))func_00158E28;
    unkE0->unk18 = (s32)func_00158380;
    unkE0->unk10 = (void (*)(void))func_00158ED8;
    unkE0->unk14 = (void (*)(void))func_001593E8;
    for (i = 3; i != -1; i--) {
        entity->unkBC[i] = 0x80;
    }
    for (i = 5; i != -1; i--) {
        entity->unkC0[i] = 0x80;
    }
    ENTITY_FLAG_BITS(entity)->bit28 = 1;
    entity->unk5C = 0;
    if (ENTITY_SPAWN_FLAGS(entity) & (0x8000L << 43)) {
        func_001958B8((char *)entity, unkE0->unk10, 1);
    } else {
        func_001958B8((char *)entity, (void (*)(void))unkE0->unk18, 0);
    }
    if (unk138 != NULL) {
        func_00216778(unkE0->unk20, unk138 + 0x50);
    }
    unkE0->unk40 = 0.5f;
    entity->unkD6 = 0;
    entity->model->unk15C = 0.0f;
    func_00191478(entity->model);
    func_00193378(entity->model, 0, 0);
}
