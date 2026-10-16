#include "kf_map_object.h"
#include "kf_model.h"
#include "kf_game.h"
#include "kf_message.h"
#include "kf_map.h"

void MapObject_RebuildMatrices(char *);
void Vector_ApplyMatrix(void *, void *, void *);
void Vector_Copy(void *, void *);
void Vector_Subtract(void *, void *, void *);
float sqrtf(float);
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void Vector_Scale(float *, float *, float);
void Vector_Add(void *, void *, void *);
void Player_AddItem(int);
extern GameStatus game_status;
extern void *faced_object[];
extern float player_state[];
extern f32 identity_matrix[16];
extern ItemCommonDef item_common_defs[];

/* Step 0 of the pedestals (kind 0xD: the Bridge, Binding and Release Pedestals); the item sits on part 2.
   Empty: animation 4, step 1; with bit 17 set it holds its item: animation 5, step 4. */
void MapObject_StartPedestal(MapObject *object) {
    MapObjectWorkSlide *work = (MapObjectWorkSlide *)object->work;

    work->part = 2;
    work->fired = 0;
    if (!(object->unk10 & 0x20000)) {
        MapObject_StartAnim(object, 4);
        MAP_OBJECT_BYTES(object)->step = 1;
    } else {
        MapObject_StartAnim(object, 5);
        MAP_OBJECT_BYTES(object)->step = 4;
    }
}

/* Step 1 of a pedestal: used by the player it only shows its name, at most every 45 frames. Once its item has
   been placed (MapObject_PlaceItemOnPedestal) the item's part is put 115 above the player's feet (y grows
   downward) and given the steps (6.67 a frame) that carry it back onto the pedestal; step 2. */
void MapObject_UpdateEmptyPedestal(MapObject *object) {
    MapObjectWorkSlide *work = (MapObjectWorkSlide *)object->work;
    ModelPart *part = &object->model->parts[work->part];
    f32 matrix[16];
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
        Message_ShowNotice(0, 0, 0, 0, 0x2D, (s16)object->unk04->unk68);
        work->timer = 0x2D;
    } else if (work->fired != 0) {
        work->fired = 0;
        object->unk10 |= 0x20000;
        MAP_OBJECT_BYTES(object)->step = 2;
        MapObject_StepAnim(object);
        MapObject_RebuildMatrices((char *)object);
        origin.w = 1.0f;
        origin.x = origin.y = origin.z = 0.0f;
        Vector_ApplyMatrix(&origin, part, &origin);
        Vector_ApplyMatrix(&origin, object->model->unk40, &origin);
        Vector_Copy(&target, player_state);
        target.y -= 115.0f;
        Vector_Subtract(work, &origin, &target);
        sceVu0RotMatrixX(matrix, identity_matrix, object->model->rotation.x);
        sceVu0RotMatrixY(matrix, identity_matrix, -object->model->rotation.y);
        Vector_ApplyMatrix(work, matrix, work);
        distance = sqrtf(work->step.x * work->step.x + work->step.y * work->step.y + work->step.z * work->step.z);
        work->frames = distance / 6.6666665f;
        Vector_Subtract(part->unk40, part->unk40, work);
        Vector_Scale(&work->step.x, &work->step.x, 1.0f / work->frames);
        game_status.unk30 |= 1;
    }
}

/* Step 2 of a pedestal: moves the item's part one step toward the pedestal; after the last one, step 3. */
void MapObject_MovePedestalItemIn(MapObject *object) {
    MapObjectWorkSlide *work = (MapObjectWorkSlide *)object->work;
    ModelPart *part = &object->model->parts[work->part];

    if (work->timer > 0) {
        work->timer--;
    }
    Vector_Add(part->unk40, part->unk40, work);
    if (--work->frames <= 0) {
        MAP_OBJECT_BYTES(object)->step = 3;
    }
}

/* Step 3 of a pedestal: runs the animation; at its end the pedestal holds the item (animation 5, step 4) and
   game_status.unk30 bit 0 is cleared. */
void MapObject_SettlePedestalItem(MapObject *object) {
    MapObjectWork *work = MAP_OBJECT_WORK(object);

    if (work->timer > 0) {
        work->timer--;
    }
    if (MODEL_CHANNEL(object->model)->param != 0) {
        MapObject_StepAnim(object);
        return;
    }
    MapObject_StartAnim(object, 5);
    MAP_OBJECT_BYTES(object)->step = 4;
    game_status.unk30 &= ~1U;
}

