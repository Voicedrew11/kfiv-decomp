#include "kf_entity.h"
#include "kf_map.h"

extern MapSlot D_0038CD20[];
extern MapSlotMemory D_004321F0[2];
extern Entity D_003BEDF0[][0x80];
extern int D_004467D8[2];
extern EntitySpawnInfo *D_004467D0[2];
extern char *D_004467C0;
extern void *D_004FEB90[][0x100];
void *memset(void *, int, u32);

#define SPAWN_FLAGS(spawn_info) ((EntitySpawnFlags *)&(spawn_info)->unk38)

/* Sets up the entities of a map from its spawn records, filling in defaults for values left at 0. */
void func_00124F00(int map) {
    Entity *entity;
    EntitySpawnInfo *spawn_info;
    ModelInstance *model;
    int i;
    int count;
    MapSlot *slot;
    MapSlotMemory *memory;

    slot = &D_0038CD20[map];
    memory = &D_004321F0[map];
    entity = D_003BEDF0[map];
    spawn_info = D_004467D0[map];
    model = memory->unk44;
    D_004467D8[map] = ((MapSlotUnk08 *)slot->header)->unk42;
    memset(entity, 0, sizeof(D_003BEDF0[map]));
    memset(D_004FEB90[map], 0, sizeof(D_004FEB90[map]));
    count = D_004467D8[map];
    for (i = 0; i < count; i++, entity++, spawn_info++, model++) {
        entity->group_id = map;
        entity->model = (EntityModel *)model;
        entity->params = (EntityParams *)(D_004467C0 + spawn_info->type_id * sizeof(EntityParams));
        entity->unkD3 = -1;
        entity->spawn_info = spawn_info;
        D_004FEB90[map][SPAWN_FLAGS(spawn_info)->bytes[4]] = entity;
        if ((SPAWN_FLAGS(spawn_info)->word & (0xC000L << 45)) == (0x8000L << 45)) {
            ENTITY_FLAG_BITS(entity)->bit7 = FALSE;
        } else {
            ENTITY_FLAG_BITS(entity)->bit7 = TRUE;
        }
        entity->unkD7 = i;
        ENTITY_FLAG_BITS(entity)->bit5 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit6 = TRUE;
        if (entity->spawn_info->unk4C == 0) {
            entity->spawn_info->unk4C = 1500;
        }
        if (entity->spawn_info->unk56 == 0) {
            entity->spawn_info->unk56 = 500;
        }
        if (entity->spawn_info->unk50 == 0) {
            entity->spawn_info->unk50 = 1800;
        }
        if (entity->spawn_info->unk52 == 0) {
            entity->spawn_info->unk52 = 100;
        }
        if (entity->spawn_info->unk54 == 0) {
            entity->spawn_info->unk54 = 800;
        }
        if (entity->spawn_info->despawn_range == 0) {
            entity->spawn_info->despawn_range = 2000;
        }
        if (SPAWN_FLAGS(entity->spawn_info)->bytes[6] == 0) {
            SPAWN_FLAGS(entity->spawn_info)->bytes[6] = 80;
        }
        if (entity->params->unk42C == 0) {
            entity->params->unk42C = 75;
        }
        if (entity->params->unk0A == 0) {
            entity->params->unk0A = 30;
        }
        if (entity->params->unk0C == 0) {
            entity->params->unk0C = 70;
        }
        if (entity->params->unk42E == 0) {
            entity->params->unk42E = 1500;
        }
    }
}
