#include "map/map.h"
#include "map_object/map_object.h"
#include "model/model.h"
#include "core/game.h"
#include "ui/message.h"

int Game_GetTriggerItem(int, int);
void MapObject_ShowMarkerStone(MapObject *, int);
void MapObject_RebuildMatrices(MapObject *object);
void Matrix_Multiply(void *, void *, void *);
void Vector_ApplyMatrix(void *, void *, void *);
void Vector_Copy(void *, void *);
void sceVu0InversMatrix(void *, void *);
void Vector_Subtract(void *, void *, void *);
float sqrtf(float);
void Vector_Scale(float *, float *, float);
void Vector_Add(void *, void *, void *);
int Game_ClearTriggerItem(int, int);
void Player_AddItem(int);
void MapObject_RebuildMarkerMatrices(MapObject *object);
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
int Game_AddTriggerItem(int, int, int);
extern MapSlot map_slots[];
extern GameStatus game_status;
extern void *faced_object[];
extern float player_state[];
extern EventRunner view_ray_status;
extern f32 identity_matrix[16];
extern ItemCommonDef item_common_defs[];

/* Step 0 of the Guide Stone markers (kind 0x13); the stone sits on part 2. One that starts holding a stone
   keeps it only while the map's flag block still records a stone for it (Game_GetTriggerItem), and its model shows
   that stone (MapObject_ShowMarkerStone). Empty: animation 4, step 1; holding a stone: animation 5, step 4. */
void MapObject_StartMarker(MapObject *object) {
    MapObjectWorkKind121DA8 *work = (MapObjectWorkKind121DA8 *)object->work;
    int item;

    work->part = 2;
    work->fired = 0;
    item = 0;
    if (object->state_flags & MAP_OBJECT_ON) {
        item = Game_GetTriggerItem(map_slots[MAP_OBJECT_BYTES(object)->map_slot & 1].tag, object->source->spawn_flags.bytes[2]);
        if (item <= 0) {
            object->state_flags &= ~MAP_OBJECT_ON;
        }
    }
    MapObject_ShowMarkerStone(object, item);
    if (!(object->state_flags & MAP_OBJECT_ON)) {
        MapObject_StartAnim(object, 4);
        MAP_OBJECT_BYTES(object)->step = 1;
    } else {
        MapObject_StartAnim(object, 5);
        MAP_OBJECT_BYTES(object)->step = 4;
    }
}

/* Step 1 of a marker: used by the player it only shows its name, at most every 45 frames. Once a stone has
   been placed (MapObject_PlaceStoneOnMarker) the stone's part is put 220 above the player's feet (y grows
   downward), given the steps (6.67 a frame) that would carry it back, and the marker goes to step 3. */
void MapObject_UpdateEmptyMarker(MapObject *object) {
    MapObjectWorkKind121DA8 *work = (MapObjectWorkKind121DA8 *)object->work;
    ModelPart *part = &object->model->parts[work->part];
    f32 matrix[16];
    f32 unused[32];
    Vector4 origin;
    Vector4 target;
    f32 distance;

    if (work->timer > 0) {
        work->timer--;
    }
    if (game_status.bit1 && faced_object[0] == object) {
        if (work->timer > 0) {
            return;
        }
        Message_ShowNotice(0, 0, 0, 0, 0x2D, (s16)object->definition->definition_flags);
        work->timer = 0x2D;
    } else if (work->fired != 0) {
        work->fired = 0;
        object->state_flags |= MAP_OBJECT_ON;
        MAP_OBJECT_BYTES(object)->step = 3;
        MapObject_StepAnim(object);
        MapObject_RebuildMatrices(object);
        Vector_Copy(&origin, part->position);
        origin.w = 1.0f;
        Vector_Copy(&target, player_state);
        target.w = 1.0f;
        target.y -= 220.0f;
        Matrix_Multiply(matrix, object->model->model_matrix, part);
        sceVu0InversMatrix(matrix, matrix);
        Vector_ApplyMatrix(&target, matrix, &target);
        Vector_Subtract(work, &origin, &target);
        distance = sqrtf(work->step.x * work->step.x + work->step.y * work->step.y + work->step.z * work->step.z);
        work->frames = distance / 6.6666665f;
        Vector_Copy(part->position, &target);
        Vector_Scale(&work->step.x, &work->step.x, 1.0f / work->frames);
        game_status.update_hold_flags |= 1;
    }
}

