#include "kf_map_object.h"
#include "kf_collision.h"
#include "kf_effect.h"
#include "kf_entity.h"
#include "kf_game.h"
#include "kf_gs.h"
#include "kf_linked_point.h"
#include "kf_map.h"
#include "kf_player.h"
#include "kf_scroll_texture.h"
#include "kf_model.h"

void *memset(void *, int, u32);
float sinf(float);
float sqrtf(float);
float atan2f(float, float);
void Vector_Scale(float *, float *, float);
void Vector_Subtract(void *, void *, void *);
int Game_IsMapVisited(int);
int Game_GetObjectActive(int, int);
int Game_GetObjectState(int, int);
int Game_GetObjectFlag(int, int, u32);
int Game_GetTriggerItem(int, int);
void Game_SetObjectActive(int, int, int);
void Game_SetObjectState(int, int, int);
void Game_SetObjectFlag(int, int, u32, int);
void ModelMotion_Select(ModelInstance *, int);
void ModelMotion_Step(void *);
void Packet_BeginGsRegisters(void);
void Packet_AddGsRegister(int, s64);
void Packet_EndGsRegisters(void);
void GsDraw_EnableDepthTest(void);
void ModelTexture_SendForModel(void *);
void DrawList_Add(ModelInstance *, s32, s32);
void Model_DrawPosedWithAlpha(ModelInstance *);
MapRecordInfo *Map_FindRecordAtInSlot(CameraState *, f32, int);
int Map_TestFlagBlockBit(int, int, int, int);
void MapObject_DrawModel(int *);
void MapObject_DrawModelWithAlpha(ModelInstance *);
void MapObject_DrawModelNoDepthWrite(void *);
void MapObject_UpdateNoBonePoints(void);
char *MapObject_GetPositionAsPoint(char *);
void MapObject_StopSound(MapObject *);
float Angle_Wrap(float);
int Sound_PlayCommonAt(float, int, void *);
int Sound_PlayMapAtRange(u8, u8, float *, float);
s32 Sound_StartTaggedAt(s8, u8, u8, f32, Vector4Arg *);
int Sound_StopTagged(char);
void ModelMatrix_BuildPartLocal(ModelInstance *, ModelPart *, ModelPart *);
void ModelMatrix_Build(ModelInstance *, Vector4 *, Vector4 *, f32 *);
int Script_Start(int, int);
void Vector_Copy(void *, void *);
void Vector_Add(void *, void *, void *);

extern MapDrawState map_draw_state;
extern int suspended_updates[3];
extern MapSlot map_slots[];
extern Vector4 map_object_glow_colors[4];
extern u8 item_counts[];
extern MapObject map_slot_objects[][0x200];
extern void (*map_object_step_handlers[][16])(MapObject *);
extern void (*map_object_end_hooks[])(MapObject *);
extern int (*map_object_item_hooks[])(MapObject *, int);
extern void (*map_object_signal_hooks[])(MapObject *, int);
extern void (*map_object_update_hooks[])(MapObject *);
extern void (*map_object_offset_hooks[])(MapObject *, f32 *);
extern void (*map_object_setup_hooks[])(MapObject *);
extern PlayerState player_state;
extern int current_map_slot;

static s32 sway_frame = 0;                      /* 0x4467A0 counts 0 to 59 */
static s32 next_sound_channel = 0;              /* 0x4467A4 0 to 31, the next of the channels 0x20 to 0x3F */
s32 map_objects_started[2] = { 0, 0 };         /* the objects of each map have been started */
int map_object_placements[2] = { 0, 0 };                   /* the MapObjectUnk08 array of each map */
s32 map_object_counts[2] = { 0, 0 };           /* how many objects each map has */

static s32 glow_item_worn;                      /* 0x447078 Myu's Necklace is worn */
static s32 glow_item_was_worn;                  /* 0x44707C the same, last frame */
static s32 glow_frame;                          /* 0x447080 0 to 209 */
static MapObject *plain_draw_list[2];           /* 0x447088 per map, through MapObject.next_in_draw_list */
static MapObject *jointed_draw_list[2];         /* 0x447090 per map, through MapObject.next_in_draw_list */

static MapObject *objects_by_id[2][0x100];      /* 0x472240 per map, by MapObjectUnk08.spawn_flags.bytes[2] */
static MapSlotUnk10Vectors glow_lights_shared;  /* 0x472A40 map_draw_state.unkC0 blended for the glow */
static MapSlotUnk10Vectors glow_lights[2][0x20]; /* 0x472AC0 map_draw_state.unkA90 blended for the glow */

/* Empties the map objects of both maps. */
void MapObject_ClearAllMaps(void) {
    MapObject_ClearMap(0);
    MapObject_ClearMap(1);
}

/* Sets up the objects of a map once it is loaded: puts each model where its
   source says, works out how far the object reaches, picks the functions that
   draw it, runs the every-frame handler of its kind once, and threads the
   objects into the two draw lists of the map, grouped by the model they use. */
