#include "map/map.h"
#include "model/linked_point.h"
#include "gfx/gs.h"
#include "map_object/map_object.h"
#include "map/map_scroll.h"
#include "entity/entity.h"
#include "collision/collision.h"
#include "gfx/screen.h"
#include "core/game.h"
#include "item/gold.h"
#include "entity/entity_world.h"
#include "sound/sound.h"
#include "effect/effect.h"
#include "model/model.h"
#include "player/player.h"
#include "ui/message.h"
#include "gfx/packet.h"
#include "gfx/texture.h"
#include "input/pad.h"

#define FOG_COLOR() \
    ((u64)map_draw_state.current_fog_color.red | ((u64)map_draw_state.current_fog_color.green << 8) | ((u64)map_draw_state.current_fog_color.blue << 16) | \
     ((u64)0x80 << 24))
#define USABLE(node) (!((node)->flags & 0x40000000))
#define IN_X(node) ((node)->left - margin <= point[0] && point[0] <= (node)->right + margin)
#define IN_Y(node) ((node)->lowest_y - margin <= point[1] && point[1] <= (node)->highest_y + margin)
#define IN_Z(node) ((node)->top - margin <= point[2] && point[2] <= (node)->bottom + margin)
/* how far the point is outside the box along one axis */
#define DISTANCE(low, high, value)                                                                 \
    if ((value) < (low) - margin) {                                                                \
        distance = ((low) - margin) - (value);                                                     \
    } else {                                                                                       \
        distance = (value) - ((high) + margin);                                                    \
    }                                                                                              \
    if (distance < best_distance) {                                                                \
        best_distance = distance;                                                                  \
        best = node;                                                                               \
    }
#define USABLE_IN_MAP(node) (((u8)(AREA_NODE_WORD(node) >> 27) & 1) == map && USABLE(node))
void Packet_BeginGsRegisters(void);
void Packet_AddGsRegister(int, s64);
void Packet_EndGsRegisters(void);
void GsDraw_QueueFlush(void);
void GsDraw_QueueTexFlush(void);
void GsDraw_ResetDrawTarget(void);
void ModelScreen_CutClippedTriangles(ModelInstance *);
void Wobble_DrawRippledModel(ModelInstance *, int, int, int, int);
void Map_SetOffscreenTarget(void);
void Map_QueueSlotPacket(int, int);
int Map_ClassifyRecordOnScreen(MapDrawRecord *, ModelInstance *);
void *memset(void *, int, u32);
void Map_InsertRecordBySize(AreaNode *);
void Model_SetUpPosed(ModelInstance *, char **, char **);
void Model_SetUpSkinned(ModelInstance *, int *, int *);
MotionData *ModelMotion_LoadFile(MotionFile *, ModelChannel **, ModelInstance *, int);
u32 *Map_GetSlotPacket(int, int);
void Model_SetPrimBits(ModelInstance *, s64, s64);
void Model_SetAlphaBlend(ModelInstance *, s64);
MapRecordInfo *Map_FindRecordAtInSlot(CameraState *, f32, int);
void Wobble_BuildWaveUvs(ModelInstance *, u32 *);
void Vector_Copy(void *, void *);
char *Collision_RelocateMesh(char *, ModelInstance *, int);
void Player_MoveByOffset(f32 *);
void Interact_OffsetViewRay(f32 *);
void Map_OffsetRecords(int, f32 *);
void Map_OffsetRecord(MapRecordBounds *, f32 *);
void Map_PlaceRecordModels(int);
void MapItem_MoveMap(int, f32 *);
void Script_Init(int);
int Script_Start(int, int);
void Player_StopFootstepSound(void);
void Sound_StopAll(int);
void Vector_Add(void *, void *, void *);
void Matrix_Multiply(void *, void *, void *);
void ModelMatrix_BuildPartLocal(ModelInstance *, ModelPart *, ModelPart *);
void sceVu0CopyMatrix(void *, void *);
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0RotMatrixZ(float *, float *, float);
void sceVu0TransMatrix(float *, float *, float *);
void sceVu0ViewScreenMatrix(void *, float, float, float, float, float, float, float, float, float);
void sceVu0MulMatrix(void *, void *, void *);
void Vector_Scale(float *, float *, float);
void Vector_Subtract(void *, void *, void *);
void GameLoop_RunSoundLoads(void);
void Packet_OnFrameStart(void);
void Packet_OnFrameEnd(void);
void Packet_SwapBuffers(void);
void Packet_SendChain(void);
void Packet_WaitForSend(void);
void GsDraw_QueueVu1Matrix(void *);
void DrawList_Clear(void);
void Map_DrawFreeCameraView(void);
float Angle_Wrap(float);
void Input_Update(void);
void Display_SwapBuffers(int);
void Vector_Normalize(void *, void *);
void Game_BlendScreenOffsetDown(void);
void GsDraw_QueueTexturedSprite(void *);
void TextureTable_QueueAll(TextureTableHeader *);
void Model_DrawPosed(ModelInstance *);
void GsDraw_SetDrawTarget(s64, s64, s64);
void GsDraw_FillScreen(int, int, int, int, s64);
u32 *Packet_Alloc(int);
void Packet_AppendToChain(u32 *, u32 *);
void Vu0_LoadMatrix(void *);
void Vu0_ProjectToScreen(int *, void *);
int Map_TestFlagBlockBit(int, int, int, int);
void Vu0_ProjectToScreen3(int *, int *, int *, f32 *, f32 *, f32 *);
extern MapSlot map_slots[];
extern MapDrawState map_draw_state;
extern s64 game_dbuff_clear_color[];
extern s64 game_dbuff_context2_clear_color[];
extern MapDrawRecord *record_draw_lists[2];
extern int current_map_slot;
extern MapDrawRecord *late_record_draw_lists[2];
extern MapObject map_slot_objects[][0x200];
extern Entity entity_pool[][0x80];
extern MapSlotMemory map_slot_memory[2];
extern int map_object_placements[2];
extern EntitySpawnInfo *map_spawn_infos[2];
extern MapItemDef *map_item_defs[2];
extern s16 *map_item_model_kinds[2];
extern u8 *map_item_group_thresholds[2];
extern MapProp *map_slot_props[2][0xFF];
extern char map_object_defs[];
extern s32 map_object_counts[2];
extern MapLoadState map_load_state;
extern SoundLoadQueue sound_load_queue;
extern f32 identity_matrix[16];
extern f32 world_screen_matrix[16];
extern AreaNode *record_boxes;
extern AreaNode *backdrop_record_boxes;
extern f32 D_00446768[]; /* the largest float */
extern f32 D_0044676C[]; /* the largest float */
extern GameStatus game_status;
extern u16 pad_state[];
extern PlayerState player_state;
extern MapLightBlock map_object_default_lights;
extern TextureTableHeader *D_00446790;
extern u8 player_floor_block[];

