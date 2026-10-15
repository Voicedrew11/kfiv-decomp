/* Script commands 100 to 199 (handler table D_00433440): the map's fixed items, objects and props, the
   player, the camera, the map's fog and lights, sounds, the screens the main loop runs between frames and
   the screen overlay. A command that names an object looks it up in the current map (D_00446770); one
   that eases something toward a goal moves it 1/frames of the way each frame. */
#include "kf_script.h"
#include "kf_collision.h"
#include "kf_effect.h"
#include "kf_entity.h"
#include "kf_entity_world.h"
#include "kf_game.h"
#include "kf_map.h"
#include "kf_map_object.h"
#include "kf_model.h"
#include "kf_player.h"
#include "kf_shot.h"

void *memset(void *, int, u32);
int Script_StoreFloatResult(ScriptArg *target, double value); // match: the definition takes a float, but retail converts the argument to double here (fptodp): there was no prototype in scope
int func_001C8C80(int map, int item_id);
int func_001C9B58(int item_id, int value);
ItemNode *func_001C8AF0(int item_id);
s32 func_001C8CB0(Entity *entity);
int Angle_EaseToward(float target, float *angle, int frames);
float Angle_Wrap(float angle);
MapRecordBounds *func_00116160(int map, int prop_id);
void func_00115550(MapRecordBounds *record, f32 *offset);
void func_0010E8E0(ModelInstance *model, int red, int green, int blue, int alpha);
void func_001BB360(void);
int func_001B4F30(u16, void *, void *, void *, void *);
void func_001187F0(int map_tag);
int Sound_PlayCommonAt(float range, int sound, float *position);
int Sound_PlayMapAtRange(u8 sound, u8 mode, float *position, float range);
void Sound_StartTaggedAt(s8 sound_id, u8 voice, u8 mode, f32 volume, Vector4Arg position); // match: retail passes the position as a struct; the definition takes a pointer
int Sound_StopTagged(char tag);
s32 Sound_StartTaggedAlong(s8 sound_id, u8 voice, u8 mode, Vector4Arg *origin, Vector4Arg *target, f32 volume);
int Sound_PlayMapVolume(u8 sound, int volume);
int Sound_StartTagged(s8 sound, u8 voice, int volume);
void Sound_StopVoice(s8 slot);
void func_001B57A0(u8 strength, u8 duration, u8 delay);
void Menu_AddKeyItem(u32, u32, int); // match: three arguments are passed; the definition takes the first alone

extern int D_00446770;
extern int D_00446794;
extern GameStatus D_0038CC50;
extern MapSlot D_0038CD20[];
extern PlayerState D_00413F80;
extern CameraState D_004325A0;
extern MapLoadState D_0038D610;
extern MapLoadRequest D_0038D640;
extern MapDrawStateOverlays D_0038A1B0;
extern u8 item_counts[];
extern ItemCommonDef D_0041A6F8[];

static s32 waiting_for_menu = 0;            /* 0x446FD4 ScriptCmd_OpenMenu is waiting for the menu */
static s32 waiting_for_trade = 0;           /* 0x446FD8 ScriptCmd_OpenTrade is waiting */
static s32 waiting_for_smith = 0;           /* 0x446FDC ScriptCmd_OpenSmith is waiting */
static s32 waiting_for_save = 0;            /* 0x446FE0 ScriptCmd_OpenSaveScreen is waiting */

/* Command 109, and every unused number from 100 to 199: finishes at once. */
void ScriptCmd_Nop109(ScriptThreadCommand *command) {
    command->active = 0;
}

/* Command 100: puts the current map's fixed item `value` in place (func_001C8C80), then finishes. */
void ScriptCmd_PlaceMapItem(ScriptValueCommand *command) {
    func_001C8C80(D_00446770, command->value.value);
    command->active = 0;
}

/* Command 101: starts the pickup of the fixed item `first` of the current map (func_001C9B58 makes its node
   D_004469D8 the one being picked up); `second` 0 marks it as one the player cannot put back. Then finishes. */
void ScriptCmd_StartItemPickup(ScriptPairCommand *command) {
    func_001C9B58(command->first.value, command->second.value);
    command->active = 0;
}

/* Command 102: eases the fixed item `target_id` toward the position x, y, z over `frames` frames. */
void ScriptCmd_MoveItemTo(ScriptMoveCommand *command) {
    ItemNode *node;
    f32 divisor;
    int frames;
    int frames_left;

    node = func_001C8AF0(command->target_id.value);
    if (node != NULL) {
        if (command->x.value != node->model->position.x || command->y.value != node->model->position.y ||
            command->z.value != node->model->position.z) {
            frames = command->frames.value;
            if (frames == 0) {
                frames = (command->frames.value = 1);
            }
            divisor = (f32)frames;
            frames_left = frames - 1;
            command->frames.value = frames_left;
            node->model->position.x += (command->x.value - node->model->position.x) / divisor;
            node->model->position.y += (command->y.value - node->model->position.y) / divisor;
            node->model->position.z += (command->z.value - node->model->position.z) / divisor;
            if (frames_left == 0) {
                command->active = 0;
            }
            return;
        }
    }
    command->active = 0;
}

/* Command 103: turns the fixed item `target_id` toward the rotation x, y, z over `frames` frames. */
void ScriptCmd_TurnItemTo(ScriptMoveCommand *command) {
    ItemNode *node;

    node = func_001C8AF0(command->target_id.value);
    if (node == NULL) {
        command->active = 0;
        return;
    }
    if (command->x.value == node->model->rotation.x && command->y.value == node->model->rotation.y &&
        command->z.value == node->model->rotation.z) {
        command->active = 0;
        return;
    }
    Angle_EaseToward(command->x.value, &node->model->rotation.x, command->frames.value);
    Angle_EaseToward(command->y.value, &node->model->rotation.y, command->frames.value);
    Angle_EaseToward(command->z.value, &node->model->rotation.z, command->frames.value);
    if (--command->frames.value == 0) {
        command->active = 0;
    }
}

/* Command 104: makes the entity `object_id` drop the item `value` (func_001C8CB0, with the item of its
   spawn record swapped for `value` during the call), then finishes. */
void ScriptCmd_DropItemFromEntity(ScriptSetValueCommand *command) {
    u16 saved_value;
    Entity *entity;
    EntitySpawnInfo *spawn_info;

    entity = Entity_GetBySpawnId(command->object_id.value);
    if (entity != NULL) {
        spawn_info = entity->spawn_info;
        saved_value = *(u16 *)&spawn_info->unk38; // match: the low 16 bits of the 64-bit field
        *(u16 *)&spawn_info->unk38 = command->value.value;
        func_001C8CB0(entity);
        spawn_info = entity->spawn_info;
        *(u16 *)&spawn_info->unk38 = saved_value;
    }
    command->active = 0;
}

