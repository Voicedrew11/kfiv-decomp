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

void *memset(void *, int, u32);
float sinf(float);
float sqrtf(float);
float atan2f(float, float);
void Vector_Scale(float *, float *, float);
void Vector_Subtract(void *, void *, void *);
int func_001034A8(int);
int func_00103698(int, int);
int func_001036C8(int, int);
int func_00103668(int, int, u32);
int func_00103AE0(int, int);
void func_001036B8(int, int, int);
void func_001036E8(int, int, int);
void func_00103648(int, int, u32, int);
void func_00108A08(ModelInstance *, int);
void func_00108AC8(void *);
void func_00109A50(void);
void func_00109A80(int, s64);
void func_00109AD0(void);
void func_00109EB8(void);
void func_0010AEB8(void *);
void func_0010B018(ModelInstance *, s32, s32);
void func_0010E010(ModelInstance *);
MapRecordInfo *func_00115CD0(CameraState *, f32, int);
int func_00116C08(int, int, int, int);
void func_0011E8E8(int *);
void func_0011E8F0(ModelInstance *);
void func_0011E9D0(void *);
void func_0011EA70(void);
char *func_0011EA78(char *);
void func_0011EA80(MapObject *);
float Angle_Wrap(float);
int Sound_PlayCommonAt(float, int, void *);
int Sound_PlayMapAtRange(u8, u8, float *, float);
s32 Sound_StartTaggedAt(s8, u8, u8, f32, Vector4Arg *);
void ModelMatrix_BuildPartLocal(ModelInstance *, ModelPart *, ModelPart *);
void ModelMatrix_Build(ModelInstance *, Vector4 *, Vector4 *, f32 *);
int Script_Start(int, int);
void Vector_Copy(void *, void *);
void Vector_Add(void *, void *, void *);

extern MapDrawState D_0038A1B0;
extern int D_0038CC80[3];
extern MapSlot D_0038CD20[];
extern Vector4 D_0038D9F0[4];
extern u8 item_counts[];
extern MapObject D_003A1A30[][0x200];
extern void (*D_003BDA30[][16])(MapObject *);
extern void (*D_003BE230[])(MapObject *);
extern int (*D_003BE2B0[])(MapObject *, int);
extern void (*D_003BE330[])(MapObject *, int);
extern void (*D_003BE3B0[])(MapObject *);
extern void (*D_003BE430[])(MapObject *, f32 *);
extern void (*D_003BE4B0[])(MapObject *);
extern PlayerState D_00413F80;
extern int D_00446770;

static s32 sway_frame = 0;                      /* 0x4467A0 counts 0 to 59 */
static s32 next_sound_channel = 0;              /* 0x4467A4 0 to 31, the next of the channels 0x20 to 0x3F */
s32 D_004467A8[2] = { 0, 0 };                   /* the objects of each map have been started */
int D_004467B0[2] = { 0, 0 };                   /* the MapObjectUnk08 array of each map */
s32 D_004467B8[2] = { 0, 0 };                   /* how many objects each map has */

static s32 glow_item_worn;                      /* 0x447078 Myu's Necklace is worn */
static s32 glow_item_was_worn;                  /* 0x44707C the same, last frame */
static s32 glow_frame;                          /* 0x447080 0 to 209 */
static MapObject *plain_draw_list[2];           /* 0x447088 per map, through MapObject.unk1C */
static MapObject *jointed_draw_list[2];         /* 0x447090 per map, through MapObject.unk1C */

static MapObject *objects_by_id[2][0x100];      /* 0x472240 per map, by MapObjectUnk08.unk38.bytes[2] */
static MapSlotUnk10Vectors glow_lights_shared;  /* 0x472A40 D_0038A1B0.unkC0 blended for the glow */
static MapSlotUnk10Vectors glow_lights[2][0x20]; /* 0x472AC0 D_0038A1B0.unkA90 blended for the glow */

/* Sets up the objects of a map once it is loaded: puts each model where its
   source says, works out how far the object reaches, picks the functions that
   draw it, runs the every-frame handler of its kind once, and threads the
   objects into the two draw lists of the map, grouped by the model they use. */
