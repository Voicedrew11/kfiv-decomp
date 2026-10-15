#include "common.h"
#include "kf_entity_world.h"
#include "kf_entity.h"
#include "kf_gs.h"
#include "kf_model.h"
#include "kf_sound.h"
#include "kf_effect.h"

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
extern float player_state[];
extern char file_load_buffer[0x80000];
extern SoundLoadQueue sound_load_queue;
extern char *D_004467C0;
extern int D_004467C4;
extern char D_004467C8[];
extern char D_0043E790[];
extern Entity entity_pool[][0x80];
extern s64 entity_work_area[];
extern ModelInstance entity_models[ENTITY_POOL_SLOT_COUNT];

static EntityTypeFileHeader entity_type_file_header;                            /* 0x474B70 the loaded type file's first 0x20 bytes */
static char entity_pool_slot_buffers[ENTITY_POOL_SLOT_COUNT][0x2000];            /* 0x474B90 the type data of each slot */
char character_params[0x80000] __attribute__((section(".bss")));         /* 0x47EB90 param/chr.prm, read whole */

int File_Sync(int, int);
int File_Close(int);
void File_Unlock(void);
int File_IsLocked(void);
int File_Open(char *, int, int);
int File_Read(int, void *, int);
void File_Lock(void);
void Model_SetUpSkinned(ModelInstance *, int *, int *);
s64 *Model_SaveGsWordsAndUvs(ModelInstance *, s64 *);
MotionData *ModelMotion_LoadFile(MotionFile *, ModelChannel **, ModelInstance *, int);
u32 *Map_GetSlotPacket(int, int);
void Model_SetUpPosed(ModelInstance *, char **, char **);
void Model_SetVu1Program(ModelInstance *, s32);
void Model_SetPrimBits(ModelInstance *, s64, s64);
void Packet_BeginGsRegisters(void);
void Packet_AddGsRegister(int, s64);
void Packet_EndGsRegisters(void);
void ModelTexture_SendForModel(void *);
void GsDraw_QueueTexFlush(void);
void GsDraw_ResetDrawTarget(void);
void GsDraw_CopyFrameToTexture(int, int, int, int);
void Model_SetBlockGsWords(void *, s64, s64, s64, s64);
void Model_SetUvsFromScreen(void *);
void Model_RestoreGsWordsAndUvs(void *);

/* file-local functions, in order of definition */
static int Entity_TryActivate(Entity *entity);
static void Entity_ReleaseWorkArea(Entity *entity);
static int EntityPool_FindType(int type_id);
static int EntityPool_ClaimBlock(void);
static int EntityPool_ClaimBlockPair(void);
static int EntityPool_ClaimFixedBlock(int idx);
static int EntityPool_ClaimFixedBlockPair(int pool_size);
static int EntityPool_HasFixedUser(int slot_index);
static void EntityPool_EvictBlockUsers(int group_id);
static void EntityPool_EmptyBlock(int slot_index);
static void EntityPool_QueueLoad(int slot_index, int type_id, int two_blocks);
static void EntityPool_AddUser(int slot_index);
static void EntityPool_RemoveUser(int slot_index);
static void EntityPool_CopyLoadedFile(EntityPoolSlot *slot);
static void EntityPool_AbandonLoad(EntityPoolSlot *slot);
static void EntityPool_FreeUnusedBlocks(void);
static int Entity_FindFreeWorkArea(void);
static int Entity_ClaimWorkArea(int index);
static void Entity_FreeWorkArea(int index);
static void Entity_PrepareModelDraw(ModelInstance *model);
static void Entity_PrepareScreenTexturedDraw(ModelInstance *model);
static void func_00124AF8(void);
static void EntityPool_BuildBlockModel(int slot_index);
static void Entity_AppendToWaiting(Entity *entity);
static Entity *Entity_UnlinkWaiting(Entity *entity);
static void Entity_AppendToLoading(Entity *entity);
static Entity *Entity_UnlinkLoading(Entity *entity);
static void Entity_AppendToActive(Entity *entity);
static Entity *Entity_UnlinkActive(Entity *entity);
static void Entity_StartLoadsInRange(void);
static void Entity_ActivateLoaded(void);
static void EntityPool_StepLoads(void);

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

float sqrtf(float);

