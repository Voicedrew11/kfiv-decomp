#include "core/game.h"
#include "map_object/map_object.h"
#include "model/model.h"
#include "ui/message.h"
#include <libvu0.h>
#include <math.h>
#include "math/vu0.h"
#include "model/model_matrix.h"
#include "sound/sound_game.h"
#include "player/player.h"
#include "save/save_state.h"

/* MapObject.step of the door-like kinds (1, 2, 6 and 0x11) */
#define DOOR_STEP_CLOSED            1       /* shut, waiting for the player */
#define DOOR_STEP_OPENING           2       /* the opening animation runs */
#define DOOR_STEP_OPEN              3       /* open */
#define DOOR_STEP_CLOSING           4       /* the closing animation runs */

/* MapObject.step of the bones and gravestones (kind 3) */
#define ITEM_HOLDER_STEP_READY      1       /* waiting for the player to use it */

/* Start handler of the door-like kinds: closed (animation 4) or already open
   (animation 5). Kind 6 remembers where its second part sits and, when it starts
   open, is run until it gets there. */
void MapObject_StartDoor(MapObject *object) {
    MapObjectWork *work = MAP_OBJECT_WORK(object);
    ModelPart *part;

    if (object->definition->kind == MAP_OBJECT_KIND_MOVING_DOOR) {
        MapObject_StartAnim(object, MAP_OBJECT_ANIM_OFF);
        MAP_OBJECT_BYTES(object)->step = DOOR_STEP_CLOSED;
        Vector_Copy(work, object->model->parts[1].position);
        work->timer = 0;
        part = object->model->parts;
        work->base_x = part[1].position[0];
        work->base_y = part[1].position[1];
        work->base_z = part[1].position[2];
        if (MAP_OBJECT_FLAGS(object) & MAP_OBJECT_ON) {
            MAP_OBJECT_BYTES(object)->step = DOOR_STEP_OPENING;
            do {
                MapObject_UpdateOpeningDoor(object);
            } while (MAP_OBJECT_BYTES(object)->step != DOOR_STEP_OPEN);
        }
    } else {
        if (!(object->state_flags & MAP_OBJECT_ON)) {
            MapObject_StartAnim(object, MAP_OBJECT_ANIM_OFF);
            MAP_OBJECT_BYTES(object)->step = DOOR_STEP_CLOSED;
        } else {
            MapObject_StartAnim(object, MAP_OBJECT_ANIM_ON);
            MAP_OBJECT_BYTES(object)->step = DOOR_STEP_OPEN;
        }
        MAP_OBJECT_UNK60_BITS(object)->bit0 = FALSE;
    }
}

/* Update handler of a closed door (kinds 1, 2, 6 and 0x11): when the player uses it, it opens if it needs no
   item or the player holds the item (the key), else its name ("Jail Door", ...) or "Locked" is shown. */
void MapObject_UpdateClosedDoor(MapObject *object) {
    MapObjectWork *work = MAP_OBJECT_WORK(object);
    s16 item;
    f32 range;
    int message;

    if (work->timer > 0) {
        work->timer--;
    }
    if (faced_object[0] == object && GAME_STATUS_FLAG_BITS(game_status_flags)->using_faced_object) {
        item = object->source->kind_param_0;
        if (item != 0 && !(object->state_flags & MAP_OBJECT_EMPTIED)) {
            if (item_counts.bytes[item] != 0) {
                MapObject_ShowKeyUsed(item);
                range = 800.0f;
                if (object->definition->sound_range > 0.0f) {
                    range = object->definition->sound_range;
                }
                Sound_PlayMapAtRange(0x2E, 0, &object->model->position.x, range);
                object->state_flags |= MAP_OBJECT_EMPTIED;
            } else {
                if (work->timer > 0) {
                    return;
                }
                message = (s16)object->definition->definition_flags;
                if (message == 0) {
                    message = MSG_LOCKED;
                }
                Message_ShowNotice(0, 0, 0, 0, 0x2D, message);
                work->timer = 0x2D;
                return;
            }
        }
        object->state_flags |= MAP_OBJECT_ON;
        MAP_OBJECT_BYTES(object)->step = DOOR_STEP_OPENING;
        MAP_OBJECT_UNK60_BITS(object)->bit0 = TRUE;
    }
}

/* Update handler of an opening door-like object: runs the animation (kind 6
   moves with its second part; kind 2 clears door_open_flags bit 0 once both pairs of its
   points are more than 225 apart); at its end the object is open. */
void MapObject_UpdateOpeningDoor(MapObject *object) {
    MapObjectWork *work = MAP_OBJECT_WORK(object);
    Vector4 delta;
    Vector4 offset;
    f32 sine;
    f32 cosine;
    f32 *point_a;
    f32 *point_b;
    f32 *point_c;
    f32 *point_d;
    f32 gap_ad;
    f32 gap_bc;
    f32 delta_x;
    f32 delta_z;
    u8 kind;

    if (work->timer > 0) {
        work->timer--;
    }
    if (MODEL_CHANNEL(object->model)->frame != 0) {
        MapObject_StepAnim(object);
        kind = object->definition->kind;
        if (kind == 6) {
            Vector_Subtract(&delta, object->model->parts[1].position, work);
            sine = sinf(-object->model->rotation.y);
            cosine = cosf(-object->model->rotation.y);
            offset.y = delta.y;
            offset.x = delta.x * cosine - delta.z * sine;
            offset.z = delta.x * sine + delta.z * cosine;
            Vector_Add(&object->model->position, &object->model->position, &offset);
            Vector_Copy(work, object->model->parts[1].position);
        } else if (kind == 2) {
            if (object->door_open_flags & 1) {
                object->model->update_bone_points(object->model);
                point_a = object->model->get_point(object->model, 2);
                point_b = object->model->get_point(object->model, 3);
                point_c = object->model->get_point(object->model, 4);
                point_d = object->model->get_point(object->model, 5);
                delta_x = point_a[0] - point_d[0];
                delta_z = point_a[2] - point_d[2];
                gap_ad = sqrtf(delta_x * delta_x + delta_z * delta_z);
                delta_x = point_b[0] - point_c[0];
                delta_z = point_b[2] - point_c[2];
                gap_bc = sqrtf(delta_x * delta_x + delta_z * delta_z);
                if (gap_ad > 225.0f && gap_bc > 225.0f) {
                    MAP_OBJECT_UNK60_BITS(object)->bit0 = FALSE;
                }
            }
        }
    } else {
        MapObject_StartAnim(object, MAP_OBJECT_ANIM_ON);
        MAP_OBJECT_BYTES(object)->step = DOOR_STEP_OPEN;
    }
}

