#include "common.h"
#include "entity/entity_world.h"
#include "entity/entity.h"
#include "gfx/gs.h"
#include "model/model.h"
#include "sound/sound.h"
#include "effect/effect.h"

void *memset(void *, int, u32);
int sprintf(char *, char *, ...);

#define ENTITY_POOL_BLOCK_SIZE  0x80000

/* the game's own data and functions, outside this file */
extern Entity *active_entities_tail;        /* list C tail */
extern int entity_pool_area_size;           /* the bytes of the pool blocks and work areas */
extern char *entity_pool_area;              /* where they start */
extern int screen_textured_entity_type;     /* type of the last entity given the screen-textured draw, -1: none */
extern int map_spawn_counts[2];
extern u32 frame_counter;
extern float player_state[];
extern char file_load_buffer[0x80000];
extern SoundLoadQueue sound_load_queue;
extern char *entity_params;
extern int chr_file_slot;                   /* the file slot of the .chr being read, -1: none */
extern char chr_directory[];                /* "chr/" */
extern char chr_file_path_format[];         /* "%sc%4.4d.chr" */
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
static void func_00124AF8(ModelInstance *model);
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

/* Entity pool state. Lists A (waiting for a pool block), B (waiting for the block to load) and C (active) hold
   the entities; the pool blocks and work areas are below. */
static EntityPoolSlot *loading_pool_slot;   /* 0x447098 the block being read or unpacked */
static int entity_work_areas_base;          /* 0x44709C base of the work areas */
static u8 entity_work_areas_used[8];        /* 0x4470A0 bit per work area in use */
static Entity *waiting_entities_head;       /* 0x4470A8 list A head */
static Entity *waiting_entities_tail;       /* 0x4470AC list A tail */
static Entity *loading_entities_head;       /* 0x4470B0 list B head */
static Entity *loading_entities_tail;       /* 0x4470B4 list B tail */
static int waiting_entity_count;            /* 0x4470B8 list A count */
static int loading_entity_count;            /* 0x4470BC list B count */
static int active_entity_count;             /* 0x4470C0 list C count */

float sqrtf(float);

/* Clear the entity pool and empty every entity list. */
void Entity_ResetAllLists(void) {
    EntityPoolSlot *block;
    int block_address;
    int blocks_left;

    memset(entity_pool_area, 0, entity_pool_area_size);

    memset(entity_pool_slots, 0, sizeof(entity_pool_slots));
    waiting_entities_head = NULL;
    screen_textured_entity_type = -1;
    loading_entities_head = NULL;
    active_entities_head = NULL;
    waiting_entities_tail = NULL;
    loading_entities_tail = NULL;
    active_entities_tail = 0;
    memset(entity_work_areas_used, 0, 8);
    waiting_entity_count = 0;
    loading_entity_count = 0;
    active_entity_count = 0;
    memset(entity_models, 0, 0x8C0);
    block_address = (int)entity_pool_area;
    block = entity_pool_slots;
    blocks_left = ENTITY_POOL_SLOT_COUNT - 1;
    do {
        block->base_address = block_address;
        blocks_left -= 1;
        block->head.fields.kind = ENTITY_POOL_FREE;
        block->head.fields.unk2 = 0xFFFF;
        block_address += ENTITY_POOL_BLOCK_SIZE;
        block->entity_count = 0;
        block->reuse_delay = 0;
        block->age = 0;
        block++;
    } while (blocks_left >= 0);
    entity_work_areas_base = block_address;
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
    int group_count = map_spawn_counts[idx];
    for (i = 0; i < group_count; i++) {
        Entity_AppendToWaiting(group_entity);
        group_entity++;
    }
}