void MapObject_InitMap(int map) {
    int i = 0;
    int count = D_004467B8[map];
    MapObject *object = D_003A1A30[map];
    f32 scale;
    int link_id;
    int j;

    for (; i < count; i++, object++) {
        MAP_OBJECT_BYTES(object)->step = 0;
        if (object->unk04->unk04 == 0xFFFF) {
            object->model->get_point = (int (*)(void *, int))func_0011EA78;
            object->model->unk1B4 = (void (*)(ModelInstance *))func_0011EA70;
        }
        if (object->unk08->unk38.bytes[2] != 0) {
            link_id = object->unk08->unk38.bytes[2];
            if (objects_by_id[map][link_id] == NULL) {
                objects_by_id[map][link_id] = object;
            }
        }
        Vector_Copy(&object->model->position, &object->unk08->position);
        Vector_Copy(&object->model->rotation, &object->unk08->rotation);
        Vector_Copy(&object->model->unkE0, &object->unk08->scale);
        object->unk20 = object->model->position.x;
        object->unk24 = object->model->position.y;
        object->unk28 = object->model->position.z;
        object->unk30 = object->model->rotation.x;
        object->unk34 = object->model->rotation.y;
        object->unk38 = object->model->rotation.z;
        scale = ABS(object->unk08->scale.x) > ABS(object->unk08->scale.z) ? ABS(object->unk08->scale.x) : ABS(object->unk08->scale.z);
        if (object->unk04->unk0C > 0.0f) {
            object->unk14 = object->unk04->unk0C * scale + 100.0f;
        } else if (object->model->strips != NULL) {
            object->unk14 = ((CollisionMesh *)object->model->strips)->unk10 * scale + 100.0f;
        } else {
            object->unk14 = 0.0f;
        }
        object->unk3C = object->unk04->unk6C * scale + 100.0f;
        object->model->radius = object->unk14;
        if (object->model->unk100 & 1) {
            MAP_OBJECT_BYTES(object)->anim = 0;
        } else {
            MAP_OBJECT_BYTES(object)->anim = -1;
            ModelMatrix_Build(object->model, &object->model->position, &object->model->rotation, &object->model->unkE0);
            for (j = 0; j < object->model->part_count; j++) {
                ModelMatrix_BuildPartLocal(object->model, &object->model->parts[j], &object->model->parts[j]);
            }
        }
        D_003BE3B0[object->unk04->kind](object);
        switch (object->unk04->bit8) {
            case 0:
                object->model->draw = (void (*)(ModelInstance *))func_0011E8E8;
                object->model->unk1A8 = func_0010E010;
                break;
            case 1:
                object->model->draw = func_0011E8F0;
                break;
            default:
                object->model->draw = NULL;
                break;
        }
        if (object->unk04->kind == 5) {
            object->model->draw = (void (*)(ModelInstance *))func_0011E9D0;
        }
        if (object->unk08->unk30 == 0x91) {
            object->model->unk1BC = (void (*)(ModelInstance *))ScrollTexture_DrawAll;
            ScrollTexture_Init();
        }
        MAP_OBJECT_BITS(object)->bit1 = FALSE;
        MAP_OBJECT_BITS(object)->bit2 = FALSE;
    }
    plain_draw_list[map] = NULL;
    for (i = ((MapSlotUnk08 *)D_0038CD20[map].header)->unk30 - 1; i >= 0; i--) {
        for (object = &D_003A1A30[map][count - 1], j = count - 1; j >= 0; j--, object--) {
            if (MAP_OBJECT_DEF_WORD(object) & 0x400) {
                continue;
            }
            if (MAP_OBJECT_DEF_WORD(object) & 0x100) {
                continue;
            }
            if (object->unk08->unk32 != i) {
                continue;
            }
            object->unk1C = plain_draw_list[map];
            plain_draw_list[map] = object;
        }
    }
    jointed_draw_list[map] = NULL;
    for (i = ((MapSlotUnk08 *)D_0038CD20[map].header)->unk32 - 1; i >= 0; i--) {
        for (object = &D_003A1A30[map][count - 1], j = count - 1; j >= 0; j--, object--) {
            if (MAP_OBJECT_DEF_WORD(object) & 0x400) {
                continue;
            }
            if (!(MAP_OBJECT_DEF_WORD(object) & 0x100)) {
                continue;
            }
            if (object->unk08->unk32 != i) {
                continue;
            }
            object->unk1C = jointed_draw_list[map];
            jointed_draw_list[map] = object;
        }
    }
}

/* Starts the objects of a map: their flags come from the flag block of the map
   when it has been visited, else from their sources; each is then put in use or
   handed to the handler of its kind in D_003BE4B0. */
