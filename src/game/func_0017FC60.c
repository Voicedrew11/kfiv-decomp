#include "kf_entity.h"

#define ENTITY_WORK_AREA_SIZE 0xA0

void func_001958A8(char *, s32);
void func_001958B8(char *, void (*)(void), int);
void func_00191478(EntityModel *);
void func_00193378(EntityModel *, s32, s32);
void func_00216778(void *, void *);
void func_0017FFC0(Entity *, s32);
// match: the definition takes an Actor, a struct local to func_00182A08.c
void func_00182A08(void);
void func_00182BD0(char *);
void func_00183570(Entity *);
void func_00181038(Entity *);
void func_001818B8(Entity *);

extern s32 D_003E6E40[][2];

void func_0017FC60(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    u8 *unkBC = entity->unkBC;
    u8 *unkC0 = entity->unkC0;
    char *unk138 = entity->model->unk138;
    EntityModelPart80 *part;
    s32 i;
    s32 part_index;
    s32 first_part;
    s32 part_count;

    // match: in the three loops the pointer has to be formed inside the loop
    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        u8 *dest = (u8 *)&entity->unkE0;

        dest[i] = 0;
    }
    func_001958A8((char *)entity, D_003E6E40[entity->params->unk02][1]);
    unkE0->unk00 = func_0017FFC0;
    unkE0->unk04 = func_00182A08;
    unkE0->unk08 = (void (*)(void))func_00182BD0;
    unkE0->unk18 = (void (*)(void))func_00183570;
    unkE0->unk10 = (void (*)(void))func_00181038;
    unkE0->unk14 = (void (*)(void))func_001818B8;
    for (i = 4 - 1; i != -1; i--) {
        entity->unkBC[i] = 0x80;
    }
    for (i = 6 - 1; i != -1; i--) {
        entity->unkC0[i] = 0x80;
    }
    ENTITY_UNK5C(entity)->as_s32 = 0;
    ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
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
    func_00191478(entity->model);
    unkE0->unk68 = entity->params->unk3D8[0].record.unk00;
    unkE0->unk6A = entity->params->unk240[1].unk00;
    unkE0->unk6C = (u8)entity->params->unk350[0];
    unkE0->unk7A = 3;
    for (part_index = 0; part_index < 25; part_index++) {
        part = &((EntityModelPart80 *)entity->model->unkF8)[part_index + 8];
        ENTITY_MODEL_PART_FLAG_BITS(part)->bit40 = TRUE;
        ENTITY_MODEL_PART_FLAG_BITS(part)->bit41 = TRUE;
        unkE0->unk68 = 0;
        unkE0->unk6A = 0;
        unkE0->unk6C = 0;
    }
    switch (unkE0->unk7A) {
    case 0:
        first_part = 0x16;
        part_count = 10;
        unkE0->unk68 = entity->params->unk3D8[0].record.unk00;
        break;
    case 1:
        first_part = 0xB;
        part_count = 2;
        unkE0->unk6A = entity->params->unk240[1].unk00;
        break;
    case 2:
        first_part = 0xD;
        part_count = 9;
        unkE0->unk6C = (u8)entity->params->unk350[0];
        break;
    default:
        first_part = 8;
        part_count = 3;
        unkE0->unk68 = entity->params->unk3D8[0].record.unk00;
        unkE0->unk6A = entity->params->unk240[1].unk00;
        unkE0->unk6C = (u8)entity->params->unk350[0];
        break;
    }
    for (part_index = 0; part_index < part_count; part_index++) {
        part = &((EntityModelPart80 *)entity->model->unkF8)[first_part + part_index];
        ENTITY_MODEL_PART_FLAG_BITS(part)->bit40 = FALSE;
        ENTITY_MODEL_PART_FLAG_BITS(part)->bit41 = FALSE;
    }
    unkE0->unk6E = entity->params->unk434;
    func_00193378(entity->model, 0, 0);
}
