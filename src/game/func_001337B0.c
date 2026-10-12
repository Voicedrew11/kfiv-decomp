#include "kf_entity.h"

void func_001958A8(char *, s32);
void func_001958B8(char *, void (*)(void), int);
void func_00191478(EntityModel *);
void func_00193378(EntityModel *, s32, s32);
void func_00216778(void *, void *);
void func_001914B0(EntityModel *, s32, s32, s32);
void func_001349E8(Entity *, s32);
void func_001339D8(Entity *);
void func_00133CA0(Entity *);
void func_00135280(Entity *);
void func_00134160(Entity *);

extern s32 D_003E6E40[][2];

void func_001337B0(Entity *entity) {
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
    unkE0->unk00 = func_001349E8;
    unkE0->unk04 = (void (*)(void))func_001339D8;
    unkE0->unk08 = (void (*)(void))func_00133CA0;
    unkE0->unk18 = (void (*)(void))func_00135280;
    unkE0->unk14 = (void (*)(void))func_00134160;
    unkE0->unk10 = NULL;
    for (i = 4 - 1; i != -1; i--) {
        entity->unkBC[i] = 0x80;
    }
    for (i = 6 - 1; i != -1; i--) {
        entity->unkC0[i] = 0x80;
    }
    ENTITY_UNK5C(entity)->as_s32 = 0;
    if (entity->spawn_info->unk38 & (0x8000L << 43)) {
        func_001958B8((char *)entity, unkE0->unk10, 1);
    } else {
        func_001958B8((char *)entity, unkE0->unk18, 0);
    }
    ENTITY_FLAG_BITS(entity)->bit28 = TRUE;
    if (unk138 != NULL) {
        func_00216778(unkE0->unk20, unk138 + 0x50);
    }
    unkE0->unk40 = 0.5f;
    entity->unkD6 = (entity->params->flags & 0x4000) ? 0x40 : 0x80;
    entity->model->unk15C = entity->unkD6;
    func_00191478(entity->model);
    func_001914B0(entity->model, 10, 0x13, 1);
    ((EntityUnkE0Kind151D30 *)unkE0)->unk80.bit5 = 1;
    ((EntityUnkE0Kind151D30 *)unkE0)->unk80.bit6 = 1;
    ((EntityUnkE0Kind151D30 *)unkE0)->unk88.bit6 = 1;
    func_00193378(entity->model, 0, 0);
}
