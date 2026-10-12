#include "kf_entity.h"

#define ENTITY_WORK_AREA_SIZE 0xA0

void func_001958A8(char *, s32);
void func_001958B8(char *, void (*)(void), int);
void func_00191478(EntityModel *);
void func_00193378(EntityModel *, s32, s32);
void func_00216778(void *, void *);
void func_00168368(Entity *, s32);
// match: the definition takes an Actor, a struct local to func_00182A08.c
void func_00182A08(void);
void func_00169918(Entity *);
void func_00168C10(Entity *);
void func_00181038(Entity *);
void func_001818B8(Entity *);

extern s32 D_003E6E40[][2];

void func_00168160(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    u8 *unkBC = entity->unkBC;
    u8 *unkC0 = entity->unkC0;
    char *unk138 = entity->model->unk138;
    s32 i;

    // match: the three fills name their target inside the loop; unkBC and unkC0 above are
    // otherwise unused but retail computes them up front and copies them before each loop
    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        u8 *work_area = (u8 *)&entity->unkE0;

        work_area[i] = 0;
    }
    func_001958A8((char *)entity, D_003E6E40[entity->params->unk02][1]);
    unkE0->unk00 = func_00168368;
    unkE0->unk04 = func_00182A08;
    unkE0->unk08 = (void (*)(void))func_00169918;
    unkE0->unk18 = (void (*)(void))func_00168C10;
    unkE0->unk10 = (void (*)(void))func_00181038;
    unkE0->unk14 = (void (*)(void))func_001818B8;
    for (i = 3; i != -1; i--) {
        entity->unkBC[i] = 0x80;
    }
    for (i = 5; i != -1; i--) {
        entity->unkC0[i] = 0x80;
    }
    entity->target = NULL;
    entity->flags &= ~0x10000000;
    if (ENTITY_FLAG_WORD(entity->spawn_info->unk38) & (0x8000L << 43)) {
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
    func_00191478(entity->model);
    unkE0->unk6C = entity->params->unk3D8[2].record.unk00;
    func_00193378(entity->model, 0, 0);
}
