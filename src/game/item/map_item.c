#include "model/model.h"
#include "player/player.h"
#include "core/game.h"
#include "map/map.h"
#include "entity/entity.h"
#include "collision/collision.h"
#include "model/linked_point.h"
#include "ui/message.h"
#include "input/pad.h"
#include "map_object/map_object.h"
#include "core/file.h"
#include "enemy/enemy.h"
#include "gfx/gs_draw.h"
#include "gfx/packet.h"
#include "item/item.h"
#include <libvu0.h>
#include <math.h>
#include "math/angle.h"
#include "math/vu0.h"
#include "model/model_matrix.h"
#include "player/player_update.h"

/* Radians the held item turns each frame while the pickup is held. */
#define PICKUP_HOLD_SPIN_PER_FRAME 0.08377580f

void MapItem_ClearMap(int);
s32 MapItem_Drop(s32, s32, s32, f32 *, f32, f32 *, f32 *, f32 *, u32);
ItemModelSource *MapItem_FindModelSource(int, int);
void Model_SetTextureBase(char *, int, int);
void Model_SetVertexColour(void *, int, int, int, int);
void TextureTable_SetBasePointers(char *, int, int);
void MapItem_PrepareDraw(ModelInstance *model);

void MapItem_PickupAim(void);
void MapItem_PickupBringUp(void);
void MapItem_PickupHold(void);
void MapItem_PickupTake(void);
void MapItem_PickupPutBack(void);

/* 0x4147E0 the steps of the item pickup in progress, MapItem_UpdatePickup runs the current one */
void (*pickup_step_handlers[6])(void) = { MapItem_PickupAim, MapItem_PickupBringUp, MapItem_PickupHold, MapItem_PickupTake, MapItem_PickupPutBack, 0 };

int MapItem_PlaceFixed(int, int);
ItemNode *MapItem_Unlink(ItemNode *);
int MapItem_GetFixedIndex(int, int);
void MapItem_Link(ListNode *);
int MapItem_UniqueNotCarried(int);
int MapItem_UniqueNotInTradeStock(int);
int MapItem_UniqueNotDropped(int);

extern GameDefinition item_name_defs[380];
extern DroppedItemRecord dropped_item_records[0x80][16];
extern u8 map_item_pickup_counts[0x80][0x40];
extern MapDrawState map_draw_state;
extern GameStatus game_status;
extern EventRunner view_ray_status;
extern u8 item_counts[];
extern u8 trade_stock_1[];
extern u8 trade_stock_2[];
extern u8 trade_stock_3[];
extern u8 trade_stock_4[];
extern u8 trade_stock_5[];

/* The per-map item blocks: nodes, models and the model data behind them, one set for each map. */
static ItemModelSource item_model_sources[2][32];   /* 0x57E040 where the model and texture of each item kind lie */
static ItemNode fixed_items[2][64];          /* 0x57E240 the fixed items of the map */
static ItemNode dropped_items[2][16];          /* 0x580040 the dropped items of the map */
static ItemNode object_items[2][32];          /* 0x5807C0 the items the player may pick up */
static ModelInstance fixed_item_models[2][64];     /* 0x5816C0 the models of the fixed items */
static ModelInstance dropped_item_models[2][16];     /* 0x58F6C0 the models of the dropped items */
static ModelInstance object_item_models[2][32];     /* 0x592EC0 the models of the pickup items */
static char fixed_item_model_data[2][64][0x200];       /* 0x599EC0 the model data of the fixed items */
static char dropped_item_model_data[2][16][0x200];       /* 0x5A9EC0 the model data of the dropped items */
static char object_item_model_data[2][32][0x200];       /* 0x5ADEC0 the model data of the pickup items */
static s8 fixed_item_by_id[2][0x100];             /* 0x5B5EC0 the node of each item slot, -1 for none */
static f32 pickup_start_position[4];                   /* 0x5B60C0 where the item being picked up is */
static f32 pickup_start_rotation[4];                   /* 0x5B60D0 its rotation */
static f32 pickup_target_position[4];                   /* 0x5B60E0 where it is being taken to */
static f32 pickup_velocity[4];                   /* 0x5B60F0 its speed */

/* Resets the item code: no items lying about, none being picked up, both map slots' tables cleared; and
   every item definition whose state is 0 gets 1. */
void MapItem_Init(void) {
    LevelDef *def;
    s32 i;

    lying_items = NULL;
    pickup_pose_scale = 1.0f;
    pickup_item = NULL;
    pickup_no_put_back = 0;
    pickup_step = 0;
    item_drop_counter = 0;
    MapItem_ClearMap(0);
    MapItem_ClearMap(1);
    def = (LevelDef *)item_name_defs;
    for (i = 0; i < 0x17C; i++, def++) {
        if (def->state == 0) {
            def->state = 1;
        }
    }
}

