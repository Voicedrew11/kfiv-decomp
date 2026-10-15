#include "kf_model.h"
#include "kf_game.h"
#include "kf_message.h"
#include "kf_map_object.h"
#include "kf_map.h"
#include "kf_item.h"

void func_0011FBE8(MapObject *);
void func_0011FC10(MapObject *);
void ModelMatrix_Build(ModelInstance *, Vector4 *, Vector4 *, f32 *);
void ModelMatrix_BuildPartLocal(ModelInstance *, ModelPart *, ModelPart *);
void func_0010B5D0(ModelPart *);
void Matrix_Multiply(void *, void *, void *);
void sceVu0CopyMatrix(void *, void *);
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0RotMatrixZ(float *, float *, float);
void sceVu0TransMatrix(float *, float *, float *);
void Vector_Scale(float *, float *, float);
void func_0010E6A8(ModelInstance *, s64, s64);
void func_0010E820(ModelInstance *, s64);
void func_0010E8E0(void *, int, int, int, int);
int func_0010EA08(int, void *, int *, int *, float);
void *memset(void *, int, u32);
int rand(void);
extern GameStatus game_status;
extern void *D_00420D50[];
extern u32 D_0038CC74[];
extern f32 D_00431AF0[16];

/* Start handler of the save point: step 1. */
void func_001226D0(MapObject *object) {
    MAP_OBJECT_BYTES(object)->step = 1;
}

/* Update handler of the save point (kind 20): used by the player, it asks for the save screen (GameStatus.bit5)
   and hands over its word 0x44, the message id of the area's name that the save list shows. */
void MapObject_UpdateSavePoint(MapObject *object) {
    MapObject *used;

    if (game_status.bit1 && (used = D_00420D50[0]) == object) {
        game_status.save_requested = TRUE;
        game_status.save_area = MAP_OBJECT_WORDS(used)->unk44;
    }
}

/* Start handler of the notice kind: no wait, step 1. */
void func_00122720(MapObject *object) {
    object->work[0] = 0;
    MAP_OBJECT_BYTES(object)->step = 1;
}

/* Update handler: used by the player, it shows the message of its definition,
   at most every 45 frames. */
void func_00122730(MapObject *object) {
    int message;

    if (object->work[0] > 0) {
        object->work[0]--;
    }
    if (((D_0038CC74[0] >> 1) & 1) && D_00420D50[0] == object) {
        if (object->work[0] > 0) {
            return;
        }
        message = (s16)object->unk04->unk68;
        if (message == 0) {
            return;
        }
        Message_ShowNotice(0, 0, 0, 0, 0x2D, message);
        object->work[0] = 0x2D;
    }
}

void func_001227C0(MapObject *object) {
    func_0011FBE8(object);
}

void func_001227C8(MapObject *object) {
    func_0011FC10(object);
}

/* Start handler: closed (animation 4, step 1) or, with bit 17 set, open (animation 5, step 3). */
void func_001227D0(MapObject *object) {
    if (!(object->unk10 & 0x20000)) {
        MapObject_StartAnim(object, 4);
        MAP_OBJECT_BYTES(object)->step = 1;
    } else {
        MapObject_StartAnim(object, 5);
        MAP_OBJECT_BYTES(object)->step = 3;
    }
}

/* Update handler, step 1: used by the player, it starts turning on. */
void func_00122828(MapObject *object) {
    MapObject *used = D_00420D50[0];

    if (used == object && ((D_0038CC74[0] >> 1) & 1)) {
        used->unk10 |= 0x20000;
        MAP_OBJECT_BYTES(used)->step = 2;
    }
}

/* Update handler of a switch turning on: runs the animation, then it is on. */
void func_00122868(MapObject *object) {
    if (MODEL_CHANNEL(object->model)->param != 0) {
        MapObject_StepAnim(object);
    } else {
        MapObject_StartAnim(object, 5);
        MAP_OBJECT_BYTES(object)->step = 3;
    }
}

/* Update handler, step 3: used by the player, it starts turning off. */
void func_001228C0(MapObject *object) {
    MapObject *used = D_00420D50[0];

    if (used == object && ((D_0038CC74[0] >> 1) & 1)) {
        used->unk10 &= ~0x20000;
        MAP_OBJECT_BYTES(used)->step = 4;
    }
}

