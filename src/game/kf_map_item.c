#include "kf_model.h"
#include "kf_player.h"
#include "kf_game.h"
#include "kf_map.h"
#include "kf_entity.h"
#include "kf_collision.h"
#include "kf_linked_point.h"
#include "kf_message.h"
#include "kf_map_object.h"

void MapItem_ClearMap(int);
s32 MapItem_Drop(s32, s32, s32, f32 *, f32, f32 *, f32 *, f32 *, u32);
void *MapItem_FindModelSource(int, int);
void func_0010D018(ModelInstance *, char **, char **);
void func_0010ED80(char *, int, int);
void func_0010E8E0(void *, int, int, int, int);
void func_0010B510(char *, int, int);
void MapItem_PrepareDraw(void *);

void MapItem_PickupAim(void);
void MapItem_PickupBringUp(void);
void MapItem_PickupHold(void);
void MapItem_PickupTake(void);
void MapItem_PickupPutBack(void);

/* 0x4147E0 the steps of the item pickup in progress, MapItem_UpdatePickup runs the current one */
void (*pickup_step_handlers[6])(void) = { MapItem_PickupAim, MapItem_PickupBringUp, MapItem_PickupHold, MapItem_PickupTake, MapItem_PickupPutBack, 0 };

int MapItem_PlaceFixed(int, int);
ItemNode *MapItem_Unlink(ItemNode *);
void Vector_Add(void *, void *, void *);
int MapItem_GetFixedIndex(int, int);
void Vector_Copy(void *, void *);
void MapItem_Link(ListNode *);
int func_001C0E40(f32, void *);
float Angle_Wrap(float);
int MapItem_UniqueNotCarried(int);
int MapItem_UniqueNotInTradeStock(int);
int MapItem_UniqueNotDropped(int);
void func_00109A50(void);
void func_00109A80(int, s64);
void func_00109AD0(void);
MapRecordInfo *Map_FindRecordAtInSlot(CameraState *, f32, int);
void Vector_Scale(float *, float *, float);
void ModelMatrix_Build(ModelInstance *, Vector4 *, Vector4 *, f32 *);
void ModelMatrix_BuildPartLocal(ModelInstance *, ModelPart *, ModelPart *);
void func_0010AEB8(void *);
void func_0010DB40(ModelInstance *);
void func_0010A698(void);
void sceVu0CopyMatrix(void *, void *);
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0RotMatrixZ(float *, float *, float);
void sceVu0TransMatrix(float *, float *, float *);
void func_00109E18(void);
void func_001B9478(void);
float sinf(float);
float cosf(float);
float atan2f(float, float);
double sqrt(double);
void Vector_Subtract(void *, void *, void *);
int Angle_EaseToward(float, float *, int);
int Player_CannotCarryItem(int);
void Player_AddItem(int);
void *memset(void *, int, u32);

extern GameDefinition item_name_defs[380];
extern s32 D_004469D0;
extern ItemNode *D_004469D4;            /* the items lying in the maps, a list */
extern ItemNode *D_004469D8;            /* the one being picked up */
extern s32 D_00447198;
extern f32 D_0044719C;
extern s32 D_004471A4;
extern MapSlot map_slots[];
extern DroppedItemRecord dropped_item_records[0x80][16];
extern MapItemDef *D_004469B8[2];
extern s16 *D_004469C0[2];
extern s32 D_00447190[2];
extern u8 *D_004469C8[2];
extern int D_00446770;
extern u8 map_item_pickup_counts[0x80][0x40];
extern const Vector4Arg D_0043F610;
extern MapLoadState map_load_state;
extern int D_0038CC80[3];
extern MapDrawState map_draw_state;
extern f32 D_00431AF0[16];
extern GameStatus game_status;
extern PlayerState player_state;
extern EventRunner D_00420D00;
extern f32 D_004471A8;
extern int D_004471AC;
extern int D_004471B0;
extern float player_rotation[];
extern u16 pad_pressed[];
extern u8 item_counts[];
extern u8 trade_stock_1[];
extern u8 trade_stock_2[];
extern u8 trade_stock_3[];
extern u8 trade_stock_4[];
extern u8 trade_stock_5[];
extern ItemCommonDef item_common_defs[];

