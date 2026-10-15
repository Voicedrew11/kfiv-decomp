#include "kf_map_object.h"
#include "kf_model.h"
#include "kf_game.h"
#include "kf_message.h"
#include "kf_map.h"

void func_0011F148(char *);
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
extern void *D_00420D50[];
extern float player_state[];
extern f32 D_00431AF0[16];
extern ItemCommonDef item_common_defs[];

/* Start handler of a sliding kind: part 2, closed (animation 4, step 1) or, with bit 17 set,
   open (animation 5, step 4). */
void func_00120EE0(MapObject *object) {
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

/* Update handler of a sliding kind, step 1: used by the player it only shows a
   message; once fired (func_00121620) it puts the part at a point 115 below the
   player and works out the steps that carry it back (6.67 a frame). */
void func_00120F48(MapObject *object) {
    MapObjectWorkSlide *work = (MapObjectWorkSlide *)object->work;
    ModelPart *part = &object->model->parts[work->part];
    f32 matrix[16];
    Vector4 origin;
    Vector4 target;
    f32 distance;

    if (work->timer > 0) {
        work->timer--;
    }
    if (game_status.bit1 && D_00420D50[0] == object) {
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
        func_0011F148((char *)object);
        origin.w = 1.0f;
        origin.x = origin.y = origin.z = 0.0f;
        Vector_ApplyMatrix(&origin, part, &origin);
        Vector_ApplyMatrix(&origin, object->model->unk40, &origin);
        Vector_Copy(&target, player_state);
        target.y -= 115.0f;
        Vector_Subtract(work, &origin, &target);
        sceVu0RotMatrixX(matrix, D_00431AF0, object->model->rotation.x);
        sceVu0RotMatrixY(matrix, D_00431AF0, -object->model->rotation.y);
        Vector_ApplyMatrix(work, matrix, work);
        distance = sqrtf(work->step.x * work->step.x + work->step.y * work->step.y + work->step.z * work->step.z);
        work->frames = distance / 6.6666665f;
        Vector_Subtract(part->unk40, part->unk40, work);
        Vector_Scale(&work->step.x, &work->step.x, 1.0f / work->frames);
        game_status.unk30 |= 1;
    }
}

/* Update handler of a sliding kind, step 2: moves the part one step; after the
   last one it is at step 3. */
void func_001211B0(MapObject *object) {
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

/* Step handler: counts the timer down and runs the animation; at its end starts
   animation 5 at step 4 and clears bit 0 of game_status.unk30. */
void func_00121228(MapObject *object) {
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

/* Update handler of a sliding kind, step 3: used by the player, it gives back
   the item it holds and goes to step 5, unless the item is of type 5: then a
   message is shown, at most every 45 frames. */
void func_001212B0(MapObject *object) {
    MapObjectWorkSlide *work = (MapObjectWorkSlide *)object->work;
    MapObjectUnk08Words *source = MAP_OBJECT_WORDS(object);
    ItemCommonDef *item_def = &item_common_defs[source->unk44];

    if (work->timer > 0) {
        work->timer--;
    }
    if (game_status.bit1 && D_00420D50[0] == object) {
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

/* Update handler of a sliding kind, step 5: once the animation has ended, works
   out the steps that carry the part to a point 115 below the player (6.67 a frame). */
void func_00121398(MapObject *object) {
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
    func_0011F148((char *)object);
    origin.w = 1.0f;
    origin.x = origin.y = origin.z = 0.0f;
    Vector_ApplyMatrix(&origin, part, &origin);
    Vector_ApplyMatrix(&origin, object->model->unk40, &origin);
    Vector_Copy(&target, player_state);
    target.y -= 115.0f;
    Vector_Subtract(work, &target, &origin);
    distance = sqrtf(((MapObjectWorkSlide *)object->work)->step.x * ((MapObjectWorkSlide *)object->work)->step.x + work->step.y * work->step.y + work->step.z * work->step.z);
    sceVu0RotMatrixX(matrix, D_00431AF0, object->model->rotation.x);
    sceVu0RotMatrixY(matrix, D_00431AF0, -object->model->rotation.y);
    Vector_ApplyMatrix(work, matrix, work);
    frames = distance / 6.6666665f;
    work->frames = frames;
    Vector_Scale(&work->step.x, &work->step.x, 1.0f / frames);
}

/* Update handler of a sliding kind: moves the part one step; after the last one it
   starts animation 4 at step 1 and clears bit 0 of game_status.unk30. */
void func_00121578(MapObject *object) {
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

void func_00121618(char *object) {
    func_0011F148(object);
}

/* Fires a resting object of this kind (step 1, not fired yet) when the item is
   the one it waits for; returns whether it fired. */
int func_00121620(MapObject *object, int item) {
    MapObjectWorkSlide *work = (MapObjectWorkSlide *)object->work;
    u8 step = MAP_OBJECT_BYTES(object)->step;

    if (step == 1 && work->fired == 0 && item == MAP_OBJECT_WORDS(object)->unk44) {
        work->fired = step;
        return TRUE;
    }
    return FALSE;
}
