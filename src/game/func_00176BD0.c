#include "kf_entity.h"

#define ENTITY_WORK_AREA_SIZE 0xA0

void func_001958A8(char *, s32);
void func_00193378(EntityModel *, s32, s32);
void func_00176CD8(void);
// match: the definition takes an Actor, a struct local to func_00182A08.c
void func_00182A08(void);

extern s32 D_003E6E40[][2];

void func_00176BD0(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    s32 i;

    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        // match: the pointer has to be formed inside the loop
        u8 *dest = (u8 *)&entity->unkE0;

        dest[i] = 0;
    }
    func_001958A8((char *)entity, D_003E6E40[entity->params->unk02][1]);
    unkE0->unk00 = (void (*)(Entity *, s32))func_00176CD8;
    unkE0->unk04 = func_00182A08;
    unkE0->unk08 = NULL;
    unkE0->unk18 = 0;
    unkE0->unk10 = NULL;
    unkE0->unk14 = NULL;
    ENTITY_FLAG_BITS(entity)->bit21 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit28 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit16 = TRUE;
    entity->unkD6 = (entity->params->unk10 & 0x4000) ? 0x40 : 0x80;
    entity->model->unk15C = entity->unkD6;
    func_00193378(entity->model, 0, 0);
}