void MapObject_InitMap(int map) {
    int i = 0;
    int count = map_object_counts[map];
    MapObject *object = map_slot_objects[map];
    f32 scale;
    int link_id;
    int j;

    for (; i < count; i++, object++) {
        MAP_OBJECT_BYTES(object)->step = 0;
        if (object->definition->model_index == 0xFFFF) {
            object->model->get_point = (int (*)(void *, int))MapObject_GetPositionAsPoint;
            object->model->update_bone_points = (void (*)(ModelInstance *))MapObject_UpdateNoBonePoints;
        }
        if (object->source->spawn_flags.bytes[2] != 0) {
            link_id = object->source->spawn_flags.bytes[2];
            if (objects_by_id[map][link_id] == NULL) {
                objects_by_id[map][link_id] = object;
            }
        }
        Vector_Copy(&object->model->position, &object->source->position);
        Vector_Copy(&object->model->rotation, &object->source->rotation);
        Vector_Copy(&object->model->scale_x, &object->source->scale);
        object->last_position_x = object->model->position.x;
        object->last_position_y = object->model->position.y;
        object->last_position_z = object->model->position.z;
        object->last_rotation_x = object->model->rotation.x;
        object->last_rotation_y = object->model->rotation.y;
        object->last_rotation_z = object->model->rotation.z;
        scale = ABS(object->source->scale.x) > ABS(object->source->scale.z) ? ABS(object->source->scale.x) : ABS(object->source->scale.z);
        if (object->definition->collision_radius > 0.0f) {
            object->reach_radius = object->definition->collision_radius * scale + 100.0f;
        } else if (object->model->strips != NULL) {
            object->reach_radius = ((CollisionMesh *)object->model->strips)->reach_radius * scale + 100.0f;
        } else {
            object->reach_radius = 0.0f;
        }
        object->interact_radius = object->definition->interact_radius * scale + 100.0f;
        object->model->radius = object->reach_radius;
        if (object->model->model_flags & 1) {
            MAP_OBJECT_BYTES(object)->anim = 0;
        } else {
            MAP_OBJECT_BYTES(object)->anim = -1;
            ModelMatrix_Build(object->model, &object->model->position, &object->model->rotation, &object->model->scale_x);
            for (j = 0; j < object->model->part_count; j++) {
                ModelMatrix_BuildPartLocal(object->model, &object->model->parts[j], &object->model->parts[j]);
            }
        }
        map_object_update_hooks[object->definition->kind](object);
        switch (object->definition->bit8) {
            case 0:
                object->model->draw = (void (*)(ModelInstance *))MapObject_DrawModel;
                object->model->draw_parts = Model_DrawPosedWithAlpha;
                break;
            case 1:
                object->model->draw = MapObject_DrawModelWithAlpha;
                break;
            default:
                object->model->draw = NULL;
                break;
        }
        if (object->definition->kind == 5) {
            object->model->draw = (void (*)(ModelInstance *))MapObject_DrawModelNoDepthWrite;
        }
        if (object->source->def_index == 0x91) {
            object->model->on_textures_sent = (void (*)(ModelInstance *))ScrollTexture_DrawAll;
            ScrollTexture_Init();
        }
        MAP_OBJECT_BITS(object)->bit1 = FALSE;
        MAP_OBJECT_BITS(object)->bit2 = FALSE;
    }
    plain_draw_list[map] = NULL;
    for (i = ((MapSlotUnk08 *)map_slots[map].header)->plain_object_model_count - 1; i >= 0; i--) {
        for (object = &map_slot_objects[map][count - 1], j = count - 1; j >= 0; j--, object--) {
            if (MAP_OBJECT_DEF_WORD(object) & 0x400) {
                continue;
            }
            if (MAP_OBJECT_DEF_WORD(object) & 0x100) {
                continue;
            }
            if (object->source->model_index != i) {
                continue;
            }
            object->next_in_draw_list = plain_draw_list[map];
            plain_draw_list[map] = object;
        }
    }
    jointed_draw_list[map] = NULL;
    for (i = ((MapSlotUnk08 *)map_slots[map].header)->skinned_object_model_count - 1; i >= 0; i--) {
        for (object = &map_slot_objects[map][count - 1], j = count - 1; j >= 0; j--, object--) {
            if (MAP_OBJECT_DEF_WORD(object) & 0x400) {
                continue;
            }
            if (!(MAP_OBJECT_DEF_WORD(object) & 0x100)) {
                continue;
            }
            if (object->source->model_index != i) {
                continue;
            }
            object->next_in_draw_list = jointed_draw_list[map];
            jointed_draw_list[map] = object;
        }
    }
}

/* Starts the objects of a map: their flags come from the flag block of the map
   when it has been visited, else from their sources; each is then put in use or
   handed to the handler of its kind in map_object_setup_hooks. */
void MapObject_StartMap(int map) {
    int i = 0;
    int count = map_object_counts[map];
    MapObject *object = map_slot_objects[map];
    int tag = map_slots[map].tag;
    int visited = Game_IsMapVisited(tag);
    int in_use;
    int flag;
    MapObjectUnk08 *source;
    u8 kind;

    map_objects_started[map] = 1;
    for (; i < count; i++, object++) {
        source = object->source;
        flag = 0;
        in_use = ((s32)(source->spawn_flags.flags >> 49) ^ 1) & 1;
        if (visited) {
            in_use = Game_GetObjectActive(tag, i);
            flag = Game_GetObjectState(tag, i);
            MAP_OBJECT_BITS(object)->bit18 = Game_GetObjectFlag(tag, i, 0);
            MAP_OBJECT_BITS(object)->bit19 = Game_GetObjectFlag(tag, i, 1);
        } else {
            kind = object->definition->kind;
            if (kind == 8 || kind == 4 || kind == 0xD || kind == 0x10) {
                flag = ((MapObjectUnk08Words *)source)->kind_param_7;
            }
        }
        if (Game_GetTriggerItem(tag, object->source->spawn_flags.bytes[2])) {
            flag = 1;
        }
        object->map_record = (s32)Map_FindRecordAtInSlot((CameraState *)&object->model->position, 0.0f, MAP_OBJECT_BYTES(object)->unk00 & 1);
        if (in_use) {
            MapObject_Activate(object);
        } else {
            map_object_setup_hooks[object->definition->kind](object);
        }
        MAP_OBJECT_BITS(object)->bit17 = flag;
    }
}

