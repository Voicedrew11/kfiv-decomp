#include "entity/entity_world.h"
#include "collision/collision.h"
#include "model/linked_point.h"
#include "gfx/gs.h"
#include "map/map.h"
#include "entity/entity.h"

void *memset(void *, int, u32);
int rand(void);

extern MapSlot map_slots[];
extern MapSlotMemory map_slot_memory[2];
extern Entity entity_pool[][0x80];
extern int map_spawn_counts[2];
extern EntitySpawnInfo *map_spawn_infos[2];
extern char *entity_params;
extern Vector4Arg entity_unit_scale;
extern s32 entity_type_handlers[][2];
extern int suspended_updates[3];
extern Entity *active_entities_head;
extern PlayerData player_state;
extern MapDrawState map_draw_state;
extern f32 identity_matrix[16];
extern u32 frame_counter;
extern int current_map_slot;
extern u8 player_floor_block[];
extern const f32 entity_no_hit_distance[];

void Vector_Copy(void *, void *);
void Vector_Scale(float *, float *, float);
void Vector_MultiplyComponents(float *, float *, float *);
int Motion_Start(void *, int, int, u32, int);
void Vector_Add(void *, void *, void *);
void Model_UpdateSkinnedBonePoints(ModelInstance *);
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
void Packet_BeginGsRegisters(void);
void Packet_AddGsRegister(int, s64);
void Packet_EndGsRegisters(void);
void DrawList_Add(ModelInstance *, s32, s32);
void ModelTexture_SendForModel(void *);
void Model_DrawSkinned(ModelInstance *);
void FireLord_DrawAfterimages(Entity *, s32);
void ModelMatrix_BuildPartLocal(ModelInstance *, ModelPart *, ModelPart *);
void Model_MultiplyChildMatrices(ModelPart *);
f32 EntityCollision_DistanceToPropFloor(void *, void *);

#define SCRATCH_MATRIX ((f32 *)0x70000000)
#define SCRATCH_VECTOR ((f32 *)0x70000040)
#define MAP_RECORD(entity) ((MapRecord *)(entity)->map_record)
#define SPAWN_FLAGS(spawn_info) ((EntitySpawnFlags *)&(spawn_info)->spawn_flags)

static Entity *entity_by_spawn_id[2][0x100]; /* 0x4FEB90 entity lookup by spawn id byte per map slot */

static void Entity_RebuildModelMatrices(Entity *entity);
static int Entity_IsMapRecordActive(Entity *entity);
static MapRecordBounds *Entity_FindMapRecordUnder(Entity *entity);
static inline void entity_draw_set_model_tint(Entity *entity, f32 red, f32 green, f32 blue);

void EntityAi_SetUpdate(char *, s32);

/* The set-up function entity_type_handlers gives types 27 and 83 to 95, which have no AI: puts the motion at
   animation 0, clears the work area, sets flags 22-28 and installs the table's update (Entity_UpdateInert).
   No handlers. */
void Entity_InitInert(Entity *entity) {
    Motion_Start(entity, 0, 0, 0, 0);
    memset(&entity->ai_state, 0, ENTITY_WORK_AREA_SIZE);
    ENTITY_FLAG_BITS(entity)->bit22 = TRUE;
    ENTITY_FLAG_BITS(entity)->no_wall_collision = TRUE;
    ENTITY_FLAG_BITS(entity)->no_second_wall_collision = TRUE;
    ENTITY_FLAG_BITS(entity)->no_map_object_collision = TRUE;
    ENTITY_FLAG_BITS(entity)->no_entity_collision = TRUE;
    ENTITY_FLAG_BITS(entity)->no_player_collision = TRUE;
    ENTITY_FLAG_BITS(entity)->no_floor_lift = TRUE;
    EntityAi_SetUpdate((char *)entity, entity_type_handlers[entity->params->entity_type][1]);
}

/* The per-frame update entity_type_handlers gives the types Entity_InitInert sets up: does nothing. */
void Entity_UpdateInert(void) {
}

/* Empty. main calls it once among the set-up calls at start; what it was for is not known. */
void Entity_StartupHook(void) {
}

