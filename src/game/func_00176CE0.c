#include "kf_entity.h"

#define ENTITY_WORK_AREA_SIZE 0xA0

void func_001958A8(char *, s32);
void func_00191478(EntityModel *);
void func_00193378(EntityModel *, s32, s32);
void func_00216778(void *, void *);
void func_00177040(Entity *);
// match: the definition takes an Actor, a struct local to func_00176E80.c
void func_00176E80(void);

extern s32 D_003E6E40[][2];

void func_00176CE0(Entity *entity) {
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
    unkE0->unk00 = (void (*)(Entity *, s32))func_00177040;
    unkE0->unk04 = func_00176E80;
    unkE0->unk08 = NULL;
    unkE0->unk18 = NULL;
    unkE0->unk10 = NULL;
    unkE0->unk14 = NULL;
    for (i = 4 - 1; i != -1; i--) {
        entity->unkBC[i] = 0x80;
    }
    for (i = 6 - 1; i != -1; i--) {
        entity->unkC0[i] = 0x80;
    }
    entity->target = NULL;
    ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
    if (unk138 != NULL) {
        func_00216778(unkE0->unk20, unk138 + 0x50);
    }
    unkE0->unk40 = 0.5f;
    entity->unkD6 = (entity->params->flags & 0x4000) ? 0x40 : 0x80;
    entity->model->unk15C = entity->unkD6;
    func_00191478(entity->model);
    func_00193378(entity->model, 0, 0);
}
