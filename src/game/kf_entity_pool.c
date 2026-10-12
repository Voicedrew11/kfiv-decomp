#include "common.h"
#include "kf_entity.h"
#include "kf_gs.h"
#include "kf_model.h"
#include "kf_sound.h"

void *memset(void *, int, u32);
int sprintf(char *, char *, ...);

#define ENTITY_POOL_BLOCK_SIZE  0x80000

/* the game's own data and functions, outside this file */
extern Entity *D_004467E4;
extern int D_00446F14;
extern int D_00446F60;
extern int D_004467E8;
extern int D_004467D8[2];
extern u32 D_00446ED4;
extern float D_00413F80[];
extern char D_00249680[0x80000];
extern EntityTypeFileHeader D_00474B70;
extern SoundLoadQueue D_0038A130;
extern char *D_004467C0;
extern int D_004467C4;
extern char D_004467C8[];
extern char D_0043E790[];
extern char D_00474B90[ENTITY_POOL_SLOT_COUNT][0x2000];
extern Entity D_003BEDF0[][0x80];
extern s64 D_003D6E40[];
extern ModelInstance D_003BE530[ENTITY_POOL_SLOT_COUNT];

int func_00102790(int, int);
int func_001024B0(int);
void func_00102228(void);
int func_00102230(void);
int func_00102238(int, int, int);
int func_001025C0(int, void *, int);
void func_00102218(void);
void func_0010B758(ModelInstance *, int *, int *);
s64 *func_0010CE08(ModelInstance *, s64 *);
void func_00108940(int, int *, ModelInstance *, int);
u32 *func_00116BD0(int, int);
void func_001251E0(Entity *);
void func_0010D018(ModelInstance *, char **, char **);
void func_0010E8A0(ModelInstance *, s32);
void func_0010E6A8(ModelInstance *, s64, s64);
void func_00109A50(void);
void func_00109A80(int, s64);
void func_00109AD0(void);
void func_0010AEB8(void *);
void func_00109E88(void);
void func_0010A008(void);
void func_0010AC80(int, int, int, int);
void func_0010CAE8(void *, s64, s64, s64, s64);
void func_0010CB98(void *);
void func_0010CF10(void *);
void func_00125600(Entity *);
void func_001A32A8(Entity *);

/* file-local functions, in order of definition */
static int func_00123918(Entity *entity);
static void func_00123AB8(Entity *entity);
static int func_00123AF0(int type_id);
static int func_00123B40(void);
static int func_00123C30(void);
static int func_00123DD8(int idx);
static int func_00123ED8(int pool_size);
static int func_00124008(int slot_index);
static void func_001240B8(int group_id);
static void func_001241A0(int slot_index);
static void func_00124318(int slot_index, int type_id, int two_blocks);
static void func_00124370(int slot_index);
static void func_001243E8(int slot_index);
static void func_00124428(EntityPoolSlot *slot);
static void func_001244C0(EntityPoolSlot *slot);
static void func_00124570(void);
static int func_001246A8(void);
static int func_00124720(int index);
static void func_00124798(int index);
static void func_001247F8(ModelInstance *model);
static void func_00124960(ModelInstance *model);
static void func_00124AF8(void);
static void func_00124B00(int slot_index);
static void func_00124BC8(Entity *entity);
static Entity *func_00124C08(Entity *entity);
static void func_00124C60(Entity *entity);
static Entity *func_00124CA0(Entity *entity);
static void func_00124CF8(Entity *entity);
static Entity *func_00124D48(Entity *entity);
static void func_001232C0(void);
static void func_001234F0(void);
static void func_00123690(void);

/* Entity pool state. Lists A, B and C hold the entities; the pool blocks and work areas are below. */
static EntityPoolSlot *D_00447098;      /* 0x447098 the block being read or unpacked */
static int D_0044709C;                  /* 0x44709C base of the work areas */
static u8 D_004470A0[8];                /* 0x4470A0 bit per work area in use */
static Entity *D_004470A8;              /* 0x4470A8 list A head */
static Entity *D_004470AC;              /* 0x4470AC list A tail */
static Entity *D_004470B0;              /* 0x4470B0 list B head */
static Entity *D_004470B4;              /* 0x4470B4 list B tail */
static int D_004470B8;                  /* 0x4470B8 list A count */
static int D_004470BC;                  /* 0x4470BC list B count */
static int D_004470C0;                  /* 0x4470C0 list C count */

/* Clear the entity pool and empty every entity list. */
void KF_Entity_ResetAllLists(void) {
    EntityPoolSlot *block;
    int block_address;
    int blocks_left;

    memset((void *)D_00446F60, 0, D_00446F14);

    memset(D_003D6DF0, 0, sizeof(D_003D6DF0));
    D_004470A8 = NULL;
    D_004467E8 = -1;
    D_004470B0 = NULL;
    D_004467E0 = NULL;
    D_004470AC = NULL;
    D_004470B4 = NULL;
    D_004467E4 = 0;
    memset(D_004470A0, 0, 8);
    D_004470B8 = 0;
    D_004470BC = 0;
    D_004470C0 = 0;
    memset(D_003BE530, 0, 0x8C0);
    block_address = D_00446F60;
    block = D_003D6DF0;
    blocks_left = ENTITY_POOL_SLOT_COUNT - 1;
    do {
        block->base_address = block_address;
        blocks_left -= 1;
        block->head.fields.kind = ENTITY_POOL_FREE;
        block->head.fields.unk2 = 0xFFFF;
        block_address += ENTITY_POOL_BLOCK_SIZE;
        block->unk4 = 0;
        block->unk6 = 0;
        block->age = 0;
        block++;
    } while (blocks_left >= 0);
    D_0044709C = block_address;
}

