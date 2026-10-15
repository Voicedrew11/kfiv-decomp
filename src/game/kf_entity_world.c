#include "kf_entity_world.h"
#include "kf_collision.h"
#include "kf_linked_point.h"
#include "kf_gs.h"
#include "kf_map.h"

void *memset(void *, int, u32);
int rand(void);

extern MapSlot D_0038CD20[];
extern MapSlotMemory D_004321F0[2];
extern Entity D_003BEDF0[][0x80];
extern int D_004467D8[2];
extern EntitySpawnInfo *D_004467D0[2];
extern char *D_004467C0;
extern Vector4Arg D_0043E7C0;
extern s32 D_003E6E40[][2];
extern int D_0038CC80[3];
extern Entity *D_004467E0;
extern PlayerData D_00413F80;
extern MapDrawState D_0038A1B0;
extern f32 D_00431AF0[16];
extern u32 D_00446ED4;
extern int D_00446770;
extern u8 D_004141D0[];
extern const f32 D_004467F8[];

void Vector_Copy(void *, void *);
void Vector_Scale(float *, float *, float);
void Vector_MultiplyComponents(float *, float *, float *);
int Motion_Start(void *, int, int, u32, int);
void Vector_Add(void *, void *, void *);
void func_0010C9E8(ModelInstance *);
s32 EntityAi_ShouldRespawn(Entity *);
void EntityEvent_Respawn(char *);
s32 EntityAi_RemoveIfFallen(Entity *);
f32 EntityMath_Distance(f32 *, f32 *);
float EntityMath_DistanceXZ(void *, void *);
float EntityMath_HeadingTo(float *, float *);
void ModelMatrix_Build(ModelInstance *, Vector4 *, Vector4 *, f32 *);
float EntityMath_WrapAngle(float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0ApplyMatrix(float *, float *, float *);
void func_00109A50(void);
void func_00109A80(int, s64);
void func_00109AD0(void);
void func_0010B018(ModelInstance *, s32, s32);
void func_0010AEB8(void *);
void func_0010BA30(ModelInstance *);
void func_00133708(Entity *, s32);
void ModelMatrix_BuildPartLocal(ModelInstance *, ModelPart *, ModelPart *);
void func_0010B5D0(ModelPart *);
f32 EntityCollision_DistanceToPropFloor(void *, void *);

#define SCRATCH_MATRIX ((f32 *)0x70000000)
#define SCRATCH_VECTOR ((f32 *)0x70000040)
#define MAP_RECORD(entity) ((MapRecord *)(entity)->unk14)
#define SPAWN_FLAGS(spawn_info) ((EntitySpawnFlags *)&(spawn_info)->unk38)

static Entity *entity_by_spawn_id[2][0x100]; /* 0x4FEB90 entity lookup by spawn id byte per map slot */

static void Entity_RebuildModelMatrices(Entity *entity);
static int Entity_IsMapRecordActive(Entity *entity);
static MapRecordBounds *Entity_FindMapRecordUnder(Entity *entity);
static inline void entity_draw_set_model_tint(Entity *entity, f32 red, f32 green, f32 blue);

/* Sets up the entities of a map from its spawn records, filling in defaults for values left at 0. */
void Entity_InitFromMapSpawns(int map) {
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
    memset(entity_by_spawn_id[map], 0, sizeof(entity_by_spawn_id[map]));
    count = D_004467D8[map];
    for (i = 0; i < count; i++, entity++, spawn_info++, model++) {
        entity->group_id = map;
        entity->model = (EntityModel *)model;
        entity->params = (EntityParams *)(D_004467C0 + spawn_info->type_id * sizeof(EntityParams));
        entity->unkD3 = -1;
        entity->spawn_info = spawn_info;
        entity_by_spawn_id[map][SPAWN_FLAGS(spawn_info)->bytes[4]] = entity;
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

/* Clears flag bit 4 of every entity in one of the two entity arrays. */
void Entity_ClearFlag10InSlot(int array_index) {
    Entity *entity;
    int i;

    entity = D_003BEDF0[array_index];
    for (i = 0; i < D_004467D8[array_index]; i++, entity++) {
        entity->flags &= ~0x10;
    }
}

/* Puts an entity back to how it spawns: flags, counters, position, rotation and its sizes scaled. */
void Entity_ResetToSpawn(Entity *entity) {
    Vector4Arg scale = D_0043E7C0;
    f32 widest;

    // match: read first; after the stores below, the scheduler would hold this load back behind them
    entity->unk2C = entity->params->unk0A;
    ENTITY_FLAG_BITS(entity)->bit1 = FALSE;
    ENTITY_FLAG_BITS(entity)->bits2 = 0;
    ENTITY_FLAG_BITS(entity)->bit14 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit15 = FALSE;
    ENTITY_FLAG_BITS(entity)->pad8 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit19 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit20 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit21 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit22 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit23 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit24 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit25 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit26 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit27 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit30 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit10 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit17 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit31 = FALSE;
    entity->unk50 = 0;
    entity->handler = NULL;
    entity->prev_handler = NULL;
    entity->unk82 = 0;
    entity->unk80 = 0;
    entity->unkA3 = 0;
    entity->unk9E = 0;
    entity->unkA2 = 0;
    entity->unk20 = 0;
    entity->unk24 = 0;
    entity->unk28 = 0;
    entity->unk2A = 0;
    entity->action_step = 0;
    entity->unkCB = 0;
    entity->state = 0;
    entity->unkCD = 0;
    entity->prev_state = 0;
    entity->unkCF = 0;
    ((EntityFlags *)&entity->flags)->halves[1] = 0;
    entity->unkC6 = 0;
    entity->unkC8 = 0;
    entity->unkA0 = 0;
    entity->unk48 = 10.0f;
    Vector_Copy(entity->model->position, entity->spawn_info);
    Vector_Copy(entity->model->unkD0, &entity->spawn_info->unk10);
    Vector_Scale(entity->model->unkE0, &scale.x, (f32)entity->params->unkD2 / 100.0f);
    Vector_Copy(&scale, entity->spawn_info->unk20);
    if (!(entity->params->flags & 0x240)) {
        Vector_Scale(&scale.x, &scale.x, (f32)(rand() % 11 + 95) / 100.0f);
    }
    Vector_MultiplyComponents(entity->model->unkE0, entity->model->unkE0, &scale.x);
    widest = (scale.z < scale.x) ? scale.x : scale.z;
    entity->unk30 = (f32)entity->params->unk06 * widest;
    entity->unk34 = (f32)entity->params->unk08 * scale.y;
    entity->radius = (f32)entity->params->unkCC * widest;
    entity->height = (f32)entity->params->unkEE * scale.y;
    entity->unk44 = (f32)entity->params->unk04 * widest;
    entity->unk40 = (f32)entity->params->unk0C * scale.x * scale.y * scale.z;
    entity->unk40 = ABS(entity->unk40);
    entity->model->unk13C = (entity->unk30 < entity->unk34) ? entity->unk34 : entity->unk30;
    Motion_Start(entity, 0, 0, 0, 1);
    ((void (*)(Entity *))D_003E6E40[entity->params->unk02][0])(entity);
    Entity_RebuildModelMatrices(entity);
}

/* Empties one of the two entity arrays and makes the other array's entities forget the map
   records (Entity.unk14) that belonged to its map. */
void Entity_ClearMapSlot(int array_index) {
    Entity *entity;
    int i;

    D_004467D8[array_index] = 0;
    memset(D_003BEDF0[array_index], 0, sizeof(D_003BEDF0[array_index]));
    for (i = 0; i < 0x80; i++) {
        entity = &D_003BEDF0[array_index ^ 1][i];
        if (entity->unk14 != NULL && ((u8)(((MapRecord *)entity->unk14)->unk38 >> 27) & 1) == array_index) {
            entity->unk14 = NULL;
        }
    }
}

void Entity_SetSpawnedFlags(Entity *entity) {
    entity->flags = (entity->flags | 0xC) & ~0x100;
}

/* Moves every entity of one of the two entity arrays by an offset. */
void Entity_OffsetAllInSlot(int array_index, f32 *offset) {
    Entity *entity;
    int i;
    int count;

    entity = D_003BEDF0[array_index];
    count = D_004467D8[array_index];
    for (i = 0; i < count; i++, entity++) {
        Vector_Add(entity->model->position, entity->model->position, offset);
        Vector_Add(entity->unk70, entity->unk70, offset);
        Vector_Add(entity->spawn_info, entity->spawn_info, offset);
    }
}

/* The per-frame update of every live entity: respawn, the countdowns, where the player is, the push it is
   under, its own update function, and the map blocks its model is drawn with. */
void Entity_UpdateAll(void) {
    Entity *entity;
    EntitySpawnInfo *spawn_info;

    if (D_0038CC80[0] & 4) {
        return;
    }
    for (entity = D_004467E0; entity != NULL; entity = entity->next) {
        func_0010C9E8(ENTITY_MODEL(entity));
        if (ENTITY_FLAG_BITS(entity)->bits2 >= 3 || !ENTITY_FLAG_BITS(entity)->bit1) {
            if (!EntityAi_ShouldRespawn(entity)) {
                continue;
            }
            EntityEvent_Respawn((char *)entity);
        }
        if (EntityAi_RemoveIfFallen(entity)) {
            continue;
        }
        if (entity->unk82 != 0) {
            entity->unk82--;
            continue;
        }
        if (ENTITY_FLAG_BITS(entity)->bit16) {
            entity->unk20 = 0;
            entity->unk24 = 0;
            ENTITY_FLAG_BITS(entity)->pad40 = FALSE;
            entity->unk28 = 0;
            entity->unk2A = 0;
        }
        entity->unk60 = ((f32 (*)(f32 *, f32 *))EntityMath_Distance)(entity->model->position, D_00413F80.position); // match: returns the distance (other files declare it void)
        entity->distance_to_player = EntityMath_DistanceXZ(entity->model->position, D_00413F80.position);
        entity->unk68 = EntityMath_HeadingTo(entity->model->position, D_00413F80.position);
        entity->unk84 = entity->unk88 = entity->unk8C = entity->unk90 = entity->unk94 = entity->unk98 = 0.0f;
        if (!Entity_IsMapRecordActive(entity)) {
            if (!ENTITY_FLAG_BITS(entity)->bit30) {
                ModelMatrix_Build(ENTITY_MODEL(entity), &ENTITY_MODEL(entity)->position, &ENTITY_MODEL(entity)->rotation, &ENTITY_MODEL(entity)->unkE0);
            }
            ENTITY_FLAG_BITS(entity)->bit30 = TRUE;
            continue;
        }
        ENTITY_FLAG_BITS(entity)->bit30 = FALSE;
        if (entity->unk80 != 0) {
            entity->unk80--;
        }
        if (entity->unkA3 != 0) {
            entity->unkA3--;
        }
        if (entity->unk24 != 0) {
            entity->unk24--;
        }
        if (entity->unk28 != 0) {
            entity->unk28--;
        }
        if (entity->unk2A != 0) {
            entity->unk2A--;
        }
        if (entity->unk20 != 0) {
            if (entity->unk20 % 30 == 0) {
                entity->unk2C = (entity->unk2C < 3) ? 0 : entity->unk2C - 3;
                entity->flags &= ~(3L << 38);
            }
            entity->unk20--;
        }
        if (ENTITY_FLAG_BITS(entity)->bits36 != 0) {
            if (entity->params->unk0A < entity->unk2E) {
                entity->unk2E = entity->params->unk0A;
            }
            if (ENTITY_FLAG_BITS(entity)->pad40) {
                entity->unk4C = (f32)entity->unk2E * 75.0f / (f32)entity->params->unk0A;
                entity->unkA3 = 6;
            } else {
                entity->unk4C = (f32)entity->unk2E * 25.0f / (f32)entity->params->unk0A;
                entity->unkA3 = 3;
            }
        }
        if (entity->params->flags & 0x1000) {
            entity->unk4C = 0.0f;
        }
        if (entity->unk4C > 0.0f) {
            f32 forward = 1.0f; // match: declared ahead of the pointer, or the pointer is hoisted out of the loop
            f32 *push = SCRATCH_VECTOR;

            push[0] = 0.0f;
            push[1] = 0.0f;
            push[2] = forward;
            push[3] = 0.0f;
            sceVu0RotMatrixY(SCRATCH_MATRIX, D_00431AF0, EntityMath_WrapAngle(entity->unk6C));
            sceVu0ApplyMatrix(SCRATCH_VECTOR, SCRATCH_MATRIX, SCRATCH_VECTOR);
            Vector_Scale(SCRATCH_VECTOR, SCRATCH_VECTOR, entity->unk4C);
            Vector_Add(entity->model->position, entity->model->position, SCRATCH_VECTOR);
        }
        if (entity->unk50 == 0) {
            continue;
        }
        ((void (*)(Entity *))entity->unk50)(entity);
        if (entity->unk4C > 0.0f) {
            entity->unk4C -= entity->unk40 * 4.0f / 10.0f;
        }
        Entity_RebuildModelMatrices(entity);
        if (entity->unk14 == NULL) {
            entity->unk14 = Entity_FindMapRecordUnder(entity);
        }
        spawn_info = entity->spawn_info;
        if ((s8)SPAWN_FLAGS(spawn_info)->bytes[2] != -1 || entity->unk14 == NULL) {
            ENTITY_MODEL(entity)->unk14C = &D_0038A1B0.unk290[entity->group_id][(s8)SPAWN_FLAGS(spawn_info)->bytes[2]];
        } else {
            ENTITY_MODEL(entity)->unk14C = &D_0038A1B0.unk290[(u8)(MAP_RECORD(entity)->unk38 >> 27) & 1][MAP_RECORD(entity)->unk36];
        }
        if ((s8)SPAWN_FLAGS(spawn_info)->bytes[3] != -1 || entity->unk14 == NULL) {
            ENTITY_MODEL(entity)->unk148 = (s32)&D_0038A1B0.unkA90[entity->group_id][(s8)SPAWN_FLAGS(spawn_info)->bytes[3]];
        } else {
            ENTITY_MODEL(entity)->unk148 = (s32)&D_0038A1B0.unkA90[(u8)(MAP_RECORD(entity)->unk38 >> 27) & 1][MAP_RECORD(entity)->unk37];
        }
        func_0010C9E8(ENTITY_MODEL(entity));
    }
}

/* Sets the colour the entity's model is drawn in, and its alpha from the entity's own. */
static inline void entity_draw_set_model_tint(Entity *entity, f32 red, f32 green, f32 blue) {
    ModelInstance *model = ENTITY_MODEL(entity);

    model->unk150 = red;
    model->unk154 = green;
    model->unk158 = blue;
    model->alpha = (f32)entity->unkD6;
}

/* Draws the entities of both maps, pool block by pool block: red while one is hit, pulsing in the colour of
   the timer that is running (unk20, unk24, unk28, unk2A), plain otherwise. */
void Entity_DrawAll(int list) {
    Entity *entity;
    s32 map;
    s32 block;
    s32 period;
    s32 half;
    s32 phase;
    s32 falling;

    period = 40;
    func_00109A50();
    func_00109A80(GS_REG_ALPHA_1, 0x44);
    func_00109A80(GS_REG_ALPHA_2, 9);
    func_00109A80(GS_REG_TEST_1, 0x5000D);
    func_00109A80(GS_REG_TEST_2, 0x5000D);
    func_00109AD0();
    for (map = 0; map < 2; map++) {
        for (block = 0; block < 5; block++) {
            for (entity = D_004467E0; entity != NULL; entity = entity->next) {
                if (!ENTITY_FLAG_BITS(entity)->bit1) {
                    continue;
                }
                if (ENTITY_FLAG_BITS(entity)->bits2 >= 3) {
                    continue;
                }
                if (entity->group_id != map) {
                    continue;
                }
                if (entity->unkD3 != block) {
                    continue;
                }
                if (entity->unk14 == NULL) {
                    continue;
                }
                if (ENTITY_FLAG_BITS(entity)->bit30) {
                    continue;
                }
                if (entity->unkA3 != 0 && entity->unk82 == 0) {
                    ENTITY_MODEL(entity)->unk150 = 255.0f;
                    ENTITY_MODEL(entity)->unk154 = 64.0f;
                    ENTITY_MODEL(entity)->unk158 = 64.0f;
                    ENTITY_MODEL(entity)->alpha = (f32)entity->unkD6;
                } else if (ENTITY_FLAG_BITS(entity)->bits2 < 2) {
                    half = 20;
                    phase = D_00446ED4 % period;
                    falling = period - phase;
                    if (phase >= half) {
                        phase = falling;
                    }
                    if (entity->unk20 != 0 && entity->unk26 == 1) {
                        entity_draw_set_model_tint(entity, 128.0f - phase * 64.0f / half, phase * 127.0f / half + 128.0f, 128.0f - phase * 64.0f / half);
                    } else if (entity->unk24 != 0 && entity->unk26 == 2) {
                        entity_draw_set_model_tint(entity, phase * 127.0f / half + 128.0f, 128.0f - phase * 64.0f / half, phase * 127.0f / half + 128.0f);
                    } else if (entity->unk28 != 0 && entity->unk26 == 3) {
                        entity_draw_set_model_tint(entity, phase * 127.0f / half + 128.0f, phase * 127.0f / half + 128.0f, 128.0f - phase * 64.0f / half);
                    } else if (entity->unk2A != 0 && entity->unk26 == 4) {
                        entity_draw_set_model_tint(entity, 128.0f - phase * 64.0f / half, phase * 127.0f / half + 128.0f, phase * 127.0f / half + 128.0f);
                    } else if (entity->unk20 != 0) {
                        entity_draw_set_model_tint(entity, 128.0f - phase * 64.0f / half, phase * 127.0f / half + 128.0f, 128.0f - phase * 64.0f / half);
                    } else if (entity->unk24 != 0) {
                        entity_draw_set_model_tint(entity, phase * 127.0f / half + 128.0f, 128.0f - phase * 64.0f / half, phase * 127.0f / half + 128.0f);
                    } else if (entity->unk28 != 0) {
                        entity_draw_set_model_tint(entity, phase * 127.0f / half + 128.0f, phase * 127.0f / half + 128.0f, 128.0f - phase * 64.0f / half);
                    } else if (entity->unk2A != 0) {
                        entity_draw_set_model_tint(entity, 128.0f - phase * 64.0f / half, phase * 127.0f / half + 128.0f, phase * 127.0f / half + 128.0f);
                    } else {
                        entity_draw_set_model_tint(entity, 128.0f, 128.0f, 128.0f);
                    }
                }
                LinkedPoint_FindHitsForModel(ENTITY_MODEL(entity));
                if (ENTITY_MODEL(entity)->alpha < 128.0f || (ENTITY_MODEL(entity)->unk100 & 0x200) || entity->spawn_info->type_id == 0x10) {
                    func_0010B018(ENTITY_MODEL(entity), 0, list);
                } else {
                    func_0010AEB8(ENTITY_MODEL(entity));
                    func_0010BA30(ENTITY_MODEL(entity));
                }
                if (entity->spawn_info->type_id == 0xF) {
                    func_00133708(entity, list);
                }
            }
        }
    }
}

/* Rebuilds the entity's model matrices with the root part's own offset and rotation left out. */
static void Entity_RebuildModelMatrices(Entity *entity) {
    f32 saved_offset[4];
    f32 saved_rotation[4];
    ModelPart *root;
    ModelPart *part;
    int part_count;
    int i;

    root = ENTITY_MODEL(entity)->unk130;
    part_count = ENTITY_MODEL(entity)->part_count;
    ModelMatrix_Build(ENTITY_MODEL(entity), &ENTITY_MODEL(entity)->position, &ENTITY_MODEL(entity)->rotation, &ENTITY_MODEL(entity)->unkE0);
    Vector_Copy(saved_offset, root->unk40);
    Vector_Copy(saved_rotation, &root->unk50);
    root->unk40[0] = root->unk40[1] = root->unk40[2] = 0.0f;
    root->unk50 = root->unk54 = root->unk58 = 0.0f;
    part = ENTITY_MODEL(entity)->parts;
    for (i = 0; i < part_count; i++) {
        ModelMatrix_BuildPartLocal(ENTITY_MODEL(entity), part, part);
        part++;
    }
    Vector_Copy(root->unk40, saved_offset);
    Vector_Copy(&root->unk50, saved_rotation);
    part = ENTITY_MODEL(entity)->parts;
    for (i = 0; i < part_count; i++) {
        func_0010B5D0(part);
        part++;
    }
}

/* Whether the map record the entity belongs to (Entity.unk14) is switched on: FALSE when the record
   is flagged off or its map flag is clear. The record's map flag is a bit of the current map's
   flag block; for a record of the other map it is looked up among the 16 flags the block keeps
   for other maps. TRUE for an entity without a record and for types 0x10, 0x2F, 0x53 and 0x9E. */
static int Entity_IsMapRecordActive(Entity *entity) {
    MapRecordBounds *record;
    MapFlagBlock *block;
    MapSlot *slot;
    u8 *bits;
    int found;
    int index; // match: the map slot of the record, then reused as the counter of the last loop
    int record_index;
    int result;
    u8 tag;

    if (entity->unk14 == NULL) {
        return TRUE;
    }
    if (entity->spawn_info->type_id == 0x10 || entity->spawn_info->type_id == 0x2F ||
        entity->spawn_info->type_id == 0x53 || entity->spawn_info->type_id == 0x9E) {
        return TRUE;
    }
    found = FALSE;
    for (index = 0; index < 2; index++) {
        record = (MapRecordBounds *)D_0038CD20[index].records;
        for (record_index = 0; record_index < D_0038CD20[index].header->record_count; record_index++, record++) {
            if (record == entity->unk14) {
                found = TRUE;
                break;
            }
        }
        if (found) {
            break;
        }
    }
    if (!found) {
        return TRUE;
    }
    if (record->unk3C & 0x40000000) {
        return FALSE;
    }
    if (index == D_00446770) {
        slot = &D_0038CD20[index];
        bits = slot->flag_blocks[D_004141D0[0]].bits;
        if (slot->unk00 == 0) {
            return FALSE;
        }
        return (bits[record->unk38 >> 3] >> (record->unk38 & 7)) & 1;
    }
    slot = &D_0038CD20[D_00446770];
    block = &slot->flag_blocks[D_004141D0[0]];
    slot = &D_0038CD20[D_00446770 ^ 1];
    if (slot->unk00 == 0) {
        return FALSE;
    }
    tag = slot->tag;
    result = FALSE;
    for (index = 0; index < 0x10; index++) {
        if (tag == block->tags[index] && record->unk38 == block->values[index]) {
            result = TRUE;
        }
    }
    return result;
}

/* The map record the entity stands on: the one with the nearest floor under it, or failing that the one whose
   middle is nearest, among the records of the entity's map and then the other map. NULL when there is none. */
static MapRecordBounds *Entity_FindMapRecordUnder(Entity *entity) {
    MapSlot *slots[2];
    f32 middle[4];
    f32 position[4];
    MapRecordBounds *nearest;
    MapRecordBounds *record;
    f32 nearest_distance;
    f32 distance;
    f32 reach;
    int map;
    int i;
    f32 height;

    nearest = NULL;
    nearest_distance = D_004467F8[0];
    reach = (entity->unk34 < entity->unk30) ? entity->unk30 : entity->unk34;
    Vector_Copy(position, entity->model->position);
    position[1] -= (reach < 75.0f) ? 75.0f : reach;
    slots[0] = &D_0038CD20[entity->group_id];
    slots[1] = &D_0038CD20[(char)(entity->group_id ^ 1)];
    for (map = 0; map < 2; map++) {
        if (slots[map]->unk00 != 0) {
            record = (MapRecordBounds *)slots[map]->records;
            for (i = 0; i < ((MapSlotUnk08 *)slots[map]->header)->record_count; i++, record++) {
                if (record->unk3C & 0x40000000) {
                    continue;
                }
                if (record->unk32 == 0xFFFF) {
                    continue;
                }
                if (record->model->strips == NULL) {
                    continue;
                }
                if (entity->model->position[0] < record->lowest[0] - reach) {
                    continue;
                }
                if (record->highest[0] + reach < entity->model->position[0]) {
                    continue;
                }
                if (entity->model->position[2] < record->lowest[2] - reach) {
                    continue;
                }
                if (record->highest[2] + reach < entity->model->position[2]) {
                    continue;
                }
                height = EntityCollision_DistanceToPropFloor(record, position);
                if (height != D_004467F8[0]) {
                    distance = height; // match: the copy to a second variable keeps the loop long enough that its end test is not hoisted
                    if (distance < nearest_distance) {
                        nearest_distance = distance;
                        nearest = record;
                    }
                }
            }
        }
        if (nearest != NULL) {
            return nearest;
        }
    }
    for (map = 0; map < 2; map++) {
        if (slots[map]->unk00 != 0) {
            record = (MapRecordBounds *)slots[map]->records;
            for (i = 0; i < ((MapSlotUnk08 *)slots[map]->header)->record_count; i++, record++) {
                if (record->unk3C & 0x40000000) {
                    continue;
                }
                if (entity->model->position[0] < record->unk40[0] - reach) {
                    continue;
                }
                if (record->unk50[0] + reach < entity->model->position[0]) {
                    continue;
                }
                if (entity->model->position[1] < record->unk40[1] - reach) {
                    continue;
                }
                if (record->unk50[1] + reach < entity->model->position[1]) {
                    continue;
                }
                if (entity->model->position[2] < record->unk40[2] - reach) {
                    continue;
                }
                if (record->unk50[2] + reach < entity->model->position[2]) {
                    continue;
                }
                middle[0] = (record->unk40[0] + record->unk50[0]) * 0.5f;
                middle[1] = (record->unk40[1] + record->unk50[1]) * 0.5f;
                middle[2] = (record->unk40[2] + record->unk50[2]) * 0.5f;
                distance = ((f32 (*)(f32 *, f32 *))EntityMath_Distance)(middle, entity->model->position); // match: returns the distance (other files declare it void)
                if (distance < nearest_distance) {
                    nearest_distance = distance;
                    nearest = record;
                }
            }
        }
        if (nearest != NULL) {
            return nearest;
        }
    }
    return NULL;
}

/* Looks up an entity of the current map by id. */
Entity *Entity_GetBySpawnId(int entity_id) {
    if ((u32)entity_id >= 0x100) {
        return NULL;
    }
    return entity_by_spawn_id[D_00446770][entity_id];
}

/* Looks up an entity of the given map by id. */
Entity *Entity_GetBySpawnIdForMap(int map_index, int entity_id) {
    if ((u32)entity_id >= 0x100) {
        return NULL;
    }
    return entity_by_spawn_id[map_index][entity_id];
}