/* Clear the entity pool and empty every entity list. */
void Entity_ResetAllLists(void) {
    EntityPoolSlot *block;
    int block_address;
    int blocks_left;

    memset((void *)D_00446F60, 0, D_00446F14);

    memset(entity_pool_slots, 0, sizeof(entity_pool_slots));
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
    memset(entity_models, 0, 0x8C0);
    block_address = D_00446F60;
    block = entity_pool_slots;
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
int EntityPool_IsLoading(void) {
    int i;
    int kind;

    for (i = 0; i < ENTITY_POOL_SLOT_COUNT; i++) {
        kind = entity_pool_slots[i].head.fields.kind;
        if (kind != ENTITY_POOL_FREE && kind != ENTITY_POOL_READY && kind != ENTITY_POOL_EXTENSION) {
            return TRUE;
        }
    }
    return FALSE;
}

/* Puts every entity of group `idx` on list A (waiting for a pool block). */
void Entity_AddGroupToWaiting(int idx) {
    int i;
    Entity *group_entity = entity_pool[idx];
    int group_count = D_004467D8[idx];
    for (i = 0; i < group_count; i++) {
        Entity_AppendToWaiting(group_entity);
        group_entity++;
    }
}

/* Free every entity in the group from all three lists. */
void Entity_FreeAllInGroup(int group_id) {
    Entity *entity;

    entity = D_004470A8;
    while (entity != NULL) {
        if (entity->group_id == group_id) {
            entity = Entity_UnlinkWaiting(entity);
        } else {
            entity = entity->next;
        }
    }

    entity = D_004470B0;
    while (entity != NULL) {
        if (entity->group_id == group_id) {
            entity = Entity_UnlinkLoading(entity);
        } else {
            entity = entity->next;
        }
    }

    entity = D_004467E0;
    while (entity != NULL) {
        if (entity->group_id == group_id) {
            EntityPool_RemoveUser(entity->unkD3);
            Entity_ReleaseWorkArea(entity);
            entity = Entity_UnlinkActive(entity);
            Effect_FreeForOwner(entity);
        } else {
            entity = entity->next;
        }
    }
}

/* Per-frame entity update: runs the pool, list and despawn steps in order. */
void Entity_UpdatePool(void) {
    Entity_StartLoadsInRange();
    Entity_ActivateLoaded();
    Entity_DespawnOutOfRange();
    EntityPool_StepLoads();
    EntityPool_FreeUnusedBlocks();
}

/* Gives each entity a pool block and queues its load; out-of-range entities without a block are skipped. */
static void Entity_StartLoadsInRange(void) {
    Entity *entity = D_004470A8;
    while (entity != NULL) {
        Entity *queued;
        float distance;
        float height_difference;

        distance = Entity_SpawnDistanceToPlayer(entity);
        entity->distance_to_player = distance;
        height_difference = ABS(player_state[1] - entity->spawn_info->spawn_y);
        if (entity->unkD3 >= 0 ||
            (distance <= (float)entity->spawn_info->despawn_range &&
             (entity->spawn_info->vertical_despawn_range == 0 ||
              height_difference < (float)entity->spawn_info->vertical_despawn_range))) {
            entity->unkD3 = EntityPool_FindType(entity->spawn_info->type_id);
            // match: the slot is tested as a signed byte, as unkD3 is stored
            if ((char)entity->unkD3 >= 0) {
                queued = entity;
                entity = entity->next;
                if (Entity_TryActivate(queued)) {
                    Entity_UnlinkWaiting(queued);
                    Entity_AppendToActive(queued);
                }
                continue;
            }
            // match: pool_size is read unsigned (lbu) for this compare
            if ((u8)entity->spawn_info->pool_size < 5) {
                if (ENTITY_CONTROLLER(entity)->flags & CONTROLLER_FLAG_TWO_BLOCKS) {
                    entity->unkD3 = EntityPool_ClaimFixedBlockPair(entity->spawn_info->pool_size);
                    if (entity->unkD3 >= 0) {
                        entity_pool_slots[entity->unkD3].head.word |= ENTITY_POOL_FLAG_200;
                        entity_pool_slots[entity->unkD3 + 1].head.word |= ENTITY_POOL_FLAG_200;
                    }
                } else {
                    entity->unkD3 = EntityPool_ClaimFixedBlock(entity->spawn_info->pool_size);
                    if (entity->unkD3 >= 0) {
                        entity_pool_slots[entity->unkD3].head.word |= ENTITY_POOL_FLAG_200;
                    }
                }
            } else if (ENTITY_CONTROLLER(entity)->flags & CONTROLLER_FLAG_TWO_BLOCKS) {
                entity->unkD3 = EntityPool_ClaimBlockPair();
                if (entity->unkD3 >= 0) {
                    entity_pool_slots[entity->unkD3].head.word |= ENTITY_POOL_FLAG_200;
                    entity_pool_slots[entity->unkD3 + 1].head.word |= ENTITY_POOL_FLAG_200;
                }
            } else {
                entity->unkD3 = EntityPool_ClaimBlock();
                if (entity->unkD3 >= 0) {
                    entity_pool_slots[entity->unkD3].head.word |= ENTITY_POOL_FLAG_200;
                }
            }
            if (entity->unkD3 >= 0) {
                queued = entity;
                entity = entity->next;
                EntityPool_QueueLoad(queued->unkD3, queued->spawn_info->type_id, ENTITY_CONTROLLER(queued)->flags >> 15);
                Entity_UnlinkWaiting(queued);
                Entity_AppendToLoading(queued);
                continue;
            }
        }
        entity = entity->next;
    }
}

/* Walks the D_004470B0 list and finishes each entity whose pool block is ready. */
static void Entity_ActivateLoaded(void) {
    Entity *entity;
    Entity *current;

    entity = D_004470B0;
    while (entity != NULL) {
        if (entity_pool_slots[entity->unkD3].head.fields.kind == ENTITY_POOL_READY) {
            current = entity;
            entity = entity->next;
            if (Entity_TryActivate(current)) {
                Entity_UnlinkLoading(current);
                Entity_AppendToActive(current);
            }
        } else {
            entity = entity->next;
        }
    }
}

/* Free entities that are out of range of the player. */
void Entity_DespawnOutOfRange(void) {
    Entity *entity = D_004467E0;
    while (entity != NULL) {
        EntitySpawnInfo *spawn_info;
        float distance;
        float height_difference;
        int out_of_range;
        Entity *despawned;

        distance = Entity_SpawnDistanceToPlayer(entity);
        entity->distance_to_player = distance;
        spawn_info = entity->spawn_info;
        height_difference = player_state[1] - spawn_info->spawn_y;
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
            EntityPool_RemoveUser(despawned->unkD3);
            Entity_ReleaseWorkArea(despawned);
            Entity_UnlinkActive(despawned);
            Entity_AppendToWaiting(despawned);
            Effect_FreeForOwner(despawned);
        } else {
            entity = entity->next;
        }
    }
}