/* Step 2 of a marker (MapObject_UpdateEmptyMarker goes to step 3 instead): moves the stone's part one step;
   after the last one, step 3. */
void MapObject_MoveMarkerStoneIn(MapObject *object) {
    MapObjectWorkKind121DA8 *work = (MapObjectWorkKind121DA8 *)object->work;
    ModelPart *part = &object->model->parts[work->part];

    if (work->timer > 0) {
        work->timer--;
    }
    Vector_Add(part->position, part->position, work);
    if (--work->frames <= 0) {
        MAP_OBJECT_BYTES(object)->step = 3;
    }
}

/* Step 3 of a marker: runs the animation; at its end the marker holds the stone (animation 5, step 4) and
   game_status.unk30 bit 0 is cleared. */
void MapObject_SettleMarkerStone(MapObject *object) {
    MapObjectWorkKind121DA8 *work = (MapObjectWorkKind121DA8 *)object->work;

    if (work->timer > 0) {
        work->timer--;
    }
    if (MODEL_CHANNEL(object->model)->frame != 0) {
        MapObject_StepAnim(object);
    } else {
        MapObject_StartAnim(object, 5);
        MAP_OBJECT_BYTES(object)->step = 4;
        game_status.update_hold_flags &= ~1U;
    }
}

/* Step 4 of a marker: used by the player, it takes the stone recorded for it out of the map's flag block
   (Game_GetTriggerItem, Game_ClearTriggerItem), gives it to the player (Player_AddItem), runs the animation and goes to
   step 5. */
void MapObject_UpdateFilledMarker(MapObject *object) {
    MapObjectWorkKind121DA8 *work = (MapObjectWorkKind121DA8 *)object->work;
    u8 timer = work->timer;
    u8 tag = map_slots[MAP_OBJECT_BYTES(object)->map_slot & 1].tag;
    int item;

    if (work->timer > 0) {
        work->timer = timer - 1;
    }
    if (game_status.bit1 && view_ray_status.current == object) {
        MAP_OBJECT_BYTES(object)->step = 5;
        // match: two leftover arguments are passed
        item = ((int (*)(int, int, void *, int))Game_GetTriggerItem)(tag, object->source->spawn_flags.bytes[2], work, timer);
        Game_ClearTriggerItem(tag, object->source->spawn_flags.bytes[2]);
        MapObject_StepAnim(object);
        MapObject_RebuildMatrices(object);
        Player_AddItem(item);
        object->state_flags &= ~MAP_OBJECT_ON;
        game_status.update_hold_flags |= 1;
    }
}

/* Step 5 of a marker: once the animation has ended, works out the steps (6.67 a frame) that carry the stone's
   part from the model's point 0 to 115 above the player's feet; step 6. */
