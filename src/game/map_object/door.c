#include "map_object/map_object.h"
#include "model/model.h"
#include "ui/message.h"

void Vector_Copy(void *, void *);
void MapObject_UpdateOpeningDoor(MapObject *);
void MapObject_ShowKeyUsed(int);
int Sound_PlayMapAtRange(u8, u8, float *, float);
void Vector_Subtract(void *, void *, void *);
float sinf(float);
float cosf(float);
float sqrtf(float);
void Vector_Add(void *, void *, void *);
void ModelMatrix_Build(ModelInstance *, Vector4 *, Vector4 *, f32 *);
void ModelMatrix_BuildPartLocal(ModelInstance *, ModelPart *, ModelPart *);
void Model_MultiplyChildMatrices(ModelPart *);
void sceVu0CopyMatrix(void *, void *);
extern void *faced_object[];
extern u32 game_status_flags[];
extern u8 item_counts[];
extern float player_state[];

/* Start handler of the door-like kinds: closed (animation 4) or already open
   (animation 5). Kind 6 remembers where its second part sits and, when it starts
   open, is run until it gets there. */
void MapObject_StartDoor(MapObject *object) {
    MapObjectWork *work = MAP_OBJECT_WORK(object);
    ModelPart *part;

    if (object->definition->kind == 6) {
        MapObject_StartAnim(object, 4);
        MAP_OBJECT_BYTES(object)->step = 1;
        Vector_Copy(work, object->model->parts[1].position);
        work->timer = 0;
        part = object->model->parts;
        work->unk10 = part[1].position[0];
        work->unk14 = part[1].position[1];
        work->unk18 = part[1].position[2];
        if (MAP_OBJECT_FLAGS(object) & 0x20000) {
            MAP_OBJECT_BYTES(object)->step = 2;
            do {
                MapObject_UpdateOpeningDoor(object);
            } while (MAP_OBJECT_BYTES(object)->step != 3);
        }
    } else {
        if (!(object->state_flags & 0x20000)) {
            MapObject_StartAnim(object, 4);
            MAP_OBJECT_BYTES(object)->step = 1;
        } else {
            MapObject_StartAnim(object, 5);
            MAP_OBJECT_BYTES(object)->step = 3;
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
    if (faced_object[0] == object && ((game_status_flags[0] >> 1) & 1)) {
        item = object->source->kind_param_0;
        if (item != 0 && !(object->state_flags & 0x40000)) {
            if (item_counts[item] != 0) {
                MapObject_ShowKeyUsed(item);
                range = 800.0f;
                if (object->definition->sound_range > 0.0f) {
                    range = object->definition->sound_range;
                }
                Sound_PlayMapAtRange(0x2E, 0, &object->model->position.x, range);
                object->state_flags |= 0x40000;
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
        object->state_flags |= 0x20000;
        MAP_OBJECT_BYTES(object)->step = 2;
        MAP_OBJECT_UNK60_BITS(object)->bit0 = TRUE;
    }
}

/* Update handler of an opening door-like object: runs the animation (kind 6
   moves with its second part; kind 2 clears unk60 bit 0 once both pairs of its
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
    if (MODEL_CHANNEL(object->model)->param != 0) {
        ((int (*)(char *))MapObject_StepAnim)((char *)object); // match: it returns a value
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
                point_a = (f32 *)object->model->get_point(object->model, 2);
                point_b = (f32 *)object->model->get_point(object->model, 3);
                point_c = (f32 *)object->model->get_point(object->model, 4);
                point_d = (f32 *)object->model->get_point(object->model, 5);
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
        MapObject_StartAnim(object, 5);
        MAP_OBJECT_BYTES(object)->step = 3;
    }
}

/* Update handler of an open door that shuts by itself (unless its second word is
   set) once the player is further than its reach plus 100. */
void MapObject_UpdateOpenDoor(MapObject *object) {
    ModelInstance *model;
    MapObjectWork *work;
    float *player;
    float delta_x;
    float delta_z;
    float distance;

    work = MAP_OBJECT_WORK(object);
    if (work->timer > 0) {
        work->timer--;
    }
    if (MAP_OBJECT_WORDS(object)->kind_param_1 == 0) {
        model = object->model;
        player = player_state;
        delta_x = model->position.x - player[0];
        delta_z = model->position.z - player[2];
        distance = sqrtf(delta_x * delta_x + delta_z * delta_z);
        if (!(distance < object->reach_radius + 100.0f)) {
            object->state_flags &= ~0x20000;
            MAP_OBJECT_BYTES(object)->step = 4;
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
    if (MODEL_CHANNEL(object->model)->param != 0) {
        ((int (*)(char *))MapObject_StepAnim)((char *)object); // match: it returns a value
        if (object->definition->kind == 6) {
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
        MapObject_StartAnim(object, 4);
        MAP_OBJECT_BYTES(object)->step = 1;
        Vector_Copy(&object->model->position, object->source);
    }
}

/* Signal hook of the doors: opens a closed door (step 1 to 2) or closes an open one (3 to 4). */
void MapObject_SetDoorOpen(MapObject *object, int open) {
    if ((object->state_flags & 0x2FF00) == 0x100 && open) {
        object->state_flags |= 0x20000;
        MAP_OBJECT_BYTES(object)->step = 2;
    } else if ((object->state_flags & 0x2FF00) == 0x20300 && !open) {
        object->state_flags &= ~0x20000;
        MAP_OBJECT_BYTES(object)->step = 4;
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
    Vector_Copy(object->model->parts[1].position, &MAP_OBJECT_WORK(object)->unk10);
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
    MAP_OBJECT_BYTES(object)->step = 1;
}
