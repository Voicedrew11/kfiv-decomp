#include "kf_entity.h"

void func_001958A8(char *, s32);
int EntityAi_StartAction(char *, void (*)(void), int);
void func_00191478(EntityModel *);
void func_00193378(EntityModel *, s32, s32);
void func_00216778(void *, void *);
void func_00183E98(Entity *, s32);
// match: the definition takes an Actor, a struct local to func_00183C78.c
void func_00183C78(void);
void func_00183E20(char *);
void func_00184C78(Entity *);
void EntityAi_Dormant(Entity *);
void func_001849E0(Entity *);

extern s32 D_003E6E40[][2];

void func_00183A40(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    u8 *unkBC = entity->unkBC;
    u8 *unkC0 = entity->unkC0;
    EntityModelPart80 *part;
    s32 i;

    part = (EntityModelPart80 *)entity->model->unkF8 + entity->params->unk43C;
    entity->model->unk138 = part;
    // match: in the three loops the pointer has to be formed inside the loop
    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        u8 *dest = (u8 *)&entity->unkE0;

        dest[i] = 0;
    }
    func_001958A8((char *)entity, D_003E6E40[entity->params->unk02][1]);
    unkE0->think = func_00183E98;
    unkE0->animate = func_00183C78;
    unkE0->on_hit = (void (*)(void))func_00183E20;
    unkE0->move = (void (*)(void))func_00184C78;
    unkE0->dormant = (void (*)(void))EntityAi_Dormant;
    unkE0->death = (void (*)(void))func_001849E0;
    for (i = 4 - 1; i != -1; i--) {
        entity->unkBC[i] = 0x80;
    }
    for (i = 6 - 1; i != -1; i--) {
        entity->unkC0[i] = 0x80;
    }
    ENTITY_UNK5C(entity)->as_s32 = 0;
    if (ENTITY_PARAMS_BITS(entity)->bit25 || ENTITY_PARAMS_BITS(entity)->bit26) {
        ENTITY_FLAG_BITS(entity)->bit28 = TRUE;
    }
    if (entity->spawn_info->unk38 & (0x8000L << 43)) {
        EntityAi_StartAction((char *)entity, unkE0->dormant, 1);
    } else {
        EntityAi_StartAction((char *)entity, unkE0->move, 0);
    }
    if (part != NULL) {
        func_00216778(unkE0->unk20, part->unk50);
    }
    unkE0->unk40 = 0.5f;
    entity->unkD6 = (entity->params->flags & 0x4000) ? 0x40 : 0x80;
    entity->model->unk15C = entity->unkD6;
    func_00191478(entity->model);
    // match: called as returning an int, which keeps $2 busy after the call
    ((int (*)(EntityModel *, s32, s32))func_00193378)(entity->model, 0, 0);
    ENTITY_FLAG_BITS(entity)->bit16 = TRUE;
}