/* Unloads the objects of a map: saves four flags of each into the flag block
   of the map, stops their sounds, then empties the table. */
void MapObject_UnloadMap(int map) {
    int count = map_object_counts[map];
    MapObject *object = map_slot_objects[map];
    int tag = map_slots[map].tag;
    int i;

    for (i = 0; i < count; i++, object++) {
        Game_SetObjectActive(tag, i, MAP_OBJECT_BITS(object)->bit16);
        Game_SetObjectState(tag, i, MAP_OBJECT_BITS(object)->bit17);
        Game_SetObjectFlag(tag, i, 0, MAP_OBJECT_BITS(object)->bit18);
        Game_SetObjectFlag(tag, i, 1, MAP_OBJECT_BITS(object)->bit19);
        MapObject_StopSound(object);
    }
    MapObject_ClearMap(map);
}

/* Saves four flags of every object of both maps into the flag blocks of the maps. */
void MapObject_SaveAllFlags(void) {
    int map;
    int count;
    MapObject *object;
    int tag;
    int i;

    for (map = 0; map < 2; map++) {
        if (map_slots[map].loaded != 0) {
            count = map_object_counts[map];
            tag = map_slots[map].tag;
            object = map_slot_objects[map];
            for (i = 0; i < count; i++, object++) {
                // match: each flag is narrowed to a byte first; without these four conversions the outer loop is
                // small enough for the compiler to hoist the 0xE000 object-table stride out of it
                Game_SetObjectActive(tag, i, (u8)MAP_OBJECT_BITS(object)->bit16);
                Game_SetObjectState(tag, i, (u8)MAP_OBJECT_BITS(object)->bit17);
                Game_SetObjectFlag(tag, i, 0, (u8)MAP_OBJECT_BITS(object)->bit18);
                Game_SetObjectFlag(tag, i, 1, (u8)MAP_OBJECT_BITS(object)->bit19);
            }
        }
    }
}

/* Moves every object of a map by offset through its kind's handler in map_object_offset_hooks. */
void MapObject_OffsetAll(int map, f32 *offset) {
    int count = map_object_counts[map];
    MapObject *object = map_slot_objects[map];
    int i;

    for (i = 0; i < count; i++, object++) {
        map_object_offset_hooks[object->definition->kind](object, offset);
    }
}

/* the objects of the current map (map 0) or the other loaded one (map 1), which the game inlines */
static inline MapObject *map_objects(s32 map) {
    if (map_objects_started[current_map_slot == map ? 0 : 1] != 0) {
        if (current_map_slot != map) {
            return map_slot_objects[1];
        }
        return map_slot_objects[0];
    }
    return NULL;
}

static inline s32 map_object_count(s32 map) {
    if (map_objects_started[current_map_slot == map ? 0 : 1] != 0) {
        return map_object_counts[current_map_slot == map ? 0 : 1];
    }
    return 0;
}

/* Updates the map objects of both maps once a frame: remembers where each was,
   runs the step handler of its kind (map_object_step_handlers) and the kind's every-frame
   handler (map_object_update_hooks), works out whether it is in view, and starts or stops
   the effect tied to it as it comes into or goes out of view. */