/* Command 105: moves the fixed item `target_id` by x, y, z every frame for `frames` frames. */
void ScriptCmd_MoveItemBy(ScriptMoveCommand *command) {
    ItemNode *node;
    int frames_left;

    node = func_001C8AF0(command->target_id.value);
    if (node == NULL) {
        command->active = 0;
        return;
    }
    if (command->frames.value <= 0) {
        command->active = 0;
        return;
    }
    frames_left = command->frames.value - 1;
    command->frames.value = frames_left;
    node->model->position.x += command->x.value;
    node->model->position.y += command->y.value;
    node->model->position.z += command->z.value;
    if (frames_left == 0) {
        command->active = 0;
    }
}

/* Command 106: turns the fixed item `target_id` by x, y, z every frame for `frames` frames. */
void ScriptCmd_TurnItemBy(ScriptMoveCommand *command) {
    ItemNode *node;
    s32 frames;

    node = func_001C8AF0(command->target_id.value);
    if (node == NULL || command->frames.value <= 0) {
        command->active = 0;
    } else {
        node->model->rotation.x = Angle_Wrap(node->model->rotation.x + command->x.value);
        node->model->rotation.y = Angle_Wrap(node->model->rotation.y + command->y.value);
        node->model->rotation.z = Angle_Wrap(node->model->rotation.z + command->z.value);
        frames = command->frames.value - 1;
        command->frames.value = frames;
        if (frames == 0) {
            command->active = 0;
        }
    }
}

/* Command 107: hands the script the position of the fixed item `target_id` through x, y and z, then
   finishes. */
void ScriptCmd_GetItemPosition(ScriptMoveCommand *command) {
    ItemNode *node;

    node = func_001C8AF0(command->target_id.value);
    if (node != NULL) {
        Script_StoreFloatResult((ScriptArg *)&command->x, node->model->position.x);
        Script_StoreFloatResult((ScriptArg *)&command->y, node->model->position.y);
        Script_StoreFloatResult((ScriptArg *)&command->z, node->model->position.z);
    }
    command->active = 0;
}

/* Command 108: hands the script the rotation of the fixed item `target_id` through x, y and z, then
   finishes. */
void ScriptCmd_GetItemRotation(ScriptMoveCommand *command) {
    ItemNode *node;

    node = func_001C8AF0(command->target_id.value);
    if (node != NULL) {
        Script_StoreFloatResult((ScriptArg *)&command->x, node->model->rotation.x);
        Script_StoreFloatResult((ScriptArg *)&command->y, node->model->rotation.y);
        Script_StoreFloatResult((ScriptArg *)&command->z, node->model->rotation.z);
    }
    command->active = 0;
}

/* Command 110: activates the map object `object_id` (MapObject_Activate), then finishes. */
void ScriptCmd_ActivateMapObject(ScriptObjectCommand *command) {
    MapObject *object;

    object = MapObject_GetById(D_00446770, command->object_id.value);
    if (object != NULL) {
        MapObject_Activate(object);
    }
    command->active = 0;
}

/* Command 111: deactivates the map object `object_id` (MapObject_Deactivate), then finishes. */
void ScriptCmd_DeactivateMapObject(ScriptObjectCommand *command) {
    MapObject *object;

    object = MapObject_GetById(D_00446770, command->object_id.value);
    if (object != NULL) {
        MapObject_Deactivate(object);
    }
    command->active = 0;
}

/* Command 112: eases the map object `target_id` toward the position x, y, z over `frames` frames. */
void ScriptCmd_MoveMapObjectTo(ScriptMoveCommand *command) {
    MapObject *object = MapObject_GetById(D_00446770, command->target_id.value);
    int frames;
    f32 divisor;
    int frames_left;

    if (object == NULL) {
        command->active = 0;
        return;
    }
    if (command->x.value == object->model->position.x && command->y.value == object->model->position.y &&
        command->z.value == object->model->position.z) {
        command->active = 0;
    } else {
        frames = command->frames.value;
        if (frames == 0) {
            command->frames.value = 1;
            frames = 1;
        }
        divisor = frames;
        frames_left = frames - 1;
        command->frames.value = frames_left;
        object->model->position.x += (command->x.value - object->model->position.x) / divisor;
        object->model->position.y += (command->y.value - object->model->position.y) / divisor;
        object->model->position.z += (command->z.value - object->model->position.z) / divisor;
        if (frames_left <= 0) {
            command->active = 0;
        }
    }
}

/* Command 113: turns the map object `target_id` toward the rotation x, y, z over `frames` frames. */
void ScriptCmd_TurnMapObjectTo(ScriptMoveCommand *command) {
    MapObject *object;

    object = MapObject_GetById(D_00446770, command->target_id.value);
    if (object == NULL) {
        command->active = 0;
        return;
    }
    if (command->x.value == object->model->rotation.x && command->y.value == object->model->rotation.y &&
        command->z.value == object->model->rotation.z) {
        command->active = 0;
        return;
    }
    Angle_EaseToward(command->x.value, &object->model->rotation.x, command->frames.value);
    Angle_EaseToward(command->y.value, &object->model->rotation.y, command->frames.value);
    Angle_EaseToward(command->z.value, &object->model->rotation.z, command->frames.value);
    if (--command->frames.value < 0) {
        command->active = 0;
    }
}

/* Command 114: breaks the map object `object_id` (MapObject_Break), then finishes. */
void ScriptCmd_BreakMapObject(ScriptObjectCommand *command) {
    MapObject *object;

    object = MapObject_GetById(D_00446770, command->object_id.value);
    if (object != NULL) {
        MapObject_Break(object);
    }
    command->active = 0;
}

/* Command 115: hands the script, through `first`, whether the map object `target_id` is open (bit 17, the
   latched open/on state a door, chest or switch keeps; 0 when there is no such object). Then finishes. */
void ScriptCmd_IsMapObjectOpen(ScriptCallCommand *command) {
    MapObject *object;

    object = MapObject_GetById(D_00446770, command->target_id.value);
    if (object != NULL) {
        Script_StoreResult(&command->first, MAP_OBJECT_BITS(object)->bit17);
    } else {
        Script_StoreResult(&command->first, 0);
    }
    command->active = 0;
}

/* Command 116: hands the map object `target_id` to the script (`first` nonzero) or back to its own state
   machine (MapObject_SetScripted), then finishes. */
void ScriptCmd_SetMapObjectScripted(ScriptCallCommand *command) {
    MapObject *object;

    object = MapObject_GetById(D_00446770, command->target_id.value);
    if (object != NULL) {
        MapObject_SetScripted(object, command->first.value);
    }
    command->active = 0;
}