/* Returns TRUE while any pool block is held by a loading or unpacking job. */
int func_00123108(void) {
    int i;
    int kind;

    for (i = 0; i < ENTITY_POOL_SLOT_COUNT; i++) {
        kind = D_003D6DF0[i].head.fields.kind;
        if (kind != ENTITY_POOL_FREE && kind != ENTITY_POOL_READY && kind != ENTITY_POOL_EXTENSION) {
            return TRUE;
        }
    }
    return FALSE;
}

void func_00123158(int idx) {
    int i;
    Entity *group_entity = D_003BEDF0[idx];
    int group_count = D_004467D8[idx];
    for (i = 0; i < group_count; i++) {
        func_00124BC8(group_entity);
        group_entity++;
    }
}

/* Free every entity in the group from all three lists. */
void KF_Entity_FreeAllInGroup(int group_id) {
    Entity *entity;

    entity = D_004470A8;
    while (entity != NULL) {
        if (entity->group_id == group_id) {
            entity = func_00124C08(entity);
        } else {
            entity = entity->next;
        }
    }

    entity = D_004470B0;
    while (entity != NULL) {
        if (entity->group_id == group_id) {
            entity = func_00124CA0(entity);
        } else {
            entity = entity->next;
        }
    }

    entity = D_004467E0;
    while (entity != NULL) {
        if (entity->group_id == group_id) {
            func_001243E8(entity->unkD3);
            func_00123AB8(entity);
            entity = func_00124D48(entity);
            func_001A32A8(entity);
        } else {
            entity = entity->next;
        }
    }
}

/* Per-frame entity update: runs the pool, list and despawn steps in order. */
void func_00123288(void) {
    func_001232C0();
    func_001234F0();
    KF_Entity_DespawnOutOfRange();
    func_00123690();
    func_00124570();
}

/* Gives each entity a pool block and queues its load; out-of-range entities without a block are skipped. */
static void func_001232C0(void) {
    Entity *entity = D_004470A8;
    while (entity != NULL) {
        Entity *queued;
        float distance;
        float height_difference;

        distance = func_00124DE0(entity);
        entity->distance_to_player = distance;
        height_difference = ABS(D_00413F80[1] - entity->spawn_info->spawn_y);
        if (entity->unkD3 >= 0 ||
            (distance <= (float)entity->spawn_info->despawn_range &&
             (entity->spawn_info->vertical_despawn_range == 0 ||
              height_difference < (float)entity->spawn_info->vertical_despawn_range))) {
            entity->unkD3 = func_00123AF0(entity->spawn_info->type_id);
            // match: the slot is tested as a signed byte, as unkD3 is stored
            if ((char)entity->unkD3 >= 0) {
                queued = entity;
                entity = entity->next;
                if (func_00123918(queued)) {
                    func_00124C08(queued);
                    func_00124CF8(queued);
                }
                continue;
            }
            // match: pool_size is read unsigned (lbu) for this compare
            if ((u8)entity->spawn_info->pool_size < 5) {
                if (ENTITY_CONTROLLER(entity)->flags & CONTROLLER_FLAG_TWO_BLOCKS) {
                    entity->unkD3 = func_00123ED8(entity->spawn_info->pool_size);
                    if (entity->unkD3 >= 0) {
                        D_003D6DF0[entity->unkD3].head.word |= ENTITY_POOL_FLAG_200;
                        D_003D6DF0[entity->unkD3 + 1].head.word |= ENTITY_POOL_FLAG_200;
                    }
                } else {
                    entity->unkD3 = func_00123DD8(entity->spawn_info->pool_size);
                    if (entity->unkD3 >= 0) {
                        D_003D6DF0[entity->unkD3].head.word |= ENTITY_POOL_FLAG_200;
                    }
                }
            } else if (ENTITY_CONTROLLER(entity)->flags & CONTROLLER_FLAG_TWO_BLOCKS) {
                entity->unkD3 = func_00123C30();
                if (entity->unkD3 >= 0) {
                    D_003D6DF0[entity->unkD3].head.word |= ENTITY_POOL_FLAG_200;
                    D_003D6DF0[entity->unkD3 + 1].head.word |= ENTITY_POOL_FLAG_200;
                }
            } else {
                entity->unkD3 = func_00123B40();
                if (entity->unkD3 >= 0) {
                    D_003D6DF0[entity->unkD3].head.word |= ENTITY_POOL_FLAG_200;
                }
            }
            if (entity->unkD3 >= 0) {
                queued = entity;
                entity = entity->next;
                func_00124318(queued->unkD3, queued->spawn_info->type_id, ENTITY_CONTROLLER(queued)->flags >> 15);
                func_00124C08(queued);
                func_00124C60(queued);
                continue;
            }
        }
        entity = entity->next;
    }
}

