#include "kf_entity.h"

#define ENTITY_WORK_AREA_SIZE 0xA0

void func_001958A8(char *, s32);
void func_001958B8(char *, void (*)(void), int);
void func_00191478(EntityModel *);
void func_00193378(EntityModel *, s32, s32);
void func_00216778(void *, void *);
void func_0016AEC0(Entity *, s32);
void func_0016B408(Entity *, s32);
void func_0016AA00(Entity *);
void func_0016AE00(Entity *);
void func_0016AFD8(Entity *);
void func_0016B088(Entity *);
void func_00181038(void);
void func_001818B8(void);
void func_00183570(void);

// match: read as pointers, so that the int store to unk5C is not taken to alias the table
extern void *D_003E6E40[][2];

void func_0016A788(Entity *entity) {
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
    for (i = 3; i != -1; i--) {
        entity->unkBC[i] = 0x80;
    }
    for (i = 5; i != -1; i--) {
        entity->unkC0[i] = 0x80;
    }
    entity->flags &= ~0x10000000;
    entity->unk5C = 0;
    func_001958A8((char *)entity, (s32)D_003E6E40[entity->params->unk02][1]);
    if (entity->spawn_info->unk40 != 0) {
        unkE0->unk00 = func_0016AEC0;
        unkE0->unk04 = (void (*)(void))func_0016AA00;
        unkE0->unk08 = (void (*)(void))func_0016AE00;
        unkE0->unk10 = (void (*)(void))func_0016B088;
        unkE0->unk18 = (s32)func_0016AFD8;
        unkE0->unk14 = func_001818B8;
        ENTITY_FLAG_BITS(entity)->bit18 = 1;
        func_001958B8((char *)entity, unkE0->unk10, 1);
    } else {
        unkE0->unk00 = func_0016B408;
        unkE0->unk04 = (void (*)(void))func_0016AA00;
        unkE0->unk08 = (void (*)(void))func_0016AE00;
        unkE0->unk14 = func_001818B8;
        unkE0->unk10 = func_00181038;
        unkE0->unk18 = (s32)func_00183570;
        if (ENTITY_FLAG_WORD(entity->spawn_info->unk38) & (0x8000L << 43)) {
            func_001958B8((char *)entity, unkE0->unk10, 1);
        } else {
            func_001958B8((char *)entity, (void (*)(void))unkE0->unk18, 0);
        }
        entity->unkD6 = (entity->params->unk10 & 0x4000) ? 0x40 : 0x80;
        entity->model->unk15C = entity->unkD6;
    }
    if (unk138 != NULL) {
        func_00216778(unkE0->unk20, unk138 + 0x50);
    }
    unkE0->unk40 = 0.5f;
    func_00191478(entity->model);
    func_00193378(entity->model, 0, 0);
}