/* Command 117: starts the animation `first` (bit 22 from `second`) on the scripted map object `target_id`
   and sets its bit 21, so the animation runs every frame. Then finishes. */
void ScriptCmd_PlayMapObjectAnimRunning(ScriptCallCommand *command) {
    MapObject *object;

    object = MapObject_GetById(D_00446770, command->target_id.value);
    if (object != NULL) {
        MapObject_PlayScriptedAnim(object, command->first.value, command->second.value);
        MapObject_SetScriptedBit21(object, 1);
    }
    command->active = 0;
}

/* Command 118: waits until the motion of the map object `object_id` has run out; finishes at once when
   there is no such object, its model has no flag 1 or its bit 22 is set. */
void ScriptCmd_WaitMapObjectMotion(ScriptObjectCommand *command) {
    MapObject *object;

    object = MapObject_GetById(D_00446770, command->object_id.value);
    if (object == NULL || !(object->model->unk100 & 1) || (object->unk10 & 0x400000) ||
        MODEL_CHANNEL(object->model)->param == 0) {
        command->active = 0;
    }
}

/* Command 119: hands the script, through `first`, whether the motion of the map object `target_id` has
   run out (-1 when it has none, or bit 22 is set), then finishes. */
void ScriptCmd_IsMapObjectMotionDone(ScriptCallCommand *command) {
    MapObject *object = MapObject_GetById(D_00446770, command->target_id.value);

    if (object != NULL && (object->model->unk100 & 1) && !(object->unk10 & 0x400000)) {
        Script_StoreResult(&command->first, MODEL_CHANNEL(object->model)->param < 1);
    } else {
        Script_StoreResult(&command->first, -1);
    }
    command->active = 0;
}

/* Command 180: sets the colour of the model of the map object `target_id` (ModelInstance.unk150, unk154,
   unk158 and alpha), then finishes. */
void ScriptCmd_SetMapObjectColor(ScriptObjectValuesCommand *command) {
    MapObject *object;

    object = MapObject_GetById(D_00446770, command->target_id.value);
    if (object != NULL && object->model != NULL) {
        object->model->unk150 = command->values[0].value;
        object->model->unk154 = command->values[1].value;
        object->model->unk158 = command->values[2].value;
        object->model->alpha = command->values[3].value;
    }
    command->active = 0;
}

/* Command 181: hides part `first` of the model of the map object `target_id` when `second` is 0 and
   shows it otherwise, then finishes. */
void ScriptCmd_SetMapObjectPartShown(ScriptCallCommand *command) {
    MapObject *object;
    int part;

    object = MapObject_GetById(D_00446770, command->target_id.value);
    if (object != NULL && object->model != NULL) {
        part = command->first.value;
        if (part >= 0 && part < object->model->part_count) {
            MODEL_PART_FLAG_BITS(&object->model->parts[part])->bit40 = command->second.value == 0;
        }
    }
    command->active = 0;
}

/* Command 182: sends the signal `first` to the map object `target_id` (MapObject_Signal), then
   finishes. */
void ScriptCmd_SignalMapObject(ScriptCallCommand *command) {
    MapObject *object;

    object = MapObject_GetById(D_00446770, command->target_id.value);
    if (object != NULL) {
        MapObject_Signal(object, command->first.value);
    }
    command->active = 0;
}

/* Command 183: hands the script the four parameters of the map object `target_id` when it is of kind 9,
   then finishes. */
void ScriptCmd_GetMapObjectParams(ScriptObjectValuesCommand *command) {
    MapObject *object;

    object = MapObject_GetById(D_00446770, command->target_id.value);
    if (object != NULL && object->model != NULL && object->unk04->kind == 9) {
        Script_StoreResult(&command->values[0], MAP_OBJECT_WORDS(object)->unk44);
        Script_StoreResult(&command->values[1], MAP_OBJECT_WORDS(object)->unk46);
        Script_StoreResult(&command->values[2], MAP_OBJECT_WORDS(object)->unk48);
        Script_StoreResult(&command->values[3], MAP_OBJECT_WORDS(object)->unk4A);
    }
    command->active = 0;
}

/* Command 184: sets the four parameters of the map object `target_id` when it is of kind 9, then
   finishes. */
void ScriptCmd_SetMapObjectParams(ScriptObjectShortsCommand *command) {
    MapObject *object;
    MapObjectUnk08Words *source;

    object = MapObject_GetById(D_00446770, command->target_id.value);
    if (object != NULL && object->model != NULL && object->unk04->kind == 9) {
        source = MAP_OBJECT_WORDS(object);
        source->unk44 = command->values[0].value;
        source->unk46 = command->values[1].value;
        source->unk48 = command->values[2].value;
        source->unk4A = command->values[3].value;
    }
    command->active = 0;
}

/* Command 185: moves the map object `target_id` by x, y, z every frame for `frames` frames. */
void ScriptCmd_MoveMapObjectBy(ScriptMoveCommand *command) {
    MapObject *object;
    ModelInstance *model;
    int frames_left;

    object = MapObject_GetById(D_00446770, command->target_id.value);
    if (object == NULL || command->frames.value <= 0) {
        command->active = 0;
        return;
    }
    frames_left = command->frames.value - 1;
    model = object->model;
    model->position.x += command->x.value;
    model->position.y += command->y.value;
    model->position.z += command->z.value;
    command->frames.value = frames_left;
    if (frames_left <= 0) {
        command->active = 0;
    }
}

/* Command 186: turns the map object `target_id` by x, y, z every frame for `frames` frames. */
void ScriptCmd_TurnMapObjectBy(ScriptMoveCommand *command) {
    MapObject *map_object;
    s32 frames;

    map_object = MapObject_GetById(D_00446770, command->target_id.value);
    if (map_object == NULL || command->frames.value <= 0) {
        command->active = 0;
    } else {
        map_object->model->rotation.x = Angle_Wrap(map_object->model->rotation.x + command->x.value);
        map_object->model->rotation.y = Angle_Wrap(map_object->model->rotation.y + command->y.value);
        map_object->model->rotation.z = Angle_Wrap(map_object->model->rotation.z + command->z.value);
        frames = command->frames.value - 1;
        command->frames.value = frames;
        if (frames <= 0) {
            command->active = 0;
        }
    }
}

/* Command 187: hands the script the position of the map object `object_id` through from_x, from_y and
   from_z, then finishes. */
void ScriptCmd_GetMapObjectPosition(ScriptPointCommand *command) {
    MapObject *object;

    object = MapObject_GetById(D_00446770, command->object_id.value);
    if (object != NULL) {
        Script_StoreFloatResult((ScriptArg *)&command->from_x, object->model->position.x);
        Script_StoreFloatResult((ScriptArg *)&command->from_y, object->model->position.y);
        Script_StoreFloatResult((ScriptArg *)&command->from_z, object->model->position.z);
    }
    command->active = 0;
}