void MapObject_UpdateAll(void) {
    Vector4 angles;
    int period;
    int map;
    int i;
    int count;
    MapObject *object;
    ModelInstance *model;
    ModelInstance *shown;
    MapObjectAnim *anim;
    MapRecord *record;
    MapObjectUnk08 *source;
    int kind;
    int in_view;
    f32 *from;
    f32 *next;
    f32 delta_x;
    f32 delta_y;
    f32 delta_z;

    if (suspended_updates[0] & 8) {
        return;
    }
    period = 60; // match: the divisor is a variable
    sway_frame = (sway_frame + 1) % period;
    sinf(sway_frame * 6.2831855f / 60.0f); // match: the result is not used
    for (map = 0; map < 2; map++) {
        object = map_objects(map);
        count = map_object_count(map);
        for (i = 0; i < count; i++, object++) {
            if (!(object->state_flags & 0x10000)) {
                continue;
            }
            model = object->model;
            object->last_position_x = model->position.x;
            object->last_position_y = model->position.y;
            object->last_position_z = model->position.z;
            object->last_rotation_x = model->rotation.x;
            object->last_rotation_y = model->rotation.y;
            object->last_rotation_z = model->rotation.z;
            MAP_OBJECT_BITS(object)->bit2 = MAP_OBJECT_BITS(object)->bit1;
            MAP_OBJECT_BITS(object)->bit1 = FALSE;
            MAP_OBJECT_BITS_LOW(object)->bit3 = FALSE;
            if (MODEL_UNK100_BITS(model)->has_motion) {
                anim = &object->definition->anims[MAP_OBJECT_BYTES(object)->anim];
                if (anim->unk06 == MODEL_CHANNEL(model)->param) {
                    Effect_StartSpawner(anim->unk02, &model->position, &model->rotation, object);
                }
            }
            kind = (object->state_flags & 0x100000) ? 0 : object->definition->kind;
            if (MAP_OBJECT_BYTES(object)->step < 0x10) {
                if (map_object_step_handlers[kind][MAP_OBJECT_BYTES(object)->step] != NULL) {
                    map_object_step_handlers[kind][MAP_OBJECT_BYTES(object)->step](object);
                }
            }
            if (!(object->state_flags & 0x10000)) {
                continue;
            }
            map_object_update_hooks[object->definition->kind](object);
            record = (MapRecord *)object->map_record;
            if (record != NULL) {
                if ((MapObject *)player_state.floor_map_object == object) {
                    object->state_flags |= 2;
                } else if (object->source->linked_point_owner != 0 && object->source->linked_point_owner != 0xFF) {
                    if (Map_TestFlagBlockBit(current_map_slot, player_state.record_flag_block, MAP_OBJECT_BYTES(object)->unk00 & 1, object->source->linked_point_owner)) {
                        object->state_flags |= 2;
                    }
                } else if (record->record_flags & 0x4000000) {
                    object->state_flags |= 2;
                }
            }
            source = object->source;
            if (source->appear_effect == 0) {
                continue;
            }
            in_view = object->state_flags & 6;
            if (in_view == 2) {
                shown = object->model;
                if (shown->parts != NULL) {
                    shown->update_bone_points(shown);
                    from = (f32 *)object->model->get_point(object->model, object->source->effect_start_point);
                    next = (f32 *)object->model->get_point(object->model, object->source->effect_start_point + 1);
                    delta_x = next[0] - from[0];
                    delta_y = next[1] - from[1];
                    delta_z = next[2] - from[2];
                    angles.x = atan2f(delta_y, sqrtf(delta_x * delta_x + delta_z * delta_z));
                    angles.y = Angle_Wrap(-atan2f(delta_z, delta_x) + 1.5707964f);
                    angles.z = angles.w = 0.0f;
                    Effect_StartSpawner(object->source->appear_effect, from, &angles, object);
                } else {
                    Effect_StartSpawner(source->appear_effect, &shown->position, &shown->rotation, object);
                }
            } else if (in_view == 4) {
                Effect_FreeForOwner((Entity *)object);
            }
        }
    }
}

/* Uses an item on a map object through its kind's handler; FALSE when the item isn't carried. */
int MapObject_UseItem(MapObject *object, int item) {
    int used;

    if (item_counts[item] == 0) {
        used = FALSE;
    } else {
        used = map_object_item_hooks[object->definition->kind](object, item) != 0;
    }
    return used;
}

/* Passes a signal to a map object's kind (map_object_signal_hooks) unless a script drives it. */
void MapObject_Signal(MapObject *object, int signal) {
    if (object != NULL && !(object->state_flags & 0x100000)) {
        map_object_signal_hooks[object->definition->kind](object, signal);
    }
}

/* the flags of the map record an object stands in, and which of the two maps that record is of */
#define RECORD_FLAGS(object) (((MapDrawRecord *)(object)->map_record)->flags)
#define RECORD_MAP(object) ((u8)(((MapRecord *)(object)->map_record)->record_flags >> 27) & 1)

/* Draws the map objects of both maps (or of one, when only_one is set): first
   the jointed ones, then the plain ones. While the player wears item 0x9D the
   objects that allow it are lit with their light blocks blended towards
   map_object_glow_colors and back, over 210 frames (fully at frame 105). */