/* Walks the D_004470B0 list and finishes each entity whose pool block is ready. */
static void func_001234F0(void) {
    Entity *entity;
    Entity *current;

    entity = D_004470B0;
    while (entity != NULL) {
        if (D_003D6DF0[entity->unkD3].head.fields.kind == ENTITY_POOL_READY) {
            current = entity;
            entity = entity->next;
            if (func_00123918(current)) {
                func_00124CA0(current);
                func_00124CF8(current);
            }
        } else {
            entity = entity->next;
        }
    }
}

/* Free entities that are out of range of the player. */
void KF_Entity_DespawnOutOfRange(void) {
    Entity *entity = D_004467E0;
    while (entity != NULL) {
        EntitySpawnInfo *spawn_info;
        float distance;
        float height_difference;
        int out_of_range;
        Entity *despawned;

        distance = func_00124DE0(entity);
        entity->distance_to_player = distance;
        spawn_info = entity->spawn_info;
        height_difference = D_00413F80[1] - spawn_info->spawn_y;
        height_difference = ABS(height_difference);
        out_of_range = FALSE;
        if ((float)spawn_info->despawn_range + ENTITY_DESPAWN_MARGIN < distance ||
            (spawn_info->vertical_despawn_range != 0 &&
             (float)spawn_info->vertical_despawn_range + ENTITY_VERTICAL_DESPAWN_MARGIN < height_difference)) {
            out_of_range = TRUE;
        }
        despawned = entity;
        if (out_of_range) {
            entity = entity->next;
            func_001243E8(despawned->unkD3);
            func_00123AB8(despawned);
            func_00124D48(despawned);
            func_00124BC8(despawned);
            func_001A32A8(despawned);
        } else {
            entity = entity->next;
        }
    }
}

/* Runs the entity pool's loads: steps the block being read or unpacked, ages the idle ones, and when
   nothing is busy starts reading the type file of the first queued block. */
static void func_00123690(void) {
    char path[0x100];
    EntityPoolSlot *slot;
    EntityPoolSlot *queued;
    int busy;
    int i;

    queued = NULL;
    busy = FALSE;
    for (i = 0, slot = D_003D6DF0; i < ENTITY_POOL_SLOT_COUNT; i++, slot++) {
        switch (slot->head.fields.kind) {
        case ENTITY_POOL_FREE:
            break;
        case ENTITY_POOL_QUEUED:
            if (queued == NULL) {
                queued = slot;
            }
            break;
        case ENTITY_POOL_READING:
            if (func_00102790(D_004467C4, 1) == 0) {
                if (((EntityController *)(D_004467C0 + slot->head.fields.unk2 * 0x450))->flags & CONTROLLER_FLAG_TWO_BLOCKS) {
                    slot[1].head.fields.kind = ENTITY_POOL_EXTENSION;
                    slot[1].head.fields.unk2 = 0xFFFF;
                }
                func_001024B0(D_004467C4);
                func_00124428(slot);
                func_00124B00(i);
                D_00474B70 = *(EntityTypeFileHeader *)D_00249680;
                D_004467C4 = -1;
                if (D_00474B70.unk18 != 0 && D_00474B70.unk14 != 0) {
                    D_00474B70.unk18 += (int)D_00249680;
                    D_00474B70.unk14 += (int)D_00249680;
                    slot->head.fields.kind = ENTITY_POOL_UNPACKING;
                    D_0038A130.unk1C = (int)&D_00474B70;
                    D_0038A130.unk18 = 1;
                    D_0038A130.unk20 = i;
                    D_0038A130.unk24 = (int)slot;
                } else {
                    slot->head.fields.kind = ENTITY_POOL_READY;
                }
            }
            busy = TRUE;
            break;
        case ENTITY_POOL_UNPACKING:
            if (D_0038A130.unk18 == 0) {
                slot->head.fields.kind = ENTITY_POOL_READY;
                func_00102228();
                D_00447098 = NULL;
            }
            busy = TRUE;
            break;
        case ENTITY_POOL_READY:
            if (slot->unk4 > 0) {
                break;
            }
        case ENTITY_POOL_6:
            if (slot->unk6 != 0) {
                slot->unk6--;
            }
            break;
        case ENTITY_POOL_EXTENSION:
            break;
        }
    }
    if (queued == NULL) {
        return;
    }
    if (busy) {
        return;
    }
    if (D_0038A130.unk04 != 0) {
        return;
    }
    if (func_00102230() != 0) {
        return;
    }
    sprintf(path, D_0043E790, D_004467C8, ((EntityParams *)(D_004467C0 + queued->head.fields.unk2 * 0x450))->unk00_0);
    D_004467C4 = func_00102238((int)path, 1, 1);
    if (D_004467C4 < 0) {
        func_001244C0(queued);
        return;
    }
    queued->head.fields.kind = ENTITY_POOL_READING;
    func_001025C0(D_004467C4, D_00249680, -1);
    func_00102218();
    D_00447098 = queued;
}