/* Sets up a map's items once it is loaded: notes where the model and texture of each kind of
   item lie in the map file, gives every fixed item its node and model, and lays the dropped
   items of the map's save record down again. */
void MapItem_InitMap(int map) {
    DroppedItemRecords saved;
    f32 position[4];
    f32 rotation[4];
    f32 scale[4];
    f32 offset[4];
    char *model_data;
    char *cursor;
    MapSlot *slot = &map_slots[map];
    s16 *kinds = map_item_model_kinds[map];
    MapHeader *header;
    MapItemDef *def;
    ItemNode *node;
    GameDefinition *definition;
    ItemModelSource *source;
    DroppedItemRecord *record;
    s32 count;
    s32 i;

    memset(offset, 0, sizeof(offset));
    header = slot->header;
    map_fixed_item_counts[map] = header->item_count;
    cursor = (char *)header + header->item_models_offset;
    for (i = 0; i < 32 && kinds[i] >= 0; i++) {
        item_model_sources[map][i].model_data = cursor;
        cursor += *(s32 *)cursor;
        item_model_sources[map][i].texture = cursor;
        cursor += *(s32 *)cursor;
    }
    count = map_fixed_item_counts[map];
    def = map_item_defs[map];
    node = fixed_items[map];
    for (i = 0; i < count; i++, def++, node++) {
        fixed_item_by_id[map][def->slot_id] = i;
        definition = &item_name_defs[def->item];
        source = MapItem_FindModelSource(map, definition->model_file_number);
        if (source != NULL) {
            ITEM_NODE_BITS(node)->map = map;
            node->def = def;
            node->item = def->item;
            node->record = NULL;
            node->index = i;
            node->model = &fixed_item_models[map][i];
            model_data = source->model_data;
            cursor = fixed_item_model_data[map][i];
            Model_SetUpPosed(node->model, &model_data, &cursor);
            Model_SetTextureBase((char *)node->model, 0x3B40, 0x3EC0);
            Model_SetVertexColour(node->model, 0xFF, 0xFF, 0xFF, -1);
            node->model->draw = MapItem_PrepareDraw;
            node->model->texture_data = (int)source->texture;
            node->model->texture_slot = 2;
            TextureTable_SetBasePointers(source->texture, 0x3B40, 0x3EC0);
        }
    }
    saved = *(DroppedItemRecords *)dropped_item_records[map_slots[map].tag];
    memset(dropped_item_records[map_slots[map].tag], 0, sizeof(dropped_item_records[0]));
    position[3] = rotation[3] = scale[3] = 1.0f;
    record = saved.records;
    for (i = 0; i < 16; i++, record++) {
        if (record->used) {
            position[0] = record->position[0];
            position[1] = record->position[1];
            position[2] = record->position[2];
            rotation[0] = record->rotation[0] * ITEM_RECORD_FIXED_SCALE;
            rotation[1] = record->rotation[1] * ITEM_RECORD_FIXED_SCALE;
            rotation[2] = record->rotation[2] * ITEM_RECORD_FIXED_SCALE;
            scale[0] = record->scale[0] * ITEM_RECORD_FIXED_SCALE;
            scale[1] = record->scale[1] * ITEM_RECORD_FIXED_SCALE;
            scale[2] = record->scale[2] * ITEM_RECORD_FIXED_SCALE;
            MapItem_Drop(map, record->item, record->priority, position, position[1], offset, rotation, scale,
                          record->order);
        }
    }
}

/* Rolls which of a map's fixed items appear: one roll per group against the map's threshold
   for it, then one per item against its own chance. */
void MapItem_RollFixedItems(int map) {
    u8 rolls[16];
    u8 *thresholds = map_item_group_thresholds[map];
    s32 i;
    ItemNode *node;

    for (i = 0; i < 16; i++) {
        rolls[i] = rand() * 100.0f * RAND_SCALE;
    }
    node = fixed_items[map];
    for (i = 0; i < map_fixed_item_counts[map]; i++, node++) {
        if (rolls[node->def->group] < thresholds[node->def->group]) {
            if ((s32)(rand() * 100.0f * RAND_SCALE) < node->def->chance) {
                MapItem_PlaceFixed(map, i);
            }
        }
    }
}

/* Unlinks every item of a map from the list of the ones lying about and clears the map's item tables. */
void MapItem_UnloadMap(int map) {
    ItemNode *node;

    node = lying_items;
    while (node != 0) {
        if ((node->flags & 1) == map) {
            node = MapItem_Unlink(node);
        } else {
            node = node->next;
        }
    }
    MapItem_ClearMap(map);
}

/* Moves every item of a map by `offset`: the fixed items' definitions and models, and the
   dropped items lying about. */