/* Draws the map records of both map slots whose flag bit 0 is clear (the list Map_UpdateDrawState builds each
   frame), then each slot's map objects. mode 0 draws every record, the others only some; below 2 the
   drawing goes to the screen, at 2 to the off-screen buffer (Map_SetOffscreenTarget). */
void Map_DrawRecords(int mode) {
    MapSlot *slot;
    MapDrawRecord *record;
    MapSlotUnk08 *header;
    int map;
    int active;
    int flags;

    game_dbuff_clear_color[0] = FOG_COLOR();
    game_dbuff_clear_color[30] = FOG_COLOR();
    game_dbuff_context2_clear_color[0] = FOG_COLOR();
    game_dbuff_context2_clear_color[30] = FOG_COLOR();
    active = 0;
    if (map_slots[0].loaded != 0) {
        active = map_slots[0].has_flag_80000000_record != 0;
    }
    if (map_slots[1].loaded != 0) {
        if (map_slots[1].has_flag_80000000_record != 0) {
            active++;
        }
    }
    for (map = 0; map < 2; map++) {
        record = record_draw_lists[map];
        slot = &map_slots[map];
        if (slot->loaded != 0) {
            Map_QueueSlotPacket(map, 0);
            Map_QueueSlotPacket(map, 1);
            Map_QueueSlotPacket(map, 5);
            GsDraw_QueueTexFlush();
            MapScroll_UpdateDraw(map, 0);
            MapScroll_UpdateCopy(map, 0);
            if (mode < 2) {
                GsDraw_ResetDrawTarget();
            } else {
                Map_SetOffscreenTarget();
            }
            Packet_BeginGsRegisters();
            Packet_AddGsRegister(GS_REG_TEST_1, 0x5000D);
            Packet_AddGsRegister(8, 0);
            Packet_EndGsRegisters();
            GsDraw_QueueFlush();
            for (; record != NULL; record = record->next) {
                flags = record->flags;
                if ((flags & 2) && mode != 0) {
                    continue;
                }
                if ((flags & 0x200) && mode != 0) {
                    continue;
                }
                if (!(flags & 0x20) && mode != 0) {
                    continue;
                }
                if (active >= 2 && flags < 0 && map != current_map_slot) {
                    continue;
                }
                if (flags & 0x10) {
                    record->model->light_block = (s32)&map_draw_state.current_light_block;
                    record->model->fog_block = &map_draw_state.current_fog_block;
                } else {
                    record->model->light_block = (s32)&map_draw_state.light_blocks[map][record->light_block_index];
                    record->model->fog_block = &map_draw_state.fog_blocks[map][record->fog_block_index];
                }
                LinkedPoint_FindHitsForMapDraw(record, record->model);
                if (record->flags & 0x200) {
                    header = slot->header;
                    Wobble_DrawRippledModel(record->model, header->screen_tint_red, header->screen_tint_green, header->screen_tint_blue, header->screen_tint_alpha);
                }
                record->model->draw_parts(record->model);
                if (Map_ClassifyRecordOnScreen(record, record->model) == 0) {
                    ModelScreen_CutClippedTriangles(record->model);
                }
                GsDraw_QueueFlush();
            }
            MapObject_DrawAll(mode, 1, map);
        }
    }
}

/* Draws the map records of both map slots whose flag bit 0 is set, after the entities and items, switching
   z writes on and off as the records ask (flag 0x80). mode as for Map_DrawRecords. */
void Map_DrawLateRecords(int mode) {
    MapSlot *slot;
    MapDrawRecord *record;
    MapSlotUnk08 *header;
    int map;
    int active;
    int flags;
    int z_mask;
    int old_z_mask;

    active = 0;
    if (map_slots[0].loaded != 0) {
        active = map_slots[0].has_flag_80000000_record != 0;
    }
    if (map_slots[1].loaded != 0) {
        if (map_slots[1].has_flag_80000000_record != 0) {
            active++;
        }
    }
    for (map = 0; map < 2; map++) {
        record = late_record_draw_lists[map];
        slot = &map_slots[map];
        if (slot->loaded != 0) {
            Map_QueueSlotPacket(map, 2);
            Map_QueueSlotPacket(map, 5);
            GsDraw_QueueTexFlush();
            MapScroll_UpdateDraw(map, 1);
            MapScroll_UpdateCopy(map, 1);
            if (mode < 2) {
                GsDraw_ResetDrawTarget();
            } else {
                Map_SetOffscreenTarget();
            }
            Packet_BeginGsRegisters();
            z_mask = 1;
            Packet_AddGsRegister(GS_REG_TEST_1, 0x5000D);
            Packet_AddGsRegister(GS_REG_ZBUF_1, 0x10A000118L);
            Packet_EndGsRegisters();
            for (; record != NULL; record = record->next) {
                flags = record->flags;
                if ((flags & 2) && mode != 0) {
                    continue;
                }
                if ((flags & 0x200) && mode != 0) {
                    continue;
                }
                if (!(flags & 0x20) && mode != 0) {
                    continue;
                }
                if (active >= 2 && flags < 0 && map != current_map_slot) {
                    continue;
                }
                old_z_mask = z_mask;
                z_mask = !((flags >> 7) & 1);
                if (z_mask != old_z_mask) {
                    Packet_BeginGsRegisters();
                    Packet_AddGsRegister(GS_REG_ZBUF_1, ((s64)z_mask << 32) | 0xA000118);
                    Packet_EndGsRegisters();
                }
                if (record->flags & 0x10) {
                    record->model->light_block = (s32)&map_draw_state.current_light_block;
                    record->model->fog_block = &map_draw_state.current_fog_block;
                } else {
                    record->model->light_block = (s32)&map_draw_state.light_blocks[map][record->light_block_index];
                    record->model->fog_block = &map_draw_state.fog_blocks[map][record->fog_block_index];
                }
                LinkedPoint_FindHitsForMapDraw(record, record->model);
                if (record->flags & 0x200) {
                    header = slot->header;
                    Wobble_DrawRippledModel(record->model, header->screen_tint_red, header->screen_tint_green, header->screen_tint_blue, header->screen_tint_alpha);
                }
                record->model->draw_parts(record->model);
                if (Map_ClassifyRecordOnScreen(record, record->model) == 0) {
                    ModelScreen_CutClippedTriangles(record->model);
                }
                GsDraw_QueueFlush();
            }
        }
    }
    Packet_BeginGsRegisters();
    Packet_AddGsRegister(GS_REG_ZBUF_1, 0xA000118);
    Packet_EndGsRegisters();
}

