#include "kf_model.h"
#include "kf_message.h"
#include "kf_map_object.h"
#include "kf_gold.h"
#include "kf_game.h"

ItemNode *func_001C9340(int, int, void *, void *, void *, void *);
int func_001C9AB0(ModelNode *);
void MapObject_SpawnHeldItem(MapObject *);
void MapObject_UpdateBreaking(MapObject *);
void *memset(void *, int, u32);
void MapObject_ShowKeyUsed(int);
int Sound_PlayMapAtRange(u8, u8, float *, float);
void MapObject_UpdateOpenChest(MapObject *);
ModelNode *func_001C9580(int);
void MapObject_UpdateClosedChest(void *, int);
void MapObject_SetupItemHolder(MapObject *);
extern void (*map_object_update_hooks[])(MapObject *);
extern Qword D_0043E780;
extern u32 D_0038CC74[];
extern void *D_00420D50[];
extern u8 item_counts[];
extern GameStatus game_status;
extern EventRunner D_00420D00;

/* Step 1 of the bones and gravestones (kind 3): used by the player, one that still holds its item spawns it
   at its point 0 (func_001C9340) and the player picks it up at once (func_001C9AB0); otherwise its name is
   shown, at most every 45 frames. */
void MapObject_UpdateItemHolder(MapObject *object) {
    Qword unk_value = D_0043E780;
    s32 *work = object->work;

    if (object->work[0] > 0) {
        object->work[0]--;
    }
    if (((D_0038CC74[0] >> 1) & 1) && D_00420D50[0] == object) {
        if (object->unk08->unk44 != 0 && !(object->unk10 & 0x40000)) {
            object->model->unk1B4(object->model);
            func_001C9AB0((ModelNode *)func_001C9340(
                MAP_OBJECT_BYTES(object)->unk00 & 1, object->unk08->unk44, (void *)object->model->get_point(object->model, 0),
                &object->model->rotation, &unk_value, object));
        } else if (work[0] <= 0) {
            Message_ShowNotice(0, 0, 0, 0, 0x2D, (s16)object->unk04->unk68);
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

/* Spawns the item a map object holds (MapObjectUnk08.unk44) at its point 0 (func_001C9340), unless it was
   handed out already (unk10 bit 0x40000); the kind's every-frame hook runs first, so the point is current. */
void MapObject_SpawnHeldItem(MapObject *object) {
    s64 drop_params[2];

    drop_params[0] = D_0043E780.unk00;
    drop_params[1] = D_0043E780.unk08;
    if (object->unk08->unk44 != 0) {
        if (!(object->unk10 & 0x40000)) {
            map_object_update_hooks[object->unk04->kind](object);
            object->model->unk1B4(object->model);
            func_001C9340((u8)object->unk10 & 1, object->unk08->unk44, (void *)object->model->get_point(object->model, 0),
                          &object->model->rotation, drop_params, object);
        }
    }
}

/* Start handler of a chest: closed (animation 4, step 1) or open (animation 5, step 3);
   an open chest that isn't emptied yet drops its item (MapObjectUnk08.unk44) through func_001C9340. */
void MapObject_StartChest(MapObject *object) {
    f32 drop_position[3];
    s64 drop_params[2];
    ModelInstance *model;
    MapObjectUnk08 *source;

    drop_params[0] = D_0043E780.unk00;
    drop_params[1] = D_0043E780.unk08;
    if (!(object->unk10 & 0x20000)) {
        MapObject_StartAnim(object, 4);
        MAP_OBJECT_BYTES(object)->step = 1;
    } else {
        MapObject_StartAnim(object, 5);
        source = object->unk08;
        MAP_OBJECT_BYTES(object)->step = 3;
        if (source->unk44 != 0 && !(object->unk10 & 0x40000)) {
            model = object->model;
            drop_position[0] = model->position.x;
            drop_position[1] = model->position.y - 25.0f;
            drop_position[2] = model->position.z;
            func_001C9340((u8)object->unk10 & 1, source->unk44, drop_position, &model->rotation, drop_params, object);
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
    unk_value = D_0043E780;
    if (work[1] > 0) {
        work[1]--;
    }
    if (((D_0038CC74[0] >> 1) & 1) && D_00420D50[0] == object) {
        source = object->unk08;
        key = ((MapObjectUnk08Words *)source)->unk46;
        if (key != 0) {
            if (item_counts[key] == 0) {
                if (work[1] > 0) {
                    return;
                }
                message = (s16)object->unk04->unk68;
                if (message == 0) {
                    message = MSG_LOCKED;
                }
                Message_ShowNotice(0, 0, 0, 0, 0x2D, message);
                work[1] = 0x2D;
                return;
            }
            MapObject_ShowKeyUsed(key);
            range = 800.0f;
            if (object->unk04->unk70 > 0.0f) {
                range = object->unk04->unk70;
            }
            Sound_PlayMapAtRange(0x2D, 0, &object->model->position.x, range);
            source = object->unk08;
        }
        MAP_OBJECT_BYTES(object)->step = 2;
        MAP_OBJECT_BITS(object)->bit17 = TRUE;
        work[1] = 0;
        MAP_OBJECT_BITS(object)->bit18 = TRUE;
        if (source->unk44 != 0) {
            position.x = object->model->position.x;
            position.y = object->model->position.y - 25.0f;
            position.z = object->model->position.z;
            func_001C9340(MAP_OBJECT_BYTES(object)->unk00 & 1, source->unk44, &position, &object->model->rotation, &unk_value, object);
            MAP_OBJECT_BITS(object)->bit18 = FALSE;
        } else if (((MapObjectUnk08Words *)source)->unk48 > 0) {
            position.x = object->model->position.x;
            position.y = object->model->position.y - 25.0f;
            position.z = object->model->position.z;
            turn.y = object->model->rotation.y;
            Gold_Drop(&position, &turn, MAP_OBJECT_BYTES(object)->unk00 & 1, ((MapObjectUnk08Words *)source)->unk48, source->unk38.bytes[2], 0, 1);
            MAP_OBJECT_BITS(object)->bit18 = FALSE;
        }
    }
}

/* Step 2 of a chest: runs the opening animation; at its end the chest is open (animation 5, step 3, work[0]
   set to 90). Then the open-chest update. */
void MapObject_UpdateOpeningChest(MapObject *object) {
    if (MODEL_CHANNEL(object->model)->param != 0) {
        MapObject_StepAnim(object);
    } else {
        MapObject_StartAnim(object, 5);
        MAP_OBJECT_BYTES(object)->step = 3;
        object->work[0] = 0x5A;
    }
    MapObject_UpdateOpenChest(object);
}

/* Update handler of an open chest (kind 4, step 3): when the player uses one that still holds something it
   is handed out (func_001C9580), else "Empty" is shown, at most every 45 frames. */
void MapObject_UpdateOpenChest(MapObject *object) {
    s32 *work = object->work;
    MapObjectUnk08Words *source;
    ModelNode *pickup;

    if (work[1] > 0) {
        work[1]--;
    }
    if (((D_0038CC74[0] >> 1) & 1) && D_00420D50[0] == object) {
        if (!(object->unk10 & 0x40000) &&
            ((source = MAP_OBJECT_WORDS(object))->unk44 != 0 || source->unk48 != 0)) {
            pickup = func_001C9580((int)object);
            if (pickup != NULL) {
                func_001C9AB0(pickup);
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
void func_00120128(void) {
}

/* Signal hook of the chests: opens a closed chest (step 1) as if the player had used it, by running
   MapObject_UpdateClosedChest with the use bit (GameStatus.bit1) set and the chest as the thing in front of
   the player. */
void MapObject_OpenChestOnSignal(GameEvent *event, int mode) {
    void *saved;
    int bit;

    if ((event->flags & 0x2FF00) == 0x100 && mode != 0) {
        saved = D_00420D00.current;
        bit = game_status.bit1;
        game_status.bit1 = 1;
        D_00420D00.current = event;
        MapObject_UpdateClosedChest(event, mode);
        D_00420D00.current = saved;
        game_status.bit1 = bit;
    }
}

/* Set-up hook of the chests that start out of use: spawns the item they hold (MapObject_SetupItemHolder). */
void MapObject_SetupChest(MapObject *object) {
    MapObject_SetupItemHolder(object);
}