/* Sets up the entities of a map from its spawn records, filling in defaults for values left at 0. */
void Entity_InitFromMapSpawns(int map) {
    Entity *entity;
    EntitySpawnInfo *spawn_info;
    ModelInstance *model;
    int i;
    int count;
    MapSlot *slot;
    MapSlotMemory *memory;

    slot = &map_slots[map];
    memory = &map_slot_memory[map];
    entity = entity_pool[map];
    spawn_info = map_spawn_infos[map];
    model = memory->entity_models;
    map_spawn_counts[map] = ((MapSlotUnk08 *)slot->header)->spawn_count;
    memset(entity, 0, sizeof(entity_pool[map]));
    memset(entity_by_spawn_id[map], 0, sizeof(entity_by_spawn_id[map]));
    count = map_spawn_counts[map];
    for (i = 0; i < count; i++, entity++, spawn_info++, model++) {
        entity->group_id = map;
        entity->model = (EntityModel *)model;
        entity->params = (EntityParams *)(entity_params + spawn_info->type_id * sizeof(EntityParams));
        entity->pool_slot_index = -1;
        entity->spawn_info = spawn_info;
        entity_by_spawn_id[map][SPAWN_FLAGS(spawn_info)->bytes[4]] = entity;
        if ((SPAWN_FLAGS(spawn_info)->word & (0xC000L << 45)) == (0x8000L << 45)) {
            ENTITY_FLAG_BITS(entity)->respawnable = FALSE;
        } else {
            ENTITY_FLAG_BITS(entity)->respawnable = TRUE;
        }
        entity->spawn_index = i;
        ENTITY_FLAG_BITS(entity)->own_think = TRUE;
        ENTITY_FLAG_BITS(entity)->handler_runs = TRUE;
        if (entity->spawn_info->wander_radius == 0) {
            entity->spawn_info->wander_radius = 1500;
        }
        if (entity->spawn_info->wander_height_range == 0) {
            entity->spawn_info->wander_height_range = 500;
        }
        if (entity->spawn_info->respawn_distance == 0) {
            entity->spawn_info->respawn_distance = 1800;
        }
        if (entity->spawn_info->respawn_chance == 0) {
            entity->spawn_info->respawn_chance = 100;
        }
        if (entity->spawn_info->wake_distance == 0) {
            entity->spawn_info->wake_distance = 800;
        }
        if (entity->spawn_info->despawn_range == 0) {
            entity->spawn_info->despawn_range = 2000;
        }
        if (SPAWN_FLAGS(entity->spawn_info)->bytes[6] == 0) {
            SPAWN_FLAGS(entity->spawn_info)->bytes[6] = 80;
        }
        if (entity->params->floor_probe_depth == 0) {
            entity->params->floor_probe_depth = 75;
        }
        if (entity->params->hit_points == 0) {
            entity->params->hit_points = 30;
        }
        if (entity->params->body_volume == 0) {
            entity->params->body_volume = 70;
        }
        if (entity->params->sound_id == 0) {
            entity->params->sound_id = 1500;
        }
    }
}

/* Clears flag bit 4 of every entity in one of the two entity arrays. */
void Entity_ClearFlag10InSlot(int array_index) {
    Entity *entity;
    int i;

    entity = entity_pool[array_index];
    for (i = 0; i < map_spawn_counts[array_index]; i++, entity++) {
        entity->flags &= ~ENTITY_FLAG_NO_RESPAWN;
    }
}