/* Gives an entity whose pool block is ready its work area and model; FALSE if it has to wait. */
static int func_00123918(Entity *entity) {
    int data_address;
    int work_address;
    EntityPoolSlot *slot = &D_003D6DF0[entity->unkD3];
    EntityPoolData *data = (EntityPoolData *)slot->base_address;
    ModelInstance *model;
    int work_index;

    if (slot->head.fields.kind != ENTITY_POOL_READY) {
        return FALSE;
    }
    work_index = func_001246A8();
    if (work_index < 0) {
        return FALSE;
    }
    entity->unkD4 = work_index;
    work_address = func_00124720(work_index);
    data_address = slot->base_address + (int)data->model_offset;
    func_0010B758(ENTITY_MODEL(entity), &data_address, &work_address);
    model = ENTITY_MODEL(entity);
    model->draw = func_001247F8;
    model->unk1B0 = NULL;
    if (entity->spawn_info->type_id == 0x47) {
        model->unk100 |= 0x200;
        model->unk102 |= MODEL_FLAG_800;
        model->draw = func_00124960;
        model->unk1B0 = func_00124AF8;
        D_004467E8 = entity->spawn_info->type_id;
        work_address = (int)D_003D6E40;
        work_address = (int)func_0010CE08(model, (s64 *)work_address);
    }
    data_address = slot->base_address + (int)data->motion_offset;
    func_00108940(data_address, &work_address, ENTITY_MODEL(entity), 1);
    data_address = slot->base_address + data->unk0C;
    ENTITY_MODEL(entity)->unk194 = data_address;
    ENTITY_MODEL(entity)->unk190 = 0;
    ENTITY_MODEL(entity)->unk19C = (int)func_00116BD0(entity->group_id, 3);
    ENTITY_MODEL(entity)->unk1A0 = (int)func_00116BD0(entity->group_id, 5);
    entity->unkD2 = slot->unk7;
    func_00124370(entity->unkD3);
    ((void (*)(Entity *))func_001251E0)(entity); // match: declared without its argument elsewhere
    return TRUE;
}

/* Releases the entity's work area and clears its pool and work-area indices. */
static void func_00123AB8(Entity *entity) {
    func_00124798(entity->unkD4);
    entity->unkD4 = entity->unkD3 = -1;
}

/* Returns the pool block index in use (kinds 1 to 4) for a type id, or -1. */
static int func_00123AF0(int type_id) {
    int i;
    EntityPoolSlot *slot;

    for (i = 0; i < ENTITY_POOL_SLOT_COUNT; i++) {
        slot = &D_003D6DF0[i];
        if ((u32)(slot->head.fields.kind - 1) < 4 && slot->head.fields.unk2 == type_id) {
            return i;
        }
    }
    return -1;
}

/* Claims the first free pool block, else the unused block with the lowest size; returns its index. */
static int func_00123B40(void) {
    int i;
    int best;
    EntityPoolSlot *slot;

    best = -1;
    for (i = 0; i < 5; i++) {
        slot = &D_003D6DF0[i];
        if ((slot->head.word >> 9) & 1) { continue; }
        if ((slot->head.word >> 8) & 1) { continue; }
        if (slot->head.fields.kind == 0) { return i; }
        if (slot->head.fields.kind != 4) { continue; }
        if (slot->unk4 != 0) { continue; }
        if (slot->unk6 != 0) { continue; }
        if (best == -1 || (u32)slot->age < (u32)D_003D6DF0[best].age) {
            best = i;
        }
    }
    if (best >= 0) {
        D_003D6DF0[best].head.fields.kind = 0;
        if ((i = best + 1) < 5 && D_003D6DF0[i].head.fields.kind == 5) {
            D_003D6DF0[i].head.fields.kind = 0;
        }
    }
    return best;
}

/* Claims a free pair of pool blocks for a two-block type, else the first reusable pair; returns its first index or -1. */
static int func_00123C30(void) {
    int i;
    int j;
    int pair;
    int best;

    best = -1;
    for (i = 0; i < 2; i++) {
        pair = i * 2;
        if (D_003D6DF0[pair].head.bits.flag_200) { continue; }
        if (D_003D6DF0[pair + 1].head.bits.flag_200) { continue; }
        if (D_003D6DF0[pair].head.bits.flag_100) { continue; }
        if (D_003D6DF0[pair + 1].head.bits.flag_100) { continue; }
        if (D_003D6DF0[pair].head.fields.kind == ENTITY_POOL_FREE && D_003D6DF0[pair + 1].head.fields.kind == ENTITY_POOL_FREE) {
            return pair;
        }
    }
    // match: a second counter, so the pair index gets the register retail uses
    for (j = 0; j < 2; j++) {
        pair = j * 2;
        if (D_003D6DF0[pair].head.bits.flag_200) { continue; }
        if (D_003D6DF0[pair + 1].head.bits.flag_200) { continue; }
        if (D_003D6DF0[pair].head.bits.flag_100) { continue; }
        if (D_003D6DF0[pair + 1].head.bits.flag_100) { continue; }
        if (D_003D6DF0[pair].head.fields.kind != ENTITY_POOL_FREE) {
            if (D_003D6DF0[pair].head.fields.kind != ENTITY_POOL_READY) { continue; }
            if (D_003D6DF0[pair].unk4 != 0) { continue; }
            if (D_003D6DF0[pair].unk6 != 0) { continue; }
        }
        if (D_003D6DF0[pair + 1].head.fields.kind == ENTITY_POOL_FREE || D_003D6DF0[pair + 1].head.fields.kind == ENTITY_POOL_EXTENSION) {
            best = pair;
            break;
        }
        if (D_003D6DF0[pair + 1].head.fields.kind != ENTITY_POOL_READY) { continue; }
        if (D_003D6DF0[pair + 1].unk4 != 0) { continue; }
        if (D_003D6DF0[pair + 1].unk6 != 0) { continue; }
        best = pair;
        break;
    }
    if (best >= 0) {
        D_003D6DF0[best].head.fields.kind = ENTITY_POOL_FREE;
        if ((j = best + 1) < 5 && D_003D6DF0[j].head.fields.kind == ENTITY_POOL_EXTENSION) {
            D_003D6DF0[j].head.fields.kind = ENTITY_POOL_FREE;
        }
    }
    return best;
}