void MapObject_DrawAll(int mode, int only_one, int which) {
    f32 scaled[4];
    f32 blend;
    f32 rest;
    int map;
    int i;
    MapSlotUnk10Vectors *dest;
    MapSlotUnk10Vectors *source;
    MapObject *object;

    glow_item_was_worn = glow_item_worn;
    glow_item_worn = player_state.equipment[EQUIP_AMULET] == ITEM_WORN_INDEX(ITEM_MYUS_NECKLACE);
    if (glow_item_was_worn != glow_item_worn) {
        glow_frame = 0;
    } else if (glow_item_was_worn != 0) {
        glow_frame = (glow_frame + 1) % 210;
    }
    if (glow_frame < 105) {
        blend = glow_frame / 105.0f;
    } else {
        blend = (210 - glow_frame) / 105.0f;
    }
    rest = 1.0f - blend;
    for (map = 0; map < 2; map++) {
        if (map_slots[map].loaded != 0) {
            for (i = 0; i < 0x20; i++) {
                dest = &glow_lights[map][i];
                source = (MapSlotUnk10Vectors *)&map_draw_state.light_blocks[map][i];
                Vector_Copy(&dest->unk00[0], &source->unk00[0]);
                Vector_Copy(&dest->unk00[1], &source->unk00[1]);
                Vector_Copy(&dest->unk00[2], &source->unk00[2]);
                Vector_Copy(&dest->unk00[3], &source->unk00[3]);
                Vector_Scale(scaled, &map_object_glow_colors[0].x, blend);
                Vector_Scale(&dest->unk40[0].x, &source->unk40[0].x, rest);
                Vector_Add(&dest->unk40[0], &dest->unk40[0], scaled);
                Vector_Scale(scaled, &map_object_glow_colors[1].x, blend);
                Vector_Scale(&dest->unk40[1].x, &source->unk40[1].x, rest);
                Vector_Add(&dest->unk40[1], &dest->unk40[1], scaled);
                Vector_Scale(scaled, &map_object_glow_colors[2].x, blend);
                Vector_Scale(&dest->unk40[2].x, &source->unk40[2].x, rest);
                Vector_Add(&dest->unk40[2], &dest->unk40[2], scaled);
                Vector_Scale(scaled, &map_object_glow_colors[3].x, blend);
                Vector_Scale(&dest->unk40[3].x, &source->unk40[3].x, rest);
                Vector_Add(&dest->unk40[3], &dest->unk40[3], scaled);
            }
        }
    }
    Vector_Copy(&glow_lights_shared.unk00[0], &((MapSlotUnk10Vectors *)&map_draw_state.unkC0)->unk00[0]);
    Vector_Copy(&glow_lights_shared.unk00[1], &((MapSlotUnk10Vectors *)&map_draw_state.unkC0)->unk00[1]);
    Vector_Copy(&glow_lights_shared.unk00[2], &((MapSlotUnk10Vectors *)&map_draw_state.unkC0)->unk00[2]);
    Vector_Copy(&glow_lights_shared.unk00[3], &((MapSlotUnk10Vectors *)&map_draw_state.unkC0)->unk00[3]);
    Vector_Scale(scaled, &map_object_glow_colors[0].x, blend);
    Vector_Scale(&glow_lights_shared.unk40[0].x, &((MapSlotUnk10Vectors *)&map_draw_state.unkC0)->unk40[0].x, rest);
    Vector_Add(&glow_lights_shared.unk40[0], &glow_lights_shared.unk40[0], scaled);
    Vector_Scale(scaled, &map_object_glow_colors[1].x, blend);
    Vector_Scale(&glow_lights_shared.unk40[1].x, &((MapSlotUnk10Vectors *)&map_draw_state.unkC0)->unk40[1].x, rest);
    Vector_Add(&glow_lights_shared.unk40[1], &glow_lights_shared.unk40[1], scaled);
    Vector_Scale(scaled, &map_object_glow_colors[2].x, blend);
    Vector_Scale(&glow_lights_shared.unk40[2].x, &((MapSlotUnk10Vectors *)&map_draw_state.unkC0)->unk40[2].x, rest);
    Vector_Add(&glow_lights_shared.unk40[2], &glow_lights_shared.unk40[2], scaled);
    Vector_Scale(scaled, &map_object_glow_colors[3].x, blend);
    Vector_Scale(&glow_lights_shared.unk40[3].x, &((MapSlotUnk10Vectors *)&map_draw_state.unkC0)->unk40[3].x, rest);
    Vector_Add(&glow_lights_shared.unk40[3], &glow_lights_shared.unk40[3], scaled);
    Packet_BeginGsRegisters();
    Packet_AddGsRegister(GS_REG_ALPHA_1, 0x44);
    Packet_AddGsRegister(GS_REG_ALPHA_2, 9);
    Packet_AddGsRegister(GS_REG_TEST_1, 0x5000D);
    Packet_AddGsRegister(GS_REG_TEST_2, 0x5000D);
    Packet_EndGsRegisters();
    for (map = 0; map < 2; map++) {
        if (only_one != 0 && which != map) {
            continue;
        }
        for (object = jointed_draw_list[map]; object != NULL; object = object->next_in_draw_list) {
            if (!(object->state_flags & 2)) {
                continue;
            }
            if (mode != 0 && !(RECORD_FLAGS(object) & 0x20)) {
                continue;
            }
            if (only_one != 0) {
                if (!(object->definition->definition_flags & 0x10000)) {
                    continue;
                }
            } else {
                if (object->definition->definition_flags & 0x10000) {
                    continue;
                }
            }
            if ((MAP_OBJECT_DEF_WORD(object) & 0x4000) && glow_item_worn != 0) {
                if ((s8)object->source->spawn_flags.bytes[5] != -1) {
                    object->model->light_block = (s32)&glow_lights[MAP_OBJECT_BYTES(object)->unk00 & 1][(s8)object->source->spawn_flags.bytes[5]];
                } else if (RECORD_FLAGS(object) & 0x10) {
                    object->model->light_block = (s32)&glow_lights_shared;
                } else {
                    object->model->light_block = (s32)&glow_lights[RECORD_MAP(object)][((MapDrawRecord *)object->map_record)->light_block_index];
                }
            } else {
                if ((s8)object->source->spawn_flags.bytes[5] != -1) {
                    object->model->light_block = (s32)&map_draw_state.light_blocks[MAP_OBJECT_BYTES(object)->unk00 & 1][(s8)object->source->spawn_flags.bytes[5]];
                } else if (RECORD_FLAGS(object) & 0x10) {
                    object->model->light_block = (s32)&map_draw_state.unkC0;
                } else {
                    object->model->light_block = (s32)&map_draw_state.light_blocks[RECORD_MAP(object)][((MapDrawRecord *)object->map_record)->light_block_index];
                }
            }
            if ((s8)object->source->spawn_flags.bytes[4] != -1) {
                object->model->fog_block = &map_draw_state.fog_blocks[MAP_OBJECT_BYTES(object)->unk00 & 1][(s8)object->source->spawn_flags.bytes[4]];
            } else if (RECORD_FLAGS(object) & 0x10) {
                object->model->fog_block = &map_draw_state.unkA0;
            } else {
                object->model->fog_block = &map_draw_state.fog_blocks[RECORD_MAP(object)][((MapDrawRecord *)object->map_record)->fog_block_index];
            }
            if (object->definition->kind == 9) {
                LinkedPoint_FindHitsExclude(object->model, *(s16 *)object->work);
            } else if (object->source->linked_point_owner != 0 && object->source->linked_point_owner != 0xFF) {
                LinkedPoint_FindHitsForOwner(object->model, object->source->linked_point_owner);
            } else {
                LinkedPoint_FindHitsForOwner(object->model, ((MapRecordInfo *)object->map_record)->flag_block_bit);
            }
            if ((object->source->spawn_flags.flags & 0x1000000000000L) || (object->model->alpha < 128.0f && !(object->definition->definition_flags & 0x10000))) {
                DrawList_Add(object->model, 0, mode);
            } else {
                ModelTexture_SendForModel(object->model);
                object->model->draw_parts(object->model);
            }
        }
    }
    for (map = 0; map < 2; map++) {
        if (only_one != 0 && which != map) {
            continue;
        }
        for (object = plain_draw_list[map]; object != NULL; object = object->next_in_draw_list) {
            if (!(object->state_flags & 2)) {
                continue;
            }
            if (mode != 0 && !(RECORD_FLAGS(object) & 0x20)) {
                continue;
            }
            if (only_one != 0) {
                if (!(object->definition->definition_flags & 0x10000)) {
                    continue;
                }
            } else {
                if (object->definition->definition_flags & 0x10000) {
                    continue;
                }
            }
            if ((MAP_OBJECT_DEF_WORD(object) & 0x4000) && glow_item_worn != 0) {
                if ((s8)object->source->spawn_flags.bytes[5] != -1) {
                    object->model->light_block = (s32)&glow_lights[MAP_OBJECT_BYTES(object)->unk00 & 1][(s8)object->source->spawn_flags.bytes[5]];
                } else if (RECORD_FLAGS(object) & 0x10) {
                    object->model->light_block = (s32)&glow_lights_shared;
                } else {
                    object->model->light_block = (s32)&glow_lights[RECORD_MAP(object)][((MapDrawRecord *)object->map_record)->light_block_index];
                }
            } else {
                if ((s8)object->source->spawn_flags.bytes[5] != -1) {
                    object->model->light_block = (s32)&map_draw_state.light_blocks[MAP_OBJECT_BYTES(object)->unk00 & 1][(s8)object->source->spawn_flags.bytes[5]];
                } else if (RECORD_FLAGS(object) & 0x10) {
                    object->model->light_block = (s32)&map_draw_state.unkC0;
                } else {
                    object->model->light_block = (s32)&map_draw_state.light_blocks[RECORD_MAP(object)][((MapDrawRecord *)object->map_record)->light_block_index];
                }
            }
            if ((s8)object->source->spawn_flags.bytes[4] == -1) {
                if (RECORD_FLAGS(object) & 0x10) {
                    object->model->fog_block = &map_draw_state.unkA0;
                } else {
                    object->model->fog_block = &map_draw_state.fog_blocks[RECORD_MAP(object)][((MapDrawRecord *)object->map_record)->fog_block_index];
                }
            } else {
                object->model->fog_block = &map_draw_state.fog_blocks[MAP_OBJECT_BYTES(object)->unk00 & 1][(s8)object->source->spawn_flags.bytes[4]];
            }
            if (object->definition->kind == 9) {
                LinkedPoint_FindHitsExclude(object->model, *(s16 *)object->work);
            } else if (object->source->linked_point_owner != 0 && object->source->linked_point_owner != 0xFF) {
                LinkedPoint_FindHitsForOwner(object->model, object->source->linked_point_owner);
            } else {
                LinkedPoint_FindHitsForOwner(object->model, ((MapRecordInfo *)object->map_record)->flag_block_bit);
            }
            if ((object->source->spawn_flags.flags & 0x1000000000000L) || (object->model->fade_enabled != 0 && (s16)object->model->fade_alpha < 0x80 && !(object->definition->definition_flags & 0x10000))) {
                DrawList_Add(object->model, 0, mode);
            } else {
                ModelTexture_SendForModel(object->model);
                object->model->draw_parts(object->model);
            }
        }
    }
    GsDraw_EnableDepthTest();
}