/* The per-map item blocks: nodes, models and the model data behind them, one set for each map. */
static ItemModelSource D_0057E040[2][32];   /* 0x57E040 where the model and texture of each item kind lie */
static ItemNode D_0057E240[2][64];          /* 0x57E240 the fixed items of the map */
static ItemNode D_00580040[2][16];          /* 0x580040 the dropped items of the map */
static ItemNode D_005807C0[2][32];          /* 0x5807C0 the items the player may pick up */
static ModelInstance D_005816C0[2][64];     /* 0x5816C0 the models of the fixed items */
static ModelInstance D_0058F6C0[2][16];     /* 0x58F6C0 the models of the dropped items */
static ModelInstance D_00592EC0[2][32];     /* 0x592EC0 the models of the pickup items */
static char D_00599EC0[2][64][0x200];       /* 0x599EC0 the model data of the fixed items */
static char D_005A9EC0[2][16][0x200];       /* 0x5A9EC0 the model data of the dropped items */
static char D_005ADEC0[2][32][0x200];       /* 0x5ADEC0 the model data of the pickup items */
static s8 D_005B5EC0[2][0x100];             /* 0x5B5EC0 the node of each item slot, -1 for none */
static f32 D_005B60C0[4];                   /* 0x5B60C0 where the item being picked up is */
static f32 D_005B60D0[4];                   /* 0x5B60D0 its rotation */
static f32 D_005B60E0[4];                   /* 0x5B60E0 where it is being taken to */
static f32 D_005B60F0[4];                   /* 0x5B60F0 its speed */

/* Resets the item code: no items lying about, none being picked up, both map slots' tables cleared; and
   every item definition whose state is 0 gets 1. */
void MapItem_Init(void) {
    LevelDef *def;
    s32 i;

    D_004469D4 = NULL;
    D_0044719C = 1.0f;
    D_004469D8 = NULL;
    D_00447198 = 0;
    D_004471A4 = 0;
    D_004469D0 = 0;
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
    s16 *kinds = D_004469C0[map];
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
    D_00447190[map] = header->item_count;
    cursor = (char *)header + header->item_models_offset;
    for (i = 0; i < 32 && kinds[i] >= 0; i++) {
        D_0057E040[map][i].model_data = cursor;
        cursor += *(s32 *)cursor;
        D_0057E040[map][i].texture = cursor;
        cursor += *(s32 *)cursor;
    }
    count = D_00447190[map];
    def = D_004469B8[map];
    node = D_0057E240[map];
    for (i = 0; i < count; i++, def++, node++) {
        D_005B5EC0[map][def->slot_id] = i;
        definition = &item_name_defs[def->item];
        source = MapItem_FindModelSource(map, definition->unk30);
        if (source != NULL) {
            ITEM_NODE_BITS(node)->map = map;
            node->def = def;
            node->item = def->item;
            node->record = NULL;
            node->index = i;
            node->model = &D_005816C0[map][i];
            model_data = source->model_data;
            cursor = D_00599EC0[map][i];
            func_0010D018(node->model, &model_data, &cursor);
            func_0010ED80((char *)node->model, 0x3B40, 0x3EC0);
            func_0010E8E0(node->model, 0xFF, 0xFF, 0xFF, -1);
            node->model->draw = (void (*)(ModelInstance *))MapItem_PrepareDraw;
            node->model->unk194 = (int)source->texture;
            node->model->unk190 = 2;
            func_0010B510(source->texture, 0x3B40, 0x3EC0);
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
            rotation[0] = record->rotation[0] * 0.0625f;
            rotation[1] = record->rotation[1] * 0.0625f;
            rotation[2] = record->rotation[2] * 0.0625f;
            scale[0] = record->scale[0] * 0.0625f;
            scale[1] = record->scale[1] * 0.0625f;
            scale[2] = record->scale[2] * 0.0625f;
            MapItem_Drop(map, record->item, record->priority, position, position[1], offset, rotation, scale,
                          record->order);
        }
    }
}