void MapItem_MoveMap(int map, f32 *offset) {
    s32 i;
    ItemNode *node;

    for (i = 0; i < map_fixed_item_counts[map]; i++) {
        if (fixed_items[map][i].def != NULL) {
            Vector_Add(fixed_items[map][i].def, fixed_items[map][i].def, offset);
        }
        if (fixed_items[map][i].model != NULL) {
            Vector_Add(&fixed_items[map][i].model->position, &fixed_items[map][i].model->position, offset);
        }
    }
    for (node = lying_items; node != NULL; node = node->next) {
        if ((node->flags & 1) == map && node->def == NULL) {
            // match: the same lines twice (one copy in the binary) give `offset` its saved register
            if (node->record != NULL) {
                Vector_Add(&node->model->position, &node->model->position, offset);
                node->floor_y += offset[1];
                node->position_y += offset[1];
            } else if (node->map_object != 0) {
                Vector_Add(&node->model->position, &node->model->position, offset);
                node->floor_y += offset[1];
                node->position_y += offset[1];
            }
        }
    }
}

/* Returns the current map's fixed item with this id, or NULL when the id is invalid or the item has no model. */
void *MapItem_GetFixedById(int slot) {
    int index = MapItem_GetFixedIndex(current_map_slot, slot);

    if (index >= 0 && fixed_items[current_map_slot][index].model != NULL) {
        return &fixed_items[current_map_slot][index];
    }
    return NULL;
}

/* Puts fixed item `index` of a map where its definition says and links it into the list of
   items lying about. Returns 0 when it cannot (already linked, no model, or its flag byte has reached the limit). */
int MapItem_PlaceFixed(int map, int index) {
    ItemNode *node;
    u8 tag = map_slots[map].tag;

    if (index < 0 || index >= map_fixed_item_counts[map]) {
        return 0;
    }
    node = &fixed_items[map][index];
    if (node->prev != NULL || node->next != NULL || node == lying_items || node->model == NULL) {
        return 0;
    }
    if (node->def->required_flag != 0 && map_item_pickup_counts[tag][node->index] >= node->def->required_flag) {
        return 0;
    }
    Vector_Copy(&node->model->position, &node->def->position);
    Vector_Copy(&node->model->rotation, &node->def->rotation);
    Vector_Copy(&node->model->scale_x, &node->def->scale);
    MapItem_Link((ListNode *)node);
}

/* Puts the fixed item with this id of a map in place (MapItem_PlaceFixed). */
int MapItem_PlaceFixedById(int map, int item_id) {
    return MapItem_PlaceFixed(map, MapItem_GetFixedIndex(map, item_id));
}

/* Makes a dying enemy drop its item under itself: the one its spawn record names when bit 62
   of that record's flags is set, otherwise the first or second item of its kind by their
   chances. Returns FALSE when nothing was dropped. */
s32 MapItem_DropFromEnemy(Entity *entity) {
    f32 position[4];
    f32 rotation[4];
    Vector4Arg scale;
    f32 offset[4];
    s32 item;
    s32 priority;
    // match: one variable for the spawn record's item, both rolls and the result (separate ones move the item out of $3)
    s32 value;

    memset(rotation, 0, sizeof(rotation));
    rotation[3] = 1.0f;
    scale = enemy_drop_scale;
    if (ENTITY_SPAWN_FLAGS(entity) & (0x8000L << 47)) {
        // match: the low 16 bits of the 64-bit field, copied before the test
        item = value = *(u16 *)&entity->spawn_info->spawn_flags;
        if (value == 0xFFFF) {
            return FALSE;
        }
        priority = 1;
    } else {
        if (entity->params->drop_item == 0xFFFF) {
            return FALSE;
        }
        value = rand() * 100.0f * RAND_SCALE;
        if (value < entity->params->drop_chance) {
            item = entity->params->drop_item;
            priority = 0;
        } else {
            if (entity->params->second_drop_item == 0xFFFF) {
                return FALSE;
            }
            value = rand() * 100.0f * RAND_SCALE;
            if (value >= entity->params->second_drop_chance) {
                return FALSE;
            }
            item = entity->params->second_drop_item;
            priority = 0;
        }
    }
    position[0] = entity->model->position[0];
    position[1] = entity->model->position[1] - entity->height * 0.5f;
    position[2] = entity->model->position[2];
    rotation[1] = (s32)(rand() * 361.0f * RAND_SCALE);
    rotation[1] = Angle_Wrap(DEG_TO_RAD(rotation[1]));
    if (!Collision_FindFloorBelow(2000.0f, position)) {
        return FALSE;
    }
    if (entity->group_id == current_map_slot) {
        offset[0] = offset[1] = offset[2] = offset[3] = 0.0f;
    } else {
        Vector_Copy(offset, &map_load_state.position);
    }
    value = MapItem_Drop(entity->group_id, item, priority, position, COLLISION_RESULT->point[1], offset, rotation,
                          (f32 *)&scale, 0xFFFFFFFF);
    return value;
}

/* Lays a dropped item down in a map: takes a free node of the 16 at dropped_items, or the oldest
   one whose priority is not above this one's, sets up its model and fills in the record that is
   saved for it. `order` is the record's own when it is put back from a save, 0xFFFFFFFF for a
   new drop. Returns FALSE when the player may not have another of the item or nothing is free. */