/* Free every entity in the group from all three lists. */
void Entity_FreeAllInGroup(int group_id) {
    Entity *entity;

    entity = waiting_entities_head;
    while (entity != NULL) {
        if (entity->group_id == group_id) {
            entity = Entity_UnlinkWaiting(entity);
        } else {
            entity = entity->next;
        }
    }

    entity = loading_entities_head;
    while (entity != NULL) {
        if (entity->group_id == group_id) {
            entity = Entity_UnlinkLoading(entity);
        } else {
            entity = entity->next;
        }
    }

    entity = active_entities_head;
    while (entity != NULL) {
        if (entity->group_id == group_id) {
            EntityPool_RemoveUser(entity->pool_slot_index);
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
    Entity *entity = waiting_entities_head;
    while (entity != NULL) {
        Entity *queued;
        float distance;
        float height_difference;

        distance = Entity_SpawnDistanceToPlayer(entity);
        entity->distance_to_player = distance;
        height_difference = ABS(player_state[1] - entity->spawn_info->spawn_y);
        if (entity->pool_slot_index >= 0 ||
            (distance <= (float)entity->spawn_info->despawn_range &&
             (entity->spawn_info->vertical_despawn_range == 0 ||
              height_difference < (float)entity->spawn_info->vertical_despawn_range))) {
            entity->pool_slot_index = EntityPool_FindType(entity->spawn_info->type_id);
            // match: the slot is tested as a signed byte, as unkD3 is stored
            if ((char)entity->pool_slot_index >= 0) {
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
                    entity->pool_slot_index = EntityPool_ClaimFixedBlockPair(entity->spawn_info->pool_size);
                    if (entity->pool_slot_index >= 0) {
                        entity_pool_slots[entity->pool_slot_index].head.word |= ENTITY_POOL_FLAG_200;
                        entity_pool_slots[entity->pool_slot_index + 1].head.word |= ENTITY_POOL_FLAG_200;
                    }
                } else {
                    entity->pool_slot_index = EntityPool_ClaimFixedBlock(entity->spawn_info->pool_size);
                    if (entity->pool_slot_index >= 0) {
                        entity_pool_slots[entity->pool_slot_index].head.word |= ENTITY_POOL_FLAG_200;
                    }
                }
            } else if (ENTITY_CONTROLLER(entity)->flags & CONTROLLER_FLAG_TWO_BLOCKS) {
                entity->pool_slot_index = EntityPool_ClaimBlockPair();
                if (entity->pool_slot_index >= 0) {
                    entity_pool_slots[entity->pool_slot_index].head.word |= ENTITY_POOL_FLAG_200;
                    entity_pool_slots[entity->pool_slot_index + 1].head.word |= ENTITY_POOL_FLAG_200;
                }
            } else {
                entity->pool_slot_index = EntityPool_ClaimBlock();
                if (entity->pool_slot_index >= 0) {
                    entity_pool_slots[entity->pool_slot_index].head.word |= ENTITY_POOL_FLAG_200;
                }
            }
            if (entity->pool_slot_index >= 0) {
                queued = entity;
                entity = entity->next;
                EntityPool_QueueLoad(queued->pool_slot_index, queued->spawn_info->type_id, ENTITY_CONTROLLER(queued)->flags >> 15);
                Entity_UnlinkWaiting(queued);
                Entity_AppendToLoading(queued);
                continue;
            }
        }
        entity = entity->next;
    }
}

/* Walks list B (loading_entities_head) and finishes each entity whose pool block is ready. */
static void Entity_ActivateLoaded(void) {
    Entity *entity;
    Entity *current;

    entity = loading_entities_head;
    while (entity != NULL) {
        if (entity_pool_slots[entity->pool_slot_index].head.fields.kind == ENTITY_POOL_READY) {
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
    Entity *entity = active_entities_head;
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
            EntityPool_RemoveUser(despawned->pool_slot_index);
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
            if (File_Sync(chr_file_slot, 1) == 0) {
                if (((EntityController *)(entity_params + slot->head.fields.unk2 * 0x450))->flags & CONTROLLER_FLAG_TWO_BLOCKS) {
                    slot[1].head.fields.kind = ENTITY_POOL_EXTENSION;
                    slot[1].head.fields.unk2 = 0xFFFF;
                }
                File_Close(chr_file_slot);
                EntityPool_CopyLoadedFile(slot);
                EntityPool_BuildBlockModel(i);
                entity_type_file_header = *(EntityTypeFileHeader *)file_load_buffer;
                chr_file_slot = -1;
                if (entity_type_file_header.unk18 != 0 && entity_type_file_header.unk14 != 0) {
                    entity_type_file_header.unk18 += (int)file_load_buffer;
                    entity_type_file_header.unk14 += (int)file_load_buffer;
                    slot->head.fields.kind = ENTITY_POOL_UNPACKING;
                    sound_load_queue.entity_bank_args = (int)&entity_type_file_header;
                    sound_load_queue.entity_bank_load_pending = 1;
                    sound_load_queue.entity_slot_index = i;
                    sound_load_queue.entity_slot_voice_out = (int)slot;
                } else {
                    slot->head.fields.kind = ENTITY_POOL_READY;
                }
            }
            busy = TRUE;
            break;
        case ENTITY_POOL_UNPACKING:
            if (sound_load_queue.entity_bank_load_pending == 0) {
                slot->head.fields.kind = ENTITY_POOL_READY;
                File_Unlock();
                loading_pool_slot = NULL;
            }
            busy = TRUE;
            break;
        case ENTITY_POOL_READY:
            if (slot->entity_count > 0) {
                break;
            }
        case ENTITY_POOL_6:
            if (slot->reuse_delay != 0) {
                slot->reuse_delay--;
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
    if (sound_load_queue.bgm_load_pending != 0) {
        return;
    }
    if (File_IsLocked() != 0) {
        return;
    }
    sprintf(path, chr_file_path_format, chr_directory, ((EntityParams *)(entity_params + queued->head.fields.unk2 * 0x450))->model_number);
    chr_file_slot = File_Open(path, 1, 1);
    if (chr_file_slot < 0) {
        EntityPool_AbandonLoad(queued);
        return;
    }
    queued->head.fields.kind = ENTITY_POOL_READING;
    File_Read(chr_file_slot, file_load_buffer, -1);
    File_Lock();
    loading_pool_slot = queued;
}

/* Gives an entity whose pool block is ready its work area and model; FALSE if it has to wait. */
static int Entity_TryActivate(Entity *entity) {
    int data_address;
    int work_address;
    EntityPoolSlot *slot = &entity_pool_slots[entity->pool_slot_index];
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
    entity->work_area_index = work_index;
    work_address = Entity_ClaimWorkArea(work_index);
    data_address = slot->base_address + (int)data->model_offset;
    Model_SetUpSkinned(ENTITY_MODEL(entity), &data_address, &work_address);
    model = ENTITY_MODEL(entity);
    model->draw = Entity_PrepareModelDraw;
    model->after_draw = NULL;
    if (entity->spawn_info->type_id == 0x47) {
        model->model_flags |= 0x200;
        model->draw_flags |= MODEL_FLAG_800;
        model->draw = Entity_PrepareScreenTexturedDraw;
        model->after_draw = func_00124AF8;
        screen_textured_entity_type = entity->spawn_info->type_id;
        work_address = (int)entity_work_area;
        work_address = (int)Model_SaveGsWordsAndUvs(model, (s64 *)work_address);
    }
    data_address = slot->base_address + (int)data->motion_offset;
    ModelMotion_LoadFile(data_address, &work_address, ENTITY_MODEL(entity), 1);
    data_address = slot->base_address + data->texture_data;
    ENTITY_MODEL(entity)->texture_data = data_address;
    ENTITY_MODEL(entity)->texture_slot = 0;
    ENTITY_MODEL(entity)->texture_packet_3 = (int)Map_GetSlotPacket(entity->group_id, 3);
    ENTITY_MODEL(entity)->texture_packet_5 = (int)Map_GetSlotPacket(entity->group_id, 5);
    entity->sound_bank = slot->sound_bank;
    EntityPool_AddUser(entity->pool_slot_index);
    ((void (*)(Entity *))Entity_ResetToSpawn)(entity); // match: declared without its argument elsewhere
    return TRUE;
}

/* Releases the entity's work area and clears its pool and work-area indices. */
static void Entity_ReleaseWorkArea(Entity *entity) {
    Entity_FreeWorkArea(entity->work_area_index);
    entity->work_area_index = entity->pool_slot_index = -1;
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

/* Claims the first free pool block, else the ready block without users that was released first (lowest
   age); blocks flagged 0x100 or 0x200 are passed over. Returns its index, or -1. */
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
        if (slot->entity_count != 0) { continue; }
        if (slot->reuse_delay != 0) { continue; }
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
            if (entity_pool_slots[pair].entity_count != 0) { continue; }
            if (entity_pool_slots[pair].reuse_delay != 0) { continue; }
        }
        if (entity_pool_slots[pair + 1].head.fields.kind == ENTITY_POOL_FREE || entity_pool_slots[pair + 1].head.fields.kind == ENTITY_POOL_EXTENSION) {
            best = pair;
            break;
        }
        if (entity_pool_slots[pair + 1].head.fields.kind != ENTITY_POOL_READY) { continue; }
        if (entity_pool_slots[pair + 1].entity_count != 0) { continue; }
        if (entity_pool_slots[pair + 1].reuse_delay != 0) { continue; }
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
    if (slot->head.fields.kind == ENTITY_POOL_READY && !(*(int *)&slot->entity_count & 0xFFFFFF)) {
        EntityPool_EmptyBlock(idx);
        if (idx < 4 && slot[1].head.fields.kind == ENTITY_POOL_EXTENSION) {
            slot++;
            slot->head.fields.unk2 = 0xFFFF;
            slot->head.fields.kind = ENTITY_POOL_FREE;
            slot->entity_count = 0;
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
        (slot->head.fields.kind == ENTITY_POOL_READY && slot->entity_count == 0 && slot->reuse_delay == 0)) {
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
    slot->entity_count = 0;
    slot->head.bits.flag_100 = TRUE;
    next->head.fields.unk2 = 0xFFFF;
    next->entity_count = 0;
    next->head.bits.flag_100 = TRUE;
    return pool_size;
}

/* TRUE if a one-block entity (pool_size 0-4) uses this pool slot, or the block it extends. */
static int EntityPool_HasFixedUser(int slot_index) {
    Entity *entity;

    if (entity_pool_slots[slot_index].head.fields.kind == ENTITY_POOL_EXTENSION) {
        slot_index--;
    }

    entity = loading_entities_head;
    while (entity != NULL) {
        if (slot_index == entity->pool_slot_index && entity->spawn_info->pool_size >= 0 && entity->spawn_info->pool_size < 5) {
            return TRUE;
        }
        entity = entity->next;
    }

    entity = active_entities_head;
    while (entity != NULL) {
        if (slot_index == entity->pool_slot_index && entity->spawn_info->pool_size >= 0 && entity->spawn_info->pool_size < 5) {
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

    entity = loading_entities_head;
    while (entity != NULL) {
        if (group_id == entity->pool_slot_index ||
            (ENTITY_CONTROLLER(entity)->flags & CONTROLLER_FLAG_TWO_BLOCKS && entity->pool_slot_index == other && entity->pool_slot_index >= 0)) {
            next = entity->next;
            Entity_UnlinkLoading(entity);
            Entity_AppendToWaiting(entity);
            entity = next;
        } else {
            entity = entity->next;
        }
    }

    entity = active_entities_head;
    while (entity != NULL) {
        if (group_id == entity->pool_slot_index) {
            Effect_FreeForOwner(entity);
            EntityPool_RemoveUser(entity->pool_slot_index);
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
    entity = loading_entities_head;
    while (entity != NULL) {
        if (slot_index == entity->pool_slot_index) {
            // match: nested ifs keep the signed pool_size compares
            if (entity->spawn_info->pool_size >= 0) {
                if (entity->spawn_info->pool_size < 5) {
                    return;
                }
            }
        }
        entity = entity->next;
    }

    entity = active_entities_head;
    while (entity != NULL) {
        if (slot_index == entity->pool_slot_index) {
            // match: nested ifs keep the signed pool_size compares
            if (entity->spawn_info->pool_size >= 0) {
                if (entity->spawn_info->pool_size < 5) {
                    return;
                }
            }
        }
        entity = entity->next;
    }

    entity = loading_entities_head;
    while (entity != NULL) {
        if (slot_index == entity->pool_slot_index) {
            next = entity->next;
            Entity_UnlinkLoading(entity);
            Entity_AppendToWaiting(entity);
            entity = next;
        } else {
            entity = entity->next;
        }
    }

    entity = active_entities_head;
    while (entity != NULL) {
        if (slot_index == entity->pool_slot_index) {
            Effect_FreeForOwner(entity);
            EntityPool_RemoveUser(entity->pool_slot_index);
            Entity_ReleaseWorkArea(entity);
            next = entity->next;
            Entity_UnlinkActive(entity);
            Entity_AppendToWaiting(entity);
            entity = next;
        } else {
            entity = entity->next;
        }
    }

    slot->entity_count = 0;
    slot->head.fields.unk2 = 0xFFFF;
    slot->head.word |= ENTITY_POOL_FLAG_100;
}

/* Marks a pool slot QUEUED for a type id; a two-block type also marks the next slot as its extension. */
static void EntityPool_QueueLoad(int slot_index, int type_id, int two_blocks) {
    EntityPoolSlot *slot = &entity_pool_slots[slot_index];

    slot->head.fields.kind = ENTITY_POOL_QUEUED;
    slot->head.fields.unk2 = type_id;
    slot->entity_count = 0;
    slot->reuse_delay = 0;
    slot->age = 0;
    if (two_blocks) {
        slot++;
        slot->head.fields.kind = ENTITY_POOL_EXTENSION;
        slot->head.fields.unk2 = 0xFFFF;
        slot->entity_count = 0;
        slot->reuse_delay = 0;
        slot->age = 0;
    }
}

/* Adds a user to a pool block and clears its flags; a two-block type's second block gets the same flag clear. */
static void EntityPool_AddUser(int slot_index) {
    entity_pool_slots[slot_index].head.bits.flag_100 = 0;
    entity_pool_slots[slot_index].entity_count++;
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
    if (--slot->entity_count <= 0) {
        slot->reuse_delay = 2;
        slot->age = frame_counter;
        slot->entity_count = 0;
    }
}

/* Copies the loaded type file into the slot's block; hangs if it is empty or too big for the block. */
static void EntityPool_CopyLoadedFile(EntityPoolSlot *slot) {
    EntityController *controller;
    int *file;
    int table_offset;
    int size;

    controller = (EntityController *)(entity_params + slot->head.fields.unk2 * 0x450);
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

    entity = waiting_entities_head;
    while (entity != NULL) {
        if (entity->spawn_info->type_id == slot->head.fields.unk2) {
            entity = Entity_UnlinkWaiting(entity);
        } else {
            entity = entity->next;
        }
    }

    entity = loading_entities_head;
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

    list_a = waiting_entities_head;
    list_b = loading_entities_head;
    list_c = active_entities_head;
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
    u8 *flags = entity_work_areas_used;
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

    entity_work_areas_used[index / 8] |= 1 << (index % 8);
    work_area = entity_work_areas_base + index * 0x2200;
    memset((void *)work_area, 0, 0x2200);
    return work_area;
}

/* Marks work area `index` free and clears it. */
static void Entity_FreeWorkArea(int index) {
    u8 bit;
    int work_area;

    bit = 1 << (index % 8);
    entity_work_areas_used[index / 8] &= ~bit;
    work_area = entity_work_areas_base + index * 0x2200;
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

/* Sets Z writes off, the Z test GEQUAL (no alpha test) and additive blending, then sends the model's textures.
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
    if (model->draw_flags & MODEL_FLAG_800) {
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
        if (model->draw_flags & MODEL_FLAG_800) {
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
static void func_00124AF8(ModelInstance *model) {
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

    if (waiting_entities_head != NULL) {
        tail = waiting_entities_tail;
        waiting_entities_tail = entity;
        tail->next = entity;
        entity->prev = tail;
        entity->next = NULL;
    } else {
        waiting_entities_head = entity;
        waiting_entities_tail = entity;
        entity->prev = NULL;
        entity->next = NULL;
    }
    waiting_entity_count++;
}

/* Unlinks an entity from list A and returns the next one. */
static Entity *Entity_UnlinkWaiting(Entity *entity) {
    Entity *next;

    next = entity->next;
    if (waiting_entities_head == entity) {
        waiting_entities_head = next;
    }
    if (waiting_entities_tail == entity) {
        waiting_entities_tail = entity->prev;
    }
    if (entity->prev != NULL) {
        entity->prev->next = next;
    }
    if (entity->next != NULL) {
        entity->next->prev = entity->prev;
    }
    waiting_entity_count--;
    return next;
}

/* Appends an entity to the tail of list B (waiting for its pool block to load). */
static void Entity_AppendToLoading(Entity *entity) {
    Entity *tail;

    if (loading_entities_head != NULL) {
        tail = loading_entities_tail;
        loading_entities_tail = entity;
        tail->next = entity;
        entity->prev = tail;
        entity->next = NULL;
    } else {
        loading_entities_head = entity;
        loading_entities_tail = entity;
        entity->prev = NULL;
        entity->next = NULL;
    }
    loading_entity_count++;
}

/* Unlinks an entity from list B and returns the next one. */
static Entity *Entity_UnlinkLoading(Entity *entity) {
    Entity *next;

    next = entity->next;
    if (loading_entities_head == entity) {
        loading_entities_head = next;
    }
    if (loading_entities_tail == entity) {
        loading_entities_tail = entity->prev;
    }
    if (entity->prev != 0) {
        entity->prev->next = next;
    }
    if (entity->next != 0) {
        entity->next->prev = entity->prev;
    }
    loading_entity_count--;
    return next;
}

/* Appends an entity to the tail of list C (active) and sets flag bit 0. */
static void Entity_AppendToActive(Entity *entity) {
    Entity *tail;

    if (active_entities_head != NULL) {
        tail = active_entities_tail;
        active_entities_tail = entity;
        tail->next = entity;
        entity->prev = tail;
        entity->next = NULL;
    } else {
        active_entities_head = entity;
        active_entities_tail = entity;
        entity->prev = NULL;
        entity->next = NULL;
    }
    ENTITY_FLAG_BITS(entity)->bit0 = 1;
    active_entity_count++;
}

/* Unlinks an entity from list C and returns the next one. */
static Entity *Entity_UnlinkActive(Entity *entity) {
    Entity *next;

    next = entity->next;
    if (active_entities_head == entity) {
        active_entities_head = next;
    }
    if (active_entities_tail == entity) {
        active_entities_tail = entity->prev;
    }
    if (entity->prev != NULL) {
        entity->prev->next = next;
    }
    if (entity->next != NULL) {
        entity->next->prev = entity->prev;
    }
    Entity_SetSpawnedFlags(entity);
    entity->flags &= ~ENTITY_FLAG_ACTIVE_LIST;
    active_entity_count--;
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