/* Puts a map object in use: not started, fully opaque. */
void MapObject_Activate(MapObject *object) {
    ModelInstance *model;

    if (object != NULL && !(object->state_flags & 0x10000)) {
        object->state_flags |= 0x10000;
        MAP_OBJECT_BITS(object)->step = 0;
        MAP_OBJECT_BITS(object)->bit1 = FALSE;
        MAP_OBJECT_BITS(object)->bit2 = FALSE;
        MAP_OBJECT_BITS(object)->anim = 4;
        model = object->model;
        model->fade_enabled = 0;
        model->colour_red = model->colour_green = model->colour_blue = model->alpha = 128.0f;
        object->model->fade_alpha = 0x80;
    }
}

/* Takes a map object out of use: ends its kind's state machine and clears its state. */
void MapObject_Deactivate(MapObject *object) {
    if (object != NULL) {
        if (MAP_OBJECT_BITS(object)->step != 0) {
            map_object_end_hooks[object->definition->kind](object);
        }
        MapObject_StopSound(object);
        Effect_FreeForOwner((Entity *)object); // match: shared prototype takes an Entity
        object->state_flags &= ~0x10000;
        MAP_OBJECT_BITS(object)->step = 0;
        MAP_OBJECT_BITS(object)->bit1 = FALSE;
        MAP_OBJECT_BITS(object)->bit2 = FALSE;
        memset(object->work, 0, sizeof(object->work));
    }
}