/* the address rounded up to a multiple of 16 */
static inline u32 align_16(u32 address) {
    u32 remainder = address & 0xF;
    u32 aligned = address;

    if (remainder != 0) {
        aligned = remainder - 0x10;
        aligned = address - aligned;
    }
    return aligned;
}

/* Finds the parts of a map file that was just read into a slot's memory, and shares out the
   slot's working memory. */
void Map_ParseFile(int map) {
    MapSlotMemory *memory = &map_slot_memory[map];
    // match: set twice, the compiler no longer knows the slot is not the memory block, so the store
    // in the loop below makes it read memory->unk1C again afterwards
    MapSlot *slot = NULL;
    MapSlotUnk08 *header;
    char *cursor;
    char *work;
    u32 *packet;
    int i;
    int offset;
    int size;
    u32 address;

    slot = &map_slots[map];
    memset(map_slot_objects[map], 0, sizeof(map_slot_objects[map]));
    memset(entity_pool[map], 0, sizeof(entity_pool[map]));
    header = (MapSlotUnk08 *)memory->file_base;
    work = memory->work_cursor;

    /* the working memory: the models, then the two tables of the slot */
    // match: stored as a model pointer (it is room for models); as a char * it is scheduled earlier
    *(ModelInstance **)&memory->record_models = (ModelInstance *)work;
    work += header->record_count * 0x1C0;
    memory->object_models = work;
    work += header->object_count * 0x1C0;
    memory->entity_models = (ModelInstance *)work;
    work += header->spawn_count * 0x1C0;
    memory->extra_models = work;
    work += header->extra_model_count * 0x1C0;
    slot->record_model_table = work;
    work += header->record_model_count * 4;
    slot->record_model_spare_bytes = work;
    work += header->record_model_count;
    // match: stored as a u8 *; as a char * the read of header->unk4C below comes one instruction late
    *(u8 **)&memory->work_cursor = (u8 *)work;

    /* the file: its fixed parts, then the tables whose sizes the header gives */
    cursor = (char *)header;
    slot->header = (MapHeader *)cursor;
    cursor = (char *)header + 0x80;
    slot->fog_blocks = (MapFogBlock *)cursor;
    cursor = (char *)header + 0x480;
    slot->light_blocks = cursor;
    cursor = (char *)header + 0x1480;
    slot->scroll_copy_textures = cursor;
    cursor = (char *)header + 0x1540;
    slot->scroll_draw_textures = cursor;
    cursor = (char *)header + 0x17C0;
    slot->records = cursor;
    cursor += header->record_count * 0x80;
    slot->flag_blocks = (MapFlagBlock *)cursor;
    cursor += header->flag_block_count * 0x40;
    slot->origins = (Vector4 *)cursor;
    cursor += 0x200;
    map_object_placements[map] = (int)cursor;
    cursor += header->object_count * 0x80;
    map_spawn_infos[map] = (EntitySpawnInfo *)cursor;
    cursor += header->spawn_count * 0x60;
    map_item_defs[map] = (MapItemDef *)cursor;
    cursor += header->item_count * 0x40;
    map_item_model_kinds[map] = (s16 *)cursor;
    cursor += 0x40;
    map_item_group_thresholds[map] = (u8 *)cursor;

    /* the parts the header gives offsets for */
    cursor = (char *)header;
    cursor += header->script_offset;
    slot->script = (s32)cursor;
    offset = header->sound_bank_offset;
    cursor = (char *)header;
    cursor += header->packet_table_offset;
    slot->packet_data = cursor;
    size = header->file_size;
    packet = (u32 *)cursor;
    for (i = 0; i < 6; i++) {
        // match: stored as a char *, the type memory->unk1C has (see the note on slot)
        ((char **)slot->packets.items)[i] = (char *)packet;
        packet += (*packet + 1) * 4;
    }
    cursor = (char *)header;
    cursor += offset;
    slot->sound_bank = (s32 *)cursor;
    /* two offsets in that part become addresses */
    ((char **)cursor)[1] += (s32)memory->file_base;
    ((char **)cursor)[3] += (s32)memory->file_base;
    address = (u32)memory->file_base;
    address += size;
    memory->file_read_cursor = (char *)align_16(address);
}


/* align_16 written another way; Map_BuildModels only matches with this one */
static inline u32 round_up_16(u32 address) {
    u32 remainder = address & 0xF;

    if (remainder != 0) {
        remainder -= 0x10;
        address -= remainder;
    }
    return address;
}

/* Builds the models of a map that was just read: one per record, then one per map object, with
   their collision meshes and motions. */
