#include "model/model.h"
#include "ui/message.h"
#include "map_object/map_object.h"
#include "item/gold.h"
#include "core/game.h"

ItemNode *MapItem_PutOutForObject(int, int, void *, void *, void *, void *);
int MapItem_StartPickup(ModelNode *);
void MapObject_SpawnHeldItem(MapObject *);
void MapObject_UpdateBreaking(MapObject *);
void *memset(void *, int, u32);
void MapObject_ShowKeyUsed(int);
int Sound_PlayMapAtRange(u8, u8, float *, float);
void MapObject_UpdateOpenChest(MapObject *);
ModelNode *MapItem_FindForObject(int);
void MapObject_UpdateClosedChest(void *, int);
void MapObject_SetupItemHolder(MapObject *);
extern void (*map_object_update_hooks[])(MapObject *);
extern Qword object_item_scale;
extern void *faced_object[];
extern u8 item_counts[];
extern GameStatus game_status;
extern u32 game_status_flags[];
extern EventRunner view_ray_status;

/* Step 1 of the bones and gravestones (kind 3): used by the player, one that still holds its item spawns it
   at its point 0 (MapItem_PutOutForObject) and the player picks it up at once (MapItem_StartPickup); otherwise its name is
   shown, at most every 45 frames. */
void MapObject_UpdateItemHolder(MapObject *object) {
    Qword unk_value = object_item_scale;
    s32 *work = object->work;

    if (object->work[0] > 0) {
        object->work[0]--;
    }
    if (GAME_STATUS_FLAG_BITS(game_status_flags)->using_faced_object && faced_object[0] == object) {
        if (object->source->kind_param_0 != 0 && !(object->state_flags & MAP_OBJECT_EMPTIED)) {
            object->model->update_bone_points(object->model);
            MapItem_StartPickup((ModelNode *)MapItem_PutOutForObject(
                MAP_OBJECT_BYTES(object)->map_slot & 1, object->source->kind_param_0, (void *)object->model->get_point(object->model, 0),
                &object->model->rotation, &unk_value, object));
        } else if (work[0] <= 0) {
            Message_ShowNotice(0, 0, 0, 0, 0x2D, (s16)object->definition->definition_flags);
            work[0] = 0x2D;
        }
    }
}

/* Step 0xF of the bones and gravestones: spawns the item they hold (MapObject_SpawnHeldItem), then breaks
   like the other kinds (MapObject_UpdateBreaking). */
void MapObject_UpdateBreakingItemHolder(MapObject *object) {
    MapObject_SpawnHeldItem(object);
    MapObject_UpdateBreaking(object);
}

/* Set-up hook of the bones and gravestones that start out of use (and, through MapObject_SetupChest, of the
   chests): spawns the item they hold. */
void MapObject_SetupItemHolder(MapObject *object) {
    MapObject_SpawnHeldItem(object);
}

/* Spawns the item a map object holds (MapObjectUnk08.kind_param_0) at its point 0 (MapItem_PutOutForObject), unless it was
   handed out already (unk10 bit 0x40000); the kind's every-frame hook runs first, so the point is current. */
void MapObject_SpawnHeldItem(MapObject *object) {
    s64 drop_params[2];

    drop_params[0] = object_item_scale.low_doubleword;
    drop_params[1] = object_item_scale.high_doubleword;
    if (object->source->kind_param_0 != 0) {
        if (!(object->state_flags & MAP_OBJECT_EMPTIED)) {
            map_object_update_hooks[object->definition->kind](object);
            object->model->update_bone_points(object->model);
            MapItem_PutOutForObject((u8)object->state_flags & 1, object->source->kind_param_0, (void *)object->model->get_point(object->model, 0),
                          &object->model->rotation, drop_params, object);
        }
    }
}

/* Start handler of a chest: closed (animation 4, step 1) or open (animation 5, step 3);
   an open chest that isn't emptied yet drops its item (MapObjectUnk08.kind_param_0) through MapItem_PutOutForObject. */
void MapObject_StartChest(MapObject *object) {
    f32 drop_position[3];
    s64 drop_params[2];
    ModelInstance *model;
    MapObjectUnk08 *source;

    drop_params[0] = object_item_scale.low_doubleword;
    drop_params[1] = object_item_scale.high_doubleword;
    if (!(object->state_flags & MAP_OBJECT_ON)) {
        MapObject_StartAnim(object, 4);
        MAP_OBJECT_BYTES(object)->step = 1;
    } else {
        MapObject_StartAnim(object, 5);
        source = object->source;
        MAP_OBJECT_BYTES(object)->step = 3;
        if (source->kind_param_0 != 0 && !(object->state_flags & MAP_OBJECT_EMPTIED)) {
            model = object->model;
            drop_position[0] = model->position.x;
            drop_position[1] = model->position.y - 25.0f;
            drop_position[2] = model->position.z;
            MapItem_PutOutForObject((u8)object->state_flags & 1, source->kind_param_0, drop_position, &model->rotation, drop_params, object);
        }
    }
}

/* Update handler of a closed chest (kind 4): used by the player, it opens (when it needs a key the player
   must hold it, else its name or "Locked" is shown) and hands out its item, or drops the gold of its third
   word. */