/* Command 188: hands the script the rotation of the map object `target_id` through x, y and z, then
   finishes. */
void ScriptCmd_GetMapObjectRotation(ScriptMoveCommand *command) {
    MapObject *object;

    object = MapObject_GetById(D_00446770, command->target_id.value);
    if (object != NULL) {
        Script_StoreFloatResult((ScriptArg *)&command->x, object->model->rotation.x);
        Script_StoreFloatResult((ScriptArg *)&command->y, object->model->rotation.y);
        Script_StoreFloatResult((ScriptArg *)&command->z, object->model->rotation.z);
    }
    command->active = 0;
}

/* Command 189: starts the animation `first` (bit 22 from `second`) on the scripted map object `target_id`
   and clears its bit 21, so the animation holds (ScriptCmd_ResumeMapObjectAnim lets it run). Then finishes. */
void ScriptCmd_PlayMapObjectAnim(ScriptCallCommand *command) {
    MapObject *object;

    object = MapObject_GetById(D_00446770, command->target_id.value);
    if (object != NULL) {
        MapObject_PlayScriptedAnim(object, command->first.value, command->second.value);
        MapObject_SetScriptedBit21(object, 0);
    }
    command->active = 0;
}

/* Command 190: lets the animation of the scripted map object `object_id` run (sets its bit 21), then
   finishes. */
void ScriptCmd_ResumeMapObjectAnim(ScriptObjectCommand *command) {
    MapObject *object;

    object = MapObject_GetById(D_00446770, command->object_id.value);
    if (object != NULL) {
        MapObject_SetScriptedBit21(object, 1);
    }
    command->active = 0;
}

/* Command 191: holds the animation of the scripted map object `object_id` where it is (clears its bit 21),
   then finishes. */
void ScriptCmd_PauseMapObjectAnim(ScriptObjectCommand *command) {
    MapObject *object;

    object = MapObject_GetById(D_00446770, command->object_id.value);
    if (object != NULL) {
        MapObject_SetScriptedBit21(object, 0);
    }
    command->active = 0;
}

/* Command 120: eases the player toward the position x, y, z over `frames` frames; finishes at once while
   GameStatus.unk00 is set. */
void ScriptCmd_MovePlayerTo(ScriptTripleCommand *command) {
    if (D_0038CC50.unk00 != 0 ||
        (command->x.value == D_00413F80.position.x && command->y.value == D_00413F80.position.y &&
         command->z.value == D_00413F80.position.z)) {
        command->active = 0;
    } else {
        int frames = command->frames.value;
        f32 divisor;
        int frames_left;

        if (frames == 0) {
            command->frames.value = 1;
            frames = 1;
        }
        divisor = (f32)frames;
        frames_left = frames - 1;
        command->frames.value = frames_left;
        D_00413F80.position.x += (command->x.value - D_00413F80.position.x) / divisor;
        D_00413F80.position.y += (command->y.value - D_00413F80.position.y) / divisor;
        D_00413F80.position.z += (command->z.value - D_00413F80.position.z) / divisor;
        if (frames_left == 0) {
            command->active = 0;
        }
    }
}

/* Command 121: turns the player toward the rotation x (`target_a`) and heading (`target_b`) over `frames`
   frames; finishes at once while GameStatus.unk00 is set. */
void ScriptCmd_TurnPlayerTo(ScriptBlendCommand *command) {
    if (D_0038CC50.unk00 != 0 ||
        (command->target_a.value == D_00413F80.rotation.x && command->target_b.value == D_00413F80.rotation.y)) {
        command->active = 0;
    } else {
        Angle_EaseToward(command->target_a.value, &D_00413F80.rotation.x, command->frames.value);
        Angle_EaseToward(command->target_b.value, &D_00413F80.rotation.y, command->frames.value);
        if (--command->frames.value == 0) {
            command->active = 0;
        }
    }
}

/* Command 122: adds `second` (which may be negative) to how many of the item `first` the player has,
   keeping it from 0 to 99 and at 1 at most for a unique item; a key item also goes to the menu's keys.
   Then finishes. */
void ScriptCmd_AddItem(ScriptPairCommand *command) {
    u32 item;
    int amount;
    int count;
    int def_offset;
    u8 *def;
    u8 *owned;

    amount = command->second.value;
    item = command->first.value;
    if (item < 381) {
        owned = &item_counts[item];
        count = *owned + amount;
        if (count >= 100) {
            *owned = 99;
        } else if (count < 0) {
            *owned = 0;
        } else {
            *owned = count;
        }
        if ((int)item < 150) {
            def_offset = item << 5; // match: the byte offset of the item's ItemCommonDef, kept for both reads
            def = (u8 *)D_0041A6F8 + def_offset;
            if (((ItemCommonDef *)def)->flags >> 1 & 1) {
                if (item_counts[item] >= 2) {
                    item_counts[item] = 1;
                }
            }
            if (((u8 *)D_0041A6F8)[def_offset] == 3) {
                Menu_AddKeyItem(item, item, def_offset);
            }
        } else {
            if (item_counts[item] >= 2) {
                item_counts[item] = 1;
            }
        }
    }
    command->active = 0;
}

/* Command 123: hands the script, through `result`, how many of the item `input` the player has (-1 for
   no such item), then finishes. */
void ScriptCmd_GetItemCount(ScriptQueryCommand *command) {
    u32 index;

    index = command->input.value;
    if (index < 380) {
        Script_StoreResult(&command->result, item_counts[index]);
    } else {
        Script_StoreResult(&command->result, -1);
    }
    command->active = 0;
}

/* Command 124: adds `value` to the player's HP, keeping it from 0 to the most it can be, then finishes. */
void ScriptCmd_AddPlayerHp(ScriptValueCommand *command) {
    PlayerState *player = &D_00413F80;
    s32 amount;

    amount = command->value.value + player->hp_now;
    if (amount < 0) {
        player->hp_now = 0;
    } else if (player->hp_max < amount) {
        player->hp_now = player->hp_max;
    } else {
        player->hp_now = amount;
    }
    command->active = 0;
}

/* Command 125: adds `value` to the player's MP, keeping it from 0 to the most it can be, then finishes. */
void ScriptCmd_AddPlayerMp(ScriptValueCommand *command) {
    PlayerState *player = &D_00413F80;
    s32 amount;

    amount = command->value.value + player->mp_now;
    if (amount < 0) {
        player->mp_now = 0;
    } else if (player->mp_max < amount) {
        player->mp_now = player->mp_max;
    } else {
        player->mp_now = amount;
    }
    command->active = 0;
}