void Map_BuildModels(int map) {
    int cursor;
    char *work;
    int next;
    MapSlotMemory *memory = &map_slot_memory[map];
    MapSlot *slot = &map_slots[map];
    MapSlotUnk08 *header = (MapSlotUnk08 *)memory->file_base;
    int record_count;
    MapRecordBounds *record;
    MapRecordBounds *other;
    ModelInstance *model;
    // match: the three unused initial values below leave no code, but without them two saved values swap stack slots
    ModelInstance *first = NULL;
    MapObjectUnk08 *info = NULL;
    MapObject *object = NULL;
    int i;
    int j;
    int k;

    record_count = header->record_count;
    record = (MapRecordBounds *)slot->records;
    model = (ModelInstance *)memory->record_models;
    work = memory->work_cursor;
    work = (char *)round_up_16((u32)work);
    for (i = 0; i < record_count; i++, model++, record++) {
        Map_InsertRecordBySize((AreaNode *)record);
        MAP_RECORD_BITS(record)->map = (u8)map;
        record->model = model;
        if (record->model_index != 0xFFFF) {
            cursor = (int)header + header->record_models_offset;
            k = record->model_index;
            while (k-- > 0) {
                cursor += *(int *)cursor;
            }
            Model_SetUpPosed(model, (char **)&cursor, &work);
            if (record->record_flags & 0x100) {
                Vector_Copy(&model->position, record);
                model->position.w = 1.0f;
                ((void (*)(ModelInstance *, u32 *, int))Wobble_BuildWaveUvs)(model, (u32 *)&work, 0); // match: a third argument the callee does not take
            }
            if (((ModelInstance **)slot->record_model_table)[record->model_index] == NULL) {
                ((ModelInstance **)slot->record_model_table)[record->model_index] = model;
            }
            Model_SetPrimBits(model, 0x20, 0x20);
            if (record->record_flags & 0x202) {
                Model_SetPrimBits(model, 0x60, 0x60);
                Model_SetAlphaBlend(model, 0x48);
            }
        }
        if (record->collision_mesh_index != 0xFFFF) {
            other = (MapRecordBounds *)slot->records;
            for (j = 0; j < i; j++, other++) {
                if (record->collision_mesh_index == other->collision_mesh_index) {
                    break;
                }
            }
            if (j >= i) {
                cursor = (int)header + header->record_collision_offset;
                k = record->collision_mesh_index;
                while (k-- > 0) {
                    cursor += (int)((CollisionMesh *)cursor)->mesh_size;
                }
                cursor = (int)Collision_RelocateMesh((char *)cursor, model, 1);
            } else {
                model->strips = ((ModelInstance *)memory->record_models)[j].strips;
            }
        }
    }
    memory->work_cursor = work;
    slot->has_flag_80000000_record = 0;
    memset(map_slot_props[map], 0, sizeof(map_slot_props[map]));
    record = (MapRecordBounds *)slot->records;
    for (i = 0; i < record_count; i++, record++) {
        if (((MapRecord *)record)->prop_id != 0) {
            if (map_slot_props[map][((MapRecord *)record)->prop_id] == NULL) {
                map_slot_props[map][((MapRecord *)record)->prop_id] = (MapProp *)record;
            }
        }
        if ((s32)record->record_flags < 0) {
            slot->has_flag_80000000_record = 1;
        }
    }
    map_object_counts[map] = header->object_count;
    object = map_slot_objects[map];
    work = memory->work_cursor;
    model = (ModelInstance *)memory->object_models;
    info = *(MapObjectUnk08 **)&map_object_placements[map]; // match: read as a pointer; read as an int it waits for the stores above
    for (i = 0; i < header->object_count; i++, model++, info++, object++) {
        object->model = model;
        object->source = info;
        object->definition = (MapObjectUnk04 *)&map_object_defs[info->def_index * 0x80];
        object->map_record = (s32)Map_FindRecordAtInSlot((CameraState *)info, 0.0f, map);
        MAP_OBJECT_BITS(object)->map = (u8)map;
    }
    cursor = (int)header + header->object_model_offset;
    for (i = 0; i < header->plain_object_model_count; i++) {
        object = map_slot_objects[map];
        for (j = 0; j < header->object_count; j++, object++) {
            if (object->definition->model_index != 0xFFFF && !(MAP_OBJECT_DEF_WORD(object) & 0x100) && object->source->model_index == i) {
                next = cursor; // match: retail bug, only set when an object uses this model (see cursor = next)
                Model_SetUpPosed(object->model, (char **)&next, &work);
                object->model->texture_slot = 1;
                object->model->texture_data = next;
                next += *(int *)next;
                object->model->texture_packet_3 = 0;
                object->model->texture_packet_5 = 0;
            }
        }
        cursor = next; // match: retail bug, next is stale (or never set) when no object uses this model
    }
    cursor = (int)header + header->skinned_object_model_offset;
    for (i = 0; i < header->skinned_object_model_count; i++) {
        object = map_slot_objects[map];
        for (j = 0; j < header->object_count; j++, object++) {
            if (object->definition->model_index != 0xFFFF && (MAP_OBJECT_DEF_WORD(object) & 0x100) && object->source->model_index == i) {
                next = cursor; // match: retail bug, as in the loop above
                Model_SetUpSkinned(object->model, &next, (int *)&work);
                object->model->texture_slot = 1;
                object->model->texture_data = next;
                next += *(int *)next;
                object->model->texture_packet_3 = (int)Map_GetSlotPacket(map, 3);
                object->model->texture_packet_5 = (int)Map_GetSlotPacket(map, 5);
            }
        }
        cursor = next; // match: retail bug, as in the loop above
    }
    cursor = (int)header + header->object_collision_offset;
    for (i = 0; i < header->object_collision_count; i++) {
        next = cursor;
        object = map_slot_objects[map];
        first = NULL;
        for (j = 0; j < header->object_count; j++, object++) {
            if (object->definition->collision_mesh_index != 0xFFFF && object->source->collision_mesh_index == i) {
                if (first == NULL) {
                    cursor = (int)Collision_RelocateMesh((char *)next, object->model, 1);
                    first = object->model;
                } else {
                    object->model->strips = first->strips;
                }
            }
        }
    }
    cursor = (int)header + header->object_motion_offset;
    for (i = 0; i < header->object_motion_count; i++) {
        object = map_slot_objects[map];
        for (j = 0; j < header->object_count; j++, object++) {
            if (object->definition->model_index != 0xFFFF && (MAP_OBJECT_DEF_WORD(object) & 0x200) && object->source->motion_index == i) {
                next = ((int (*)(int, int *, ModelInstance *, int))ModelMotion_LoadFile)(cursor, (int *)&work, object->model, 1); // match: the callee returns where the motion ends
            }
        }
        cursor = next; // match: retail bug, as above: next is only set when an object uses this motion
    }
    memory->work_cursor = work;
}

/* Makes the other map slot the current one: moves everything by the load's offset and switches
   the sound over. */
void Map_MakeCurrent(int map) {
    MapSlot *slot;
    SoundLoadQueue *queue;
    f32 *offset;
    MapSlot *loading;

    if (current_map_slot != map) {
        offset = &map_load_state.position.x;
        slot = &map_slots[map];
        queue = &sound_load_queue;
        current_map_slot = map;
        Player_MoveByOffset(offset);
        Interact_OffsetViewRay(offset);
        Map_OffsetRecords(0, offset);
        Map_OffsetRecords(1, offset);
        Map_PlaceRecordModels(0);
        Map_PlaceRecordModels(1);
        MapObject_OffsetAll(0, offset);
        MapObject_OffsetAll(1, offset);
        Entity_OffsetAllInSlot(0, offset);
        Entity_OffsetAllInSlot(1, offset);
        Effect_OffsetAll(offset);
        LinkedPoint_TransformFollows(offset);
        MapItem_MoveMap(0, offset);
        MapItem_MoveMap(1, offset);
        Gold_MoveMap(0, offset);
        Gold_MoveMap(1, offset);
        map_load_state.position.x = -map_load_state.position.x;
        map_load_state.position.y = -map_load_state.position.y;
        map_load_state.position.z = -map_load_state.position.z;
        Script_Init(map_slots[map].script);
        Script_Start(0x7FFF, 0);
        loading = queue->pending_map_slot;
        if (loading != slot && queue->next_map_slot != slot) {
            Player_StopFootstepSound();
            Sound_StopAll(1);
            queue->next_map_slot = slot;
        } else if (loading == slot && sound_load_queue.next_map_slot != slot) {
            sound_load_queue.next_map_slot = NULL;
        }
        sound_load_queue.bgm_id = ((MapSlotUnk08 *)map_slots[map].header)->sound_bank_file_index;
        sound_load_queue.bgm_load_pending = 1;
        MapScroll_CarryPairs(map == 0, map);
    }
}