/* Looks a map object up by its object_id (1 to 255); NULL when the object_id is out of range. */
MapObject *MapObject_GetById(int map, int object_id) {
    if ((u32)(object_id - 1) < 0xFF) {
        return objects_by_id[map][object_id];
    }
    return NULL;
}

/* Breaks a breakable map object that has started: step 0xF, its breaking
   animation, an effect, a sound, and the object named by its source comes into use. */
void MapObject_Break(MapObject *object) {
    MapObject *other;

    if (!(MAP_OBJECT_DEF_WORD(object) & 0x800)) {
        return;
    }
    if (MAP_OBJECT_BYTES(object)->step == 0) {
        return;
    }
    if (MAP_OBJECT_BYTES(object)->step == 0xF) {
        return;
    }
    object->state_flags &= ~0x100000;
    MAP_OBJECT_BYTES(object)->step = 0xF;
    if (MAP_OBJECT_DEF_WORD(object) & 0x1000) {
        MapObject_StartAnim(object, 1);
    }
    Effect_StartSpawner(object->definition->break_effect, &object->model->position, &object->model->rotation, &map_slots[MAP_OBJECT_BYTES(object)->unk00 & 1]);
    if (object->source->break_script != 0 && (MAP_OBJECT_BYTES(object)->unk00 & 1) == current_map_slot) {
        Script_Start(object->source->break_script, 0);
    }
    if (object->definition->definition_flags & 0x20000) {
        other = MapObject_GetById(MAP_OBJECT_BYTES(object)->unk00 & 1, object->source->spawn_flags.bytes[3]);
        if (other != NULL) {
            MapObject_Activate(other);
        }
    }
}

/* Starts one of a map object's animations from its first frame. */
// match: declared s16 but returns nothing; with void its callers allocate $2 differently
s16 MapObject_StartAnim(MapObject *object, int anim) {
    ModelChannel *channel;

    ModelMotion_Select(object->model, object->definition->anims[anim].anim);
    MAP_OBJECT_BYTES(object)->anim = anim;
    channel = MODEL_CHANNEL(object->model);
    channel->param = 0;
    channel->param_shifted = 0;
    MapObject_StepAnim(object);
}

/* Advances a map object's animation one frame, starting the sounds tied to the
   frame it is on; returns whether the animation is still running. */
int MapObject_StepAnim(MapObject *object) {
    MapObjectAnim *anim = &object->definition->anims[MAP_OBJECT_BYTES(object)->anim];
    ModelInstance *model;
    f32 range;

    if (anim->sound != 0xFFFF) {
        model = object->model;
        if (anim->sound_frame == MODEL_CHANNEL(model)->param) {
            range = 800.0f;
            if (object->definition->sound_range > 0.0f) {
                range = object->definition->sound_range;
            }
            switch (anim->sound_kind) {
                case 0:
                    Sound_PlayCommonAt(range, anim->sound, &model->position);
                    break;
                case 1:
                    // match: the range is evaluated before the position
                    ((int (*)(u8, u8, f32, f32 *))Sound_PlayMapAtRange)(anim->sound, object->definition->sound_falloff_mode, range, &model->position.x);
                    break;
                case 2:
                    Sound_StartTaggedAt((u8)next_sound_channel + 0x20, anim->sound, object->definition->sound_falloff_mode, range, (Vector4Arg *)&model->position);
                    object->sound = (u8)next_sound_channel + 0x20;
                    next_sound_channel = (next_sound_channel + 1) % 32;
                    break;
            }
        }
    }
    if (MAP_OBJECT_BITS(object)->anim == 4 || MAP_OBJECT_BITS(object)->anim == 5) {
        anim = &object->definition->anims[MAP_OBJECT_BYTES(object)->anim + 2];
        if (anim->sound != 0xFFFF && anim->sound_frame == MODEL_CHANNEL(object->model)->param) {
            range = 800.0f;
            if (object->definition->sound_range > 0.0f) {
                range = object->definition->sound_range;
            }
            MapObject_StopSound(object);
            switch (anim->sound_kind) {
                case 0:
                    Sound_PlayCommonAt(range, anim->sound, &object->model->position);
                    break;
                case 1:
                    Sound_PlayMapAtRange(anim->sound, object->definition->sound_falloff_mode, &object->model->position.x, range);
                    break;
                case 2:
                    break;
            }
        }
    }
    ModelMotion_Step(object->model);
    object->model->update_bone_points(object->model);
    object->state_flags |= 8;
    model = object->model;
    if (MODEL_CHANNEL(model)->param == 0) {
        MapObject_StopSound(object);
        model = object->model; // match: re-read here although the return reads it again; keeps the model in $6
    }
    return MODEL_CHANNEL(object->model)->param != 0;
}

/* Marks a map object that holds something (bones and gravestones, chests, containers) of the map's own sources as emptied. */
void MapObject_MarkEmptied(MapObject *object) {
    u8 kind = object->definition->kind;

    if ((kind == 3 || kind == MAP_OBJECT_KIND_CHEST || kind == MAP_OBJECT_KIND_CONTAINER) &&
        ((u32)(((int)object->source - map_object_placements[MAP_OBJECT_BYTES(object)->unk00 & 1]) >> 7) >> 7) < 0x200) {
        object->state_flags |= 0x40000;
    }
}