/* Command 126: lets the player run (`value` nonzero) or not, then finishes. */
void ScriptCmd_SetPlayerCanRun(ScriptValueCommand *command) {
    D_0038CC50.unk06 = command->value.value == 0;
    command->active = 0;
}

/* Command 127: hands the script the player's step counter (PlayerState.unk258) through `value`, then
   finishes. */
void ScriptCmd_GetPlayerStepCounter(ScriptValueCommand *command) {
    Script_StoreResult(&command->value, D_00413F80.unk258);
    command->active = 0;
}

/* Command 128: hands the script the player's HP and the most it can be through `first` and `second`,
   then finishes. */
void ScriptCmd_GetPlayerHp(ScriptPairCommand *command) {
    PlayerState *player = &D_00413F80;

    Script_StoreResult(&command->first, player->hp_now);
    Script_StoreResult(&command->second, player->hp_max);
    command->active = 0;
}

/* Command 129: hands the script the player's MP and the most it can be through `first` and `second`,
   then finishes. */
void ScriptCmd_GetPlayerMp(ScriptPairCommand *command) {
    PlayerState *player = &D_00413F80;

    Script_StoreResult(&command->first, player->mp_now);
    Script_StoreResult(&command->second, player->mp_max);
    command->active = 0;
}

/* Command 130: starts the effect `object_id` at the point `from` with the rotation `to`, owned by the
   current map, then finishes. */
void ScriptCmd_StartEffect(ScriptPointCommand *command) {
    VuVector4 position = (VuVector4){ command->from_x.value, command->from_y.value, command->from_z.value, 0.0f };
    VuVector4 rotation = (VuVector4){ command->to_x.value, command->to_y.value, command->to_z.value, 0.0f };

    Effect_StartSpawner(command->object_id.value, &position, &rotation, &D_0038CD20[D_00446770]);
    command->active = 0;
}

/* Command 131: flashes the screen red (func_001BB360), then finishes. */
void ScriptCmd_FlashScreenRed(ScriptThreadCommand *command) {
    func_001BB360();
    command->active = 0;
}

/* Command 132: starts the effect `value` at the player's position and rotation, then finishes. */
void ScriptCmd_StartEffectAtPlayer(ScriptValueCommand *command) {
    PlayerState *player = &D_00413F80;

    Effect_StartSpawner(command->value.value, &player->position, &player->rotation, player);
    command->active = 0;
}

/* Command 133: launches the shot `object_id` from the point `from` at the angles `to`, owned by the
   current map, then finishes. */
void ScriptCmd_LaunchShot(ScriptPointCommand *command) {
    VuVector4 from = (VuVector4){ command->from_x.value, command->from_y.value, command->from_z.value, 1.0f };
    VuVector4 angles = (VuVector4){ command->to_x.value, command->to_y.value, command->to_z.value, 1.0f };
    PlayerShot *shot;

    shot = Shot_Alloc();
    shot->unk00 = 1;
    shot->unk20 = 0;
    shot->unk22 = 0;
    func_001B4F30(command->object_id.value, shot, &from, &angles, &D_0038CD20[D_00446770]);
    command->active = 0;
}

/* Command 140: sets the camera mode (GameStatus.unk03) to `value`, 0 (it follows the player) or 1 (the
   script places it), then finishes. */
void ScriptCmd_SetCameraMode(ScriptValueCommand *command) {
    if ((u32)command->value.value < 2) {
        D_0038CC50.unk03 = command->value.value;
    }
    command->active = 0;
}

/* Command 141: while the script places the camera, sets how it looks at a point (GameStatus.unk04,
   `object_id` 0 or 1) and the point (`from`), then finishes. */
void ScriptCmd_SetCameraLookAt(ScriptPointCommand *command) {
    if (D_0038CC50.unk03) {
        if (command->object_id.value >= 0) {
            if (command->object_id.value < 2) {
                D_0038CC50.unk04 = command->object_id.value;
                D_004325A0.unk20[0] = command->from_x.value;
                D_004325A0.unk20[1] = command->from_y.value;
                D_004325A0.unk20[2] = command->from_z.value;
            }
        }
    }
    command->active = 0;
}

/* Command 142: while the script places the camera, eases it toward the position x, y, z over `frames`
   frames. */
void ScriptCmd_MoveCameraTo(ScriptTripleCommand *command) {
    if (D_0038CC50.unk03 != 0 &&
        (command->x.value != D_004325A0.position[0] || command->y.value != D_004325A0.position[1] ||
         command->z.value != D_004325A0.position[2])) {
        int frames = command->frames.value;
        f32 divisor;

        if (frames == 0) {
            command->frames.value = 1;
            frames = 1;
        }
        divisor = (f32)frames;
        frames--;
        command->frames.value = frames;
        D_004325A0.position[0] += (command->x.value - D_004325A0.position[0]) / divisor;
        D_004325A0.position[1] += (command->y.value - D_004325A0.position[1]) / divisor;
        D_004325A0.position[2] += (command->z.value - D_004325A0.position[2]) / divisor;
        if (frames != 0) {
            return;
        }
    }
    command->active = 0;
}

/* Command 143: while the script places the camera, turns it toward the rotation x, y, z over `frames`
   frames. */
void ScriptCmd_TurnCameraTo(ScriptTripleCommand *command) {
    if (D_0038CC50.unk03 == 0 ||
        (command->x.value == D_004325A0.unk10 && command->y.value == D_004325A0.unk14 &&
         command->z.value == D_004325A0.unk18)) {
        command->active = 0;
    } else {
        Angle_EaseToward(command->x.value, &D_004325A0.unk10, command->frames.value);
        Angle_EaseToward(command->y.value, &D_004325A0.unk14, command->frames.value);
        Angle_EaseToward(command->z.value, &D_004325A0.unk18, command->frames.value);
        if (--command->frames.value == 0) {
            command->active = 0;
        }
    }
}

/* Command 144: shakes the camera: how far the shake reaches and for how many frames. Then finishes. */
void ScriptCmd_ShakeCamera(ScriptCameraShakeCommand *command) {
    CameraState *camera = &D_004325A0;

    camera->unk74 = command->frames.value;
    camera->unk70 = command->reach.value;
    command->active = 0;
}

/* Command 147: colours every vertex of the prop `target_id` (func_0010E8E0), then finishes. */
void ScriptCmd_SetPropColor(ScriptObjectValuesCommand *command) {
    MapRecordBounds *prop;

    prop = func_00116160(D_00446770, command->target_id.value);
    if (prop != NULL) {
        func_0010E8E0(prop->model, command->values[0].value, command->values[1].value, command->values[2].value,
                      command->values[3].value);
    }
    command->active = 0;
}