/* Moves every record of a map and its model by an offset. */
void Map_OffsetRecords(int map, f32 *offset) {
    MapRecordBounds *record;
    u16 count;
    int remaining;
    MapSlot *slot;

    slot = &map_slots[map];
    record = (MapRecordBounds *)slot->records;
    if (record != NULL) {
        count = ((MapSlotUnk08 *)slot->header)->record_count;
        if (count != 0) {
            remaining = count;
            do {
                Map_OffsetRecord(record, offset);
                remaining--;
                record++;
            } while (remaining != 0);
        }
    }
}

/* Moves a map record and its model by an offset. */
void Map_OffsetRecord(MapRecordBounds *record, f32 *offset) {
    if (record != NULL) {
        Vector_Add(&record->model->position, &record->model->position, offset);
        Vector_Add(record, record, offset);
        Vector_Add(record->box_lowest, record->box_lowest, offset);
        Vector_Add(record->box_highest, record->box_highest, offset);
        Vector_Add(record->lowest, record->lowest, offset);
        Vector_Add(record->highest, record->highest, offset);
    }
}

/* Puts the model of every record of a map where the record says, and builds its matrices. */
void Map_PlaceRecordModels(int map) {
    MapSlot *slot = &map_slots[map];
    MapRecordBounds *record;
    ModelInstance *model;
    f32 (*matrix)[4];
    int i;
    int j;
    int count;

    record = (MapRecordBounds *)slot->records;
    if (record != NULL) {
        count = ((MapSlotUnk08 *)slot->header)->record_count;
        model = record->model;
        for (i = 0; i < count; i++, record++, model++) {
            Vector_Copy(&model->position, record->position);
            model->position.w = 1.0f;
            Vector_Copy(&model->rotation, record->rotation);
            model->rotation.w = 1.0f;
            Vector_Copy(&model->scale_x, record->scale);
            model->scale_w = 1.0f;
            matrix = (f32 (*)[4])model->model_matrix;
            sceVu0CopyMatrix(matrix, identity_matrix);
            MODEL_MATRIX(model)[0][0] *= model->scale_x;
            MODEL_MATRIX(model)[1][1] *= model->scale_y;
            MODEL_MATRIX(model)[2][2] *= model->scale_z;
            sceVu0RotMatrixX((f32 *)matrix, (f32 *)matrix, model->rotation.x);
            sceVu0RotMatrixY((f32 *)matrix, (f32 *)matrix, model->rotation.y);
            sceVu0RotMatrixZ((f32 *)matrix, (f32 *)matrix, model->rotation.z);
            sceVu0TransMatrix((f32 *)matrix, (f32 *)matrix, &model->position.x);
            Matrix_Multiply(model, world_screen_matrix, matrix);
            for (j = 0; j < model->part_count; j++) {
                // match: the callee returns a value here, which keeps $2 busy
                ((int (*)(ModelInstance *, ModelPart *, ModelPart *))ModelMatrix_BuildPartLocal)(model, &model->parts[j], &model->parts[j]);
            }
            sceVu0CopyMatrix(model->collision_matrix, matrix);
        }
    }
}

/* Links a map record into one of the two lists of record boxes (by its flag 0x80000000), keeping each
   sorted from the smallest box (x by z) up, so that a search finds the smallest box first. Records
   with flag 0x02000000 are left out. */
void Map_InsertRecordBySize(AreaNode *node) {
    AreaNode **head;
    AreaNode *prev;
    AreaNode *cur;
    f32 area;

    if (!(node->flags & 0x02000000)) {
        head = (node->flags & 0x80000000) ? &backdrop_record_boxes : &record_boxes;
        area = (node->right - node->left) * (node->bottom - node->top);
        prev = NULL;
        cur = *head;
        while (cur != NULL && !(area < (cur->right - cur->left) * (cur->bottom - cur->top))) {
            prev = cur;
            cur = cur->next;
        }
        if (prev != NULL) {
            prev->next = node;
        } else {
            *head = node;
        }
        node->next = cur;
    }
}

/* Takes the records of a map slot out of both lists of record boxes. */
void Map_UnlinkSlotRecords(int map) {
    AreaNode *prev;
    AreaNode *node;

    prev = NULL;
    node = record_boxes;
    while (node != NULL) {
        if (((u8)(AREA_NODE_WORD(node) >> 27) & 1) == map) {
            if (prev != NULL) {
                prev->next = node->next;
            } else {
                record_boxes = node->next;
            }
        } else {
            prev = node;
        }
        node = node->next;
    }
    if (prev == NULL) {
        record_boxes = NULL;
    }

    prev = NULL;
    node = backdrop_record_boxes;
    while (node != NULL) {
        if (((u8)(AREA_NODE_WORD(node) >> 27) & 1) == map) {
            if (prev != NULL) {
                prev->next = node->next;
            } else {
                backdrop_record_boxes = node->next;
            }
        } else {
            prev = node;
        }
        node = node->next;
    }
    if (prev == NULL) {
        backdrop_record_boxes = NULL;
    }
}

/* Returns the map record of the first list whose box holds the point (each side widened by margin).
   Failing that, the box nearest above or below it, then the nearest in z, then the nearest in x; NULL
   when none lines up with it. */
AreaNode *Map_FindRecordAt(f32 *point, f32 margin) {
    AreaNode *head = record_boxes;
    AreaNode *node;
    AreaNode *best;
    f32 best_distance;
    f32 distance;

    for (node = head; node != NULL; node = node->next) {
        if (USABLE(node) && IN_X(node) && IN_Y(node) && IN_Z(node)) {
            return node;
        }
    }
    best_distance = D_00446768[0];
    best = NULL;
    for (node = head; node != NULL; node = node->next) {
        if (USABLE(node) && IN_X(node) && IN_Z(node)) {
            DISTANCE(node->lowest_y, node->highest_y, point[1]);
        }
    }
    if (best != NULL) {
        return best;
    }
    for (node = head; node != NULL; node = node->next) {
        if (USABLE(node) && IN_X(node) && IN_Y(node)) {
            DISTANCE(node->top, node->bottom, point[2]);
        }
    }
    if (best != NULL) {
        return best;
    }
    for (node = head; node != NULL; node = node->next) {
        if (USABLE(node) && IN_Y(node) && IN_Z(node)) {
            DISTANCE(node->left, node->right, point[0]);
        }
    }
    return best;
}