/* Rolls which of a map's fixed items appear: one roll per group against the map's threshold
   for it, then one per item against its own chance. */
void MapItem_RollFixedItems(int map) {
    u8 rolls[16];
    u8 *thresholds = D_004469C8[map];
    s32 i;
    ItemNode *node;

    for (i = 0; i < 16; i++) {
        rolls[i] = rand() * 100.0f * 4.656613e-10f;
    }
    node = D_0057E240[map];
    for (i = 0; i < D_00447190[map]; i++, node++) {
        if (rolls[node->def->group] < thresholds[node->def->group]) {
            if ((s32)(rand() * 100.0f * 4.656613e-10f) < node->def->chance) {
                MapItem_PlaceFixed(map, i);
            }
        }
    }
}

/* Unlinks every item of a map from the list of the ones lying about and clears the map's item tables. */
void MapItem_UnloadMap(int map) {
    ItemNode *node;

    node = D_004469D4;
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

    for (i = 0; i < D_00447190[map]; i++) {
        if (D_0057E240[map][i].def != NULL) {
            Vector_Add(D_0057E240[map][i].def, D_0057E240[map][i].def, offset);
        }
        if (D_0057E240[map][i].model != NULL) {
            Vector_Add(&D_0057E240[map][i].model->position, &D_0057E240[map][i].model->position, offset);
        }
    }
    for (node = D_004469D4; node != NULL; node = node->next) {
        if ((node->flags & 1) == map && node->def == NULL) {
            // match: the same lines twice (one copy in the binary) give `offset` its saved register
            if (node->record != NULL) {
                Vector_Add(&node->model->position, &node->model->position, offset);
                node->unk28 += offset[1];
                node->unk2C += offset[1];
            } else if (node->unk08 != 0) {
                Vector_Add(&node->model->position, &node->model->position, offset);
                node->unk28 += offset[1];
                node->unk2C += offset[1];
            }
        }
    }
}

/* Returns the current map's fixed item with this id, or NULL when the id is invalid or the item has no model. */
void *MapItem_GetFixedById(int slot) {
    int index = MapItem_GetFixedIndex(D_00446770, slot);

    if (index >= 0 && D_0057E240[D_00446770][index].model != NULL) {
        return &D_0057E240[D_00446770][index];
    }
    return NULL;
}

/* Puts fixed item `index` of a map where its definition says and links it into the list of
   items lying about. Returns 0 when it cannot (already linked, no model, or its flag byte has reached the limit). */