/* Command 148: asks for the map `value` to be loaded in the background: waits while a load is running or
   a request is pending, then fills the request (no offset between the maps) and starts the load after two
   frames. */
void ScriptCmd_LoadMap(ScriptValueCommand *command) {
    if ((u32)command->value.value < 0x80) {
        if (D_0038D610.step == 0 && D_0038D640.map == 0xFFFF) {
            D_0038D640.map = command->value.value;
            D_0038D640.x = 0.0f;
            D_0038D640.y = 0.0f;
            D_0038D640.z = 0.0f;
            D_0038D610.step = 0;
            D_0038D610.request = &D_0038D640;
            D_0038D610.next_request = NULL;
            D_0038D610.timer = 2;
            command->active = 0;
        }
        return;
    }
    command->active = 0;
}

/* Command 149: commits the scripted map load ScriptCmd_LoadMap asked for: with a background load running
   and a request pending it sets D_00446794, which makes the loader take D_0038D640 and finish at once. */
void ScriptCmd_CommitMapLoad(ScriptThreadCommand *command) {
    if (D_0038D610.step != 0 && D_0038D640.map != 0xFFFF) {
        D_00446794 = 1;
    }
    command->active = 0;
}

/* Command 150: makes the prop `target_id` solid (`first` nonzero) or lets everything pass through it
   (bit 30 of MapRecordBounds.unk3C), then finishes. */
void ScriptCmd_SetPropSolid(ScriptCallCommand *command) {
    MapRecordBounds *prop;

    prop = func_00116160(D_00446770, command->target_id.value);
    if (prop != NULL) {
        if (command->first.value != 0) {
            prop->unk3C &= ~0x40000000;
        } else {
            prop->unk3C |= 0x40000000;
        }
    }
    command->active = 0;
}

/* Command 151: eases the prop `target_id` toward the position x, y, z over `frames` frames. */
void ScriptCmd_MovePropTo(ScriptMoveCommand *command) {
    f32 offset[4];
    MapRecordBounds *prop;
    ModelInstance *model;
    int frames;
    f32 divisor;

    prop = func_00116160(D_00446770, command->target_id.value);
    if (prop == NULL) {
        command->active = 0;
        return;
    }
    model = prop->model;
    if (command->x.value == prop->model->position.x && command->y.value == prop->model->position.y &&
        command->z.value == model->position.z) {
        command->active = 0;
        return;
    }
    frames = command->frames.value;
    if (frames == 0) {
        command->frames.value = 1;
        frames = 1;
    }
    divisor = (f32)frames;
    offset[0] = (command->x.value - prop->model->position.x) / divisor;
    offset[1] = (command->y.value - prop->model->position.y) / divisor;
    offset[2] = (command->z.value - prop->model->position.z) / divisor;
    offset[3] = 0.0f;
    func_00115550(prop, offset);
    if (--command->frames.value == 0) {
        command->active = 0;
    }
}

/* Command 152: turns the prop `target_id` toward the rotation x, y, z over `frames` frames. */
void ScriptCmd_TurnPropTo(ScriptMoveCommand *command) {
    MapRecordBounds *prop;

    prop = func_00116160(D_00446770, command->target_id.value);
    if (prop == NULL) {
        command->active = 0;
        return;
    }
    if (command->x.value == prop->model->rotation.x && command->y.value == prop->model->rotation.y &&
        command->z.value == prop->model->rotation.z) {
        command->active = 0;
        return;
    }
    if (command->frames.value == 0) {
        command->frames.value = 1;
    }
    Angle_EaseToward(command->x.value, &prop->model->rotation.x, command->frames.value);
    Angle_EaseToward(command->y.value, &prop->model->rotation.y, command->frames.value);
    Angle_EaseToward(command->z.value, &prop->model->rotation.z, command->frames.value);
    if (--command->frames.value == 0) {
        command->active = 0;
    }
}

/* Command 153: moves the prop `target_id` by x, y, z every frame for `frames` frames. */
void ScriptCmd_MovePropBy(ScriptMoveCommand *command) {
    MapRecordBounds *prop = func_00116160(D_00446770, command->target_id.value);
    VuVector4 offset = (VuVector4){ command->x.value, command->y.value, command->z.value, 0.0f };

    if (prop != NULL && command->frames.value > 0) {
        func_00115550(prop, &offset.x);
        if (--command->frames.value < 0) {
            command->active = 0;
        }
    } else {
        command->active = 0;
    }
}

/* Command 154: turns the prop `target_id` by x, y, z every frame for `frames` frames. */
void ScriptCmd_TurnPropBy(ScriptMoveCommand *command) {
    MapRecordBounds *prop;
    int steps_left;

    prop = func_00116160(D_00446770, command->target_id.value);
    if (prop == NULL || command->frames.value <= 0) {
        command->active = 0;
        return;
    }
    prop->model->rotation.x = Angle_Wrap(prop->model->rotation.x + command->x.value);
    prop->model->rotation.y = Angle_Wrap(prop->model->rotation.y + command->y.value);
    prop->model->rotation.z = Angle_Wrap(prop->model->rotation.z + command->z.value);
    steps_left = command->frames.value - 1;
    command->frames.value = steps_left;
    if (steps_left < 0) {
        command->active = 0;
    }
}

/* Command 155: hands the script the position of the prop `object_id` through from_x, from_y and from_z,
   then finishes. */
void ScriptCmd_GetPropPosition(ScriptPointCommand *command) {
    MapRecordBounds *prop;

    prop = func_00116160(D_00446770, command->object_id.value);
    if (prop != NULL) {
        Script_StoreFloatResult((ScriptArg *)&command->from_x, prop->model->position.x);
        Script_StoreFloatResult((ScriptArg *)&command->from_y, prop->model->position.y);
        Script_StoreFloatResult((ScriptArg *)&command->from_z, prop->model->position.z);
    }
    command->active = 0;
}

/* Command 156: hands the script the rotation of the prop `target_id` through x, y and z, then
   finishes. */
void ScriptCmd_GetPropRotation(ScriptMoveCommand *command) {
    MapRecordBounds *prop;

    prop = func_00116160(D_00446770, command->target_id.value);
    if (prop != NULL) {
        Script_StoreFloatResult((ScriptArg *)&command->x, prop->model->rotation.x);
        Script_StoreFloatResult((ScriptArg *)&command->y, prop->model->rotation.y);
        Script_StoreFloatResult((ScriptArg *)&command->z, prop->model->rotation.z);
    }
    command->active = 0;
}

/* Command 157: sets the fog of block `block` (below 32) of the current map: `near_value` at the distance
   `near` and `far_value` at `far`, as the two coefficients the drawing uses, and its colour (FOGCOL).
   Then finishes. */