/* MapObject_MarkEmptied on the map object with this id. */
void MapObject_MarkEmptiedById(int map, int object_id) {
    MapObject *object = MapObject_GetById(map, object_id);
    u8 kind;

    if (object != NULL) {
        kind = object->definition->kind;
        if ((kind == 3 || kind == MAP_OBJECT_KIND_CHEST || kind == MAP_OBJECT_KIND_CONTAINER) &&
            ((u32)(((int)object->source - map_object_placements[MAP_OBJECT_BYTES(object)->unk00 & 1]) >> 7) >> 7) < 0x200) {
            object->state_flags |= 0x40000;
        }
    }
}

/* The turn that would face a map object toward a point (wrapped by Angle_Wrap). */
f32 MapObject_GetTurnToward(MapObject *object, Vector4 *point) {
    Vector4 delta;

    Vector_Subtract(&delta, &object->model->position, point);
    return Angle_Wrap(atan2f(delta.x, delta.z) - object->model->rotation.y);
}

/* Hands a map object to a script (bit 20) or back, restarting its state machine. */
void MapObject_SetScripted(MapObject *object, int scripted) {
    if (MAP_OBJECT_BITS(object)->bit20 != scripted) {
        MAP_OBJECT_BITS(object)->bit20 = scripted;
        MAP_OBJECT_BITS(object)->bit21 = FALSE;
        MAP_OBJECT_BITS(object)->bit22 = FALSE;
        MAP_OBJECT_BYTES(object)->anim = 0;
        MAP_OBJECT_BYTES(object)->step = 0;
    }
}

/* Restarts a scripted map object (model flag 1, bit 20) on another animation. */
void MapObject_PlayScriptedAnim(MapObject *object, int anim, int bit22) {
    if ((object->model->model_flags & 1) && (object->state_flags & 0x100000)) {
        MAP_OBJECT_BITS(object)->bit21 = FALSE;
        MAP_OBJECT_BITS(object)->bit22 = bit22;
        MAP_OBJECT_BITS(object)->anim = anim;
        MAP_OBJECT_BITS(object)->step = 0;
    }
}

/* Sets bit 21 of a scripted map object (model flag 1, bit 20). */
void MapObject_SetScriptedBit21(MapObject *object, int bit21) {
    if ((object->model->model_flags & 1) && (object->state_flags & 0x100000)) {
        MAP_OBJECT_BITS(object)->bit21 = bit21;
    }
}

/* Empties one map's objects: the two counts, the two draw lists and the tables. */
void MapObject_ClearMap(int map) {
    map_objects_started[map] = 0;
    map_object_counts[map] = 0;
    plain_draw_list[map] = NULL;
    jointed_draw_list[map] = NULL;
    memset(map_slot_objects[map], 0, sizeof(map_slot_objects[map]));
    memset(objects_by_id[map], 0, sizeof(objects_by_id[map]));
}

/* Draw hook of a map object model drawn with the GS state as it is: draws the model. */
void MapObject_DrawModel(int *model) {
    ModelTexture_SendForModel(model);
}

/* Draw hook of an alpha-blended map object model: sets the GS alpha and test registers and the Z buffer (not
   written while the model is translucent, alpha below 128), then draws the model. */
void MapObject_DrawModelWithAlpha(ModelInstance *model) {
    Packet_BeginGsRegisters();
    if (model->alpha < 128.0f) {
        Packet_AddGsRegister(GS_REG_ZBUF_1, 0x10A000118L);
        Packet_AddGsRegister(GS_REG_ZBUF_2, 0x10A000118L);
    } else {
        Packet_AddGsRegister(GS_REG_ZBUF_1, 0xA000118L);
        Packet_AddGsRegister(GS_REG_ZBUF_2, 0xA000118L);
    }
    Packet_AddGsRegister(GS_REG_ALPHA_1, 0x44);
    Packet_AddGsRegister(GS_REG_ALPHA_2, 9);
    Packet_AddGsRegister(GS_REG_TEST_1, 0x5000D);
    Packet_AddGsRegister(GS_REG_TEST_2, 0x5000D);
    Packet_EndGsRegisters();
    ModelTexture_SendForModel(model);
}

/* Draw hook of the distance-fading kind (5): sets the GS registers for alpha blending without Z-buffer
   writes, then draws the model. */
void MapObject_DrawModelNoDepthWrite(void *model) {
    Packet_BeginGsRegisters();
    Packet_AddGsRegister(0x4E, 0x10A000118);
    Packet_AddGsRegister(0x4F, 0x10A000118);
    Packet_AddGsRegister(0x42, 0x44);
    Packet_AddGsRegister(0x43, 9);
    Packet_AddGsRegister(0x47, 0x5000D);
    Packet_AddGsRegister(0x48, 0x5000D);
    Packet_EndGsRegisters();
    ModelTexture_SendForModel(model);
}

/* Does nothing: the point-update hook (ModelInstance.update_bone_points) of a map object model without points. */
void MapObject_UpdateNoBonePoints(void) {
}

/* get_point hook of a map object model without points: returns the model's position, whatever the index. */
char *MapObject_GetPositionAsPoint(char *model) {
    return (char *)&((ModelInstance *)model)->position;
}

/* Stops the sound a map object is playing, if any. */
void MapObject_StopSound(MapObject *object) {
    if (object->sound != 0) {
        Sound_StopTagged(object->sound);
        object->sound = 0;
    }
}