/* Puts an entity back to how it spawns: flags, counters, position, rotation and its sizes scaled. */
void Entity_ResetToSpawn(Entity *entity) {
    Vector4Arg scale = entity_unit_scale;
    f32 widest;

    // match: read first; after the stores below, the scheduler would hold this load back behind them
    entity->hit_points = entity->params->hit_points;
    ENTITY_FLAG_BITS(entity)->live = FALSE;
    ENTITY_FLAG_BITS(entity)->dying = 0;
    ENTITY_FLAG_BITS(entity)->animates = TRUE;
    ENTITY_FLAG_BITS(entity)->bit15 = FALSE;
    ENTITY_FLAG_BITS(entity)->respawn_roll_done = FALSE;
    ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit19 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit20 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit21 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit22 = FALSE;
    ENTITY_FLAG_BITS(entity)->no_wall_collision = FALSE;
    ENTITY_FLAG_BITS(entity)->no_second_wall_collision = FALSE;
    ENTITY_FLAG_BITS(entity)->no_map_object_collision = FALSE;
    ENTITY_FLAG_BITS(entity)->no_entity_collision = FALSE;
    ENTITY_FLAG_BITS(entity)->no_player_collision = FALSE;
    ENTITY_FLAG_BITS(entity)->map_record_inactive = FALSE;
    ENTITY_FLAG_BITS(entity)->look_at_target = FALSE;
    ENTITY_FLAG_BITS(entity)->bit10 = FALSE;
    ENTITY_FLAG_BITS(entity)->draw_afterimage = TRUE;
    ENTITY_FLAG_BITS(entity)->turn_in_place = FALSE;
    entity->update_func = 0;
    entity->handler = NULL;
    entity->prev_handler = NULL;
    entity->stun_frames = 0;
    entity->hit_cooldown_frames = 0;
    entity->hit_flash_frames = 0;
    entity->choice_change_frame = 0;
    entity->attack_delay_frames = 0;
    entity->ailment_1_frames = 0;
    entity->ailment_2_frames = 0;
    entity->ailment_3_frames = 0;
    entity->ailment_4_frames = 0;
    entity->action_step = 0;
    entity->reaction_step = 0;
    entity->state = 0;
    entity->combat_choice = 0;
    entity->prev_state = 0;
    entity->prev_combat_choice = 0;
    ((EntityFlags *)&entity->flags)->halves[1] = 0;
    entity->busy_flags = 0;
    entity->move_command = 0;
    entity->action_timer = 0;
    entity->floor_lift = 10.0f;
    Vector_Copy(entity->model->position, entity->spawn_info);
    Vector_Copy(entity->model->rotation, &entity->spawn_info->spawn_rotation_x);
    Vector_Scale(entity->model->scale, &scale.x, (f32)entity->params->model_scale_percent / 100.0f);
    Vector_Copy(&scale, entity->spawn_info->spawn_scale);
    if (!(entity->params->flags & (ENTITY_PARAMS_FLAG_40 | ENTITY_PARAMS_FLAG_200))) {
        Vector_Scale(&scale.x, &scale.x, (f32)(rand() % 11 + 95) / 100.0f);
    }
    Vector_MultiplyComponents(entity->model->scale, entity->model->scale, &scale.x);
    widest = (scale.z < scale.x) ? scale.x : scale.z;
    entity->collision_radius = (f32)entity->params->map_collision_radius * widest;
    entity->collision_height = (f32)entity->params->map_collision_height * scale.y;
    entity->radius = (f32)entity->params->radius * widest;
    entity->height = (f32)entity->params->body_height * scale.y;
    entity->body_radius = (f32)entity->params->body_radius * widest;
    entity->knockback_damping = (f32)entity->params->body_volume * scale.x * scale.y * scale.z;
    entity->knockback_damping = ABS(entity->knockback_damping);
    entity->model->radius = (entity->collision_radius < entity->collision_height) ? entity->collision_height : entity->collision_radius;
    Motion_Start(entity, 0, 0, 0, 1);
    ((void (*)(Entity *))entity_type_handlers[entity->params->entity_type][0])(entity);
    Entity_RebuildModelMatrices(entity);
}

/* Empties one of the two entity arrays and makes the other array's entities forget the map
   records (Entity.map_record) that belonged to its map. */
void Entity_ClearMapSlot(int array_index) {
    Entity *entity;
    int i;

    map_spawn_counts[array_index] = 0;
    memset(entity_pool[array_index], 0, sizeof(entity_pool[array_index]));
    for (i = 0; i < 0x80; i++) {
        entity = &entity_pool[array_index ^ 1][i];
        if (entity->map_record != NULL && ((u8)(((MapRecord *)entity->map_record)->record_flags >> 27) & 1) == array_index) {
            entity->map_record = NULL;
        }
    }
}

void Entity_SetSpawnedFlags(Entity *entity) {
    entity->flags = (entity->flags | ENTITY_FLAG_DYING_MASK) & ~ENTITY_FLAG_RESPAWN_ROLL_DONE;
}

