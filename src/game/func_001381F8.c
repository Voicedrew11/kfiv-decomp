#include "kf_entity.h"

void func_001958A8(char *, s32);
void func_001958B8(char *, void (*)(void), int);
void func_00191478(EntityModel *);
void func_00193378(EntityModel *, s32, s32);
void func_001384C0(Entity *);
void func_001385B0(char *);
void func_00138608(Entity *);
void func_001389C8(Entity *, s32);
void func_00138A48(Entity *, s32);
void func_00139730(Entity *, s32);
void func_00139FE0(Entity *, s32);
void func_0013A890(Entity *, s32);
void func_0013B150(Entity *, s32);
void func_00182C80(Entity *, s32);
void func_00183570(Entity *);
void func_001818B8(Entity *);

extern s32 D_003E6E40[][2];

void func_001381F8(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    u8 *unkBC = entity->unkBC;
    u8 *unkC0 = entity->unkC0;
    s32 i;

    // match: in the three loops the pointer has to be formed inside the loop
    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        u8 *dest = (u8 *)&entity->unkE0;

        dest[i] = 0;
    }
    entity->unkD6 = (entity->params->flags & 0x4000) ? 0x40 : 0x80;
    entity->model->unk15C = entity->unkD6;
    func_001958A8((char *)entity, D_003E6E40[entity->params->unk02][1]);
    unkE0->unk04 = (void (*)(void))func_001384C0;
    unkE0->unk08 = (void (*)(void))func_001385B0;
    unkE0->unk18 = (void (*)(void))func_00138608;
    unkE0->unk14 = (void (*)(void))func_001818B8;
    unkE0->unk10 = NULL;
    if (entity->spawn_info->unk40 == 1) {
        unkE0->unk18 = NULL;
        unkE0->unk00 = func_001389C8;
    } else if (entity->spawn_info->unk40 == 2) {
        unkE0->unk00 = func_00138A48;
    } else if (entity->spawn_info->unk40 == 3) {
        unkE0->unk00 = func_00139730;
    } else if (entity->spawn_info->unk40 == 4) {
        unkE0->unk00 = func_00139FE0;
    } else if (entity->spawn_info->unk40 == 5) {
        unkE0->unk00 = func_0013A890;
    } else if (entity->spawn_info->unk40 == 6) {
        unkE0->unk00 = func_0013B150;
        unkE0->unk18 = (void (*)(void))func_00183570;
        entity->model->position[0] = -16200.0f;
        entity->model->position[1] = 3030.0f;
        entity->model->position[2] = 4900.0f;
        entity->unkD6 = 0;
        ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit20 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit26 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit21 = TRUE;
        unkE0->unk7A = 1;
    } else {
        unkE0->unk00 = func_00182C80;
        unkE0->unk18 = (void (*)(void))func_00183570;
    }
    for (i = 4 - 1; i != -1; i--) {
        entity->unkBC[i] = 0x80;
    }
    for (i = 6 - 1; i != -1; i--) {
        entity->unkC0[i] = 0x80;
    }
    ENTITY_UNK5C(entity)->as_s32 = 0;
    ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
    func_001958B8((char *)entity, unkE0->unk18, 0);
    unkE0->unk40 = 0.5f;
    func_00191478(entity->model);
    // match: an int result keeps $2 busy after the call
    ((int (*)(EntityModel *, s32, s32))func_00193378)(entity->model, 0, 0);
    unkE0->unk48 = entity->spawn_info->spawn_x;
    unkE0->unk4C = entity->spawn_info->spawn_y;
    unkE0->unk50 = entity->spawn_info->spawn_z;
}