/* Map_FindRecordAt among the records of one map slot. */
MapRecordInfo *Map_FindRecordAtInSlot(CameraState *position, f32 margin, int map) {
    f32 *point = (f32 *)position;
    AreaNode *head = record_boxes;
    AreaNode *node;
    AreaNode *best;
    f32 best_distance;
    f32 distance;

    for (node = head; node != NULL; node = node->next) {
        if (USABLE_IN_MAP(node) && IN_X(node) && IN_Y(node) && IN_Z(node)) {
            return (MapRecordInfo *)node;
        }
    }
    best_distance = D_0044676C[0];
    best = NULL;
    for (node = head; node != NULL; node = node->next) {
        if (USABLE_IN_MAP(node) && IN_X(node) && IN_Z(node)) {
            DISTANCE(node->lowest_y, node->highest_y, point[1]);
        }
    }
    if (best != NULL) {
        return (MapRecordInfo *)best;
    }
    for (node = head; node != NULL; node = node->next) {
        if (USABLE_IN_MAP(node) && IN_X(node) && IN_Y(node)) {
            DISTANCE(node->top, node->bottom, point[2]);
        }
    }
    if (best != NULL) {
        return (MapRecordInfo *)best;
    }
    for (node = head; node != NULL; node = node->next) {
        if (USABLE_IN_MAP(node) && IN_Y(node) && IN_Z(node)) {
            DISTANCE(node->left, node->right, point[0]);
        }
    }
    return (MapRecordInfo *)best;
}

/* Tells whether the point lies inside the node's box grown by margin on every side. */
int Map_IsPointInRecord(f32 *point, AreaNode *node, f32 margin) {
    int inside;

    if (node != NULL) {
        inside = FALSE;
        if (!(node->flags & 0x40000000) && node->left - margin <= point[0] && point[0] <= node->right + margin &&
            node->lowest_y - margin <= point[1] && point[1] <= node->highest_y + margin &&
            node->top - margin <= point[2] && point[2] <= node->bottom + margin) {
            inside = TRUE;
        }
        return inside;
    }
    return FALSE;
}

/* Returns the first map record of the slot with id prop_id (MapRecord.prop_id), NULL when there is none or
   either is out of range. */
void *Map_GetRecordById(int slot, int prop_id) {
    if ((u32)slot < 2 && prop_id >= 0 && prop_id < 0x100) {
        return map_slot_props[slot][prop_id];
    }
    return NULL;
}

/* The free-camera map viewer: while GameStatus.bit7 is set it runs its own frame loop, in which the pad
   turns the camera about the player (with PAD_CIRCLE held) or moves it, and both PAD_L1 and PAD_R1 put it back. */
void Map_RunFreeCamera(void) {
    VuVector4 position;
    VuVector4 rotation;
    int reset_held;

    reset_held = FALSE;
    memset(&position, 0, sizeof(position));
    position.z = 10000.0f;
    rotation = (VuVector4){ -1.5707964f, player_state.rotation.y, 0.0f, 0.0f };
    if (game_status.bit7) {
        f32 world_screen[16];
        f32 view_screen[16];
        f32 world_view[16];
        f32 unused[16];

        sceVu0ViewScreenMatrix(view_screen, 512.0f, 1.0f, 1.0f, 2048.0f, 2048.0f, 1.0f, 1048560.0f, 1.0f, 65535.0f);
        do {
            VuVector4 move;

            Display_SwapBuffers(0);
            Packet_SendChain();
            Packet_SwapBuffers();
            DrawList_Clear();
            Input_Update();
            move.x = move.y = move.z = move.w = 0.0f;
            if (pad_state[0] & PAD_CIRCLE) {
                if (pad_state[0] & PAD_DOWN) {
                    rotation.x = Angle_Wrap(rotation.x + 0.08726646f);
                }
                if (pad_state[0] & PAD_UP) {
                    rotation.x = Angle_Wrap(rotation.x - 0.08726646f);
                }
                if (pad_state[0] & PAD_LEFT) {
                    rotation.y = Angle_Wrap(rotation.y + 0.08726646f);
                }
                if (pad_state[0] & PAD_RIGHT) {
                    rotation.y = Angle_Wrap(rotation.y - 0.08726646f);
                }
            } else {
                if (pad_state[0] & PAD_LEFT) {
                    move.x = -1.0f;
                }
                if (pad_state[0] & PAD_RIGHT) {
                    move.x += 1.0f;
                }
                if (pad_state[0] & PAD_UP) {
                    move.y -= 1.0f;
                }
                if (pad_state[0] & PAD_DOWN) {
                    move.y += 1.0f;
                }
            }
            if (!reset_held) {
                if ((*(s32 *)pad_state & (PAD_L1 | PAD_R1)) == (PAD_L1 | PAD_R1)) {
                    reset_held = TRUE;
                    position.z = 10000.0f;
                    rotation.x = -1.5707964f;
                    rotation.y = player_state.rotation.y;
                    position.x = 0.0f;
                    position.y = 0.0f;
                    position.w = 0.0f;
                    rotation.z = 0.0f;
                    rotation.w = 0.0f;
                } else if (pad_state[0] & PAD_L1) {
                    move.z += 1.0f;
                } else if (pad_state[0] & PAD_R1) {
                    move.z -= 1.0f;
                }
            } else if (!(*(s32 *)pad_state & (PAD_L1 | PAD_R1))) {
                reset_held = FALSE;
            }
            if (move.x != 0.0f || move.y != 0.0f || move.z != 0.0f) {
                Vector_Normalize(&move, &move);
                Vector_Scale(&move.x, &move.x, 100.0f);
                Vector_Add(&position, &position, &move);
                if (position.z < 2500.0f) {
                    position.z = 2500.0f;
                }
                if (position.z > 50000.0f) {
                    position.z = 50000.0f;
                }
            }
            Vector_Scale(&move.x, &player_state.position.x, -1.0f);
            sceVu0TransMatrix(world_view, identity_matrix, &move.x);
            sceVu0RotMatrixY(world_view, world_view, -rotation.y);
            sceVu0RotMatrixX(world_view, world_view, -rotation.x);
            sceVu0TransMatrix(world_view, world_view, &player_state.position.x);
            Vector_Subtract(&move, &player_state, &position);
            Vector_Scale(&move.x, &move.x, -1.0f);
            sceVu0TransMatrix(world_view, world_view, &move.x);
            sceVu0MulMatrix(world_screen, view_screen, world_view);
            GsDraw_QueueVu1Matrix(world_screen);
            Packet_OnFrameStart();
            Map_DrawFreeCameraView();
            Message_Update();
            Packet_OnFrameEnd();
            GameLoop_RunSoundLoads();
            Packet_WaitForSend();
        } while (!(pad_state[1] & PAD_CIRCLE));
        game_status.bit7 = FALSE;
    }
}