s32 MapItem_Drop(s32 map, s32 item, s32 priority, f32 *position, f32 floor_y, f32 *offset, f32 *rotation, f32 *scale,
                  u32 order) {
    char *model_data;
    char *buffer;
    ItemNode *node;
    ItemNode *oldest;
    ItemModelSource *source;
    s32 index;
    s32 i;

    if (!MapItem_UniqueNotCarried(item)) {
        return FALSE;
    }
    if (!MapItem_UniqueNotInTradeStock(item)) {
        return FALSE;
    }
    if (!MapItem_UniqueNotDropped(item)) {
        return FALSE;
    }
    source = MapItem_FindModelSource(map, (item_name_defs + item)->model_file_number);
    if (source == NULL) {
        return FALSE;
    }
    index = -1;
    node = dropped_items[map];
    for (i = 0; i < 16; i++, node++) {
        if (node->record == NULL) {
            index = i;
            break;
        }
    }
    if (index < 0) {
        oldest = NULL;
        node = dropped_items[map];
        for (i = 0; i < 16; i++, node++) {
            if ((s32)ITEM_NODE_BITS(node)->priority <= priority) {
                if (oldest == NULL || (u32)oldest->record->order > (u32)node->record->order) {
                    oldest = node;
                    index = i;
                }
            }
        }
        if (oldest == NULL) {
            return FALSE;
        }
        node = oldest;
        MapItem_Unlink(node);
    }
    node->item = item;
    node->def = NULL;
    node->model = &dropped_item_models[map][index];
    node->record = &dropped_item_records[map_slots[map].tag][index];
    ITEM_NODE_BITS(node)->map = map;
    Vector_Copy(&node->model->position, position);
    Vector_Copy(&node->model->rotation, rotation);
    Vector_Copy(&node->model->scale_x, scale);
    ITEM_NODE_BITS(node)->bit2 = TRUE;
    node->position_y = position[1];
    node->floor_y = floor_y;
    node->fall_speed = 0.0f;
    model_data = source->model_data;
    buffer = dropped_item_model_data[map][index];
    Model_SetUpPosed(node->model, &model_data, &buffer);
    Model_SetTextureBase((char *)node->model, 0x3B40, 0x3EC0);
    Model_SetVertexColour(node->model, 0xFF, 0xFF, 0xFF, -1);
    node->model->texture_slot = 2;
    node->model->texture_data = (int)source->texture;
    TextureTable_SetBasePointers(source->texture, 0x3B40, 0x3EC0);
    if (order == 0xFFFFFFFF) {
        order = item_drop_counter++;
    }
    node->record->position[0] = position[0] + offset[0];
    node->record->position[1] = floor_y + offset[1];
    node->record->position[2] = position[2] + offset[2];
    node->record->rotation[0] = rotation[0] * 16.0f;
    node->record->rotation[1] = rotation[1] * 16.0f;
    node->record->rotation[2] = rotation[2] * 16.0f;
    node->record->scale[0] = scale[0] * 16.0f;
    node->record->scale[1] = scale[1] * 16.0f;
    node->record->scale[2] = scale[2] * 16.0f;
    node->record->item = item;
    node->record->priority = priority;
    node->record->used = TRUE;
    node->record->order = order;
    MapItem_Link((ListNode *)node);
    return TRUE;
}

/* Puts an item into a map for a map object (a chest's contents, say): takes a free node of the 32 at
   object_items and sets up its model. Returns NULL when the player may not have another of the item, when
   the object already has one out, or when no node or no model for the item is there. */
ItemNode *MapItem_PutOutForObject(int map, int item, void *position, void *rotation, void *scale, void *object) {
    char *sources[2];
    ItemNode *node;
    ItemModelSource *source;
    ModelInstance *model;
    s32 i;

    if (!MapItem_UniqueNotCarried(item)) {
        return NULL;
    }
    if (!MapItem_UniqueNotInTradeStock(item)) {
        return NULL;
    }
    if (!MapItem_UniqueNotDropped(item)) {
        return NULL;
    }
    node = object_items[map];
    for (i = 0; i < 32; i++, node++) {
        if ((void *)node->map_object == object) {
            return NULL;
        }
    }
    node = object_items[map];
    for (i = 0; i < 32 && node->map_object != 0; i++, node++) {
    }
    if (i >= 32) {
        return NULL;
    }
    source = MapItem_FindModelSource(map, (item_name_defs + item)->model_file_number);
    if (source == NULL) {
        return NULL;
    }
    model = &object_item_models[map][i];
    ITEM_NODE_MAP_BIT(node)->map = map;
    node->item = item;
    node->map_object = (s32)object;
    node->model = model;
    Vector_Copy(&model->position, position);
    Vector_Copy(&node->model->rotation, rotation);
    Vector_Copy(&node->model->scale_x, scale);
    sources[0] = source->model_data;
    sources[1] = object_item_model_data[map][i];
    Model_SetUpPosed(node->model, &sources[0], &sources[1]);
    Model_SetTextureBase((char *)node->model, 0x3B40, 0x3EC0);
    Model_SetVertexColour(node->model, 0xFF, 0xFF, 0xFF, -1);
    node->model->texture_slot = 2;
    node->model->texture_data = (int)source->texture;
    TextureTable_SetBasePointers(source->texture, 0x3B40, 0x3EC0);
    MapItem_Link((ListNode *)node);
    return node;
}