static int func_00123DD8(int idx) {
    EntityPoolSlot *slot = &D_003D6DF0[idx];

    if (slot->head.word & ENTITY_POOL_FLAG_200) {
        return -1;
    }
    if (slot->head.fields.kind == ENTITY_POOL_FREE) {
        func_001241A0(idx);
        return idx;
    }
    // match: unk4 and unk6 are tested as one word
    if (slot->head.fields.kind == ENTITY_POOL_READY && !(*(int *)&slot->unk4 & 0xFFFFFF)) {
        func_001241A0(idx);
        if (idx < 4 && slot[1].head.fields.kind == ENTITY_POOL_EXTENSION) {
            slot++;
            slot->head.fields.unk2 = 0xFFFF;
            slot->head.fields.kind = ENTITY_POOL_FREE;
            slot->unk4 = 0;
            slot->head.bits.flag_100 = 0;
        }
        return idx;
    }
    if (slot->head.fields.kind == ENTITY_POOL_EXTENSION && func_00123DD8(idx - 1) != -1) {
        func_001241A0(idx);
        return idx;
    }
    func_001241A0(idx);
    return -1;
}

/* Claims a pair of pool blocks starting at this one for a two-block type; returns its index or -1. */
static int func_00123ED8(int pool_size) {
    EntityPoolSlot *slot = &D_003D6DF0[pool_size];
    EntityPoolSlot *next = &D_003D6DF0[pool_size + 1];
    int first_free;
    int second_free;

    if (slot->head.word & ENTITY_POOL_FLAG_200) {
        return -1;
    }
    if (slot->head.fields.kind == ENTITY_POOL_FREE ||
        (slot->head.fields.kind == ENTITY_POOL_READY && slot->unk4 == 0 && slot->unk6 == 0)) {
        first_free = TRUE;
    } else {
        first_free = !func_00124008(pool_size);
    }
    if (!first_free) {
        return -1;
    }
    if (next->head.word & ENTITY_POOL_FLAG_200) {
        return -1;
    }
    // the game's own slip: this result is overwritten by the test right below (an `else` is missing),
    // so a free or ready second block is always checked with func_00124008. It leaves a dead test in the code.
    if (next->head.fields.kind == ENTITY_POOL_FREE || next->head.fields.kind == ENTITY_POOL_READY) {
        second_free = TRUE;
    }
    if (next->head.fields.kind == ENTITY_POOL_EXTENSION && first_free) {
        second_free = TRUE;
    } else {
        second_free = !func_00124008(pool_size + 1);
    }
    if (!second_free) {
        return -1;
    }
    func_001240B8(pool_size);
    func_001240B8(pool_size + 1);
    slot->head.fields.unk2 = 0xFFFF;
    slot->unk4 = 0;
    slot->head.bits.flag_100 = TRUE;
    next->head.fields.unk2 = 0xFFFF;
    next->unk4 = 0;
    next->head.bits.flag_100 = TRUE;
    return pool_size;
}

/* TRUE if a one-block entity (pool_size 0-4) uses this pool slot, or the block it extends. */
static int func_00124008(int slot_index) {
    Entity *entity;

    if (D_003D6DF0[slot_index].head.fields.kind == ENTITY_POOL_EXTENSION) {
        slot_index--;
    }

    entity = D_004470B0;
    while (entity != NULL) {
        if (slot_index == entity->unkD3 && entity->spawn_info->pool_size >= 0 && entity->spawn_info->pool_size < 5) {
            return TRUE;
        }
        entity = entity->next;
    }

    entity = D_004467E0;
    while (entity != NULL) {
        if (slot_index == entity->unkD3 && entity->spawn_info->pool_size >= 0 && entity->spawn_info->pool_size < 5) {
            return TRUE;
        }
        entity = entity->next;
    }
    return FALSE;
}

/* Frees the entities matching a group id from two lists. */
static void func_001240B8(int group_id) {
    Entity *entity;
    Entity *next;
    int other; /* never assigned: retail compares against callee-saved $19 */

    entity = D_004470B0;
    while (entity != NULL) {
        if (group_id == entity->unkD3 ||
            (ENTITY_CONTROLLER(entity)->flags & 0x8000 && entity->unkD3 == other && entity->unkD3 >= 0)) {
            next = entity->next;
            func_00124CA0(entity);
            func_00124BC8(entity);
            entity = next;
        } else {
            entity = entity->next;
        }
    }

    entity = D_004467E0;
    while (entity != NULL) {
        if (group_id == entity->unkD3) {
            func_001A32A8(entity);
            func_001243E8(entity->unkD3);
            func_00123AB8(entity);
            next = entity->next;
            func_00124D48(entity);
            func_00124BC8(entity);
            entity = next;
        } else {
            entity = entity->next;
        }
    }
}