void MapObject_StartMap(int map) {
    int i = 0;
    int count = D_004467B8[map];
    MapObject *object = D_003A1A30[map];
    int tag = D_0038CD20[map].tag;
    int visited = func_001034A8(tag);
    int in_use;
    int flag;
    MapObjectUnk08 *source;
    u8 kind;

    D_004467A8[map] = 1;
    for (; i < count; i++, object++) {
        source = object->unk08;
        flag = 0;
        in_use = ((s32)(source->unk38.flags >> 49) ^ 1) & 1;
        if (visited) {
            in_use = func_00103698(tag, i);
            flag = func_001036C8(tag, i);
            MAP_OBJECT_BITS(object)->bit18 = func_00103668(tag, i, 0);
            MAP_OBJECT_BITS(object)->bit19 = func_00103668(tag, i, 1);
        } else {
            kind = object->unk04->kind;
            if (kind == 8 || kind == 4 || kind == 0xD || kind == 0x10) {
                flag = ((MapObjectUnk08Words *)source)->unk52;
            }
        }
        if (func_00103AE0(tag, object->unk08->unk38.bytes[2])) {
            flag = 1;
        }
        object->unk0C = (s32)func_00115CD0((CameraState *)&object->model->position, 0.0f, MAP_OBJECT_BYTES(object)->unk00 & 1);
        if (in_use) {
            MapObject_Activate(object);
        } else {
            D_003BE4B0[object->unk04->kind](object);
        }
        MAP_OBJECT_BITS(object)->bit17 = flag;
    }
}

/* Unloads the objects of a map: saves four flags of each into the flag block
   of the map, stops their sounds, then empties the table. */
void MapObject_UnloadMap(int map) {
    int count = D_004467B8[map];
    MapObject *object = D_003A1A30[map];
    int tag = D_0038CD20[map].tag;
    int i;

    for (i = 0; i < count; i++, object++) {
        func_001036B8(tag, i, MAP_OBJECT_BITS(object)->bit16);
        func_001036E8(tag, i, MAP_OBJECT_BITS(object)->bit17);
        func_00103648(tag, i, 0, MAP_OBJECT_BITS(object)->bit18);
        func_00103648(tag, i, 1, MAP_OBJECT_BITS(object)->bit19);
        func_0011EA80(object);
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
        if (D_0038CD20[map].unk00 != 0) {
            count = D_004467B8[map];
            tag = D_0038CD20[map].tag;
            object = D_003A1A30[map];
            for (i = 0; i < count; i++, object++) {
                // match: each flag is narrowed to a byte first; without these four conversions the outer loop is
                // small enough for the compiler to hoist the 0xE000 object-table stride out of it
                func_001036B8(tag, i, (u8)MAP_OBJECT_BITS(object)->bit16);
                func_001036E8(tag, i, (u8)MAP_OBJECT_BITS(object)->bit17);
                func_00103648(tag, i, 0, (u8)MAP_OBJECT_BITS(object)->bit18);
                func_00103648(tag, i, 1, (u8)MAP_OBJECT_BITS(object)->bit19);
            }
        }
    }
}

/* Moves every object of a map by offset through its kind's handler in D_003BE430. */
void MapObject_OffsetAll(int map, f32 *offset) {
    int count = D_004467B8[map];
    MapObject *object = D_003A1A30[map];
    int i;

    for (i = 0; i < count; i++, object++) {
        D_003BE430[object->unk04->kind](object, offset);
    }
}

/* the objects of the current map (map 0) or the other loaded one (map 1), which the game inlines */
static inline MapObject *map_objects(s32 map) {
    if (D_004467A8[D_00446770 == map ? 0 : 1] != 0) {
        if (D_00446770 != map) {
            return D_003A1A30[1];
        }
        return D_003A1A30[0];
    }
    return NULL;
}

static inline s32 map_object_count(s32 map) {
    if (D_004467A8[D_00446770 == map ? 0 : 1] != 0) {
        return D_004467B8[D_00446770 == map ? 0 : 1];
    }
    return 0;
}