/* Update handler of an open door that shuts by itself (unless its second word is
   set) once the player is further than its reach plus 100. */
void MapObject_UpdateOpenDoor(MapObject *object) {
    ModelInstance *model;
    MapObjectWork *work;
    float delta_x;
    float delta_z;
    float distance;

    work = MAP_OBJECT_WORK(object);
    if (work->timer > 0) {
        work->timer--;
    }
    if (MAP_OBJECT_WORDS(object)->kind_param_1 == 0) {
        model = object->model;
        delta_x = model->position.x - player_state.position.x;
        delta_z = model->position.z - player_state.position.z;
        distance = sqrtf(delta_x * delta_x + delta_z * delta_z);
        if (!(distance < object->reach_radius + 100.0f)) {
            object->state_flags &= ~MAP_OBJECT_ON;
            MAP_OBJECT_BYTES(object)->step = DOOR_STEP_CLOSING;
        }
    }
}

/* Update handler of a closing door-like object: runs the animation (kind 6 moves
   with its second part); at its end the object is closed again. */
void MapObject_UpdateClosingDoor(MapObject *object) {
    MapObjectWork *work = MAP_OBJECT_WORK(object);
    Vector4 delta;
    Vector4 offset;
    f32 sine;
    f32 cosine;

    if (work->timer > 0) {
        work->timer--;
    }
    if (MODEL_CHANNEL(object->model)->frame != 0) {
        MapObject_StepAnim(object);
        if (object->definition->kind == MAP_OBJECT_KIND_MOVING_DOOR) {
            Vector_Subtract(&delta, object->model->parts[1].position, work);
            sine = sinf(-object->model->rotation.y);
            cosine = cosf(-object->model->rotation.y);
            offset.y = delta.y;
            offset.x = delta.x * cosine - delta.z * sine;
            offset.z = delta.x * sine + delta.z * cosine;
            Vector_Add(&object->model->position, &object->model->position, &offset);
            Vector_Copy(work, object->model->parts[1].position);
        }
    } else {
        MapObject_StartAnim(object, MAP_OBJECT_ANIM_OFF);
        MAP_OBJECT_BYTES(object)->step = DOOR_STEP_CLOSED;
        Vector_Copy(&object->model->position, object->source);
    }
}

/* Signal hook of the doors: opens a closed door (step 1 to 2) or closes an open one (3 to 4). */
void MapObject_SetDoorOpen(MapObject *object, int open) {
    if ((object->state_flags & MAP_OBJECT_STEP_AND_ON_MASK) == 0x100 && open) {
        object->state_flags |= MAP_OBJECT_ON;
        MAP_OBJECT_BYTES(object)->step = DOOR_STEP_OPENING;
    } else if ((object->state_flags & MAP_OBJECT_STEP_AND_ON_MASK) == 0x20300 && !open) {
        object->state_flags &= ~MAP_OBJECT_ON;
        MAP_OBJECT_BYTES(object)->step = DOOR_STEP_CLOSING;
    }
}

/* Every-frame hook of kind 6, the door that moves with its second part: rebuilds the model matrices with that
   part put back where it was when the door started, then puts the part back. */
void MapObject_RebuildMovingDoorMatrices(MapObject *object) {
    Vector4 saved;
    int i;
    ModelPart *part;
    int part_count;

    // match: it returns a value
    ((int (*)(ModelInstance *, Vector4 *, Vector4 *, f32 *))ModelMatrix_Build)(object->model, &object->model->position, &object->model->rotation, &object->model->scale_x);
    Vector_Copy(&saved, object->model->parts[1].position);
    Vector_Copy(object->model->parts[1].position, &MAP_OBJECT_WORK(object)->base_x);
    object->model->parts[1].position[3] = 1.0f;
    part_count = object->model->part_count;
    part = object->model->parts;
    for (i = 0; i < part_count; i++) {
        ModelMatrix_BuildPartLocal(object->model, part, part);
        part++;
    }
    Vector_Copy(object->model->parts[1].position, &saved);
    if (object->model->model_flags & 1) {
        part = object->model->parts;
        for (i = 0; i < part_count; i++) {
            Model_MultiplyChildMatrices(part);
            part++;
        }
    }
    sceVu0CopyMatrix(object->model->collision_matrix, object->model->model_matrix);
    object->model->update_bone_points(object->model);
}

/* Step 0 of the bones and gravestones that hold an item (kind 3): step 1. */
void MapObject_StartItemHolder(MapObject *object) {
    MAP_OBJECT_BYTES(object)->step = ITEM_HOLDER_STEP_READY;
}