/* Frees the users of a pool slot and marks the slot empty; keeps it if a small block still uses it. */
static void func_001241A0(int slot_index) {
    Entity *entity;
    Entity *next;
    EntityPoolSlot *slot;

    slot = &D_003D6DF0[slot_index];
    entity = D_004470B0;
    while (entity != NULL) {
        if (slot_index == entity->unkD3) {
            // match: nested ifs keep the signed pool_size compares
            if (entity->spawn_info->pool_size >= 0) {
                if (entity->spawn_info->pool_size < 5) {
                    return;
                }
            }
        }
        entity = entity->next;
    }

    entity = D_004467E0;
    while (entity != NULL) {
        if (slot_index == entity->unkD3) {
            // match: nested ifs keep the signed pool_size compares
            if (entity->spawn_info->pool_size >= 0) {
                if (entity->spawn_info->pool_size < 5) {
                    return;
                }
            }
        }
        entity = entity->next;
    }

    entity = D_004470B0;
    while (entity != NULL) {
        if (slot_index == entity->unkD3) {
            next = entity->next;
            func_00124CA0(entity);
            func_00124BC8(entity);
            entity = next;
        } else {
            entity = entity->next;
        }
    }

    entity = D_004467E0;
    while (entity != NULL) {
        if (slot_index == entity->unkD3) {
            func_001A32A8(entity);
            func_001243E8(entity->unkD3);
            func_00123AB8(entity);
            next = entity->next;
            func_00124D48(entity);
            func_00124BC8(entity);
            entity = next;
        } else {
            entity = entity->next;
        }
    }

    slot->unk4 = 0;
    slot->head.fields.unk2 = 0xFFFF;
    slot->head.word |= ENTITY_POOL_FLAG_100;
}

/* Marks a pool slot QUEUED for a type id; a two-block type also marks the next slot as its extension. */
static void func_00124318(int slot_index, int type_id, int two_blocks) {
    EntityPoolSlot *slot = &D_003D6DF0[slot_index];

    slot->head.fields.kind = ENTITY_POOL_QUEUED;
    slot->head.fields.unk2 = type_id;
    slot->unk4 = 0;
    slot->unk6 = 0;
    slot->age = 0;
    if (two_blocks) {
        slot++;
        slot->head.fields.kind = ENTITY_POOL_EXTENSION;
        slot->head.fields.unk2 = 0xFFFF;
        slot->unk4 = 0;
        slot->unk6 = 0;
        slot->age = 0;
    }
}

/* Adds a user to a pool block and clears its flags; a two-block type's second block gets the same flag clear. */
static void func_00124370(int slot_index) {
    D_003D6DF0[slot_index].head.bits.flag_100 = 0;
    D_003D6DF0[slot_index].unk4++;
    D_003D6DF0[slot_index].head.bits.flag_200 = 0;
    if (slot_index < 4 && D_003D6DF0[slot_index + 1].head.fields.kind == ENTITY_POOL_EXTENSION) {
        D_003D6DF0[slot_index + 1].head.bits.flag_100 = 0;
        D_003D6DF0[slot_index + 1].head.bits.flag_200 = 0;
    }
}

/* Drops one user of a pool block; when none are left, marks it released at the current frame. */
static void func_001243E8(int slot_index) {
    EntityPoolSlot *slot;

    slot = &D_003D6DF0[slot_index];
    if (--slot->unk4 <= 0) {
        slot->unk6 = 2;
        slot->age = D_00446ED4;
        slot->unk4 = 0;
    }
}

/* Copies the loaded type file into the slot's block; hangs if it is empty or too big for the block. */
static void func_00124428(EntityPoolSlot *slot) {
    EntityController *controller;
    int *file;
    int table_offset;
    int size;

    controller = (EntityController *)(D_004467C0 + slot->head.fields.unk2 * 0x450);
    file = (int *)D_00249680;
    size = 0;
    // match: header words of the loaded file, no named struct yet
    if ((table_offset = file[4]) != 0 || (table_offset = file[3]) != 0) {
        size = table_offset + *(int *)(table_offset + (char *)file);
    }
    if (size == 0) {
        goto hang;
    }
    if (controller->flags & CONTROLLER_FLAG_TWO_BLOCKS) {
        if (size > 0x100000) {
            goto hang;
        }
    } else if (size > 0x80000) {
        goto hang;
    }
    memcpy((void *)slot->base_address, D_00249680, size);
    return;
hang:
    goto hang;
}

