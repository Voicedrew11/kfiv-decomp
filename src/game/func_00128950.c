#include "kf_entity.h"

void func_001958A8(char *, s32);
int func_001958B8(Entity *, void (*)(Entity *), int);
void func_00191478(EntityModel *);
void func_00193378(EntityModel *, s32, s32);
void func_00128CA0(Entity *, s32);
// match: the definition takes an Actor, a struct local to func_00128B80.c
void func_00128B80(void);
void func_00182BD0(char *);
void func_00183570(Entity *);
void func_001295B0(Entity *);
void func_001818B8(Entity *);
void func_00129738(Entity *);
void func_00129780(Entity *);
void func_001299F8(Entity *);

extern s32 D_003E6E40[][2];

/* Init of this enemy kind: one handler table when it starts with its action, another when it starts asleep. */
void func_00128950(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    u8 *unkBC = entity->unkBC;
    u8 *unkC0 = entity->unkC0;
    s32 i;

    // match: in the three loops the pointer has to be formed inside the loop
    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        u8 *dest = (u8 *)&entity->unkE0;

        dest[i] = 0;
    }
    for (i = 4 - 1; i != -1; i--) {
        entity->unkBC[i] = 0x80;
    }
    for (i = 6 - 1; i != -1; i--) {
        entity->unkC0[i] = 0x80;
    }
    func_001958A8((char *)entity, D_003E6E40[entity->params->unk02][1]);
    ENTITY_UNK5C(entity)->as_s32 = 0;
    ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
    if (entity->spawn_info->unk38 & (0x8000L << 43)) {
        unkE0->unk00 = func_00128CA0;
        unkE0->unk04 = func_00128B80;
        unkE0->unk08 = (void (*)(void))func_00182BD0;
        unkE0->unk18 = (void (*)(void))func_00183570;
        unkE0->unk10 = (void (*)(void))func_001295B0;
        unkE0->unk14 = (void (*)(void))func_001818B8;
        func_001958B8((char *)entity, unkE0->unk10, 1);
    } else {
        unkE0->unk00 = func_00128CA0;
        unkE0->unk04 = func_00128B80;
        unkE0->unk08 = (void (*)(void))func_00129738;
        unkE0->unk18 = (void (*)(void))func_00183570;
        unkE0->unk10 = (void (*)(void))func_001295B0;
        unkE0->unk14 = (void (*)(void))func_00129780;
        func_001958B8((char *)entity, (void (*)(void))func_001299F8, 1);
    }
    unkE0->unk40 = 0.5f;
    entity->unkD6 = (entity->params->flags & 0x4000) ? 0x40 : 0x80;
    entity->model->unk15C = entity->unkD6;
    func_00191478(entity->model);
    func_00193378(entity->model, 0, 0);
}
