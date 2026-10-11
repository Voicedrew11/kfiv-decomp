#include "kf_entity.h"

#define ENTITY_WORK_AREA_SIZE 0xA0

void func_001958A8(char *, s32);
void func_001958B8(char *, void (*)(void), int);
void func_00191478(EntityModel *);
void func_00193378(EntityModel *, s32, s32);
void func_00216778(void *, void *);
void func_0016C1E0(Entity *, s32);
// match: the definition takes an Actor, a struct local to func_00182A08.c
void func_00182A08(void);
void func_0016C138(char *);
void func_0016C7F0(Entity *);
void func_0016CC58(Entity *);
void func_0016D4D0(Entity *);

extern s32 D_003E6E40[][2];

void func_0016BF98(Entity *entity) {
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
    unkE0->unk00 = func_0016C1E0;
    unkE0->unk04 = func_00182A08;
    unkE0->unk08 = (void (*)(void))func_0016C138;
    unkE0->unk18 = (s32)func_0016C7F0;
    unkE0->unk10 = (void (*)(void))func_0016CC58;
    unkE0->unk14 = (void (*)(void))func_0016D4D0;
    for (i = 3; i != -1; i--) {
        entity->unkBC[i] = 0x80;
    }
    for (i = 5; i != -1; i--) {
        entity->unkC0[i] = 0x80;
    }
    entity->unk5C = 0;
    ENTITY_FLAG_BITS(entity)->bit28 = 0;
    func_001958B8((char *)entity, unkE0->unk10, 1);
    if (unk138 != NULL) {
        func_00216778(unkE0->unk20, unk138 + 0x50);
    }
    unkE0->unk40 = 0.5f;
    func_00191478(entity->model);
    func_00193378(entity->model, 0, 0);
}