void MapObject_LiftMarkerStone(MapObject *object) {
    MapObjectWorkKind121DA8 *work = (MapObjectWorkKind121DA8 *)object->work;
    f32 matrix[16];
    Vector4 origin;
    Vector4 target;
    f32 distance;
    int frames;
    void *point;

    if (work->timer > 0) {
        work->timer--;
    }
    if (MODEL_CHANNEL(object->model)->frame != 0) {
        MapObject_StepAnim(object);
        return;
    }
    MAP_OBJECT_BYTES(object)->step = 6;
    MapObject_RebuildMarkerMatrices(object);
    object->model->update_bone_points(object->model);
    point = (void *)object->model->get_point(object->model, 0);
    Vector_Copy(&origin, point);
    Vector_Copy(&target, player_state);
    target.y -= 115.0f;
    Vector_Subtract(work, &target, &origin);
    distance = sqrtf(((MapObjectWorkKind121DA8 *)object->work)->step.x * ((MapObjectWorkKind121DA8 *)object->work)->step.x + work->step.y * work->step.y + work->step.z * work->step.z);
    sceVu0RotMatrixX(matrix, identity_matrix, object->model->rotation.x);
    sceVu0RotMatrixY(matrix, identity_matrix, -object->model->rotation.y);
    Vector_ApplyMatrix(work, matrix, work);
    frames = distance / 6.6666665f;
    work->frames = frames;
    Vector_Scale(&work->step.x, &work->step.x, 1.0f / frames);
}

/* Step 6 of a marker: moves the stone's part one step; after the last one the stone is hidden
   (MapObject_ShowMarkerStone), the marker is empty again (animation 4, step 1) and game_status.unk30 bit 0 is
   cleared. */
void MapObject_MoveMarkerStoneOut(MapObject *object) {
    MapObjectWorkKind121DA8 *work = (MapObjectWorkKind121DA8 *)object->work;
    ModelPart *part = &object->model->parts[work->part];

    if (work->timer > 0) {
        work->timer--;
    }
    Vector_Add(part->position, part->position, work);
    if (--work->frames <= 0) {
        MapObject_ShowMarkerStone(object, 0);
        MapObject_StartAnim(object, 4);
        MAP_OBJECT_BYTES(object)->step = 1;
        game_status.update_hold_flags &= ~1U;
    }
}

/* Every-frame hook of the markers: MapObject_RebuildMatrices. */
void MapObject_RebuildMarkerMatrices(MapObject *object) {
    MapObject_RebuildMatrices(object);
}

/* Item hook of the markers: an empty marker (step 1) that has taken nothing yet takes a Guide Stone (item use
   type 1), records it in the map's flag block (Game_AddTriggerItem) and shows it on its model; returns whether it
   took it. */
int MapObject_PlaceStoneOnMarker(MapTrigger *trigger, int type_index) {
    u8 type;
    MapSlot *slot;
    int tag;
    MapTriggerShot *shot = &trigger->shot;

    type = item_common_defs[type_index].type;
    slot = &map_slots[trigger->flags & 1];
    tag = slot->tag;
    if (trigger->type == 1 && shot->fired == 0 && type == trigger->type &&
        Game_AddTriggerItem(tag, (u8)trigger->source[0x3A], type_index)) {
        MapObject_ShowMarkerStone((MapObject *)trigger, type_index);
        shot->fired = type;
        return TRUE;
    }
    return FALSE;
}

/* Shows the stone on a marker's model: items 0x86 to 0x8A (the five Guide Stones) show chunks 0 to 4 (a set
   bit of ModelInstance.chunk_skip_mask hides that chunk); any other item, or none, hides all five. */
void MapObject_ShowMarkerStone(MapObject *object, int item) {
    u32 *skip_mask;
    u32 chunk;
    int shown;

    skip_mask = (u32 *)&object->model->chunk_skip_mask;
    chunk = item - 0x86;
    shown = (chunk <= 4U) ? (int)chunk : -1;

    *skip_mask &= ~1u;
    if (shown != 0) {
        *skip_mask |= 1;
    }
    *skip_mask &= ~2u;
    if (shown != 1) {
        *skip_mask |= 2;
    }
    *skip_mask &= ~4u;
    if (shown != 2) {
        *skip_mask |= 4;
    }
    *skip_mask &= ~8u;
    if (shown != 3) {
        *skip_mask |= 8;
    }
    *(u32 *)&object->model->chunk_skip_mask &= ~0x10u;
    if (shown != 4) {
        *(u32 *)&object->model->chunk_skip_mask |= 0x10;
    }
}