int MapItem_PlaceFixed(int map, int index) {
    ItemNode *node;
    u8 tag = map_slots[map].tag;

    if (index < 0 || index >= D_00447190[map]) {
        return 0;
    }
    node = &D_0057E240[map][index];
    if (node->prev != NULL || node->next != NULL || node == D_004469D4 || node->model == NULL) {
        return 0;
    }
    if (node->def->required_flag != 0 && map_item_pickup_counts[tag][node->index] >= node->def->required_flag) {
        return 0;
    }
    Vector_Copy(&node->model->position, &node->def->position);
    Vector_Copy(&node->model->rotation, &node->def->rotation);
    Vector_Copy(&node->model->unkE0, &node->def->scale);
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
    scale = D_0043F610;
    if (ENTITY_SPAWN_FLAGS(entity) & (0x8000L << 47)) {
        // match: the low 16 bits of the 64-bit field, copied before the test
        item = value = *(u16 *)&entity->spawn_info->unk38;
        if (value == 0xFFFF) {
            return FALSE;
        }
        priority = 1;
    } else {
        if (entity->params->drop_item == 0xFFFF) {
            return FALSE;
        }
        value = rand() * 100.0f * 4.656613e-10f;
        if (value < entity->params->drop_chance) {
            item = entity->params->drop_item;
            priority = 0;
        } else {
            if (entity->params->second_drop_item == 0xFFFF) {
                return FALSE;
            }
            value = rand() * 100.0f * 4.656613e-10f;
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
    rotation[1] = (s32)(rand() * 361.0f * 4.656613e-10f);
    rotation[1] = Angle_Wrap(rotation[1] * 0.017453292f);
    if (!func_001C0E40(2000.0f, position)) {
        return FALSE;
    }
    if (entity->group_id == D_00446770) {
        offset[0] = offset[1] = offset[2] = offset[3] = 0.0f;
    } else {
        Vector_Copy(offset, &map_load_state.position);
    }
    value = MapItem_Drop(entity->group_id, item, priority, position, COLLISION_RESULT->point[1], offset, rotation,
                          (f32 *)&scale, 0xFFFFFFFF);
    return value;
}

/* Lays a dropped item down in a map: takes a free node of the 16 at D_00580040, or the oldest
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
    source = MapItem_FindModelSource(map, (item_name_defs + item)->unk30);
    if (source == NULL) {
        return FALSE;
    }
    index = -1;
    node = D_00580040[map];
    for (i = 0; i < 16; i++, node++) {
        if (node->record == NULL) {
            index = i;
            break;
        }
    }
    if (index < 0) {
        oldest = NULL;
        node = D_00580040[map];
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
    node->model = &D_0058F6C0[map][index];
    node->record = &dropped_item_records[map_slots[map].tag][index];
    ITEM_NODE_BITS(node)->map = map;
    Vector_Copy(&node->model->position, position);
    Vector_Copy(&node->model->rotation, rotation);
    Vector_Copy(&node->model->unkE0, scale);
    ITEM_NODE_BITS(node)->bit2 = TRUE;
    node->unk2C = position[1];
    node->unk28 = floor_y;
    node->unk30 = 0.0f;
    model_data = source->model_data;
    buffer = D_005A9EC0[map][index];
    func_0010D018(node->model, &model_data, &buffer);
    func_0010ED80((char *)node->model, 0x3B40, 0x3EC0);
    func_0010E8E0(node->model, 0xFF, 0xFF, 0xFF, -1);
    node->model->unk190 = 2;
    node->model->unk194 = (int)source->texture;
    func_0010B510(source->texture, 0x3B40, 0x3EC0);
    if (order == 0xFFFFFFFF) {
        order = D_004469D0++;
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
   D_005807C0 and sets up its model. Returns NULL when the player may not have another of the item, when
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
    node = D_005807C0[map];
    for (i = 0; i < 32; i++, node++) {
        if ((void *)node->unk08 == object) {
            return NULL;
        }
    }
    node = D_005807C0[map];
    for (i = 0; i < 32 && node->unk08 != 0; i++, node++) {
    }
    if (i >= 32) {
        return NULL;
    }
    source = MapItem_FindModelSource(map, (item_name_defs + item)->unk30);
    if (source == NULL) {
        return NULL;
    }
    model = &D_00592EC0[map][i];
    ITEM_NODE_MAP_BIT(node)->map = map;
    node->item = item;
    node->unk08 = (s32)object;
    node->model = model;
    Vector_Copy(&model->position, position);
    Vector_Copy(&node->model->rotation, rotation);
    Vector_Copy(&node->model->unkE0, scale);
    sources[0] = source->model_data;
    sources[1] = D_005ADEC0[map][i];
    func_0010D018(node->model, &sources[0], &sources[1]);
    func_0010ED80((char *)node->model, 0x3B40, 0x3EC0);
    func_0010E8E0(node->model, 0xFF, 0xFF, 0xFF, -1);
    node->model->unk190 = 2;
    node->model->unk194 = (int)source->texture;
    func_0010B510(source->texture, 0x3B40, 0x3EC0);
    MapItem_Link((ListNode *)node);
    return node;
}

/* Returns the item lying about that a map object put out (MapItem_PutOutForObject), or NULL. */
ItemNode *MapItem_FindForObject(int object) {
    ItemNode *node;

    for (node = D_004469D4; node != NULL; node = node->next) {
        if (node->unk08 == object) {
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

    node = D_004469D4;
    if (!(D_0038CC80[0] & 0x10) && node != 0) {
        do {
            if (node->record != 0) {
                flags = node->flags;
                if (((u32)flags >> 2) & 1) {
                    height = node->unk2C + node->unk30;
                    node->unk2C = height;
                    if (node->unk28 <= height) {
                        // match: the model is read before the flags are stored, as retail schedules it
                        ModelInstance *model = node->model;
                        node->flags = flags & -5;
                        do { } while (0); // match: keeps the model load ahead of the flag store
                        model->position.y = node->unk28;
                    } else {
                        node->model->position.y = height;
                        node->unk30 += 1.0f;
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

    func_00109A50();
    func_00109A80(0x47, 0x5000D);
    func_00109AD0();
    for (node = D_004469D4; node != NULL; node = node->next) {
        node->map_record = Map_FindRecordAtInSlot((CameraState *)&node->model->position, 0.0f, node->flags & 1);
        if (node->map_record == NULL) {
            continue;
        }
        if (node == D_004469D8 && with_picked == 0) {
            continue;
        }
        Vector_Scale(scale, &node->model->unkE0, 1.0f);
        ModelMatrix_Build(node->model, &node->model->position, &node->model->rotation, scale);
        part = node->model->parts;
        count = node->model->part_count;
        for (i = 0; i < count; i++, part++) {
            ModelMatrix_BuildPartLocal(node->model, part, part);
        }
        node->model->unk148 = (s32)&map_draw_state.unkA90[node->flags & 1][map_slots[node->flags & 1].header->unk71];
        node->model->unk14C = &map_draw_state.unk290[node->flags & 1][((MapRecordInfo *)node->map_record)->unk34];
        LinkedPoint_FindHitsForModel(node->model);
        func_0010AEB8(node->model);
        func_0010DB40(node->model);
    }
}

/* an angle brought back into -pi..pi (for angles above -pi) */
static inline f32 wrap_angle(f32 angle) {
    angle += 3.1415927f;
    return angle - (f32)(s32)(angle / 6.2831855f) * 6.2831855f - 3.1415927f;
}

/* Draws the item being picked up, over everything else: each part's own offset and turn are scaled by
   D_0044719C, so that the model folds flat while it is held up. Nothing is drawn when hidden is set. */
void MapItem_DrawPicked(int hidden) {
    f32 scale[4];
    f32 offset[4];
    f32 turn[4];
    ItemNode *node = D_004469D8;
    ModelPart *part;
    s32 count;
    s32 i;

    if (node == NULL || hidden != 0) {
        return;
    }
    node->map_record = Map_FindRecordAtInSlot((CameraState *)&node->model->position, 0.0f, node->flags & 1);
    if (node->map_record == NULL) {
        return;
    }
    i = 0;
    func_0010A698();
    func_00109A50();
    func_00109A80(0x47, 0x5000D);
    func_00109AD0();
    Vector_Scale(scale, &node->model->unkE0, 1.0f);
    ModelMatrix_Build(node->model, &node->model->position, &node->model->rotation, scale);
    count = node->model->part_count;
    part = node->model->parts;
    for (; i < count; i++, part++) {
        Vector_Scale(offset, part->unk40, D_0044719C);
        Vector_Scale(turn, &part->unk50, D_0044719C);
        sceVu0CopyMatrix(part, D_00431AF0);
        PART_MATRIX(part)[0][0] = part->unk60[0];
        PART_MATRIX(part)[1][1] = part->unk60[1];
        PART_MATRIX(part)[2][2] = part->unk60[2];
        sceVu0RotMatrixX((f32 *)part, (f32 *)part, wrap_angle(turn[0]));
        sceVu0RotMatrixZ((f32 *)part, (f32 *)part, wrap_angle(turn[2]));
        sceVu0RotMatrixY((f32 *)part, (f32 *)part, wrap_angle(turn[1]));
        sceVu0TransMatrix((f32 *)part, (f32 *)part, offset);
    }
    node->model->unk148 = (s32)&map_draw_state.unkA90[node->flags & 1][map_slots[node->flags & 1].header->unk71];
    node->model->unk14C = &map_draw_state.unk290[node->flags & 1][((MapRecordInfo *)node->map_record)->unk34];
    LinkedPoint_FindHitsForModel(node->model);
    func_0010AEB8(node->model);
    func_0010DB40(node->model);
    func_00109E18();
}

/* Makes node the one being picked up, if none is yet and it is on the list of the ones in use. */
int MapItem_StartPickup(ItemNode *node) {
    if (node == NULL || D_004469D8 != NULL) {
        return FALSE;
    }
    if (D_004469D4 != node && node->prev == NULL && node->next == NULL) {
        return FALSE;
    }
    D_004469D8 = node;
    D_00447198 = 0;
    D_004471A4 = 0;
    return TRUE;
}

/* Whether the player may reach for the item: it lies about (is on the list) and is not inside a chest
   (a kind 4 map object). */
int MapItem_IsPickable(ItemNode *node) {
    MapObject *object;
    ItemNode *head;
    int result;

    head = D_004469D4;
    if (node == 0) {
        return 0;
    }
    if ((head != node) && (node->prev == 0) && (node->next == 0)) {
        return 0;
    }
    object = (MapObject *)node->unk08;
    result = 1;
    if (object != 0) {
        result = MAP_OBJECT_DEF_KIND(object->unk04) != 4;
    }
    return result;
}

/* Makes the current map's fixed item with this id the one being picked up, if none is yet and it
   is lying about; without may_put_back it cannot be put back. */
int MapItem_StartPickupById(int item_id, int may_put_back) {
    int n;
    ItemNode *node;

    n = MapItem_GetFixedIndex(D_00446770, item_id);
    if (D_004469D8 != 0 || n < 0) {
        return 0;
    }
    node = &D_0057E240[D_00446770][n];
    if (D_004469D4 != node && node->prev == 0 && node->next == 0) {
        return 0;
    }
    D_004469D8 = node;
    D_00447198 = may_put_back == 0;
    D_004471A4 = 0;
    return 1;
}

/* Runs the current step of the pickup in progress, if there is one. */
void MapItem_UpdatePickup(void) {
    if (D_004469D8 != NULL) {
        pickup_step_handlers[D_004471A4]();
    }
}

/* Pickup step 0: remembers where the item lies and aims it at a point in front of the player's eyes, five units
   a frame when it is near and in fifteen frames when it is not. */
void MapItem_PickupAim(void) {
    f32 delta[4];
    f32 distance;

    if (game_status.unk30 != 0) {
        return;
    }
    game_status.unk30 = 0x87F;
    func_001B9478();
    Vector_Copy(D_005B60C0, &D_004469D8->model->position);
    Vector_Copy(D_005B60D0, &D_004469D8->model->rotation);
    D_005B60D0[1] = Angle_Wrap(D_005B60D0[1]);
    D_005B60E0[0] = D_00420D00.unk00[0] + sinf(player_state.rotation.y) * 150.0f;
    D_005B60E0[2] = D_00420D00.unk00[2] + cosf(player_state.rotation.y) * 150.0f;
    D_005B60E0[1] = D_004469D8->model->position.y - 100.0f;
    if (D_005B60E0[1] < D_00420D00.unk00[1]) {
        D_005B60E0[1] = D_00420D00.unk00[1];
    }
    D_005B60E0[3] = 0.0f;
    D_004471A8 = atan2f(D_00420D00.unk00[1] - D_005B60E0[1], 150.0f);
    Vector_Subtract(delta, D_005B60E0, D_005B60C0);
    distance = sqrt(delta[0] * delta[0] + delta[1] * delta[1] + delta[2] * delta[2]);
    if (distance < 75.0f) {
        Vector_Scale(D_005B60F0, delta, 1.0f / distance);
        Vector_Scale(D_005B60F0, D_005B60F0, 5.0f);
        D_004471AC = distance / 5.0f;
    } else {
        Vector_Scale(D_005B60F0, delta, 0.06666667f);
        D_004471AC = 15;
    }
    D_0044719C = 1.0f;
    D_004471B0 = D_004471AC;
    D_004471A4++;
}

/* Pickup step 1: moves the item to the point in front of the player while its rotation eases to 0 and it
   folds flat (D_0044719C), and tilts the player's view toward it; then on to step 2. */
void MapItem_PickupBringUp(void) {
    D_004471B0--;
    if (D_004471B0 > 0) {
        Vector_Add(&D_004469D8->model->position, &D_004469D8->model->position, D_005B60F0);
        Angle_EaseToward(0.0f, &D_004469D8->model->rotation.x, D_004471B0);
        Angle_EaseToward(0.0f, &D_004469D8->model->rotation.y, D_004471B0);
        Angle_EaseToward(0.0f, &D_004469D8->model->rotation.z, D_004471B0);
        Angle_EaseToward(D_004471A8, player_rotation, D_004471B0);
        D_0044719C = (float)D_004471B0 / (float)D_004471AC;
        return;
    }
    Vector_Copy(&D_004469D8->model->position, D_005B60E0);
    D_0044719C = 0.0f;
    player_rotation[0] = D_004471A8;
    D_004471A4++;
}

/* Pickup step 2: the item turns in front of the player until it is taken (0x4000) or put back (0x2000). */
void MapItem_PickupHold(void) {
    f32 delta[4];

    D_0044719C = 0.0f;
    D_004469D8->model->rotation.y = Angle_Wrap(D_004469D8->model->rotation.y + 0.08377580f);
    if (pad_pressed[0] & 0x4000) {
        if (!Player_CannotCarryItem((s16)D_004469D8->item)) {
            Vector_Copy(D_005B60E0, &player_state);
            D_005B60E0[1] -= 115.0f;
            Vector_Subtract(delta, D_005B60E0, &D_004469D8->model->position);
            Vector_Scale(D_005B60F0, delta, 0.06666667f);
            D_004471B0 = 15;
            D_004471A4++;
            return;
        }
        Message_ShowNotice(0, 0, 0, 0, 0x2D, MSG_CANT_CARRY_ANY_MORE);
    } else if (!(pad_pressed[0] & 0x2000) || D_00447198 != 0) {
        return;
    }
    Vector_Scale(D_005B60F0, D_005B60F0, -1.0f);
    D_004471B0 = D_004471AC;
    D_004471A4 += 2;
}

/* Pickup step 3: moves the item to the player, then hands it over and frees its node. */
void MapItem_PickupTake(void) {
    ItemNode *node = D_004469D8;

    D_004471B0--;
    if (D_004471B0 > 0) {
        Vector_Add(&node->model->position, &node->model->position, D_005B60F0);
        return;
    }
    Player_AddItem((s16)node->item);
    if (node->def != NULL && node->def->required_flag != 0) {
        map_item_pickup_counts[map_slots[node->flags & 1].tag][node->index]++;
    } else if (node->unk08 != 0) {
        MapObject_MarkEmptied((MapObject *)node->unk08);
    } else if (node->record != NULL) {
        memset(node->record, 0, sizeof(DroppedItemRecord));
    }
    MapItem_Unlink(D_004469D8);
    D_0038CC80[0] = 0;
    D_0044719C = 1.0f;
    D_004469D8 = NULL;
    D_004471A4 = 0;
}

/* Pickup step 4 (put back): moves the item back to where it lay and lets go of it. */
void MapItem_PickupPutBack(void) {
    MapObject *object;

    D_004471B0--;
    if (D_004471B0 > 0) {
        Vector_Add(&D_004469D8->model->position, &D_004469D8->model->position, D_005B60F0);
        Angle_EaseToward(D_005B60D0[0], &D_004469D8->model->rotation.x, D_004471B0);
        Angle_EaseToward(D_005B60D0[1], &D_004469D8->model->rotation.y, D_004471B0);
        Angle_EaseToward(D_005B60D0[2], &D_004469D8->model->rotation.z, D_004471B0);
        D_0044719C = 1.0f - (f32)D_004471B0 / (f32)D_004471AC;
        return;
    }
    Vector_Copy(&D_004469D8->model->position, D_005B60C0);
    Vector_Copy(&D_004469D8->model->rotation, D_005B60D0);
    object = (MapObject *)D_004469D8->unk08;
    if (object != NULL && (object->unk10 & 0x10000) && object->unk04->kind == 3) {
        MapItem_Unlink(D_004469D8);
    }
    D_0038CC80[0] = 0;
    D_0044719C = 1.0f;
    D_004469D8 = NULL;
    D_004471A4 = 0;
}

/* Clears everything the item code keeps for a map slot. */
void MapItem_ClearMap(int map) {
    int i;

    D_00447190[map] = 0; // match: this order of the three gives the retail register for each table
    D_004469B8[map] = NULL;
    D_004469C0[map] = NULL;
    memset(D_0057E040[map], 0, sizeof(D_0057E040[0]));
    memset(D_0057E240[map], 0, sizeof(D_0057E240[0]));
    memset(D_00580040[map], 0, sizeof(D_00580040[0]));
    memset(D_005807C0[map], 0, sizeof(D_005807C0[0]));
    memset(D_005816C0[map], 0, sizeof(D_005816C0[0]));
    memset(D_0058F6C0[map], 0, sizeof(D_0058F6C0[0]));
    memset(D_00592EC0[map], 0, sizeof(D_00592EC0[0]));
    for (i = 0xFF; i >= 0; i--) {
        D_005B5EC0[map][i] = -1;
    }
}

/* Returns where the model and texture of item kind key lie in the slot's map (the map lists up to 32
   kinds), or NULL when the map has none for it. */
void *MapItem_FindModelSource(int slot, int key) {
    s16 *cursor;
    s16 value;
    int i;

    cursor = D_004469C0[slot];
    for (i = 0; i < 0x20; i++) {
        value = cursor[i];
        if (value < 0) {
            return NULL;
        }
        if (value == key) {
            return (char *)D_0057E040 + (slot << 8) + i * 8;
        }
    }
    return NULL;
}

/* Puts an item on the front of the list of items lying about (D_004469D4). */
void MapItem_Link(ListNode *node) {
    ListNode *head = (ListNode *)D_004469D4;

    node->prev = NULL;
    node->next = head;
    if (head != NULL) {
        head->prev = node;
    }
    D_004469D4 = (ItemNode *)node;
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
    if (node == D_004469D4) {
        D_004469D4 = node->next;
    }
    node->prev = NULL;
    node->next = NULL;
    node->unk08 = 0;
    node->record = NULL;
    return next;
}

/* Sends the model's textures and sets GS register 0x47 for drawing it. */
void MapItem_PrepareDraw(void *model) {
    func_0010AEB8(model);
    func_00109A50();
    func_00109A80(0x47, 0x5000D);
    func_00109AD0();
}

/* Returns the index of the slot's fixed item with this id (1..255), or -1. */
int MapItem_GetFixedIndex(int slot, int object_id) {
    if ((u32)(object_id - 1) < 0xFF) {
        return D_005B5EC0[slot][object_id];
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