/* Frees every entity on lists A and B whose type id matches the slot's, then marks the slot free. */
static void func_001244C0(EntityPoolSlot *slot) {
    Entity *entity;

    entity = D_004470A8;
    while (entity != NULL) {
        if (entity->spawn_info->type_id == slot->head.fields.unk2) {
            entity = func_00124C08(entity);
        } else {
            entity = entity->next;
        }
    }

    entity = D_004470B0;
    while (entity != NULL) {
        if (entity->spawn_info->type_id == slot->head.fields.unk2) {
            entity = func_00124CA0(entity);
        } else {
            entity = entity->next;
        }
    }

    slot->head.fields.kind = ENTITY_POOL_FREE;
    slot->head.fields.unk2 = 0xFFFF;
    slot->head.bits.flag_100 = 0;
    slot->head.bits.flag_200 = 0;
}

/* Frees each pool slot whose type no live entity uses any more. */
static void func_00124570(void) {
    EntityPoolSlot *slot;
    Entity *entity;
    Entity *list_a;
    Entity *list_b;
    Entity *list_c;
    int slot_index;

    list_a = D_004470A8;
    list_b = D_004470B0;
    list_c = D_004467E0;
    slot_index = 0;
    slot = D_003D6DF0;
    while (slot_index < ENTITY_POOL_SLOT_COUNT) {
        if ((slot->head.word & 0x2FF) == (ENTITY_POOL_FLAG_200 | ENTITY_POOL_READY)) {
            for (entity = list_a; entity != NULL; entity = entity->next) {
                if (entity->spawn_info->type_id == slot->head.fields.unk2) {
                    goto in_use;
                }
            }
            for (entity = list_b; entity != NULL; entity = entity->next) {
                if (entity->spawn_info->type_id == slot->head.fields.unk2) {
                    goto in_use;
                }
            }
            for (entity = list_c; entity != NULL; entity = entity->next) {
                if (entity->spawn_info->type_id == slot->head.fields.unk2) {
                    goto in_use;
                }
            }
            slot->head.fields.kind = ENTITY_POOL_FREE;
            slot->head.fields.unk2 = 0xFFFF;
            slot->head.bits.flag_100 = 0;
            slot->head.bits.flag_200 = 0;
            if (slot_index < 4 && slot[1].head.fields.kind == ENTITY_POOL_EXTENSION) {
                slot[1].head.fields.kind = ENTITY_POOL_FREE;
                slot[1].head.fields.unk2 = 0xFFFF;
                slot[1].head.bits.flag_100 = 0;
                slot[1].head.bits.flag_200 = 0;
            }
        }
in_use:
        slot_index++;
        slot++;
    }
}

/* Returns the index of the first clear bit among the 60 flag bits, or -1. */
static int func_001246A8(void) {
    u8 *flags = D_004470A0;
    int i;
    int j;
    u8 bit;

    for (i = 0; i < 8; i++, flags++) {
        bit = 1;
        for (j = 0; j < 8 && i * 8 + j < 60; j++) {
            if (!(*flags & bit)) {
                return i * 8 + j;
            }
            bit <<= 1;
        }
    }
    return -1;
}

/* Marks work area `index` in use, clears it, and returns its address. */
static int func_00124720(int index) {
    int work_area;

    D_004470A0[index / 8] |= 1 << (index % 8);
    work_area = D_0044709C + index * 0x2200;
    memset((void *)work_area, 0, 0x2200);
    return work_area;
}

/* Marks work area `index` free and clears it. */
static void func_00124798(int index) {
    u8 bit;
    int work_area;

    bit = 1 << (index % 8);
    D_004470A0[index / 8] &= ~bit;
    work_area = D_0044709C + index * 0x2200;
    memset((void *)work_area, 0, 0x2200);
}

/* Sets the GS Z-buffer/test/alpha registers (by the model alpha), then hands off. */
static void func_001247F8(ModelInstance *model) {
    func_00109A50();
    if (model->alpha >= 128.0f) {
        func_00109A80(GS_REG_ZBUF_1, 0xA000118L);
        func_00109A80(GS_REG_ZBUF_2, 0xA000118L);
        func_00109A80(GS_REG_TEST_1, 0x5000D);
        func_00109A80(GS_REG_TEST_2, 0x5000D);
    } else {
        func_00109A80(GS_REG_ZBUF_1, 0x10A000118L);
        func_00109A80(GS_REG_ZBUF_2, 0x10A000118L);
        func_00109A80(GS_REG_TEST_1, 0x5000D);
        func_00109A80(GS_REG_TEST_2, 0x30001);
    }
    func_00109A80(GS_REG_ALPHA_1, 0x44);
    func_00109A80(GS_REG_ALPHA_2, 9);
    func_00109AD0();
    func_0010AEB8(model);
}

/* Sets the GS Z-buffer, alpha and test registers, then draws the model. */
void func_001248F8(ModelInstance *model) {
    func_00109A50();
    func_00109A80(GS_REG_ZBUF_1, 0x10A000118L);
    func_00109A80(GS_REG_TEST_1, 0x50000);
    func_00109A80(GS_REG_ALPHA_1, 0x48);
    func_00109AD0();
    func_0010AEB8(model);
}