/* Draws the view of Map_RunFreeCamera: every record of both map slots with one plain light block, then
   four quarter-screen rectangles on top. */
void Map_DrawFreeCameraView(void) {
    ScreenQuad quad;
    MapSlot *slot;
    MapDrawRecord *record;
    int map;
    int i;

    TextureTable_QueueAll(D_00446790);
    GsDraw_QueueTexFlush();
    game_dbuff_clear_color[0] = 0;
    game_dbuff_clear_color[30] = 0;
    game_dbuff_context2_clear_color[0] = 0;
    game_dbuff_context2_clear_color[30] = 0;
    Packet_BeginGsRegisters();
    Packet_AddGsRegister(0x1A, 0);
    Packet_AddGsRegister(0x1B, 0x248);
    Packet_AddGsRegister(GS_REG_ALPHA_2, 0x4000000068L);
    Packet_AddGsRegister(GS_REG_TEST_2, 0x3080B);
    Packet_AddGsRegister(GS_REG_ZBUF_2, 0x10A000118L);
    Packet_EndGsRegisters();
    GsDraw_QueueFlush();
    for (map = 0; map < 2; map++) {
        slot = &map_slots[map];
        record = (MapDrawRecord *)slot->records;
        if (slot->loaded != 0) {
            for (i = 0; i < ((MapSlotUnk08 *)slot->header)->record_count; i++, record++) {
                if (!(record->flags & 0x80000000)) {
                    if (record->flags & 0x10) {
                        record->model->light_block = (s32)&map_draw_state.current_light_block;
                        record->model->fog_block = &map_draw_state.current_fog_block;
                    } else {
                        record->model->light_block = (s32)&map_draw_state.light_blocks[map][record->light_block_index];
                        record->model->fog_block = &map_draw_state.fog_blocks[map][record->fog_block_index];
                    }
                    record->model->hit_count = 0;
                    // match: both blocks chosen above are replaced at once by the viewer's own
                    record->model->light_block = (s32)&map_object_default_lights;
                    record->model->fog_block = &map_draw_state.current_fog_block;
                    Model_DrawPosed(record->model);
                }
            }
        }
    }
    Game_BlendScreenOffsetDown();
    Packet_BeginGsRegisters();
    Packet_AddGsRegister(0x1A, 1);
    Packet_AddGsRegister(GS_REG_ZBUF_2, 0xA000118);
    Packet_AddGsRegister(GS_REG_TEST_1, 0x31001);
    Packet_AddGsRegister(GS_REG_ALPHA_1, 0x48);
    Packet_AddGsRegister(8, 5);
    Packet_EndGsRegisters();
    GsDraw_QueueFlush();
    memset(&quad, 0, sizeof(quad));
    quad.tag = 0x4007A80621312E40L;
    quad.unk08 = 0x60;
    quad.prim = 0x156;
    quad.red = 0x80;
    quad.green = 0x80;
    quad.blue = 0x80;
    quad.alpha = 0x40;
    quad.first_x = 0x6C00;
    quad.first_y = 0x7200;
    quad.first_z = 0xFFFFFFFF;
    quad.first_u = 0;
    quad.first_v = 0;
    quad.second_x = 0x8000;
    quad.second_y = 0x8000;
    quad.second_z = 0xFFFFFFFF;
    quad.second_u = 0x1000;
    quad.second_v = 0x1000;
    GsDraw_QueueTexturedSprite(&quad);
    quad.first_x = 0x9400;
    quad.first_y = 0x7200;
    quad.second_x = 0x8000;
    quad.second_y = 0x8000;
    GsDraw_QueueTexturedSprite(&quad);
    quad.first_x = 0x6C00;
    quad.first_y = 0x8E00;
    quad.second_x = 0x8000;
    quad.second_y = 0x8000;
    GsDraw_QueueTexturedSprite(&quad);
    quad.first_x = 0x9400;
    quad.first_y = 0x8E00;
    quad.second_x = 0x8000;
    quad.second_y = 0x8000;
    GsDraw_QueueTexturedSprite(&quad);
    Packet_BeginGsRegisters();
    Packet_AddGsRegister(8, 0);
    Packet_EndGsRegisters();
    GsDraw_QueueFlush();
}

/* Points both drawing contexts at the 256-pixel-wide 16-bit buffer at GS block 0x2BC0 with a 256x256
   scissor: draw mode 2 draws its second view there (GameLoop_DrawFrame). */
void Map_SetOffscreenTarget(void) {
    GsDraw_SetDrawTarget(0xA04015E, 0xFF000000FF0000L, 0x780000007800L);
}

/* Stretches 320x224 texels of the 16-bit buffer at GS block 0x2BC0 over the whole 640x448 screen, z
   writes off and with register 8 set to 5 meanwhile. Nothing calls it. */
void Map_StretchOffscreenToScreen(void) {
    ScreenQuad quad;

    Packet_BeginGsRegisters();
    Packet_AddGsRegister(GS_REG_TEST_1, 0x30000);
    Packet_AddGsRegister(GS_REG_ZBUF_1, 0x10A000118L);
    Packet_AddGsRegister(8, 5);
    Packet_EndGsRegisters();
    quad.tag = 0x624A16BC0L;
    quad.unk08 = 0x60;
    quad.prim = 0x116;
    quad.red = 0x80;
    quad.green = 0x80;
    quad.blue = 0x80;
    quad.alpha = 0x80;
    quad.first_x = 0x6C00;
    quad.first_y = 0x7200;
    quad.second_x = 0x9400;
    quad.second_y = 0x8E00;
    quad.second_u = 0x1400;
    quad.second_v = 0xE00;
    quad.first_z = 0;
    quad.first_u = 0;
    quad.first_v = 0;
    quad.second_z = 0;
    GsDraw_QueueTexturedSprite(&quad);
    Packet_BeginGsRegisters();
    Packet_AddGsRegister(GS_REG_ZBUF_1, 0xA000118);
    Packet_AddGsRegister(8, 0);
    Packet_EndGsRegisters();
}