/* Moves every entity of one of the two entity arrays by an offset. */
void Entity_OffsetAllInSlot(int array_index, f32 *offset) {
    Entity *entity;
    int i;
    int count;

    entity = entity_pool[array_index];
    count = map_spawn_counts[array_index];
    for (i = 0; i < count; i++, entity++) {
        Vector_Add(entity->model->position, entity->model->position, offset);
        Vector_Add(entity->prev_position, entity->prev_position, offset);
        Vector_Add(entity->spawn_info, entity->spawn_info, offset);
    }
}

/* The per-frame update of every live entity: respawn, the countdowns, where the player is, the push it is
   under, its own update function, and the map blocks its model is drawn with. */
void Entity_UpdateAll(void) {
    Entity *entity;
    EntitySpawnInfo *spawn_info;

    if (suspended_updates[0] & 4) {
        return;
    }
    for (entity = active_entities_head; entity != NULL; entity = entity->next) {
        Model_UpdateSkinnedBonePoints(ENTITY_MODEL(entity));
        if (ENTITY_FLAG_BITS(entity)->dying >= 3 || !ENTITY_FLAG_BITS(entity)->live) {
            if (!EntityAi_ShouldRespawn(entity)) {
                continue;
            }
            EntityEvent_Respawn((char *)entity);
        }
        if (EntityAi_RemoveIfFallen(entity)) {
            continue;
        }
        if (entity->stun_frames != 0) {
            entity->stun_frames--;
            continue;
        }
        if (ENTITY_FLAG_BITS(entity)->not_targetable) {
            entity->ailment_1_frames = 0;
            entity->ailment_2_frames = 0;
            ENTITY_FLAG_BITS(entity)->last_hit_knockback = FALSE;
            entity->ailment_3_frames = 0;
            entity->ailment_4_frames = 0;
        }
        entity->distance_to_player_3d = EntityMath_Distance(entity->model->position, player_state.position);
        entity->distance_to_player = EntityMath_DistanceXZ(entity->model->position, player_state.position);
        entity->heading_to_player = EntityMath_HeadingTo(entity->model->position, player_state.position);
        entity->move_delta_x = entity->move_delta_y = entity->move_delta_z = entity->turn_delta_pitch = entity->turn_delta_yaw = entity->turn_delta_roll = 0.0f;
        if (!Entity_IsMapRecordActive(entity)) {
            if (!ENTITY_FLAG_BITS(entity)->map_record_inactive) {
                ModelMatrix_Build(ENTITY_MODEL(entity), &ENTITY_MODEL(entity)->position, &ENTITY_MODEL(entity)->rotation, &ENTITY_MODEL(entity)->scale_x);
            }
            ENTITY_FLAG_BITS(entity)->map_record_inactive = TRUE;
            continue;
        }
        ENTITY_FLAG_BITS(entity)->map_record_inactive = FALSE;
        if (entity->hit_cooldown_frames != 0) {
            entity->hit_cooldown_frames--;
        }
        if (entity->hit_flash_frames != 0) {
            entity->hit_flash_frames--;
        }
        if (entity->ailment_2_frames != 0) {
            entity->ailment_2_frames--;
        }
        if (entity->ailment_3_frames != 0) {
            entity->ailment_3_frames--;
        }
        if (entity->ailment_4_frames != 0) {
            entity->ailment_4_frames--;
        }
        if (entity->ailment_1_frames != 0) {
            if (entity->ailment_1_frames % 30 == 0) {
                entity->hit_points = (entity->hit_points < 3) ? 0 : entity->hit_points - 3;
                entity->flags &= ~ENTITY_FLAG_HIT_SOURCE_MASK;
            }
            entity->ailment_1_frames--;
        }
        if (ENTITY_FLAG_BITS(entity)->hit_strength != 0) {
            if (entity->params->hit_points < entity->last_hit_damage) {
                entity->last_hit_damage = entity->params->hit_points;
            }
            if (ENTITY_FLAG_BITS(entity)->last_hit_knockback) {
                entity->knockback_speed = (f32)entity->last_hit_damage * 75.0f / (f32)entity->params->hit_points;
                entity->hit_flash_frames = 6;
            } else {
                entity->knockback_speed = (f32)entity->last_hit_damage * 25.0f / (f32)entity->params->hit_points;
                entity->hit_flash_frames = 3;
            }
        }
        if (entity->params->flags & ENTITY_PARAMS_FLAG_NO_KNOCKBACK) {
            entity->knockback_speed = 0.0f;
        }
        if (entity->knockback_speed > 0.0f) {
            f32 forward = 1.0f; // match: declared ahead of the pointer, or the pointer is hoisted out of the loop
            f32 *push = SCRATCH_VECTOR;

            push[0] = 0.0f;
            push[1] = 0.0f;
            push[2] = forward;
            push[3] = 0.0f;
            sceVu0RotMatrixY(SCRATCH_MATRIX, identity_matrix, EntityMath_WrapAngle(entity->knockback_heading));
            sceVu0ApplyMatrix(SCRATCH_VECTOR, SCRATCH_MATRIX, SCRATCH_VECTOR);
            Vector_Scale(SCRATCH_VECTOR, SCRATCH_VECTOR, entity->knockback_speed);
            Vector_Add(entity->model->position, entity->model->position, SCRATCH_VECTOR);
        }
        if (entity->update_func == 0) {
            continue;
        }
        ((void (*)(Entity *))entity->update_func)(entity);
        if (entity->knockback_speed > 0.0f) {
            entity->knockback_speed -= entity->knockback_damping * 4.0f / 10.0f;
        }
        Entity_RebuildModelMatrices(entity);
        if (entity->map_record == NULL) {
            entity->map_record = Entity_FindMapRecordUnder(entity);
        }
        spawn_info = entity->spawn_info;
        if ((s8)SPAWN_FLAGS(spawn_info)->bytes[2] != -1 || entity->map_record == NULL) {
            ENTITY_MODEL(entity)->fog_block = &map_draw_state.fog_blocks[entity->group_id][(s8)SPAWN_FLAGS(spawn_info)->bytes[2]];
        } else {
            ENTITY_MODEL(entity)->fog_block = &map_draw_state.fog_blocks[(u8)(MAP_RECORD(entity)->record_flags >> 27) & 1][MAP_RECORD(entity)->draw_block_index];
        }
        if ((s8)SPAWN_FLAGS(spawn_info)->bytes[3] != -1 || entity->map_record == NULL) {
            ENTITY_MODEL(entity)->light_block = (s32)&map_draw_state.light_blocks[entity->group_id][(s8)SPAWN_FLAGS(spawn_info)->bytes[3]];
        } else {
            ENTITY_MODEL(entity)->light_block = (s32)&map_draw_state.light_blocks[(u8)(MAP_RECORD(entity)->record_flags >> 27) & 1][MAP_RECORD(entity)->light_block_index];
        }
        Model_UpdateSkinnedBonePoints(ENTITY_MODEL(entity));
    }
}