/* Runs the entity pool's loads: steps the block being read or unpacked, ages the idle ones, and when
   nothing is busy starts reading the type file of the first queued block. */
static void EntityPool_StepLoads(void) {
    char path[0x100];
    EntityPoolSlot *slot;
    EntityPoolSlot *queued;
    int busy;
    int i;

    queued = NULL;
    busy = FALSE;
    for (i = 0, slot = entity_pool_slots; i < ENTITY_POOL_SLOT_COUNT; i++, slot++) {
        switch (slot->head.fields.kind) {
        case ENTITY_POOL_FREE:
            break;
        case ENTITY_POOL_QUEUED:
            if (queued == NULL) {
                queued = slot;
            }
            break;
        case ENTITY_POOL_READING:
            if (File_Sync(D_004467C4, 1) == 0) {
                if (((EntityController *)(D_004467C0 + slot->head.fields.unk2 * 0x450))->flags & CONTROLLER_FLAG_TWO_BLOCKS) {
                    slot[1].head.fields.kind = ENTITY_POOL_EXTENSION;
                    slot[1].head.fields.unk2 = 0xFFFF;
                }
                File_Close(D_004467C4);
                EntityPool_CopyLoadedFile(slot);
                EntityPool_BuildBlockModel(i);
                entity_type_file_header = *(EntityTypeFileHeader *)file_load_buffer;
                D_004467C4 = -1;
                if (entity_type_file_header.unk18 != 0 && entity_type_file_header.unk14 != 0) {
                    entity_type_file_header.unk18 += (int)file_load_buffer;
                    entity_type_file_header.unk14 += (int)file_load_buffer;
                    slot->head.fields.kind = ENTITY_POOL_UNPACKING;
                    sound_load_queue.unk1C = (int)&entity_type_file_header;
                    sound_load_queue.unk18 = 1;
                    sound_load_queue.unk20 = i;
                    sound_load_queue.unk24 = (int)slot;
                } else {
                    slot->head.fields.kind = ENTITY_POOL_READY;
                }
            }
            busy = TRUE;
            break;
        case ENTITY_POOL_UNPACKING:
            if (sound_load_queue.unk18 == 0) {
                slot->head.fields.kind = ENTITY_POOL_READY;
                File_Unlock();
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
    if (sound_load_queue.unk04 != 0) {
        return;
    }
    if (File_IsLocked() != 0) {
        return;
    }
    sprintf(path, D_0043E790, D_004467C8, ((EntityParams *)(D_004467C0 + queued->head.fields.unk2 * 0x450))->unk00_0);
    D_004467C4 = File_Open(path, 1, 1);
    if (D_004467C4 < 0) {
        EntityPool_AbandonLoad(queued);
        return;
    }
    queued->head.fields.kind = ENTITY_POOL_READING;
    File_Read(D_004467C4, file_load_buffer, -1);
    File_Lock();
    D_00447098 = queued;
}

/* Gives an entity whose pool block is ready its work area and model; FALSE if it has to wait. */
static int Entity_TryActivate(Entity *entity) {
    int data_address;
    int work_address;
    EntityPoolSlot *slot = &entity_pool_slots[entity->unkD3];
    EntityPoolData *data = (EntityPoolData *)slot->base_address;
    ModelInstance *model;
    int work_index;

    if (slot->head.fields.kind != ENTITY_POOL_READY) {
        return FALSE;
    }
    work_index = Entity_FindFreeWorkArea();
    if (work_index < 0) {
        return FALSE;
    }
    entity->unkD4 = work_index;
    work_address = Entity_ClaimWorkArea(work_index);
    data_address = slot->base_address + (int)data->model_offset;
    Model_SetUpSkinned(ENTITY_MODEL(entity), &data_address, &work_address);
    model = ENTITY_MODEL(entity);
    model->draw = Entity_PrepareModelDraw;
    model->unk1B0 = NULL;
    if (entity->spawn_info->type_id == 0x47) {
        model->unk100 |= 0x200;
        model->unk102 |= MODEL_FLAG_800;
        model->draw = Entity_PrepareScreenTexturedDraw;
        model->unk1B0 = func_00124AF8;
        D_004467E8 = entity->spawn_info->type_id;
        work_address = (int)entity_work_area;
        work_address = (int)Model_SaveGsWordsAndUvs(model, (s64 *)work_address);
    }
    data_address = slot->base_address + (int)data->motion_offset;
    ModelMotion_LoadFile(data_address, &work_address, ENTITY_MODEL(entity), 1);
    data_address = slot->base_address + data->unk0C;
    ENTITY_MODEL(entity)->unk194 = data_address;
    ENTITY_MODEL(entity)->unk190 = 0;
    ENTITY_MODEL(entity)->unk19C = (int)Map_GetSlotPacket(entity->group_id, 3);
    ENTITY_MODEL(entity)->unk1A0 = (int)Map_GetSlotPacket(entity->group_id, 5);
    entity->unkD2 = slot->unk7;
    EntityPool_AddUser(entity->unkD3);
    ((void (*)(Entity *))Entity_ResetToSpawn)(entity); // match: declared without its argument elsewhere
    return TRUE;
}

/* Releases the entity's work area and clears its pool and work-area indices. */
static void Entity_ReleaseWorkArea(Entity *entity) {
    Entity_FreeWorkArea(entity->unkD4);
    entity->unkD4 = entity->unkD3 = -1;
}

/* Returns the pool block index in use (kinds 1 to 4) for a type id, or -1. */
static int EntityPool_FindType(int type_id) {
    int i;
    EntityPoolSlot *slot;

    for (i = 0; i < ENTITY_POOL_SLOT_COUNT; i++) {
        slot = &entity_pool_slots[i];
        if ((u32)(slot->head.fields.kind - 1) < 4 && slot->head.fields.unk2 == type_id) {
            return i;
        }
    }
    return -1;
}

/* Claims the first free pool block, else the unused block with the lowest size; returns its index. */
static int EntityPool_ClaimBlock(void) {
    int i;
    int best;
    EntityPoolSlot *slot;

    best = -1;
    for (i = 0; i < 5; i++) {
        slot = &entity_pool_slots[i];
        if ((slot->head.word >> 9) & 1) { continue; }
        if ((slot->head.word >> 8) & 1) { continue; }
        if (slot->head.fields.kind == 0) { return i; }
        if (slot->head.fields.kind != 4) { continue; }
        if (slot->unk4 != 0) { continue; }
        if (slot->unk6 != 0) { continue; }
        if (best == -1 || (u32)slot->age < (u32)entity_pool_slots[best].age) {
            best = i;
        }
    }
    if (best >= 0) {
        entity_pool_slots[best].head.fields.kind = 0;
        if ((i = best + 1) < 5 && entity_pool_slots[i].head.fields.kind == 5) {
            entity_pool_slots[i].head.fields.kind = 0;
        }
    }
    return best;
}

/* Claims a free pair of pool blocks for a two-block type, else the first reusable pair; returns its first index or -1. */
static int EntityPool_ClaimBlockPair(void) {
    int i;
    int j;
    int pair;
    int best;

    best = -1;
    for (i = 0; i < 2; i++) {
        pair = i * 2;
        if (entity_pool_slots[pair].head.bits.flag_200) { continue; }
        if (entity_pool_slots[pair + 1].head.bits.flag_200) { continue; }
        if (entity_pool_slots[pair].head.bits.flag_100) { continue; }
        if (entity_pool_slots[pair + 1].head.bits.flag_100) { continue; }
        if (entity_pool_slots[pair].head.fields.kind == ENTITY_POOL_FREE && entity_pool_slots[pair + 1].head.fields.kind == ENTITY_POOL_FREE) {
            return pair;
        }
    }
    // match: a second counter, so the pair index gets the register retail uses
    for (j = 0; j < 2; j++) {
        pair = j * 2;
        if (entity_pool_slots[pair].head.bits.flag_200) { continue; }
        if (entity_pool_slots[pair + 1].head.bits.flag_200) { continue; }
        if (entity_pool_slots[pair].head.bits.flag_100) { continue; }
        if (entity_pool_slots[pair + 1].head.bits.flag_100) { continue; }
        if (entity_pool_slots[pair].head.fields.kind != ENTITY_POOL_FREE) {
            if (entity_pool_slots[pair].head.fields.kind != ENTITY_POOL_READY) { continue; }
            if (entity_pool_slots[pair].unk4 != 0) { continue; }
            if (entity_pool_slots[pair].unk6 != 0) { continue; }
        }
        if (entity_pool_slots[pair + 1].head.fields.kind == ENTITY_POOL_FREE || entity_pool_slots[pair + 1].head.fields.kind == ENTITY_POOL_EXTENSION) {
            best = pair;
            break;
        }
        if (entity_pool_slots[pair + 1].head.fields.kind != ENTITY_POOL_READY) { continue; }
        if (entity_pool_slots[pair + 1].unk4 != 0) { continue; }
        if (entity_pool_slots[pair + 1].unk6 != 0) { continue; }
        best = pair;
        break;
    }
    if (best >= 0) {
        entity_pool_slots[best].head.fields.kind = ENTITY_POOL_FREE;
        if ((j = best + 1) < 5 && entity_pool_slots[j].head.fields.kind == ENTITY_POOL_EXTENSION) {
            entity_pool_slots[j].head.fields.kind = ENTITY_POOL_FREE;
        }
    }
    return best;
}

/* Claims pool block `idx` for a type that always loads there: a free block or a ready one nobody uses is
   emptied and claimed; returns idx, or -1 when the block is reserved or still in use. */
static int EntityPool_ClaimFixedBlock(int idx) {
    EntityPoolSlot *slot = &entity_pool_slots[idx];

    if (slot->head.word & ENTITY_POOL_FLAG_200) {
        return -1;
    }
    if (slot->head.fields.kind == ENTITY_POOL_FREE) {
        EntityPool_EmptyBlock(idx);
        return idx;
    }
    // match: unk4 and unk6 are tested as one word
    if (slot->head.fields.kind == ENTITY_POOL_READY && !(*(int *)&slot->unk4 & 0xFFFFFF)) {
        EntityPool_EmptyBlock(idx);
        if (idx < 4 && slot[1].head.fields.kind == ENTITY_POOL_EXTENSION) {
            slot++;
            slot->head.fields.unk2 = 0xFFFF;
            slot->head.fields.kind = ENTITY_POOL_FREE;
            slot->unk4 = 0;
            slot->head.bits.flag_100 = 0;
        }
        return idx;
    }
    if (slot->head.fields.kind == ENTITY_POOL_EXTENSION && EntityPool_ClaimFixedBlock(idx - 1) != -1) {
        EntityPool_EmptyBlock(idx);
        return idx;
    }
    EntityPool_EmptyBlock(idx);
    return -1;
}

/* Claims a pair of pool blocks starting at this one for a two-block type; returns its index or -1. */
static int EntityPool_ClaimFixedBlockPair(int pool_size) {
    EntityPoolSlot *slot = &entity_pool_slots[pool_size];
    EntityPoolSlot *next = &entity_pool_slots[pool_size + 1];
    int first_free;
    int second_free;

    if (slot->head.word & ENTITY_POOL_FLAG_200) {
        return -1;
    }
    if (slot->head.fields.kind == ENTITY_POOL_FREE ||
        (slot->head.fields.kind == ENTITY_POOL_READY && slot->unk4 == 0 && slot->unk6 == 0)) {
        first_free = TRUE;
    } else {
        first_free = !EntityPool_HasFixedUser(pool_size);
    }
    if (!first_free) {
        return -1;
    }
    if (next->head.word & ENTITY_POOL_FLAG_200) {
        return -1;
    }
    // the game's own slip: this result is overwritten by the test right below (an `else` is missing),
    // so a free or ready second block is always checked with EntityPool_HasFixedUser. It leaves a dead test in the code.
    if (next->head.fields.kind == ENTITY_POOL_FREE || next->head.fields.kind == ENTITY_POOL_READY) {
        second_free = TRUE;
    }
    if (next->head.fields.kind == ENTITY_POOL_EXTENSION && first_free) {
        second_free = TRUE;
    } else {
        second_free = !EntityPool_HasFixedUser(pool_size + 1);
    }
    if (!second_free) {
        return -1;
    }
    EntityPool_EvictBlockUsers(pool_size);
    EntityPool_EvictBlockUsers(pool_size + 1);
    slot->head.fields.unk2 = 0xFFFF;
    slot->unk4 = 0;
    slot->head.bits.flag_100 = TRUE;
    next->head.fields.unk2 = 0xFFFF;
    next->unk4 = 0;
    next->head.bits.flag_100 = TRUE;
    return pool_size;
}

/* TRUE if a one-block entity (pool_size 0-4) uses this pool slot, or the block it extends. */
static int EntityPool_HasFixedUser(int slot_index) {
    Entity *entity;

    if (entity_pool_slots[slot_index].head.fields.kind == ENTITY_POOL_EXTENSION) {
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

/* Sends every loading or active entity that uses pool block `group_id` back to list A. */
static void EntityPool_EvictBlockUsers(int group_id) {
    Entity *entity;
    Entity *next;
    int other; /* never assigned: retail compares against callee-saved $19 */

    entity = D_004470B0;
    while (entity != NULL) {
        if (group_id == entity->unkD3 ||
            (ENTITY_CONTROLLER(entity)->flags & 0x8000 && entity->unkD3 == other && entity->unkD3 >= 0)) {
            next = entity->next;
            Entity_UnlinkLoading(entity);
            Entity_AppendToWaiting(entity);
            entity = next;
        } else {
            entity = entity->next;
        }
    }

    entity = D_004467E0;
    while (entity != NULL) {
        if (group_id == entity->unkD3) {
            Effect_FreeForOwner(entity);
            EntityPool_RemoveUser(entity->unkD3);
            Entity_ReleaseWorkArea(entity);
            next = entity->next;
            Entity_UnlinkActive(entity);
            Entity_AppendToWaiting(entity);
            entity = next;
        } else {
            entity = entity->next;
        }
    }
}

/* Empties a pool block: sends its users back to list A and marks it reserved; does nothing while an entity
   with a fixed block (pool_size 0-4) uses it. */
static void EntityPool_EmptyBlock(int slot_index) {
    Entity *entity;
    Entity *next;
    EntityPoolSlot *slot;

    slot = &entity_pool_slots[slot_index];
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
            Entity_UnlinkLoading(entity);
            Entity_AppendToWaiting(entity);
            entity = next;
        } else {
            entity = entity->next;
        }
    }

    entity = D_004467E0;
    while (entity != NULL) {
        if (slot_index == entity->unkD3) {
            Effect_FreeForOwner(entity);
            EntityPool_RemoveUser(entity->unkD3);
            Entity_ReleaseWorkArea(entity);
            next = entity->next;
            Entity_UnlinkActive(entity);
            Entity_AppendToWaiting(entity);
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
static void EntityPool_QueueLoad(int slot_index, int type_id, int two_blocks) {
    EntityPoolSlot *slot = &entity_pool_slots[slot_index];

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
static void EntityPool_AddUser(int slot_index) {
    entity_pool_slots[slot_index].head.bits.flag_100 = 0;
    entity_pool_slots[slot_index].unk4++;
    entity_pool_slots[slot_index].head.bits.flag_200 = 0;
    if (slot_index < 4 && entity_pool_slots[slot_index + 1].head.fields.kind == ENTITY_POOL_EXTENSION) {
        entity_pool_slots[slot_index + 1].head.bits.flag_100 = 0;
        entity_pool_slots[slot_index + 1].head.bits.flag_200 = 0;
    }
}

/* Drops one user of a pool block; when none are left, marks it released at the current frame. */
static void EntityPool_RemoveUser(int slot_index) {
    EntityPoolSlot *slot;

    slot = &entity_pool_slots[slot_index];
    if (--slot->unk4 <= 0) {
        slot->unk6 = 2;
        slot->age = D_00446ED4;
        slot->unk4 = 0;
    }
}

/* Copies the loaded type file into the slot's block; hangs if it is empty or too big for the block. */
static void EntityPool_CopyLoadedFile(EntityPoolSlot *slot) {
    EntityController *controller;
    int *file;
    int table_offset;
    int size;

    controller = (EntityController *)(D_004467C0 + slot->head.fields.unk2 * 0x450);
    file = (int *)file_load_buffer;
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
    memcpy((void *)slot->base_address, file_load_buffer, size);
    return;
hang:
    goto hang;
}

/* Gives up a block whose type file would not open: the entities of that type leave lists A and B, and the
   block is marked free. */
static void EntityPool_AbandonLoad(EntityPoolSlot *slot) {
    Entity *entity;

    entity = D_004470A8;
    while (entity != NULL) {
        if (entity->spawn_info->type_id == slot->head.fields.unk2) {
            entity = Entity_UnlinkWaiting(entity);
        } else {
            entity = entity->next;
        }
    }

    entity = D_004470B0;
    while (entity != NULL) {
        if (entity->spawn_info->type_id == slot->head.fields.unk2) {
            entity = Entity_UnlinkLoading(entity);
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
static void EntityPool_FreeUnusedBlocks(void) {
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
    slot = entity_pool_slots;
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
static int Entity_FindFreeWorkArea(void) {
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
static int Entity_ClaimWorkArea(int index) {
    int work_area;

    D_004470A0[index / 8] |= 1 << (index % 8);
    work_area = D_0044709C + index * 0x2200;
    memset((void *)work_area, 0, 0x2200);
    return work_area;
}

/* Marks work area `index` free and clears it. */
static void Entity_FreeWorkArea(int index) {
    u8 bit;
    int work_area;

    bit = 1 << (index % 8);
    D_004470A0[index / 8] &= ~bit;
    work_area = D_0044709C + index * 0x2200;
    memset((void *)work_area, 0, 0x2200);
}

/* Before an entity model is drawn (ModelInstance.draw): sets the GS Z buffer, test and alpha registers by the
   model's alpha, then sends the textures it needs. */
static void Entity_PrepareModelDraw(ModelInstance *model) {
    Packet_BeginGsRegisters();
    if (model->alpha >= 128.0f) {
        Packet_AddGsRegister(GS_REG_ZBUF_1, 0xA000118L);
        Packet_AddGsRegister(GS_REG_ZBUF_2, 0xA000118L);
        Packet_AddGsRegister(GS_REG_TEST_1, 0x5000D);
        Packet_AddGsRegister(GS_REG_TEST_2, 0x5000D);
    } else {
        Packet_AddGsRegister(GS_REG_ZBUF_1, 0x10A000118L);
        Packet_AddGsRegister(GS_REG_ZBUF_2, 0x10A000118L);
        Packet_AddGsRegister(GS_REG_TEST_1, 0x5000D);
        Packet_AddGsRegister(GS_REG_TEST_2, 0x30001);
    }
    Packet_AddGsRegister(GS_REG_ALPHA_1, 0x44);
    Packet_AddGsRegister(GS_REG_ALPHA_2, 9);
    Packet_EndGsRegisters();
    ModelTexture_SendForModel(model);
}

/* Sets Z writes off, an always-pass depth test and additive blending, then sends the model's textures.
   Nothing calls it. */
void Entity_PrepareAdditiveDraw(ModelInstance *model) {
    Packet_BeginGsRegisters();
    Packet_AddGsRegister(GS_REG_ZBUF_1, 0x10A000118L);
    Packet_AddGsRegister(GS_REG_TEST_1, 0x50000);
    Packet_AddGsRegister(GS_REG_ALPHA_1, 0x48);
    Packet_EndGsRegisters();
    ModelTexture_SendForModel(model);
}

/* Before a type 0x47 model is drawn: with MODEL_FLAG_800 it copies the frame into a 256x256 texture at 0x3540
   and maps it onto the model by screen position, otherwise it puts the model's own textures and texture
   coordinates back; then sets the GS registers by the model's alpha. */
static void Entity_PrepareScreenTexturedDraw(ModelInstance *model) {
    if (model->unk102 & MODEL_FLAG_800) {
        Model_SetBlockGsWords(model, 0x220013540L, 0x60, -1, -1);
        Model_SetUvsFromScreen(model);
        GsDraw_CopyFrameToTexture(0x3540, 0, 0x100, 0x100);
        GsDraw_ResetDrawTarget();
        GsDraw_QueueTexFlush();
    } else {
        ModelTexture_SendForModel(model);
        Model_RestoreGsWordsAndUvs(model);
    }
    Packet_BeginGsRegisters();

    if (128.0f <= model->alpha) {
        Packet_AddGsRegister(GS_REG_ZBUF_1, 0xA000118L);
        Packet_AddGsRegister(GS_REG_ZBUF_2, 0xA000118L);
        Packet_AddGsRegister(GS_REG_TEST_1, 0x5000DL);
        if (model->unk102 & MODEL_FLAG_800) {
            Packet_AddGsRegister(GS_REG_TEST_2, 0x30001L);
        } else {
            Packet_AddGsRegister(GS_REG_TEST_2, 0x5000DL);
        }
    } else {
        Packet_AddGsRegister(GS_REG_ZBUF_1, 0x10A000118L);
        Packet_AddGsRegister(GS_REG_ZBUF_2, 0x10A000118L);
        Packet_AddGsRegister(GS_REG_TEST_1, 0x5000DL);
        Packet_AddGsRegister(GS_REG_TEST_2, 0x30001L);
    }
    Packet_AddGsRegister(GS_REG_ALPHA_1, 0x44);
    Packet_AddGsRegister(GS_REG_ALPHA_2, 0x9);
    Packet_EndGsRegisters();
}

/* Empty. */
static void func_00124AF8(void) {
}

/* Clears a pool slot's model and work buffer, then builds the model from the slot's loaded data. */
static void EntityPool_BuildBlockModel(int slot_index) {
    char *model_data;
    char *buffer;
    ModelInstance *model = &entity_models[slot_index];
    // match: never used, but it leaves the table address and the scaled index in registers before the calls
    EntityPoolSlot *unused_slot = &entity_pool_slots[slot_index];
    EntityPoolData *data;

    buffer = entity_pool_slot_buffers[slot_index];
    memset(model, 0, sizeof(ModelInstance));
    memset(buffer, 0, sizeof(entity_pool_slot_buffers[slot_index]));
    data = (EntityPoolData *)entity_pool_slots[slot_index].base_address;
    if (data->pool_model_offset != 0) {
        model_data = (char *)data + data->pool_model_offset;
        Model_SetUpPosed(model, &model_data, &buffer);
        Model_SetVu1Program(model, 0x5A0);
        Model_SetPrimBits(model, 0x20, 0x20);
    }
}

/* Appends an entity to the tail of list A (waiting for a pool block). */
static void Entity_AppendToWaiting(Entity *entity) {
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

/* Unlinks an entity from list A and returns the next one. */
static Entity *Entity_UnlinkWaiting(Entity *entity) {
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

/* Appends an entity to the tail of list B (waiting for its pool block to load). */
static void Entity_AppendToLoading(Entity *entity) {
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

/* Unlinks an entity from list B and returns the next one. */
static Entity *Entity_UnlinkLoading(Entity *entity) {
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

/* Appends an entity to the tail of list C (active) and sets flag bit 0. */
static void Entity_AppendToActive(Entity *entity) {
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
static Entity *Entity_UnlinkActive(Entity *entity) {
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
    Entity_SetSpawnedFlags(entity);
    entity->flags &= ~1;
    D_004470C0--;
    return next;
}

/* Horizontal distance from the entity's spawn point to the player. */
float Entity_SpawnDistanceToPlayer(Entity *entity) {
    float delta_x;
    float delta_z;

    delta_x = entity->spawn_info->spawn_x - player_state[0];
    delta_z = entity->spawn_info->spawn_z - player_state[2];
    return sqrtf(delta_x * delta_x + delta_z * delta_z);
}