/* Updates the map objects of both maps once a frame: remembers where each was,
   runs the step handler of its kind (D_003BDA30) and the kind's every-frame
   handler (D_003BE3B0), works out whether it is in view, and starts or stops
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

    if (D_0038CC80[0] & 8) {
        return;
    }
    period = 60; // match: the divisor is a variable
    sway_frame = (sway_frame + 1) % period;
    sinf(sway_frame * 6.2831855f / 60.0f); // match: the result is not used
    for (map = 0; map < 2; map++) {
        object = map_objects(map);
        count = map_object_count(map);
        for (i = 0; i < count; i++, object++) {
            if (!(object->unk10 & 0x10000)) {
                continue;
            }
            model = object->model;
            object->unk20 = model->position.x;
            object->unk24 = model->position.y;
            object->unk28 = model->position.z;
            object->unk30 = model->rotation.x;
            object->unk34 = model->rotation.y;
            object->unk38 = model->rotation.z;
            MAP_OBJECT_BITS(object)->bit2 = MAP_OBJECT_BITS(object)->bit1;
            MAP_OBJECT_BITS(object)->bit1 = FALSE;
            MAP_OBJECT_BITS_LOW(object)->bit3 = FALSE;
            if (MODEL_UNK100_BITS(model)->has_motion) {
                anim = &object->unk04->anims[MAP_OBJECT_BYTES(object)->anim];
                if (anim->unk06 == MODEL_CHANNEL(model)->param) {
                    Effect_StartSpawner(anim->unk02, &model->position, &model->rotation, object);
                }
            }
            kind = (object->unk10 & 0x100000) ? 0 : object->unk04->kind;
            if (MAP_OBJECT_BYTES(object)->step < 0x10) {
                if (D_003BDA30[kind][MAP_OBJECT_BYTES(object)->step] != NULL) {
                    D_003BDA30[kind][MAP_OBJECT_BYTES(object)->step](object);
                }
            }
            if (!(object->unk10 & 0x10000)) {
                continue;
            }
            D_003BE3B0[object->unk04->kind](object);
            record = (MapRecord *)object->unk0C;
            if (record != NULL) {
                if ((MapObject *)D_00413F80.unk208 == object) {
                    object->unk10 |= 2;
                } else if (object->unk08->unk5A != 0 && object->unk08->unk5A != 0xFF) {
                    if (func_00116C08(D_00446770, D_00413F80.unk250, MAP_OBJECT_BYTES(object)->unk00 & 1, object->unk08->unk5A)) {
                        object->unk10 |= 2;
                    }
                } else if (record->unk38 & 0x4000000) {
                    object->unk10 |= 2;
                }
            }
            source = object->unk08;
            if (source->unk54 == 0) {
                continue;
            }
            in_view = object->unk10 & 6;
            if (in_view == 2) {
                shown = object->model;
                if (shown->parts != NULL) {
                    shown->unk1B4(shown);
                    from = (f32 *)object->model->get_point(object->model, object->unk08->unk56);
                    next = (f32 *)object->model->get_point(object->model, object->unk08->unk56 + 1);
                    delta_x = next[0] - from[0];
                    delta_y = next[1] - from[1];
                    delta_z = next[2] - from[2];
                    angles.x = atan2f(delta_y, sqrtf(delta_x * delta_x + delta_z * delta_z));
                    angles.y = Angle_Wrap(-atan2f(delta_z, delta_x) + 1.5707964f);
                    angles.z = angles.w = 0.0f;
                    Effect_StartSpawner(object->unk08->unk54, from, &angles, object);
                } else {
                    Effect_StartSpawner(source->unk54, &shown->position, &shown->rotation, object);
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
        used = D_003BE2B0[object->unk04->kind](object, item) != 0;
    }
    return used;
}

/* Passes a signal to a map object's kind (D_003BE330) unless a script drives it. */
void MapObject_Signal(MapObject *object, int signal) {
    if (object != NULL && !(object->unk10 & 0x100000)) {
        D_003BE330[object->unk04->kind](object, signal);
    }
}

/* the flags of the map record an object stands in, and which of the two maps that record is of */
#define RECORD_FLAGS(object) (((MapDrawRecord *)(object)->unk0C)->flags)
#define RECORD_MAP(object) ((u8)(((MapRecord *)(object)->unk0C)->unk38 >> 27) & 1)