void MapObject_UpdateClosedChest(void *arg, int unused) {
    MapObject *object = arg;
    Vector4 position;
    Vector4 turn;
    Qword unk_value;
    s32 *work;
    MapObjectUnk08 *source;
    s16 key;
    f32 range;
    int message;

    memset(&turn, 0, sizeof(turn));
    work = object->work;
    unk_value = object_item_scale;
    if (work[1] > 0) {
        work[1]--;
    }
    if (GAME_STATUS_FLAG_BITS(game_status_flags)->using_faced_object && faced_object[0] == object) {
        source = object->source;
        key = ((MapObjectUnk08Words *)source)->kind_param_1;
        if (key != 0) {
            if (item_counts[key] == 0) {
                if (work[1] > 0) {
                    return;
                }
                message = (s16)object->definition->definition_flags;
                if (message == 0) {
                    message = MSG_LOCKED;
                }
                Message_ShowNotice(0, 0, 0, 0, 0x2D, message);
                work[1] = 0x2D;
                return;
            }
            MapObject_ShowKeyUsed(key);
            range = 800.0f;
            if (object->definition->sound_range > 0.0f) {
                range = object->definition->sound_range;
            }
            Sound_PlayMapAtRange(0x2D, 0, &object->model->position.x, range);
            source = object->source;
        }
        MAP_OBJECT_BYTES(object)->step = 2;
        MAP_OBJECT_BITS(object)->bit17 = TRUE;
        work[1] = 0;
        MAP_OBJECT_BITS(object)->bit18 = TRUE;
        if (source->kind_param_0 != 0) {
            position.x = object->model->position.x;
            position.y = object->model->position.y - 25.0f;
            position.z = object->model->position.z;
            MapItem_PutOutForObject(MAP_OBJECT_BYTES(object)->map_slot & 1, source->kind_param_0, &position, &object->model->rotation, &unk_value, object);
            MAP_OBJECT_BITS(object)->bit18 = FALSE;
        } else if (((MapObjectUnk08Words *)source)->kind_param_2 > 0) {
            position.x = object->model->position.x;
            position.y = object->model->position.y - 25.0f;
            position.z = object->model->position.z;
            turn.y = object->model->rotation.y;
            Gold_Drop(&position, &turn, MAP_OBJECT_BYTES(object)->map_slot & 1, ((MapObjectUnk08Words *)source)->kind_param_2, source->spawn_flags.bytes[2], 0, 1);
            MAP_OBJECT_BITS(object)->bit18 = FALSE;
        }
    }
}

/* Step 2 of a chest: runs the opening animation; at its end the chest is open (animation 5, step 3, work[0]
   set to 90). Then the open-chest update. */
void MapObject_UpdateOpeningChest(MapObject *object) {
    if (MODEL_CHANNEL(object->model)->frame != 0) {
        MapObject_StepAnim(object);
    } else {
        MapObject_StartAnim(object, 5);
        MAP_OBJECT_BYTES(object)->step = 3;
        object->work[0] = 0x5A;
    }
    MapObject_UpdateOpenChest(object);
}

/* Update handler of an open chest (kind 4, step 3): when the player uses one that still holds something it
   is handed out (MapItem_FindForObject), else "Empty" is shown, at most every 45 frames. */
void MapObject_UpdateOpenChest(MapObject *object) {
    s32 *work = object->work;
    MapObjectUnk08Words *source;
    ModelNode *pickup;

    if (work[1] > 0) {
        work[1]--;
    }
    if (GAME_STATUS_FLAG_BITS(game_status_flags)->using_faced_object && faced_object[0] == object) {
        if (!(object->state_flags & MAP_OBJECT_EMPTIED) &&
            ((source = MAP_OBJECT_WORDS(object))->kind_param_0 != 0 || source->kind_param_2 != 0)) {
            pickup = MapItem_FindForObject((int)object);
            if (pickup != NULL) {
                MapItem_StartPickup(pickup);
            }
        } else {
            if (work[1] > 0) {
                return;
            }
            Message_ShowNotice(0, 0, 0, 0, 0x2D, MSG_EMPTY);
            work[1] = 0x2D;
        }
    }
}

/* Does nothing: step 4 of a chest, which the chest code never sets. */
void MapObject_ChestStep4Empty(void) {
}

/* Signal hook of the chests: opens a closed chest (step 1) as if the player had used it, by running
   MapObject_UpdateClosedChest with the use bit (GameStatus.bit1) set and the chest as the thing in front of
   the player. */
void MapObject_OpenChestOnSignal(GameEvent *event, int mode) {
    void *saved;
    int bit;

    if ((event->flags & MAP_OBJECT_STEP_AND_ON_MASK) == 0x100 && mode != 0) {
        saved = view_ray_status.current;
        bit = game_status.using_faced_object;
        game_status.using_faced_object = 1;
        view_ray_status.current = event;
        MapObject_UpdateClosedChest(event, mode);
        view_ray_status.current = saved;
        game_status.using_faced_object = bit;
    }
}

/* Set-up hook of the chests that start out of use: spawns the item they hold (MapObject_SetupItemHolder). */
void MapObject_SetupChest(MapObject *object) {
    MapObject_SetupItemHolder(object);
}