void ScriptCmd_SetMapFog(ScriptFogCommand *command) {
    u32 block_index;
    MapSlot *slot;
    MapSlotUnk0CValues *block;
    f32 near_value;
    f32 far_value;
    f32 near;
    f32 far;
    f32 span;
    int red;
    int green;
    int blue;

    slot = &D_0038CD20[D_00446770];
    block_index = command->block.value;
    near_value = command->near_value.value;
    far_value = command->far_value.value;
    near = command->near.value;
    far = command->far.value;
    red = command->red.value;
    green = command->green.value;
    blue = command->blue.value;
    if (block_index < 0x20) {
        span = far - near;
        // match: the block's offset is added to the address as a number
        block = (MapSlotUnk0CValues *)((block_index << 5) + (u32)slot->unk0C);
        block->unk10 = (s64)((blue << 16) + (green << 8) + red);
        block->unk00 = block->unk04 = 0.0f;
        block->unk0C = far * near * (far_value - near_value) / span;
        block->unk08 = ((near_value - far_value) * (far + near) / span + (near_value + far_value)) * 0.5f;
    }
    command->active = 0;
}

/* Command 158: sets the direction and colour (0 to 255) of one of the three lights of light block `block`
   of the current map, then finishes. */
void ScriptCmd_SetMapLight(ScriptLightCommand *command) {
    MapSlot *slot = &D_0038CD20[D_00446770];
    VuVector4 direction = (VuVector4){ command->dir_x.value, command->dir_y.value, command->dir_z.value };
    MapSlotUnk10Lights *lights;

    if ((u32)command->block.value < 0x20 && command->light.value >= 0 && command->light.value < 3) {
        // match: the block's offset is added to the address as a number; indexing the blocks swaps two registers
        lights = (MapSlotUnk10Lights *)(command->block.value * sizeof(MapSlotUnk10Lights) + (u32)slot->unk10);
        lights->directions[0][command->light.value] = command->dir_x.value;
        lights->directions[1][command->light.value] = command->dir_y.value;
        lights->directions[2][command->light.value] = command->dir_z.value;
        lights->colors[command->light.value].x = (f32)command->red.value / 255.0f;
        lights->colors[command->light.value].y = (f32)command->green.value / 255.0f;
        lights->colors[command->light.value].z = (f32)command->blue.value / 255.0f;
    }
    command->active = 0;
}

/* Command 159: sets the fourth colour (0 to 255) of light block `block` of the current map, the one after
   the three lights' colours (the ambient light, it seems), then finishes. */
void ScriptCmd_SetMapAmbientLight(ScriptColorCommand *command) {
    u32 block_index;
    MapSlotUnk10Lights *lights;
    MapSlot *slot;

    slot = &D_0038CD20[D_00446770];
    block_index = command->block.value;
    if (block_index < 0x20) {
        // match: the block's offset is added to the address as a number
        lights = (MapSlotUnk10Lights *)((block_index << 7) + (u32)slot->unk10);
        lights->colors[3].x = (f32)command->red.value / 255.0f;
        lights->colors[3].y = (f32)command->green.value / 255.0f;
        lights->colors[3].z = (f32)command->blue.value / 255.0f;
    }
    command->active = 0;
}

/* Command 160: plays the sound `sound` at the point x, y, z (Sound_PlayCommonAt, which is handed z as the
   range too), then finishes. */
void ScriptCmd_PlaySoundAt(ScriptSoundAtCommand *command) {
    ScriptPosition copy;
    ScriptPosition position;
    f32 z;

    memset(&position, 0, sizeof(position));
    z = command->z.value;
    position.floats[0] = command->x.value;
    position.floats[1] = command->y.value;
    position.floats[2] = z;
    copy.halves[0] = position.halves[0];
    copy.halves[1] = position.halves[1];
    Sound_PlayCommonAt(z, command->sound.value, copy.floats);
    command->active = 0;
}

/* Command 161: plays the sound `sound` at the point x, y, z, within the distance `range`, at the volume
   level `mode` (Sound_PlayMapAtRange), then finishes. */
void ScriptCmd_PlaySoundAtPoint(ScriptPointSoundCommand *command) {
    Vector4Arg position = { command->x.value, command->y.value, command->z.value, 1.0f };

    Sound_PlayMapAtRange(command->sound.value, command->mode.value, (f32 *)&position, command->range.value);
    command->active = 0;
}

/* Command 162: plays the sound `sound` on the voice `voice` at the volume `volume` at the point x, y, z
   (Sound_StartTaggedAt), then finishes. */
void ScriptCmd_PlayVoiceSoundAt(ScriptVoiceSoundCommand *command) {
    Vector4Arg position = { command->x.value, command->y.value, command->z.value, 1.0f };

    Sound_StartTaggedAt(command->sound.value, command->voice.value, 0, command->volume.value, position);
    command->active = 0;
}

/* Command 168: plays the sound `sound` on the voice `voice` with the mode `mode` at the volume `volume` at
   the point x, y, z (Sound_StartTaggedAt), then finishes. */
void ScriptCmd_PlayVoiceSoundAtWithMode(ScriptVoiceSoundModeCommand *command) {
    Vector4Arg position = { command->x.value, command->y.value, command->z.value, 1.0f };

    Sound_StartTaggedAt(command->sound.value, command->voice.value, command->mode.value, command->volume.value, position);
    command->active = 0;
}

/* Command 163: stops the sound playing under the tag `value` (Sound_StopTagged), then finishes. */
void ScriptCmd_StopSound(ScriptValueCommand *command) {
    Sound_StopTagged(command->value.value);
    command->active = 0;
}

/* Command 164: plays the sound `sound` on the voice `voice`, moving from `from` to `to`, at the volume
   `volume` (Sound_StartTaggedAlong), then finishes. */
void ScriptCmd_PlayMovingSound(ScriptMovingSoundCommand *command) {
    VuVector4 origin = (VuVector4){ command->from_x.value, command->from_y.value, command->from_z.value, 1.0f };
    VuVector4 destination = (VuVector4){ command->to_x.value, command->to_y.value, command->to_z.value, 1.0f };

    Sound_StartTaggedAlong(command->sound.value, command->voice.value, 0, (Vector4Arg *)&origin,
                  (Vector4Arg *)&destination, command->volume.value);
    command->active = 0;
}

/* Command 169: command 164 with the mode `mode` in place of 0. */
void ScriptCmd_PlayMovingSoundWithMode(ScriptMovingSoundModeCommand *command) {
    Vector4Arg origin;

    origin = (Vector4Arg){ command->from_x.value, command->from_y.value, command->from_z.value, 1.0f };
    {
        Vector4Arg destination;

        destination = (Vector4Arg){ command->to_x.value, command->to_y.value, command->to_z.value, 1.0f };
        Sound_StartTaggedAlong(command->sound.value, command->voice.value, command->mode.value, &origin, &destination,
                      command->volume.value);
    }
    command->active = 0;
}