/* Update handler of a switch turning off: runs the animation, then it is off. */
void func_00122908(MapObject *object) {
    if (MODEL_CHANNEL(object->model)->param != 0) {
        MapObject_StepAnim(object);
    } else {
        MapObject_StartAnim(object, 4);
        MAP_OBJECT_BYTES(object)->step = 1;
    }
}

/* Switches the trigger on (type 1 to 2) or off (type 3 to 4) when it is at rest in the other position. */
void func_00122960(MapTrigger *trigger, int turn_on) {
    if (MAP_TRIGGER_FLAGS(trigger)->bits.type == 1 && !MAP_TRIGGER_FLAGS(trigger)->bits.flag_20000 && turn_on) {
        MAP_TRIGGER_FLAGS(trigger)->bits.flag_20000 = TRUE;
        MAP_TRIGGER_FLAGS(trigger)->bits.type = 2;
    } else if (MAP_TRIGGER_FLAGS(trigger)->bits.type == 3 && MAP_TRIGGER_FLAGS(trigger)->bits.flag_20000 && !turn_on) {
        MAP_TRIGGER_FLAGS(trigger)->bits.flag_20000 = FALSE;
        MAP_TRIGGER_FLAGS(trigger)->bits.type = 4;
    }
}

/* An angle brought back into -half_turn..half_turn. */
static inline f32 func_001229D0_wrap_angle(f32 angle, f32 half_turn) {
    angle += half_turn;
    angle -= (f32)(s32)(angle / 6.2831855f) * 6.2831855f;
    return angle - half_turn;
}

/* Rebuilds the trigger's model matrices, then turns the whole model about its second part's pivot. */
void func_001229D0(MapTrigger *trigger) {
    f32 matrix[16];
    f32 pivot_back[4];
    ModelPart *part;
    f32 *pivot;
    int part_count;
    int i;
    ModelPart *joint;
    f32 half_turn;

    ModelMatrix_Build(trigger->model, &trigger->model->position, &trigger->model->rotation, &trigger->model->unkE0);
    part_count = trigger->model->part_count;
    part = trigger->model->parts;
    for (i = 0; i < part_count; i++) {
        ModelMatrix_BuildPartLocal(trigger->model, part, part);
        part++;
    }
    part = trigger->model->parts;
    if (trigger->model->unk100 & 1) {
        for (i = 0; i < part_count; i++) {
            func_0010B5D0(part);
            part++;
        }
        part = trigger->model->parts;
    }
    joint = part;
    half_turn = 3.1415927f;
    sceVu0CopyMatrix(matrix, D_00431AF0);
    // match: an empty loop here (a debug macro compiled out?) keeps the scheduler from moving anything across
    // this point, which is what leaves the half_turn load in front of the call above
    do { } while (0);
    pivot = joint[1].unk40;
    Vector_Scale(pivot_back, pivot, -1.0f);
    joint++;
    sceVu0TransMatrix(matrix, matrix, pivot_back);
    sceVu0RotMatrixX(matrix, matrix, func_001229D0_wrap_angle(joint->unk50, half_turn));
    sceVu0RotMatrixZ(matrix, matrix, func_001229D0_wrap_angle(joint->unk58, half_turn));
    sceVu0RotMatrixY(matrix, matrix, func_001229D0_wrap_angle(joint->unk54, half_turn));
    sceVu0TransMatrix(matrix, matrix, pivot);
    Matrix_Multiply(trigger->model->unk80, trigger->model->unk40, matrix);
    trigger->model->unk1B4(trigger->model);
}

/* Starts this kind of trigger: at rest in the position its flag gives. */
void func_00122BE8(MapTrigger *trigger) {
    MapTriggerShotKind122BE8 *shot = (MapTriggerShotKind122BE8 *)&trigger->shot;

    func_0010E6A8(trigger->model, 0x40, 0x40);
    func_0010E820(trigger->model, 0x44);
    shot->unk00[0] = 0.0f;
    shot->unk00[1] = -700.0f;
    shot->unk00[2] = 0.0f;
    shot->unk00[3] = 0.0f;
    shot->unk10 = 0.0f;
    if (!(MAP_TRIGGER_FLAGS(trigger)->word & MAP_TRIGGER_FLAG_20000)) {
        trigger->type = 1;
        func_0010E8E0(trigger->model, -1, -1, -1, 0);
    } else {
        trigger->type = 3;
        func_0010E8E0(trigger->model, -1, -1, -1, 0x80);
    }
}

void func_00122CA8(void) {
}