/* Sets the colour the entity's model is drawn in, and its alpha from the entity's own. */
static inline void entity_draw_set_model_tint(Entity *entity, f32 red, f32 green, f32 blue) {
    ModelInstance *model = ENTITY_MODEL(entity);

    model->colour_red = red;
    model->colour_green = green;
    model->colour_blue = blue;
    model->alpha = (f32)entity->model_alpha;
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
    Packet_BeginGsRegisters();
    Packet_AddGsRegister(GS_REG_ALPHA_1, 0x44);
    Packet_AddGsRegister(GS_REG_ALPHA_2, 9);
    Packet_AddGsRegister(GS_REG_TEST_1, 0x5000D);
    Packet_AddGsRegister(GS_REG_TEST_2, 0x5000D);
    Packet_EndGsRegisters();
    for (map = 0; map < 2; map++) {
        for (block = 0; block < 5; block++) {
            for (entity = active_entities_head; entity != NULL; entity = entity->next) {
                if (!ENTITY_FLAG_BITS(entity)->live) {
                    continue;
                }
                if (ENTITY_FLAG_BITS(entity)->dying >= 3) {
                    continue;
                }
                if (entity->group_id != map) {
                    continue;
                }
                if (entity->pool_slot_index != block) {
                    continue;
                }
                if (entity->map_record == NULL) {
                    continue;
                }
                if (ENTITY_FLAG_BITS(entity)->map_record_inactive) {
                    continue;
                }
                if (entity->hit_flash_frames != 0 && entity->stun_frames == 0) {
                    ENTITY_MODEL(entity)->colour_red = 255.0f;
                    ENTITY_MODEL(entity)->colour_green = 64.0f;
                    ENTITY_MODEL(entity)->colour_blue = 64.0f;
                    ENTITY_MODEL(entity)->alpha = (f32)entity->model_alpha;
                } else if (ENTITY_FLAG_BITS(entity)->dying < 2) {
                    half = 20;
                    phase = frame_counter % period;
                    falling = period - phase;
                    if (phase >= half) {
                        phase = falling;
                    }
                    if (entity->ailment_1_frames != 0 && entity->ailment_tint_index == 1) {
                        entity_draw_set_model_tint(entity, 128.0f - phase * 64.0f / half, phase * 127.0f / half + 128.0f, 128.0f - phase * 64.0f / half);
                    } else if (entity->ailment_2_frames != 0 && entity->ailment_tint_index == 2) {
                        entity_draw_set_model_tint(entity, phase * 127.0f / half + 128.0f, 128.0f - phase * 64.0f / half, phase * 127.0f / half + 128.0f);
                    } else if (entity->ailment_3_frames != 0 && entity->ailment_tint_index == 3) {
                        entity_draw_set_model_tint(entity, phase * 127.0f / half + 128.0f, phase * 127.0f / half + 128.0f, 128.0f - phase * 64.0f / half);
                    } else if (entity->ailment_4_frames != 0 && entity->ailment_tint_index == 4) {
                        entity_draw_set_model_tint(entity, 128.0f - phase * 64.0f / half, phase * 127.0f / half + 128.0f, phase * 127.0f / half + 128.0f);
                    } else if (entity->ailment_1_frames != 0) {
                        entity_draw_set_model_tint(entity, 128.0f - phase * 64.0f / half, phase * 127.0f / half + 128.0f, 128.0f - phase * 64.0f / half);
                    } else if (entity->ailment_2_frames != 0) {
                        entity_draw_set_model_tint(entity, phase * 127.0f / half + 128.0f, 128.0f - phase * 64.0f / half, phase * 127.0f / half + 128.0f);
                    } else if (entity->ailment_3_frames != 0) {
                        entity_draw_set_model_tint(entity, phase * 127.0f / half + 128.0f, phase * 127.0f / half + 128.0f, 128.0f - phase * 64.0f / half);
                    } else if (entity->ailment_4_frames != 0) {
                        entity_draw_set_model_tint(entity, 128.0f - phase * 64.0f / half, phase * 127.0f / half + 128.0f, phase * 127.0f / half + 128.0f);
                    } else {
                        entity_draw_set_model_tint(entity, 128.0f, 128.0f, 128.0f);
                    }
                }
                LinkedPoint_FindHitsForModel(ENTITY_MODEL(entity));
                if (ENTITY_MODEL(entity)->alpha < 128.0f || (ENTITY_MODEL(entity)->model_flags & 0x200) || entity->spawn_info->type_id == 0x10) {
                    DrawList_Add(ENTITY_MODEL(entity), 0, list);
                } else {
                    ModelTexture_SendForModel(ENTITY_MODEL(entity));
                    Model_DrawSkinned(ENTITY_MODEL(entity));
                }
                if (entity->spawn_info->type_id == 0xF) {
                    FireLord_DrawAfterimages(entity, list);
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

    root = ENTITY_MODEL(entity)->root_part;
    part_count = ENTITY_MODEL(entity)->part_count;
    ModelMatrix_Build(ENTITY_MODEL(entity), &ENTITY_MODEL(entity)->position, &ENTITY_MODEL(entity)->rotation, &ENTITY_MODEL(entity)->scale_x);
    Vector_Copy(saved_offset, root->position);
    Vector_Copy(saved_rotation, &root->rotation_x);
    root->position[0] = root->position[1] = root->position[2] = 0.0f;
    root->rotation_x = root->rotation_y = root->rotation_z = 0.0f;
    part = ENTITY_MODEL(entity)->parts;
    for (i = 0; i < part_count; i++) {
        ModelMatrix_BuildPartLocal(ENTITY_MODEL(entity), part, part);
        part++;
    }
    Vector_Copy(root->position, saved_offset);
    Vector_Copy(&root->rotation_x, saved_rotation);
    part = ENTITY_MODEL(entity)->parts;
    for (i = 0; i < part_count; i++) {
        Model_MultiplyChildMatrices(part);
        part++;
    }
}

/* Whether the map record the entity belongs to (Entity.map_record) is switched on: FALSE when the record
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

    if (entity->map_record == NULL) {
        return TRUE;
    }
    if (entity->spawn_info->type_id == 0x10 || entity->spawn_info->type_id == 0x2F ||
        entity->spawn_info->type_id == 0x53 || entity->spawn_info->type_id == 0x9E) {
        return TRUE;
    }
    found = FALSE;
    for (index = 0; index < 2; index++) {
        record = (MapRecordBounds *)map_slots[index].records;
        for (record_index = 0; record_index < map_slots[index].header->record_count; record_index++, record++) {
            if (record == entity->map_record) {
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
    if (record->record_flags & 0x40000000) {
        return FALSE;
    }
    if (index == current_map_slot) {
        slot = &map_slots[index];
        bits = slot->flag_blocks[player_floor_block[0]].bits;
        if (slot->loaded == 0) {
            return FALSE;
        }
        return (bits[record->flag_block_bit >> 3] >> (record->flag_block_bit & 7)) & 1;
    }
    slot = &map_slots[current_map_slot];
    block = &slot->flag_blocks[player_floor_block[0]];
    slot = &map_slots[current_map_slot ^ 1];
    if (slot->loaded == 0) {
        return FALSE;
    }
    tag = slot->tag;
    result = FALSE;
    for (index = 0; index < 0x10; index++) {
        if (tag == block->tags[index] && record->flag_block_bit == block->values[index]) {
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
    nearest_distance = entity_no_hit_distance[0];
    reach = (entity->collision_height < entity->collision_radius) ? entity->collision_radius : entity->collision_height;
    Vector_Copy(position, entity->model->position);
    position[1] -= (reach < 75.0f) ? 75.0f : reach;
    slots[0] = &map_slots[entity->group_id];
    slots[1] = &map_slots[(char)(entity->group_id ^ 1)];
    for (map = 0; map < 2; map++) {
        if (slots[map]->loaded != 0) {
            record = (MapRecordBounds *)slots[map]->records;
            for (i = 0; i < ((MapSlotUnk08 *)slots[map]->header)->record_count; i++, record++) {
                if (record->record_flags & 0x40000000) {
                    continue;
                }
                if (record->collision_mesh_index == 0xFFFF) {
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
                if (height != entity_no_hit_distance[0]) {
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
        if (slots[map]->loaded != 0) {
            record = (MapRecordBounds *)slots[map]->records;
            for (i = 0; i < ((MapSlotUnk08 *)slots[map]->header)->record_count; i++, record++) {
                if (record->record_flags & 0x40000000) {
                    continue;
                }
                if (entity->model->position[0] < record->box_lowest[0] - reach) {
                    continue;
                }
                if (record->box_highest[0] + reach < entity->model->position[0]) {
                    continue;
                }
                if (entity->model->position[1] < record->box_lowest[1] - reach) {
                    continue;
                }
                if (record->box_highest[1] + reach < entity->model->position[1]) {
                    continue;
                }
                if (entity->model->position[2] < record->box_lowest[2] - reach) {
                    continue;
                }
                if (record->box_highest[2] + reach < entity->model->position[2]) {
                    continue;
                }
                middle[0] = (record->box_lowest[0] + record->box_highest[0]) * 0.5f;
                middle[1] = (record->box_lowest[1] + record->box_highest[1]) * 0.5f;
                middle[2] = (record->box_lowest[2] + record->box_highest[2]) * 0.5f;
                distance = EntityMath_Distance(middle, entity->model->position);
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
    return entity_by_spawn_id[current_map_slot][entity_id];
}

/* Looks up an entity of the given map by id. */
Entity *Entity_GetBySpawnIdForMap(int map_index, int entity_id) {
    if ((u32)entity_id >= 0x100) {
        return NULL;
    }
    return entity_by_spawn_id[map_index][entity_id];
}

/* Returns 0. Nothing calls it. */
int Entity_UnusedReturnZeroA(void) {
    return 0;
}

/* Returns 0. Nothing calls it. */
int Entity_UnusedReturnZeroB(void) {
    return 0;
}