/* Command 166: plays the sound `target_id` at the volume `first` with a random pan (Sound_PlayMapVolume), then
   finishes. */
void ScriptCmd_PlaySound(ScriptCallCommand *command) {
    Sound_PlayMapVolume(command->target_id.value, command->first.value);
    command->active = 0;
}

/* Command 167: plays the sound `target_id` on the voice `first` at the volume `second` (Sound_StartTagged),
   then finishes. */
void ScriptCmd_PlayVoiceSound(ScriptCallCommand *command) {
    Sound_StartTagged(command->target_id.value, command->first.value, command->second.value);
    command->active = 0;
}

/* Command 170: has the main loop open the game's menu between frames (GameStatus.bit2, Menu_Run) and
   waits until it has. */
void ScriptCmd_OpenMenu(ScriptThreadCommand *command) {
    if (waiting_for_menu == 0) {
        waiting_for_menu = 1;
        D_0038CC50.bit2 = TRUE;
        return;
    }
    if (!D_0038CC50.bit2) {
        waiting_for_menu = 0;
        command->active = 0;
    }
}

/* Command 171: has the main loop open the trade screen `value` between frames (GameStatus.bit3 and
   unk2C, Trade_Run) and waits until it has. */
void ScriptCmd_OpenTrade(ScriptValueCommand *command) {
    if (waiting_for_trade == 0) {
        GameStatus *status = &D_0038CC50;
        int value;

        status->bit3 = TRUE;
        value = command->value.value;
        waiting_for_trade = 1;
        status->unk2C = value;
        return;
    }
    if (!D_0038CC50.bit3) {
        waiting_for_trade = 0;
        command->active = 0;
    }
}

/* Command 172: has the main loop open the smith's screen between frames (GameStatus.bit4, Smith_Run) and
   waits until it has. */
void ScriptCmd_OpenSmith(ScriptThreadCommand *command) {
    if (waiting_for_smith == 0) {
        GameStatus *status = &D_0038CC50;

        waiting_for_smith = 1;
        status->bit4 = TRUE;
        return;
    }
    if (!D_0038CC50.bit4) {
        waiting_for_smith = 0;
        command->active = 0;
    }
}

/* Command 173: has the main loop open the save screen between frames (GameStatus.save_requested,
   SaveScreen_Run) and waits until it has. */
void ScriptCmd_OpenSaveScreen(ScriptThreadCommand *command) {
    if (waiting_for_save == 0) {
        GameStatus *status = &D_0038CC50;

        waiting_for_save = 1;
        status->save_requested = TRUE;
        return;
    }
    if (!D_0038CC50.save_requested) {
        waiting_for_save = 0;
        command->active = 0;
    }
}

/* Command 174: starts the change to the map with the tag `value` (func_001187F0), then finishes. */
void ScriptCmd_ChangeMap(ScriptValueCommand *command) {
    func_001187F0(command->value.value);
    command->active = 0;
}

/* Command 175: leaves the main loop (GameStatus bits 9 and 10), with bit 11 set when `value` is not 0,
   then finishes. */
void ScriptCmd_ExitMainLoop(ScriptValueCommand *command) {
    GameStatusSplitBits *status = GAME_STATUS_SPLIT_BITS(&D_0038CC50);

    status->bit9 = TRUE;
    status->bit10 = TRUE;
    status->bit11 = command->value.value != 0;
    command->active = 0;
}

/* Command 176: arms the main loop to hold its frame (GameStatus bits 12 and 13): it stops drawing and
   reading input at the next START press, while `value` 0 clears both bits. Then finishes. */
void ScriptCmd_HoldFrameOnStart(ScriptValueCommand *command) {
    if (command->value.value != 0) {
        if (!D_0038CC50.bit12) {
            GAME_STATUS_FLAG_WORD(&D_0038CC50) = (GAME_STATUS_FLAG_WORD(&D_0038CC50) | 0x1000) & ~0x2000;
        }
    } else {
        D_0038CC50.bit12 = FALSE;
        D_0038CC50.bit13 = FALSE;
    }
    command->active = 0;
}

/* Command 195: stops sound slot 2, then has the main loop load the player's arm models again
   (GameStatus.bit14) and waits until it has. */
void ScriptCmd_ReloadArmModels(ScriptWaitCommand *command) {
    switch (command->started) {
    case 0:
        Sound_StopVoice(2);
        command->started = 1;
        break;
    case 1:
        if (!D_0038CC50.bit14) {
            D_0038CC50.bit14 = TRUE;
            command->started = 2;
        }
        break;
    case 2:
        if (!D_0038CC50.bit14) {
            command->active = 0;
        }
        break;
    default:
        command->active = 0;
        break;
    }
}

/* Command 196: sets whether the HUD is drawn (MapDrawStateOverlays.unk68, which Hud_Draw tests before it
   draws anything), then finishes. */
void ScriptCmd_SetHudShown(ScriptValueCommand *command) {
    D_0038A1B0.unk68 = command->value.value;
    command->active = 0;
}

/* Command 197: sets the colour and alpha the screen is covered with (func_0010A4E8), then finishes. The
   blend mode is meant to come from `blend`, but the switch has no breaks and it always ends as 0x44. */
void ScriptCmd_SetScreenColor(ScriptScreenColorCommand *command) {
    MapDrawStateOverlays *overlays = &D_0038A1B0;

    overlays->unk6C = command->red.value;
    overlays->unk70 = command->green.value;
    overlays->unk74 = command->blue.value;
    overlays->unk78 = command->alpha.value;
    switch (command->blend.value) {
    case 0:
        overlays->unk80 = 0x44;
    case 1: // match: no break in any case, so the blend mode always ends as 0x44
        D_0038A1B0.unk80 = 0x48;
    case 2:
        D_0038A1B0.unk80 = 0x42;
    default:
        D_0038A1B0.unk80 = 0x44;
    }
    command->active = 0;
}

/* Command 198: makes the controller rumble (func_001B57A0), then finishes. */
void ScriptCmd_RumblePad(ScriptShakeCommand *command) {
    func_001B57A0(command->strength.value, command->duration.value, command->delay.value);
    command->active = 0;
}

/* Command 199: hands the script, through `result`, whether the script `input` is running, then
   finishes. */
void ScriptCmd_IsScriptRunning(ScriptQueryCommand *command) {
    int value;

    value = Script_IsRunning(command->input.value);
    Script_StoreResult(&command->result, value);
    command->active = 0;
}