/* Returns the item lying about that a map object put out (MapItem_PutOutForObject), or NULL. */
ItemNode *MapItem_FindForObject(int object) {
    ItemNode *node;

    for (node = lying_items; node != NULL; node = node->next) {
        if (node->map_object == object) {
            return node;
        }
    }
    return NULL;
}

/* Lets the dropped items that are still falling (flag bit 2) fall one more frame, faster each frame,
   until they reach the floor below them. */
void MapItem_UpdateFalling(void) {
    ItemNode *node;
    int flags;
    float height;

    node = lying_items;
    if (!(suspended_updates[0] & 0x10) && node != 0) {
        do {
            if (node->record != 0) {
                flags = node->flags;
                if (((u32)flags >> 2) & 1) {
                    height = node->position_y + node->fall_speed;
                    node->position_y = height;
                    if (node->floor_y <= height) {
                        // match: the model is read before the flags are stored, as retail schedules it
                        ModelInstance *model = node->model;
                        node->flags = flags & -5;
                        do { } while (0); // match: keeps the model load ahead of the flag store
                        model->position.y = node->floor_y;
                    } else {
                        node->model->position.y = height;
                        node->fall_speed += 1.0f;
                    }
                }
            }
            node = node->next;
        } while (node != 0);
    }
}

/* Draws the items lying in the maps, lit like the map record each lies in. The one being picked up is left
   out unless with_picked is set. */
void MapItem_DrawAll(int with_picked) {
    f32 scale[4];
    f32 unused[8];
    ItemNode *node;
    ModelPart *part;
    s32 count;
    s32 i;

    Packet_BeginGsRegisters();
    Packet_AddGsRegister(0x47, 0x5000D);
    Packet_EndGsRegisters();
    for (node = lying_items; node != NULL; node = node->next) {
        node->map_record = Map_FindRecordAtInSlot((f32 *)&node->model->position, 0.0f, node->flags & 1);
        if (node->map_record == NULL) {
            continue;
        }
        if (node == pickup_item && with_picked == 0) {
            continue;
        }
        Vector_Scale(scale, &node->model->scale_x, 1.0f);
        ModelMatrix_Build(node->model, &node->model->position, &node->model->rotation, scale);
        part = node->model->parts;
        count = node->model->part_count;
        for (i = 0; i < count; i++, part++) {
            ModelMatrix_BuildPartLocal(node->model, part, part);
        }
        node->model->light_block = (s32)&map_draw_state.light_blocks[node->flags & 1][map_slots[node->flags & 1].header->light_block];
        node->model->fog_block = &map_draw_state.fog_blocks[node->flags & 1][((MapRecordInfo *)node->map_record)->draw_block_index];
        LinkedPoint_FindHitsForModel(node->model);
        ModelTexture_SendForModel(node->model);
        Model_DrawPosedDepthByPart(node->model);
    }
}

/* an angle brought back into -pi..pi (for angles above -pi) */
static inline f32 wrap_angle(f32 angle) {
    angle += PI;
    return angle - (f32)(s32)(angle / TWO_PI) * TWO_PI - PI;
}

/* Draws the item being picked up, over everything else: each part's own offset and turn are scaled by
   pickup_pose_scale, so that the model folds flat while it is held up. Nothing is drawn when hidden is set. */