/* Draws the map objects of both maps (or of one, when only_one is set): first
   the jointed ones, then the plain ones. While the player wears item 0x9D the
   objects that allow it are lit with their light blocks blended towards
   D_0038D9F0 and back, over 210 frames (fully at frame 105). */
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
    glow_item_worn = D_00413F80.equipment[EQUIP_AMULET] == ITEM_WORN_INDEX(ITEM_MYUS_NECKLACE);
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
        if (D_0038CD20[map].unk00 != 0) {
            for (i = 0; i < 0x20; i++) {
                dest = &glow_lights[map][i];
                source = (MapSlotUnk10Vectors *)&D_0038A1B0.unkA90[map][i];
                Vector_Copy(&dest->unk00[0], &source->unk00[0]);
                Vector_Copy(&dest->unk00[1], &source->unk00[1]);
                Vector_Copy(&dest->unk00[2], &source->unk00[2]);
                Vector_Copy(&dest->unk00[3], &source->unk00[3]);
                Vector_Scale(scaled, &D_0038D9F0[0].x, blend);
                Vector_Scale(&dest->unk40[0].x, &source->unk40[0].x, rest);
                Vector_Add(&dest->unk40[0], &dest->unk40[0], scaled);
                Vector_Scale(scaled, &D_0038D9F0[1].x, blend);
                Vector_Scale(&dest->unk40[1].x, &source->unk40[1].x, rest);
                Vector_Add(&dest->unk40[1], &dest->unk40[1], scaled);
                Vector_Scale(scaled, &D_0038D9F0[2].x, blend);
                Vector_Scale(&dest->unk40[2].x, &source->unk40[2].x, rest);
                Vector_Add(&dest->unk40[2], &dest->unk40[2], scaled);
                Vector_Scale(scaled, &D_0038D9F0[3].x, blend);
                Vector_Scale(&dest->unk40[3].x, &source->unk40[3].x, rest);
                Vector_Add(&dest->unk40[3], &dest->unk40[3], scaled);
            }
        }
    }
    Vector_Copy(&glow_lights_shared.unk00[0], &((MapSlotUnk10Vectors *)&D_0038A1B0.unkC0)->unk00[0]);
    Vector_Copy(&glow_lights_shared.unk00[1], &((MapSlotUnk10Vectors *)&D_0038A1B0.unkC0)->unk00[1]);
    Vector_Copy(&glow_lights_shared.unk00[2], &((MapSlotUnk10Vectors *)&D_0038A1B0.unkC0)->unk00[2]);
    Vector_Copy(&glow_lights_shared.unk00[3], &((MapSlotUnk10Vectors *)&D_0038A1B0.unkC0)->unk00[3]);
    Vector_Scale(scaled, &D_0038D9F0[0].x, blend);
    Vector_Scale(&glow_lights_shared.unk40[0].x, &((MapSlotUnk10Vectors *)&D_0038A1B0.unkC0)->unk40[0].x, rest);
    Vector_Add(&glow_lights_shared.unk40[0], &glow_lights_shared.unk40[0], scaled);
    Vector_Scale(scaled, &D_0038D9F0[1].x, blend);
    Vector_Scale(&glow_lights_shared.unk40[1].x, &((MapSlotUnk10Vectors *)&D_0038A1B0.unkC0)->unk40[1].x, rest);
    Vector_Add(&glow_lights_shared.unk40[1], &glow_lights_shared.unk40[1], scaled);
    Vector_Scale(scaled, &D_0038D9F0[2].x, blend);
    Vector_Scale(&glow_lights_shared.unk40[2].x, &((MapSlotUnk10Vectors *)&D_0038A1B0.unkC0)->unk40[2].x, rest);
    Vector_Add(&glow_lights_shared.unk40[2], &glow_lights_shared.unk40[2], scaled);
    Vector_Scale(scaled, &D_0038D9F0[3].x, blend);
    Vector_Scale(&glow_lights_shared.unk40[3].x, &((MapSlotUnk10Vectors *)&D_0038A1B0.unkC0)->unk40[3].x, rest);
    Vector_Add(&glow_lights_shared.unk40[3], &glow_lights_shared.unk40[3], scaled);
    func_00109A50();
    func_00109A80(GS_REG_ALPHA_1, 0x44);
    func_00109A80(GS_REG_ALPHA_2, 9);
    func_00109A80(GS_REG_TEST_1, 0x5000D);
    func_00109A80(GS_REG_TEST_2, 0x5000D);
    func_00109AD0();
    for (map = 0; map < 2; map++) {
        if (only_one != 0 && which != map) {
            continue;
        }
        for (object = jointed_draw_list[map]; object != NULL; object = object->unk1C) {
            if (!(object->unk10 & 2)) {
                continue;
            }
            if (mode != 0 && !(RECORD_FLAGS(object) & 0x20)) {
                continue;
            }
            if (only_one != 0) {
                if (!(object->unk04->unk68 & 0x10000)) {
                    continue;
                }
            } else {
                if (object->unk04->unk68 & 0x10000) {
                    continue;
                }
            }
            if ((MAP_OBJECT_DEF_WORD(object) & 0x4000) && glow_item_worn != 0) {
                if ((s8)object->unk08->unk38.bytes[5] != -1) {
                    object->model->unk148 = (s32)&glow_lights[MAP_OBJECT_BYTES(object)->unk00 & 1][(s8)object->unk08->unk38.bytes[5]];
                } else if (RECORD_FLAGS(object) & 0x10) {
                    object->model->unk148 = (s32)&glow_lights_shared;
                } else {
                    object->model->unk148 = (s32)&glow_lights[RECORD_MAP(object)][((MapDrawRecord *)object->unk0C)->unk35];
                }
            } else {
                if ((s8)object->unk08->unk38.bytes[5] != -1) {
                    object->model->unk148 = (s32)&D_0038A1B0.unkA90[MAP_OBJECT_BYTES(object)->unk00 & 1][(s8)object->unk08->unk38.bytes[5]];
                } else if (RECORD_FLAGS(object) & 0x10) {
                    object->model->unk148 = (s32)&D_0038A1B0.unkC0;
                } else {
                    object->model->unk148 = (s32)&D_0038A1B0.unkA90[RECORD_MAP(object)][((MapDrawRecord *)object->unk0C)->unk35];
                }
            }
            if ((s8)object->unk08->unk38.bytes[4] != -1) {
                object->model->unk14C = &D_0038A1B0.unk290[MAP_OBJECT_BYTES(object)->unk00 & 1][(s8)object->unk08->unk38.bytes[4]];
            } else if (RECORD_FLAGS(object) & 0x10) {
                object->model->unk14C = &D_0038A1B0.unkA0;
            } else {
                object->model->unk14C = &D_0038A1B0.unk290[RECORD_MAP(object)][((MapDrawRecord *)object->unk0C)->unk34];
            }
            if (object->unk04->kind == 9) {
                LinkedPoint_FindHitsExclude(object->model, *(s16 *)object->work);
            } else if (object->unk08->unk5A != 0 && object->unk08->unk5A != 0xFF) {
                LinkedPoint_FindHitsForOwner(object->model, object->unk08->unk5A);
            } else {
                LinkedPoint_FindHitsForOwner(object->model, ((MapRecordInfo *)object->unk0C)->unk38);
            }
            if ((object->unk08->unk38.flags & 0x1000000000000L) || (object->model->alpha < 128.0f && !(object->unk04->unk68 & 0x10000))) {
                func_0010B018(object->model, 0, mode);
            } else {
                func_0010AEB8(object->model);
                object->model->unk1A8(object->model);
            }
        }
    }
    for (map = 0; map < 2; map++) {
        if (only_one != 0 && which != map) {
            continue;
        }
        for (object = plain_draw_list[map]; object != NULL; object = object->unk1C) {
            if (!(object->unk10 & 2)) {
                continue;
            }
            if (mode != 0 && !(RECORD_FLAGS(object) & 0x20)) {
                continue;
            }
            if (only_one != 0) {
                if (!(object->unk04->unk68 & 0x10000)) {
                    continue;
                }
            } else {
                if (object->unk04->unk68 & 0x10000) {
                    continue;
                }
            }
            if ((MAP_OBJECT_DEF_WORD(object) & 0x4000) && glow_item_worn != 0) {
                if ((s8)object->unk08->unk38.bytes[5] != -1) {
                    object->model->unk148 = (s32)&glow_lights[MAP_OBJECT_BYTES(object)->unk00 & 1][(s8)object->unk08->unk38.bytes[5]];
                } else if (RECORD_FLAGS(object) & 0x10) {
                    object->model->unk148 = (s32)&glow_lights_shared;
                } else {
                    object->model->unk148 = (s32)&glow_lights[RECORD_MAP(object)][((MapDrawRecord *)object->unk0C)->unk35];
                }
            } else {
                if ((s8)object->unk08->unk38.bytes[5] != -1) {
                    object->model->unk148 = (s32)&D_0038A1B0.unkA90[MAP_OBJECT_BYTES(object)->unk00 & 1][(s8)object->unk08->unk38.bytes[5]];
                } else if (RECORD_FLAGS(object) & 0x10) {
                    object->model->unk148 = (s32)&D_0038A1B0.unkC0;
                } else {
                    object->model->unk148 = (s32)&D_0038A1B0.unkA90[RECORD_MAP(object)][((MapDrawRecord *)object->unk0C)->unk35];
                }
            }
            if ((s8)object->unk08->unk38.bytes[4] == -1) {
                if (RECORD_FLAGS(object) & 0x10) {
                    object->model->unk14C = &D_0038A1B0.unkA0;
                } else {
                    object->model->unk14C = &D_0038A1B0.unk290[RECORD_MAP(object)][((MapDrawRecord *)object->unk0C)->unk34];
                }
            } else {
                object->model->unk14C = &D_0038A1B0.unk290[MAP_OBJECT_BYTES(object)->unk00 & 1][(s8)object->unk08->unk38.bytes[4]];
            }
            if (object->unk04->kind == 9) {
                LinkedPoint_FindHitsExclude(object->model, *(s16 *)object->work);
            } else if (object->unk08->unk5A != 0 && object->unk08->unk5A != 0xFF) {
                LinkedPoint_FindHitsForOwner(object->model, object->unk08->unk5A);
            } else {
                LinkedPoint_FindHitsForOwner(object->model, ((MapRecordInfo *)object->unk0C)->unk38);
            }
            if ((object->unk08->unk38.flags & 0x1000000000000L) || (object->model->unk145 != 0 && (s16)object->model->unk146 < 0x80 && !(object->unk04->unk68 & 0x10000))) {
                func_0010B018(object->model, 0, mode);
            } else {
                func_0010AEB8(object->model);
                object->model->unk1A8(object->model);
            }
        }
    }
    func_00109EB8();
}

