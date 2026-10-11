#include "kf_entity.h"

int rand(void);
void func_001958A8(char *, s32);
void func_001958B8(char *, void (*)(void), int);
void func_00191478(EntityModel *);
void func_001914B0(EntityModel *, s32, s32, s32);
void func_00193378(EntityModel *, s32, s32);
void func_00216778(void *, void *);
void func_00151F90(Entity *, s32);
void func_001523D0(Entity *);
void func_001532C0(Entity *);
void func_00153040(Entity *);
void func_00181038(void);
void func_001537B8(Entity *);

extern s32 D_003E6E40[][2];

void func_00151D30(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    char *unk138 = entity->model->unk138;
    s32 i;

    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        ENTITY_WORK_BYTES(entity)[i] = 0;
    }
    func_001958A8((char *)entity, D_003E6E40[entity->params->unk02][1]);
    unkE0->unk00 = func_00151F90;
    unkE0->unk04 = (void (*)(void))func_001523D0;
    unkE0->unk08 = (void (*)(void))func_001532C0;
    unkE0->unk18 = (s32)func_00153040;
    unkE0->unk10 = func_00181038;
    unkE0->unk14 = (void (*)(void))func_001537B8;
    for (i = 3; i != -1; i--) {
        entity->unkBC[i] = 0x80;
    }
    for (i = 5; i != -1; i--) {
        entity->unkC0[i] = 0x80;
    }
    ENTITY_FLAG_BITS(entity)->bit28 = 0;
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
    entity->unkD6 = (entity->params->unk10 & 0x4000) ? 0x40 : 0x80;
    entity->model->unk15C = entity->unkD6;
    func_00191478(entity->model);
    if (!(rand() & 1)) {
        func_001914B0(entity->model, 4, 5, 1);
        unkE0->unk7B = 0;
    } else {
        func_001914B0(entity->model, 6, 7, 1);
        unkE0->unk7B = 1;
    }
    ((EntityUnkE0Kind151D30 *)unkE0)->unk80.bit5 = 1;
    ((EntityUnkE0Kind151D30 *)unkE0)->unk80.bit6 = 1;
    ((EntityUnkE0Kind151D30 *)unkE0)->unk88.bit6 = 1;
    func_00193378(entity->model, 0, 0);
}