void MapItem_DrawPicked(int hidden) {
    f32 scale[4];
    f32 offset[4];
    f32 turn[4];
    ItemNode *node = pickup_item;
    ModelPart *part;
    s32 count;
    s32 i;

    if (node == NULL || hidden != 0) {
        return;
    }
    node->map_record = Map_FindRecordAtInSlot((f32 *)&node->model->position, 0.0f, node->flags & 1);
    if (node->map_record == NULL) {
        return;
    }
    i = 0;
    GsDraw_ClearDepth();
    Packet_BeginGsRegisters();
    Packet_AddGsRegister(0x47, 0x5000D);
    Packet_EndGsRegisters();
    Vector_Scale(scale, &node->model->scale_x, 1.0f);
    ModelMatrix_Build(node->model, &node->model->position, &node->model->rotation, scale);
    count = node->model->part_count;
    part = node->model->parts;
    for (; i < count; i++, part++) {
        Vector_Scale(offset, part->position, pickup_pose_scale);
        Vector_Scale(turn, &part->rotation_x, pickup_pose_scale);
        sceVu0CopyMatrix(part, identity_matrix);
        PART_MATRIX(part)[0][0] = part->scale[0];
        PART_MATRIX(part)[1][1] = part->scale[1];
        PART_MATRIX(part)[2][2] = part->scale[2];
        sceVu0RotMatrixX((f32 *)part, (f32 *)part, wrap_angle(turn[0]));
        sceVu0RotMatrixZ((f32 *)part, (f32 *)part, wrap_angle(turn[2]));
        sceVu0RotMatrixY((f32 *)part, (f32 *)part, wrap_angle(turn[1]));
        sceVu0TransMatrix((f32 *)part, (f32 *)part, offset);
    }
    node->model->light_block = (s32)&map_draw_state.light_blocks[node->flags & 1][map_slots[node->flags & 1].header->light_block];
    node->model->fog_block = &map_draw_state.fog_blocks[node->flags & 1][((MapRecordInfo *)node->map_record)->draw_block_index];
    LinkedPoint_FindHitsForModel(node->model);
    ModelTexture_SendForModel(node->model);
    Model_DrawPosedDepthByPart(node->model);
    GsDraw_QueueFlush();
}

/* Makes node the one being picked up, if none is yet and it is on the list of the ones in use. */
int MapItem_StartPickup(ItemNode *node) {
    if (node == NULL || pickup_item != NULL) {
        return FALSE;
    }
    if (lying_items != node && node->prev == NULL && node->next == NULL) {
        return FALSE;
    }
    pickup_item = node;
    pickup_no_put_back = 0;
    pickup_step = 0;
    return TRUE;
}

/* Whether the player may reach for the item: it lies about (is on the list) and is not inside a chest
   (a kind 4 map object). */
int MapItem_IsPickable(ItemNode *node) {
    MapObject *object;
    ItemNode *head;
    int result;

    head = lying_items;
    if (node == 0) {
        return 0;
    }
    if ((head != node) && (node->prev == 0) && (node->next == 0)) {
        return 0;
    }
    object = (MapObject *)node->map_object;
    result = 1;
    if (object != 0) {
        result = MAP_OBJECT_DEF_KIND(object->definition) != 4;
    }
    return result;
}

/* Makes the current map's fixed item with this id the one being picked up, if none is yet and it
   is lying about; without may_put_back it cannot be put back. */
int MapItem_StartPickupById(int item_id, int may_put_back) {
    int n;
    ItemNode *node;

    n = MapItem_GetFixedIndex(current_map_slot, item_id);
    if (pickup_item != 0 || n < 0) {
        return 0;
    }
    node = &fixed_items[current_map_slot][n];
    if (lying_items != node && node->prev == 0 && node->next == 0) {
        return 0;
    }
    pickup_item = node;
    pickup_no_put_back = may_put_back == 0;
    pickup_step = 0;
    return 1;
}

/* Runs the current step of the pickup in progress, if there is one. */
void MapItem_UpdatePickup(void) {
    if (pickup_item != NULL) {
        pickup_step_handlers[pickup_step]();
    }
}

/* Pickup step 0: remembers where the item lies and aims it at a point in front of the player's eyes, five units
   a frame when it is near and in fifteen frames when it is not. */
void MapItem_PickupAim(void) {
    f32 delta[4];
    f32 distance;

    if (game_status.update_hold_flags != 0) {
        return;
    }
    game_status.update_hold_flags = 0x87F;
    Player_StopWalking();
    Vector_Copy(pickup_start_position, &pickup_item->model->position);
    Vector_Copy(pickup_start_rotation, &pickup_item->model->rotation);
    pickup_start_rotation[1] = Angle_Wrap(pickup_start_rotation[1]);
    pickup_target_position[0] = view_ray_status.view_origin[0] + sinf(player_state.rotation.y) * 150.0f;
    pickup_target_position[2] = view_ray_status.view_origin[2] + cosf(player_state.rotation.y) * 150.0f;
    pickup_target_position[1] = pickup_item->model->position.y - 100.0f;
    if (pickup_target_position[1] < view_ray_status.view_origin[1]) {
        pickup_target_position[1] = view_ray_status.view_origin[1];
    }
    pickup_target_position[3] = 0.0f;
    pickup_view_pitch = atan2f(view_ray_status.view_origin[1] - pickup_target_position[1], 150.0f);
    Vector_Subtract(delta, pickup_target_position, pickup_start_position);
    distance = sqrt(delta[0] * delta[0] + delta[1] * delta[1] + delta[2] * delta[2]);
    if (distance < 75.0f) {
        Vector_Scale(pickup_velocity, delta, 1.0f / distance);
        Vector_Scale(pickup_velocity, pickup_velocity, 5.0f);
        pickup_move_frames = distance / 5.0f;
    } else {
        Vector_Scale(pickup_velocity, delta, (1.0f / 15.0f));
        pickup_move_frames = 15;
    }
    pickup_pose_scale = 1.0f;
    pickup_frames_left = pickup_move_frames;
    pickup_step++;
}