/* Covers the screen with the colour in bytes 0x78-0x7B of the current map's header, blended (draw mode 1
   does this between its two views). */
void Map_CoverScreenWithMapTint(void) {
    MapSlot *slot;
    MapSlotUnk08 *header;

    slot = &map_slots[current_map_slot];
    header = (MapSlotUnk08 *)slot->header;
    GsDraw_FillScreen(header->screen_tint_red, header->screen_tint_green, header->screen_tint_blue, header->screen_tint_alpha, 0x44);
}

/* Queues prebuilt packet index of the map slot (MapSlot.packets) for sending. */
void Map_QueueSlotPacket(int slot, int index) {
    MapSlot *map_slot = &map_slots[slot];
    u32 *packet_start;
    u32 *packet;

    packet_start = packet = Packet_Alloc(3);
    *packet++ = *map_slot->packets.items[index] | 0x30000000;
    *packet++ = (u32)map_slot->packets.items[index] + 0x10;
    *packet++ = 0;
    *packet++ = 0;
    Packet_AppendToChain(packet_start, packet);
}

/* Queues a packet for sending. */
void Map_QueuePacket(int *packet) {
    u32 *packet_start;
    u32 *cursor;

    if (packet != 0) {
        packet_start = cursor = Packet_Alloc(2);
        *cursor++ = *packet | 0x30000000;
        *cursor++ = (u32)packet + 0x10;
        *cursor++ = 0;
        *cursor++ = 0;
        Packet_AppendToChain(packet_start, cursor);
    }
}

/* Returns packet index of the map slot. */
u32 *Map_GetSlotPacket(int slot, int index) {
    MapSlot *map_slot = &map_slots[slot];
    MapSlotPackets *packets = &map_slot->packets;

    if (packets != NULL) {
        return packets->items[index];
    }
    return NULL;
}

/* Tests flag bit of flag block index of the slot's map; for a thing of the other map (other_slot) looks for
   it among the block's 16 pairs of that map's tag and flag instead. Returns 1 when set. */
int Map_TestFlagBlockBit(int slot, int index, int other_slot, int bit) {
    MapFlagBlock *block = map_slots[slot].flag_blocks + index;
    int other_offset = other_slot * 0x60; /* byte offset: retail multiplies before the branch */
    int i;

    if (slot == other_slot) {
        return (block->bits[bit / 8] >> (bit - bit / 8 * 8)) & 1;
    }
    for (i = 0; i < 0x10; i++) {
        if (block->tags[i] == ((MapSlot *)((char *)map_slots + other_offset))->tag && block->values[i] == bit) {
            return 1;
        }
    }
    return 0;
}

/* Projects the corners of a map record's box to the screen: 1 when none is out of the GS's range (projected
   z above 0xFFFF0), when the test is off or the record is exempt, -1 when all eight are, 0 when some are (the
   callers then cut the clipped triangles up, ModelScreen_CutClippedTriangles). The model is not used. */
int Map_ClassifyRecordOnScreen(MapDrawRecord *draw_record, ModelInstance *model) {
    MapRecord *record = (MapRecord *)draw_record;
    MapRecordBounds *bounds = (MapRecordBounds *)draw_record;
    f32 matrix[16];
    f32 corner0[4];
    f32 corner1[4];
    f32 corner2[4];
    int screen0[4];
    int screen1[4];
    int screen2[4];
    int outside;

    if (!MAP_DRAW_FLAG_BITS(&map_draw_state)->bit46) {
        return 1;
    }
    if (bounds->record_flags & 0x40) {
        return 1;
    }
    if (bounds->model_index == 0xFFFF) {
        return 1;
    }
    if (Map_TestFlagBlockBit(current_map_slot, player_floor_block[0], (u8)(record->record_flags >> 27) & 1, (u8)record->record_flags) == 0) {
        return 1;
    }
    sceVu0MulMatrix(matrix, world_screen_matrix, identity_matrix);
    Vu0_LoadMatrix(matrix);
    corner2[3] = 1.0f;
    corner1[3] = 1.0f;
    corner0[3] = 1.0f;
    corner0[0] = bounds->box_lowest[0];
    corner0[1] = bounds->box_lowest[1];
    corner0[2] = bounds->box_lowest[2];
    corner1[0] = bounds->box_lowest[0];
    corner1[1] = bounds->box_lowest[1];
    corner1[2] = bounds->box_highest[2];
    corner2[0] = bounds->box_lowest[0];
    corner2[1] = bounds->box_highest[1];
    corner2[2] = bounds->box_lowest[2];
    Vu0_ProjectToScreen3(screen0, screen1, screen2, corner0, corner1, corner2);
    outside = (u32)screen0[2] > 0xFFFF0;
    outside += (u32)screen1[2] > 0xFFFF0;
    outside += (u32)screen2[2] > 0xFFFF0;
    corner0[0] = bounds->box_lowest[0];
    corner0[1] = bounds->box_highest[1];
    corner0[2] = bounds->box_highest[2];
    corner1[0] = bounds->box_highest[0];
    corner1[1] = bounds->box_lowest[1];
    corner1[2] = bounds->box_lowest[2];
    corner2[0] = bounds->box_highest[0];
    corner2[1] = bounds->box_lowest[1];
    corner2[2] = bounds->box_highest[2];
    Vu0_ProjectToScreen3(screen0, screen1, screen2, corner0, corner1, corner2);
    outside += (u32)screen0[2] > 0xFFFF0;
    outside += (u32)screen1[2] > 0xFFFF0;
    outside += (u32)screen2[2] > 0xFFFF0;
    corner0[0] = bounds->box_highest[0];
    corner0[1] = bounds->box_highest[1];
    corner0[2] = bounds->box_lowest[2];
    Vu0_ProjectToScreen(screen0, corner0);
    outside += (u32)screen0[2] > 0xFFFF0;
    corner0[0] = bounds->box_highest[0];
    corner0[1] = bounds->box_highest[1];
    corner0[2] = bounds->box_highest[2];
    Vu0_ProjectToScreen(screen0, corner0);
    outside += (u32)screen0[2] > 0xFFFF0;
    if (outside == 0) {
        return 1;
    }
    if (outside == 8) {
        return -1;
    }
    return 0;
}