/* Update handler, type 2: grows the radius (up to 100000) and steps the model's
   alpha toward 0x80 within it (func_0010EA08); once that is done the trigger is on
   (type 3) and the radius starts over. */
void func_00122CB0(MapTrigger *trigger) {
    int targets[4];
    int steps[4];
    MapTriggerShotKind122BE8 *shot;

    shot = (MapTriggerShotKind122BE8 *)&trigger->shot;
    memset(targets, 0, 0x10);
    targets[3] = 0x80;
    memset(steps, 0, 0x10);
    steps[3] = 8;
    if (shot->unk10 < 100000.0f) {
        shot->unk10 += 6.6666f;
    }
    if (func_0010EA08((int)trigger->model, shot, steps, targets, shot->unk10) == 0) {
        trigger->type = 3;
        shot->unk10 = 0.0f;
    }
}

void func_00122D60(void) {
}

/* Update handler, type 4: grows the radius (up to 2000) and steps the model's
   alpha toward 0 within it; once that is done the trigger is off (type 1) and the
   radius starts over. */
void func_00122D68(MapTrigger *trigger) {
    int targets[4];
    int steps[4];
    MapTriggerShotKind122BE8 *shot;

    shot = (MapTriggerShotKind122BE8 *)&trigger->shot;
    memset(targets, 0, 0x10);
    memset(steps, 0, 0x10);
    steps[3] = -8;
    if (shot->unk10 < 2000.0f) {
        shot->unk10 += 4.0f;
    }
    if (func_0010EA08((int)trigger->model, shot, steps, targets, shot->unk10) == 0) {
        trigger->type = 1;
        shot->unk10 = 0.0f;
    }
}

/* Switches the trigger on (type 1 to 2) or off (type 3 to 4) when it is at rest in the other position. */
void func_00122E18(MapTrigger *trigger, int turn_on) {
    if (MAP_TRIGGER_FLAGS(trigger)->bits.type == 1 && !MAP_TRIGGER_FLAGS(trigger)->bits.flag_20000 && turn_on) {
        MAP_TRIGGER_FLAGS(trigger)->bits.flag_20000 = TRUE;
        MAP_TRIGGER_FLAGS(trigger)->bits.type = 2;
    } else if (MAP_TRIGGER_FLAGS(trigger)->bits.type == 3 && MAP_TRIGGER_FLAGS(trigger)->bits.flag_20000 && !turn_on) {
        MAP_TRIGGER_FLAGS(trigger)->bits.flag_20000 = FALSE;
        MAP_TRIGGER_FLAGS(trigger)->bits.type = 4;
    }
}

/* Starts this kind of trigger: a random period and a random phase within it. */
void func_00122E88(MapTrigger *trigger) {
    MapTriggerShotKind122E88 *shot = (MapTriggerShotKind122E88 *)&trigger->shot;
    ModelInstance *model = trigger->model;

    model->unk102 |= 0x200;
    model->unk145 = 1;
    trigger->model->unk146 = 0;
    /* 4.656613e-10f is 1 / 2^31: rand() scaled to 0..1 */
    shot->period = (int)((float)rand() * 61.0f * 4.656613e-10f) + 0x78;
    shot->phase = (int)((float)(shot->period + 1) * (float)rand() * 4.656613e-10f);
    trigger->type = 1;
}

/* Update handler of the pulsing kind: steps the phase through the period and sets
   the model's unk146 on a triangle wave that peaks at 32 half-way through. */
int func_00122F48(MapTrigger *trigger) {
    MapTriggerShotKind122E88 *shot;
    int phase;
    int period;
    float level;
    int value;

    shot = (MapTriggerShotKind122E88 *)&trigger->shot;
    phase = (shot->phase + 1) % shot->period;
    shot->phase = phase;
    period = shot->period;
    if (phase < period / 2) {
        level = (float)phase / (float)period * 0.5f;
    } else {
        level = (float)(period - phase) / (float)period * 0.5f;
    }
    value = (int)(level * 128.0f);
    trigger->model->unk146 = value;
    return value;
}

/* Shows "... Key used" when the item a door or chest needed is one of the keys. */
void MapObject_ShowKeyUsed(int item) {
    if (item >= ITEM_KEY_FIRST && item <= ITEM_KEY_LAST) {
        Message_ShowNotice(0, 0, 0, 0, 0x2D, item + (MSG_KEY_USED_FIRST - ITEM_KEY_FIRST));
    }
}