/* Pickup step 1: moves the item to the point in front of the player while its rotation eases to 0 and it
   folds flat (pickup_pose_scale), and tilts the player's view toward it; then on to step 2. */
void MapItem_PickupBringUp(void) {
    pickup_frames_left--;
    if (pickup_frames_left > 0) {
        Vector_Add(&pickup_item->model->position, &pickup_item->model->position, pickup_velocity);
        Angle_EaseToward(0.0f, &pickup_item->model->rotation.x, pickup_frames_left);
        Angle_EaseToward(0.0f, &pickup_item->model->rotation.y, pickup_frames_left);
        Angle_EaseToward(0.0f, &pickup_item->model->rotation.z, pickup_frames_left);
        Angle_EaseToward(pickup_view_pitch, player_rotation, pickup_frames_left);
        pickup_pose_scale = (float)pickup_frames_left / (float)pickup_move_frames;
        return;
    }
    Vector_Copy(&pickup_item->model->position, pickup_target_position);
    pickup_pose_scale = 0.0f;
    player_rotation[0] = pickup_view_pitch;
    pickup_step++;
}

/* Pickup step 2: the item turns in front of the player until it is taken (PAD_CROSS) or put back (PAD_CIRCLE). */
void MapItem_PickupHold(void) {
    f32 delta[4];

    pickup_pose_scale = 0.0f;
    pickup_item->model->rotation.y = Angle_Wrap(pickup_item->model->rotation.y + PICKUP_HOLD_SPIN_PER_FRAME);
    if (pad_pressed[0] & PAD_CROSS) {
        if (!Player_CannotCarryItem((s16)pickup_item->item)) {
            Vector_Copy(pickup_target_position, &player_state);
            pickup_target_position[1] -= 115.0f;
            Vector_Subtract(delta, pickup_target_position, &pickup_item->model->position);
            Vector_Scale(pickup_velocity, delta, (1.0f / 15.0f));
            pickup_frames_left = 15;
            pickup_step++;
            return;
        }
        Message_ShowNotice(0, 0, 0, 0, 0x2D, MSG_CANT_CARRY_ANY_MORE);
    } else if (!(pad_pressed[0] & PAD_CIRCLE) || pickup_no_put_back != 0) {
        return;
    }
    Vector_Scale(pickup_velocity, pickup_velocity, -1.0f);
    pickup_frames_left = pickup_move_frames;
    pickup_step += 2;
}

/* Pickup step 3: moves the item to the player, then hands it over and frees its node. */
void MapItem_PickupTake(void) {
    ItemNode *node = pickup_item;

    pickup_frames_left--;
    if (pickup_frames_left > 0) {
        Vector_Add(&node->model->position, &node->model->position, pickup_velocity);
        return;
    }
    Player_AddItem((s16)node->item);
    if (node->def != NULL && node->def->required_flag != 0) {
        map_item_pickup_counts[map_slots[node->flags & 1].tag][node->index]++;
    } else if (node->map_object != 0) {
        MapObject_MarkEmptied((MapObject *)node->map_object);
    } else if (node->record != NULL) {
        memset(node->record, 0, sizeof(DroppedItemRecord));
    }
    MapItem_Unlink(pickup_item);
    suspended_updates[0] = 0;
    pickup_pose_scale = 1.0f;
    pickup_item = NULL;
    pickup_step = 0;
}

/* Pickup step 4 (put back): moves the item back to where it lay and lets go of it. */
void MapItem_PickupPutBack(void) {
    MapObject *object;

    pickup_frames_left--;
    if (pickup_frames_left > 0) {
        Vector_Add(&pickup_item->model->position, &pickup_item->model->position, pickup_velocity);
        Angle_EaseToward(pickup_start_rotation[0], &pickup_item->model->rotation.x, pickup_frames_left);
        Angle_EaseToward(pickup_start_rotation[1], &pickup_item->model->rotation.y, pickup_frames_left);
        Angle_EaseToward(pickup_start_rotation[2], &pickup_item->model->rotation.z, pickup_frames_left);
        pickup_pose_scale = 1.0f - (f32)pickup_frames_left / (f32)pickup_move_frames;
        return;
    }
    Vector_Copy(&pickup_item->model->position, pickup_start_position);
    Vector_Copy(&pickup_item->model->rotation, pickup_start_rotation);
    object = (MapObject *)pickup_item->map_object;
    if (object != NULL && (object->state_flags & MAP_OBJECT_IN_USE) && object->definition->kind == 3) {
        MapItem_Unlink(pickup_item);
    }
    suspended_updates[0] = 0;
    pickup_pose_scale = 1.0f;
    pickup_item = NULL;
    pickup_step = 0;
}

