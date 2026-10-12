#include "kf_entity.h"

void func_001958A8(char *, s32);
void func_001958B8(char *, void (*)(void), int);
void func_00191478(EntityModel *);
void func_001914B0(EntityModel *, s32, s32, s32);
void func_00193378(EntityModel *, s32, s32);
void func_00216778(void *, void *);
void *func_001267B0(int);
void func_0014EA70(Entity *, s32);
void func_0014E7F8(Entity *);
// match: the definition takes an Actor, a struct local to func_0014EC80.c
void func_0014EC80(void);
void func_0014ECE8(Entity *);

extern s32 D_003E6E40[][2];

void func_0014E590(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    u8 *unkBC = entity->unkBC;
    u8 *unkC0 = entity->unkC0;
    char *unk138 = entity->model->unk138;
    s32 i;
    Entity *other;

    // match: in the three loops the pointer has to be formed inside the loop
    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        u8 *dest = (u8 *)&entity->unkE0;

        dest[i] = 0;
    }
    func_001958A8((char *)entity, D_003E6E40[entity->params->unk02][1]);
    unkE0->unk00 = func_0014EA70;
    unkE0->unk04 = (void (*)(void))func_0014E7F8;
    unkE0->unk18 = func_0014EC80;
    unkE0->unk14 = (void (*)(void))func_0014ECE8;
    unkE0->unk08 = NULL;
    for (i = 4 - 1; i != -1; i--) {
        entity->unkBC[i] = 0x80;
    }
    for (i = 6 - 1; i != -1; i--) {
        entity->unkC0[i] = 0x80;
    }
    ENTITY_UNK5C(entity)->as_s32 = 0;
    ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit25 = TRUE;
    func_001958B8((char *)entity, unkE0->unk18, 0);
    if (unk138 != NULL) {
        func_00216778(unkE0->unk20, unk138 + 0x50);
    }
    unkE0->unk40 = 0.5f;
    entity->unkD6 = (entity->params->flags & 0x4000) ? 0x40 : 0x80;
    entity->model->unk15C = entity->unkD6;
    func_00191478(entity->model);
    func_001914B0(entity->model, 5, 0x16, 1);
    ((EntityUnkE0Kind151D30 *)unkE0)->unk80.bit5 = 1;
    ((EntityUnkE0Kind151D30 *)unkE0)->unk80.bit6 = 1;
    ((EntityUnkE0Kind151D30 *)unkE0)->unk88.bit6 = 1;
    func_00193378(entity->model, 0, 0);
    for (i = 0; i < 6; i++) {
        other = func_001267B0(i + 10);
        if (other != NULL) {
            other->flags &= ~2;
            other->flags &= ~0x80;
            other->flags &= ~0x100;
        }
    }
}