/* Puts a map object in use: not started, fully opaque. */
void MapObject_Activate(MapObject *object) {
    ModelInstance *model;

    if (object != NULL && !(object->unk10 & 0x10000)) {
        object->unk10 |= 0x10000;
        MAP_OBJECT_BITS(object)->step = 0;
        MAP_OBJECT_BITS(object)->bit1 = FALSE;
        MAP_OBJECT_BITS(object)->bit2 = FALSE;
        MAP_OBJECT_BITS(object)->anim = 4;
        model = object->model;
        model->unk145 = 0;
        model->unk150 = model->unk154 = model->unk158 = model->alpha = 128.0f;
        object->model->unk146 = 0x80;
    }
}

/* Takes a map object out of use: ends its kind's state machine and clears its state. */
void MapObject_Deactivate(MapObject *object) {
    if (object != NULL) {
        if (MAP_OBJECT_BITS(object)->step != 0) {
            D_003BE230[object->unk04->kind](object);
        }
        func_0011EA80(object);
        Effect_FreeForOwner((Entity *)object); // match: shared prototype takes an Entity
        object->unk10 &= ~0x10000;
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
    object->unk10 &= ~0x100000;
    MAP_OBJECT_BYTES(object)->step = 0xF;
    if (MAP_OBJECT_DEF_WORD(object) & 0x1000) {
        MapObject_StartAnim(object, 1);
    }
    Effect_StartSpawner(object->unk04->unk50, &object->model->position, &object->model->rotation, &D_0038CD20[MAP_OBJECT_BYTES(object)->unk00 & 1]);
    if (object->unk08->unk42 != 0 && (MAP_OBJECT_BYTES(object)->unk00 & 1) == D_00446770) {
        Script_Start(object->unk08->unk42, 0);
    }
    if (object->unk04->unk68 & 0x20000) {
        other = MapObject_GetById(MAP_OBJECT_BYTES(object)->unk00 & 1, object->unk08->unk38.bytes[3]);
        if (other != NULL) {
            MapObject_Activate(other);
        }
    }
}

/* Starts one of a map object's animations from its first frame. */
// match: declared s16 but returns nothing; with void its callers allocate $2 differently
s16 MapObject_StartAnim(MapObject *object, int anim) {
    ModelChannel *channel;

    func_00108A08(object->model, object->unk04->anims[anim].anim);
    MAP_OBJECT_BYTES(object)->anim = anim;
    channel = MODEL_CHANNEL(object->model);
    channel->param = 0;
    channel->param_shifted = 0;
    MapObject_StepAnim(object);
}

/* Advances a map object's animation one frame, starting the sounds tied to the
   frame it is on; returns whether the animation is still running. */
int MapObject_StepAnim(MapObject *object) {
    MapObjectAnim *anim = &object->unk04->anims[MAP_OBJECT_BYTES(object)->anim];
    ModelInstance *model;
    f32 range;

    if (anim->sound != 0xFFFF) {
        model = object->model;
        if (anim->sound_frame == MODEL_CHANNEL(model)->param) {
            range = 800.0f;
            if (object->unk04->unk70 > 0.0f) {
                range = object->unk04->unk70;
            }
            switch (anim->sound_kind) {
                case 0:
                    Sound_PlayCommonAt(range, anim->sound, &model->position);
                    break;
                case 1:
                    // match: the range is evaluated before the position
                    ((int (*)(u8, u8, f32, f32 *))Sound_PlayMapAtRange)(anim->sound, object->unk04->unk74, range, &model->position.x);
                    break;
                case 2:
                    Sound_StartTaggedAt((u8)next_sound_channel + 0x20, anim->sound, object->unk04->unk74, range, (Vector4Arg *)&model->position);
                    object->sound = (u8)next_sound_channel + 0x20;
                    next_sound_channel = (next_sound_channel + 1) % 32;
                    break;
            }
        }
    }
    if (MAP_OBJECT_BITS(object)->anim == 4 || MAP_OBJECT_BITS(object)->anim == 5) {
        anim = &object->unk04->anims[MAP_OBJECT_BYTES(object)->anim + 2];
        if (anim->sound != 0xFFFF && anim->sound_frame == MODEL_CHANNEL(object->model)->param) {
            range = 800.0f;
            if (object->unk04->unk70 > 0.0f) {
                range = object->unk04->unk70;
            }
            func_0011EA80(object);
            switch (anim->sound_kind) {
                case 0:
                    Sound_PlayCommonAt(range, anim->sound, &object->model->position);
                    break;
                case 1:
                    Sound_PlayMapAtRange(anim->sound, object->unk04->unk74, &object->model->position.x, range);
                    break;
                case 2:
                    break;
            }
        }
    }
    func_00108AC8(object->model);
    object->model->unk1B4(object->model);
    object->unk10 |= 8;
    model = object->model;
    if (MODEL_CHANNEL(model)->param == 0) {
        func_0011EA80(object);
        model = object->model; // match: re-read here although the return reads it again; keeps the model in $6
    }
    return MODEL_CHANNEL(object->model)->param != 0;
}

/* Marks a map object that holds something (bones and gravestones, chests, containers) of the map's own sources as emptied. */
void MapObject_MarkEmptied(MapObject *object) {
    u8 kind = object->unk04->kind;

    if ((kind == 3 || kind == MAP_OBJECT_KIND_CHEST || kind == MAP_OBJECT_KIND_CONTAINER) &&
        ((u32)(((int)object->unk08 - D_004467B0[MAP_OBJECT_BYTES(object)->unk00 & 1]) >> 7) >> 7) < 0x200) {
        object->unk10 |= 0x40000;
    }
}

/* MapObject_MarkEmptied on the map object with this id. */
void MapObject_MarkEmptiedById(int map, int object_id) {
    MapObject *object = MapObject_GetById(map, object_id);
    u8 kind;

    if (object != NULL) {
        kind = object->unk04->kind;
        if ((kind == 3 || kind == MAP_OBJECT_KIND_CHEST || kind == MAP_OBJECT_KIND_CONTAINER) &&
            ((u32)(((int)object->unk08 - D_004467B0[MAP_OBJECT_BYTES(object)->unk00 & 1]) >> 7) >> 7) < 0x200) {
            object->unk10 |= 0x40000;
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
    if ((object->model->unk100 & 1) && (object->unk10 & 0x100000)) {
        MAP_OBJECT_BITS(object)->bit21 = FALSE;
        MAP_OBJECT_BITS(object)->bit22 = bit22;
        MAP_OBJECT_BITS(object)->anim = anim;
        MAP_OBJECT_BITS(object)->step = 0;
    }
}

/* Sets bit 21 of a scripted map object (model flag 1, bit 20). */
void MapObject_SetScriptedBit21(MapObject *object, int bit21) {
    if ((object->model->unk100 & 1) && (object->unk10 & 0x100000)) {
        MAP_OBJECT_BITS(object)->bit21 = bit21;
    }
}

/* Empties one map's objects: the two counts, the two draw lists and the tables. */
void MapObject_ClearMap(int map) {
    D_004467A8[map] = 0;
    D_004467B8[map] = 0;
    plain_draw_list[map] = NULL;
    jointed_draw_list[map] = NULL;
    memset(D_003A1A30[map], 0, sizeof(D_003A1A30[map]));
    memset(objects_by_id[map], 0, sizeof(objects_by_id[map]));
}