/* Step 4 of a pedestal: used by the player, it gives its item back (Player_AddItem) and goes to step 5,
   unless the item is of use type 5 (altar icons and keys, which stay): then its name is shown, at most every
   45 frames. */
void MapObject_UpdateFilledPedestal(MapObject *object) {
    MapObjectWorkSlide *work = (MapObjectWorkSlide *)object->work;
    MapObjectUnk08Words *source = MAP_OBJECT_WORDS(object);
    ItemCommonDef *item_def = &item_common_defs[source->unk44];

    if (work->timer > 0) {
        work->timer--;
    }
    if (game_status.bit1 && faced_object[0] == object) {
        if (item_def->type == 5) {
            if (work->timer > 0) {
                return;
            }
            Message_ShowNotice(0, 0, 0, 0, 0x2D, (s16)object->unk04->unk68);
            work->timer = 0x2D;
        } else {
            MAP_OBJECT_BYTES(object)->step = 5;
            object->unk10 &= ~0x20000;
            Player_AddItem(source->unk44);
            game_status.unk30 |= 1;
        }
    }
}

/* Step 5 of a pedestal: once the animation has ended, works out the steps (6.67 a frame) that carry the
   item's part to 115 above the player's feet; step 6. */
void MapObject_LiftPedestalItem(MapObject *object) {
    MapObjectWorkSlide *work = (MapObjectWorkSlide *)object->work;
    ModelPart *part = &object->model->parts[work->part];
    f32 matrix[16];
    Vector4 origin;
    Vector4 target;
    f32 distance;
    int frames;

    if (work->timer > 0) {
        work->timer--;
    }
    if (MODEL_CHANNEL(object->model)->param != 0) {
        MapObject_StepAnim(object);
        return;
    }
    MAP_OBJECT_BYTES(object)->step = 6;
    MapObject_RebuildMatrices((char *)object);
    origin.w = 1.0f;
    origin.x = origin.y = origin.z = 0.0f;
    Vector_ApplyMatrix(&origin, part, &origin);
    Vector_ApplyMatrix(&origin, object->model->unk40, &origin);
    Vector_Copy(&target, player_state);
    target.y -= 115.0f;
    Vector_Subtract(work, &target, &origin);
    distance = sqrtf(((MapObjectWorkSlide *)object->work)->step.x * ((MapObjectWorkSlide *)object->work)->step.x + work->step.y * work->step.y + work->step.z * work->step.z);
    sceVu0RotMatrixX(matrix, identity_matrix, object->model->rotation.x);
    sceVu0RotMatrixY(matrix, identity_matrix, -object->model->rotation.y);
    Vector_ApplyMatrix(work, matrix, work);
    frames = distance / 6.6666665f;
    work->frames = frames;
    Vector_Scale(&work->step.x, &work->step.x, 1.0f / frames);
}

/* Step 6 of a pedestal: moves the item's part one step toward the player; after the last one the pedestal is
   empty again (animation 4, step 1) and game_status.unk30 bit 0 is cleared. */
void MapObject_MovePedestalItemOut(MapObject *object) {
    MapObjectWorkSlide *work = (MapObjectWorkSlide *)object->work;
    ModelPart *part = &object->model->parts[work->part];

    if (work->timer > 0) {
        work->timer--;
    }
    Vector_Add(part->unk40, part->unk40, work);
    if (--work->frames <= 0) {
        MapObject_StartAnim(object, 4);
        MAP_OBJECT_BYTES(object)->step = 1;
        game_status.unk30 &= ~1U;
    }
}

/* Every-frame hook of the pedestals: MapObject_RebuildMatrices. */
void MapObject_RebuildPedestalMatrices(char *object) {
    MapObject_RebuildMatrices(object);
}

/* Item hook of the pedestals: an empty pedestal (step 1) that has taken nothing yet takes the item it waits
   for (its source's unk44); returns whether it took it. */
int MapObject_PlaceItemOnPedestal(MapObject *object, int item) {
    MapObjectWorkSlide *work = (MapObjectWorkSlide *)object->work;
    u8 step = MAP_OBJECT_BYTES(object)->step;

    if (step == 1 && work->fired == 0 && item == MAP_OBJECT_WORDS(object)->unk44) {
        work->fired = step;
        return TRUE;
    }
    return FALSE;
}
