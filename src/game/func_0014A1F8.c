#include "kf_entity.h"

void func_001958A8(char *, s32);
void func_001958B8(char *, void (*)(void), int);
void func_00191478(EntityModel *);
void func_00193378(EntityModel *, s32, s32);
void func_00216778(void *, void *);
int rand(void);
void func_0014A3F8(Entity *, s32);
// match: the definition takes an Actor, a struct local to func_0014B270.c
void func_0014B270(void);
void func_00182BD0(char *);
void func_0014AAE0(Entity *);
void func_00181038(Entity *);
void func_0014AB18(Entity *);

extern s32 D_003E6E40[][2];

void func_0014A1F8(Entity *entity) {
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
    unkE0->unk00 = func_0014A3F8;
    unkE0->unk04 = func_0014B270;
    unkE0->unk08 = (void (*)(void))func_00182BD0;
    unkE0->unk18 = (void (*)(void))func_0014AAE0;
    unkE0->unk10 = (void (*)(void))func_00181038;
    unkE0->unk14 = (void (*)(void))func_0014AB18;
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
    if (unk138 != NULL) {
        func_00216778(unkE0->unk20, unk138 + 0x50);
    }
    unkE0->unk40 = 0.5f;
    entity->unkD6 = (entity->params->flags & 0x4000) ? 0x40 : 0x80;
    entity->model->unk15C = entity->unkD6;
    unkE0->unk6A = rand() % 30;
    func_00191478(entity->model);
    func_00193378(entity->model, 0, 0);
}