/* Clears everything the item code keeps for a map slot. */
void MapItem_ClearMap(int map) {
    int i;

    map_fixed_item_counts[map] = 0; // match: this order of the three gives the retail register for each table
    map_item_defs[map] = NULL;
    map_item_model_kinds[map] = NULL;
    memset(item_model_sources[map], 0, sizeof(item_model_sources[0]));
    memset(fixed_items[map], 0, sizeof(fixed_items[0]));
    memset(dropped_items[map], 0, sizeof(dropped_items[0]));
    memset(object_items[map], 0, sizeof(object_items[0]));
    memset(fixed_item_models[map], 0, sizeof(fixed_item_models[0]));
    memset(dropped_item_models[map], 0, sizeof(dropped_item_models[0]));
    memset(object_item_models[map], 0, sizeof(object_item_models[0]));
    for (i = 0xFF; i >= 0; i--) {
        fixed_item_by_id[map][i] = -1;
    }
}

/* Returns where the model and texture of item kind key lie in the slot's map (the map lists up to 32
   kinds), or NULL when the map has none for it. */
ItemModelSource *MapItem_FindModelSource(int slot, int key) {
    s16 *cursor;
    s16 value;
    int i;

    cursor = map_item_model_kinds[slot];
    for (i = 0; i < 0x20; i++) {
        value = cursor[i];
        if (value < 0) {
            return NULL;
        }
        if (value == key) {
            return &item_model_sources[slot][i];
        }
    }
    return NULL;
}

/* Puts an item on the front of the list of items lying about (lying_items). */
void MapItem_Link(ListNode *node) {
    ListNode *head = (ListNode *)lying_items;

    node->prev = NULL;
    node->next = head;
    if (head != NULL) {
        head->prev = node;
    }
    lying_items = (ItemNode *)node;
}

/* Takes an item off the list of items lying about, frees its object and drop record links, and returns
   the item that followed it. */
ItemNode *MapItem_Unlink(ItemNode *node) {
    ItemNode *next;

    next = node->next;
    if (node->prev != NULL) {
        node->prev->next = next;
    }
    if (node->next != NULL) {
        node->next->prev = node->prev;
    }
    if (node == lying_items) {
        lying_items = node->next;
    }
    node->prev = NULL;
    node->next = NULL;
    node->map_object = 0;
    node->record = NULL;
    return next;
}

/* Sends the model's textures and sets GS register 0x47 for drawing it. */
void MapItem_PrepareDraw(ModelInstance *model) {
    ModelTexture_SendForModel(model);
    Packet_BeginGsRegisters();
    Packet_AddGsRegister(0x47, 0x5000D);
    Packet_EndGsRegisters();
}

/* Returns the index of the slot's fixed item with this id (1..255), or -1. */
int MapItem_GetFixedIndex(int slot, int object_id) {
    if ((u32)(object_id - 1) < 0xFF) {
        return fixed_item_by_id[slot][object_id];
    }
    return -1;
}

/* Returns FALSE when the player carries the item and it is one of a kind (equipment, or ItemCommonDef.flags
   bit 1). */
int MapItem_UniqueNotCarried(int item) {
    if (item_counts[item] != 0 && (item >= 0x96 || ((item_common_defs + item)->flags >> 1 & 1))) {
        return 0;
    }
    return 1;
}

/* Returns FALSE when one of the five tables after item_counts holds the item and it is one of a kind. */
int MapItem_UniqueNotInTradeStock(int item) {
    if (trade_stock_1[item] != 0 && (item >= 0x96 || ((item_common_defs + item)->flags >> 1 & 1))) {
        return FALSE;
    }
    if (trade_stock_2[item] != 0 && (item >= 0x96 || ((item_common_defs + item)->flags >> 1 & 1))) {
        return FALSE;
    }
    if (trade_stock_3[item] != 0 && (item >= 0x96 || ((item_common_defs + item)->flags >> 1 & 1))) {
        return FALSE;
    }
    if (trade_stock_4[item] != 0 && (item >= 0x96 || ((item_common_defs + item)->flags >> 1 & 1))) {
        return FALSE;
    }
    if (trade_stock_5[item] != 0 && (item >= 0x96 || ((item_common_defs + item)->flags >> 1 & 1))) {
        return FALSE;
    }
    return TRUE;
}

/* Returns FALSE when an item of a kind the player may hold only one of lies dropped in any map. */
int MapItem_UniqueNotDropped(int item) {
    int i;
    int j;
    DroppedItemRecord *record;
    ItemCommonDef *common;
    char *base = (char *)dropped_item_records;

    common = item_common_defs + item;
    for (i = 0; i < 0x80; i++) {
        for (j = 0, record = (DroppedItemRecord *)(i * 0x140 + (int)base); j < 0x10; j++) {
            if (record->used != 0 && record->item == item) {
                if (item >= 0x96 || (common->flags >> 1 & 1)) {
                    return 0;
                }
            }
            record++;
        }
    }
    return 1;
}