/* Draw callback: sets the GS registers for the model's alpha and flags, then draws it. */
static void func_00124960(ModelInstance *model) {
    if (model->unk102 & MODEL_FLAG_800) {
        func_0010CAE8(model, 0x220013540L, 0x60, -1, -1);
        func_0010CB98(model);
        func_0010AC80(0x3540, 0, 0x100, 0x100);
        func_0010A008();
        func_00109E88();
    } else {
        func_0010AEB8(model);
        func_0010CF10(model);
    }
    func_00109A50();

    if (128.0f <= model->alpha) {
        func_00109A80(GS_REG_ZBUF_1, 0xA000118L);
        func_00109A80(GS_REG_ZBUF_2, 0xA000118L);
        func_00109A80(GS_REG_TEST_1, 0x5000DL);
        if (model->unk102 & MODEL_FLAG_800) {
            func_00109A80(GS_REG_TEST_2, 0x30001L);
        } else {
            func_00109A80(GS_REG_TEST_2, 0x5000DL);
        }
    } else {
        func_00109A80(GS_REG_ZBUF_1, 0x10A000118L);
        func_00109A80(GS_REG_ZBUF_2, 0x10A000118L);
        func_00109A80(GS_REG_TEST_1, 0x5000DL);
        func_00109A80(GS_REG_TEST_2, 0x30001L);
    }
    func_00109A80(GS_REG_ALPHA_1, 0x44);
    func_00109A80(GS_REG_ALPHA_2, 0x9);
    func_00109AD0();
}

static void func_00124AF8(void) {
}

/* Clears a pool slot's model and work buffer, then builds the model from the slot's loaded data. */
static void func_00124B00(int slot_index) {
    char *model_data;
    char *buffer;
    ModelInstance *model = &D_003BE530[slot_index];
    // match: never used, but it leaves the table address and the scaled index in registers before the calls
    EntityPoolSlot *unused_slot = &D_003D6DF0[slot_index];
    EntityPoolData *data;

    buffer = D_00474B90[slot_index];
    memset(model, 0, sizeof(ModelInstance));
    memset(buffer, 0, sizeof(D_00474B90[slot_index]));
    data = (EntityPoolData *)D_003D6DF0[slot_index].base_address;
    if (data->pool_model_offset != 0) {
        model_data = (char *)data + data->pool_model_offset;
        func_0010D018(model, &model_data, &buffer);
        func_0010E8A0(model, 0x5A0);
        func_0010E6A8(model, 0x20, 0x20);
    }
}

/* Appends an entity to the tail of list A. */
static void func_00124BC8(Entity *entity) {
    Entity *tail;

    if (D_004470A8 != NULL) {
        tail = D_004470AC;
        D_004470AC = entity;
        tail->next = entity;
        entity->prev = tail;
        entity->next = NULL;
    } else {
        D_004470A8 = entity;
        D_004470AC = entity;
        entity->prev = NULL;
        entity->next = NULL;
    }
    D_004470B8++;
}

/* Unlinks an entity from its list and returns the next one. */
static Entity *func_00124C08(Entity *entity) {
    Entity *next;

    next = entity->next;
    if (D_004470A8 == entity) {
        D_004470A8 = next;
    }
    if (D_004470AC == entity) {
        D_004470AC = entity->prev;
    }
    if (entity->prev != NULL) {
        entity->prev->next = next;
    }
    if (entity->next != NULL) {
        entity->next->prev = entity->prev;
    }
    D_004470B8--;
    return next;
}

/* Appends an entity to the tail of list B. */
static void func_00124C60(Entity *entity) {
    Entity *tail;

    if (D_004470B0 != NULL) {
        tail = D_004470B4;
        D_004470B4 = entity;
        tail->next = entity;
        entity->prev = tail;
        entity->next = NULL;
    } else {
        D_004470B0 = entity;
        D_004470B4 = entity;
        entity->prev = NULL;
        entity->next = NULL;
    }
    D_004470BC++;
}

static Entity *func_00124CA0(Entity *entity) {
    Entity *next;

    next = entity->next;
    if (D_004470B0 == entity) {
        D_004470B0 = next;
    }
    if (D_004470B4 == entity) {
        D_004470B4 = entity->prev;
    }
    if (entity->prev != 0) {
        entity->prev->next = next;
    }
    if (entity->next != 0) {
        entity->next->prev = entity->prev;
    }
    D_004470BC--;
    return next;
}

/* Appends an entity to the tail of list C and sets flag bit 0. */
static void func_00124CF8(Entity *entity) {
    Entity *tail;

    if (D_004467E0 != NULL) {
        tail = D_004467E4;
        D_004467E4 = entity;
        tail->next = entity;
        entity->prev = tail;
        entity->next = NULL;
    } else {
        D_004467E0 = entity;
        D_004467E4 = entity;
        entity->prev = NULL;
        entity->next = NULL;
    }
    ENTITY_FLAG_BITS(entity)->bit0 = 1;
    D_004470C0++;
}

/* Unlinks an entity from list C and returns the next one. */
static Entity *func_00124D48(Entity *entity) {
    Entity *next;

    next = entity->next;
    if (D_004467E0 == entity) {
        D_004467E0 = next;
    }
    if (D_004467E4 == entity) {
        D_004467E4 = entity->prev;
    }
    if (entity->prev != NULL) {
        entity->prev->next = next;
    }
    if (entity->next != NULL) {
        entity->next->prev = entity->prev;
    }
    func_00125600(entity);
    entity->flags &= ~1;
    D_004470C0--;
    return next;
}
