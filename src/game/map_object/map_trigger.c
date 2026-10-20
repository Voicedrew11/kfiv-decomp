#include "model/model.h"
#include "core/game.h"
#include "ui/message.h"
#include "map_object/map_object.h"
#include "map/map.h"
#include "item/item.h"

void MapObject_UpdateBreakingItemHolder(MapObject *);
void MapObject_SetupItemHolder(MapObject *);
void ModelMatrix_Build(ModelInstance *, Vector4 *, Vector4 *, f32 *);
void ModelMatrix_BuildPartLocal(ModelInstance *, ModelPart *, ModelPart *);
void Model_MultiplyChildMatrices(ModelPart *);
void Matrix_Multiply(void *, void *, void *);
void sceVu0CopyMatrix(void *, void *);
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0RotMatrixZ(float *, float *, float);
void sceVu0TransMatrix(float *, float *, float *);
void Vector_Scale(float *, float *, float);
void Model_SetPrimBits(ModelInstance *, s64, s64);
void Model_SetAlphaBlend(ModelInstance *, s64);
void Model_SetVertexColour(void *, int, int, int, int);
int Model_FadeVertexColoursNear(int, void *, int *, int *, float);
void *memset(void *, int, u32);
int rand(void);
extern GameStatus game_status;
extern void *faced_object[];
extern u32 game_status_flags[];
extern f32 identity_matrix[16];

/* Start handler of the save point: step 1. */
void MapObject_StartSavePoint(MapObject *object) {
    MAP_OBJECT_BYTES(object)->step = 1;
}

/* Update handler of the save point (kind 20): used by the player, it asks for the save screen (GameStatus.bit5)
   and hands over its word 0x44, the message id of the area's name that the save list shows. */
void MapObject_UpdateSavePoint(MapObject *object) {
    MapObject *used;

    if (game_status.bit1 && (used = faced_object[0]) == object) {
        game_status.save_requested = TRUE;
        game_status.save_area = MAP_OBJECT_WORDS(used)->kind_param_0;
    }
}

/* Start handler of a container (kind 21, barrels, pots and boxes): no wait, step 1. */
void MapObject_StartContainer(MapObject *object) {
    object->work[0] = 0;
    MAP_OBJECT_BYTES(object)->step = 1;
}

/* Update handler of a container: used by the player, it shows the message of its definition (what it
   is), at most every 45 frames. */
void MapObject_UpdateContainer(MapObject *object) {
    int message;

    if (object->work[0] > 0) {
        object->work[0]--;
    }
    if (GAME_STATUS_FLAG_BITS(game_status_flags)->using_faced_object && faced_object[0] == object) {
        if (object->work[0] > 0) {
            return;
        }
        message = (s16)object->definition->definition_flags;
        if (message == 0) {
            return;
        }
        Message_ShowNotice(0, 0, 0, 0, 0x2D, message);
        object->work[0] = 0x2D;
    }
}

/* Step 0xF of a container, after MapObject_Break: puts out the item it holds, then breaks like the other
   kinds (MapObject_UpdateBreakingItemHolder). */
void MapObject_UpdateBreakingContainer(MapObject *object) {
    MapObject_UpdateBreakingItemHolder(object);
}

/* Set-up of a container: puts out the item it holds (MapObject_SetupItemHolder, as for kind 3). */
void MapObject_SetUpContainer(MapObject *object) {
    MapObject_SetupItemHolder(object);
}

/* Start handler of a switch (kind 30): off (animation 4, step 1) or, with bit 17 set, on (animation 5,
   step 3). */
void MapObject_StartPivotSwitch(MapObject *object) {
    if (!(object->state_flags & MAP_OBJECT_ON)) {
        MapObject_StartAnim(object, 4);
        MAP_OBJECT_BYTES(object)->step = 1;
    } else {
        MapObject_StartAnim(object, 5);
        MAP_OBJECT_BYTES(object)->step = 3;
    }
}

/* Update handler of a switch that is off (step 1): used by the player, it starts turning on. */
void MapObject_UpdatePivotSwitchOff(MapObject *object) {
    MapObject *used = faced_object[0];

    if (used == object && GAME_STATUS_FLAG_BITS(game_status_flags)->using_faced_object) {
        used->state_flags |= MAP_OBJECT_ON;
        MAP_OBJECT_BYTES(used)->step = 2;
    }
}

/* Update handler of a switch turning on: runs the animation, then it is on. */
void MapObject_TurnPivotSwitchOn(MapObject *object) {
    if (MODEL_CHANNEL(object->model)->frame != 0) {
        MapObject_StepAnim(object);
    } else {
        MapObject_StartAnim(object, 5);
        MAP_OBJECT_BYTES(object)->step = 3;
    }
}

/* Update handler of a switch that is on (step 3): used by the player, it starts turning off. */
void MapObject_UpdatePivotSwitchOn(MapObject *object) {
    MapObject *used = faced_object[0];

    if (used == object && GAME_STATUS_FLAG_BITS(game_status_flags)->using_faced_object) {
        used->state_flags &= ~MAP_OBJECT_ON;
        MAP_OBJECT_BYTES(used)->step = 4;
    }
}

/* Update handler of a switch turning off: runs the animation, then it is off. */
void MapObject_TurnPivotSwitchOff(MapObject *object) {
    if (MODEL_CHANNEL(object->model)->frame != 0) {
        MapObject_StepAnim(object);
    } else {
        MapObject_StartAnim(object, 4);
        MAP_OBJECT_BYTES(object)->step = 1;
    }
}

/* Signal hook of a switch: turns it on (step 1 to 2) or off (step 3 to 4) when it is at rest in the other
   position. */
void MapObject_SignalPivotSwitch(MapTrigger *trigger, int turn_on) {
    if (MAP_TRIGGER_FLAGS(trigger)->bits.type == 1 && !MAP_TRIGGER_FLAGS(trigger)->bits.flag_20000 && turn_on) {
        MAP_TRIGGER_FLAGS(trigger)->bits.flag_20000 = TRUE;
        MAP_TRIGGER_FLAGS(trigger)->bits.type = 2;
    } else if (MAP_TRIGGER_FLAGS(trigger)->bits.type == 3 && MAP_TRIGGER_FLAGS(trigger)->bits.flag_20000 && !turn_on) {
        MAP_TRIGGER_FLAGS(trigger)->bits.flag_20000 = FALSE;
        MAP_TRIGGER_FLAGS(trigger)->bits.type = 4;
    }
}

/* An angle brought back into -half_turn..half_turn. */
static inline f32 wrap_half_turn(f32 angle, f32 half_turn) {
    angle += half_turn;
    angle -= (f32)(s32)(angle / 6.2831855f) * 6.2831855f;
    return angle - half_turn;
}

/* Every-frame hook of a switch: rebuilds its model matrices, then turns the whole model about its second
   part's pivot by that part's rotation. */
void MapObject_PosePivotSwitch(MapTrigger *trigger) {
    f32 matrix[16];
    f32 pivot_back[4];
    ModelPart *part;
    f32 *pivot;
    int part_count;
    int i;
    ModelPart *joint;
    f32 half_turn;

    ModelMatrix_Build(trigger->model, &trigger->model->position, &trigger->model->rotation, &trigger->model->scale_x);
    part_count = trigger->model->part_count;
    part = trigger->model->parts;
    for (i = 0; i < part_count; i++) {
        ModelMatrix_BuildPartLocal(trigger->model, part, part);
        part++;
    }
    part = trigger->model->parts;
    if (trigger->model->model_flags & 1) {
        for (i = 0; i < part_count; i++) {
            Model_MultiplyChildMatrices(part);
            part++;
        }
        part = trigger->model->parts;
    }
    joint = part;
    half_turn = 3.1415927f;
    sceVu0CopyMatrix(matrix, identity_matrix);
    // match: an empty loop here (a debug macro compiled out?) keeps the scheduler from moving anything across
    // this point, which is what leaves the half_turn load in front of the call above
    do { } while (0);
    pivot = joint[1].position;
    Vector_Scale(pivot_back, pivot, -1.0f);
    joint++;
    sceVu0TransMatrix(matrix, matrix, pivot_back);
    sceVu0RotMatrixX(matrix, matrix, wrap_half_turn(joint->rotation_x, half_turn));
    sceVu0RotMatrixZ(matrix, matrix, wrap_half_turn(joint->rotation_z, half_turn));
    sceVu0RotMatrixY(matrix, matrix, wrap_half_turn(joint->rotation_y, half_turn));
    sceVu0TransMatrix(matrix, matrix, pivot);
    Matrix_Multiply(trigger->model->collision_matrix, trigger->model->model_matrix, matrix);
    trigger->model->update_bone_points(trigger->model);
}

/* Start handler of the fading kind (kind 31): at rest hidden (step 1, alpha 0) or, with bit 17 set, shown
   (step 3, alpha 0x80). */
void MapObject_StartFadeObject(MapTrigger *trigger) {
    MapTriggerShotKind122BE8 *shot = (MapTriggerShotKind122BE8 *)&trigger->shot;

    Model_SetPrimBits(trigger->model, 0x40, 0x40);
    Model_SetAlphaBlend(trigger->model, 0x44);
    shot->fade_vector[0] = 0.0f;
    shot->fade_vector[1] = -700.0f;
    shot->fade_vector[2] = 0.0f;
    shot->fade_vector[3] = 0.0f;
    shot->fade_progress = 0.0f;
    if (!(MAP_TRIGGER_FLAGS(trigger)->word & MAP_TRIGGER_FLAG_20000)) {
        trigger->type = 1;
        Model_SetVertexColour(trigger->model, -1, -1, -1, 0);
    } else {
        trigger->type = 3;
        Model_SetVertexColour(trigger->model, -1, -1, -1, 0x80);
    }
}

/* Step 1 of the fading kind, at rest hidden until MapObject_SignalFadeObject starts the fade-in: empty. */
void MapObject_KeepFadeObjectHidden(void) {
}

/* Update handler, type 2: grows the radius (up to 100000) and steps the model's
   alpha toward 0x80 within it (Model_FadeVertexColoursNear); once that is done the trigger is on
   (type 3) and the radius starts over. */
void MapObject_FadeInObject(MapTrigger *trigger) {
    int targets[4];
    int steps[4];
    MapTriggerShotKind122BE8 *shot;

    shot = (MapTriggerShotKind122BE8 *)&trigger->shot;
    memset(targets, 0, 0x10);
    targets[3] = 0x80;
    memset(steps, 0, 0x10);
    steps[3] = 8;
    if (shot->fade_progress < 100000.0f) {
        shot->fade_progress += 6.6666f;
    }
    if (Model_FadeVertexColoursNear((int)trigger->model, shot, steps, targets, shot->fade_progress) == 0) {
        trigger->type = 3;
        shot->fade_progress = 0.0f;
    }
}

/* Step 3 of the fading kind, at rest shown until MapObject_SignalFadeObject starts the fade-out: empty. */
void MapObject_KeepFadeObjectShown(void) {
}

/* Update handler, type 4: grows the radius (up to 2000) and steps the model's
   alpha toward 0 within it; once that is done the trigger is off (type 1) and the
   radius starts over. */
void MapObject_FadeOutObject(MapTrigger *trigger) {
    int targets[4];
    int steps[4];
    MapTriggerShotKind122BE8 *shot;

    shot = (MapTriggerShotKind122BE8 *)&trigger->shot;
    memset(targets, 0, 0x10);
    memset(steps, 0, 0x10);
    steps[3] = -8;
    if (shot->fade_progress < 2000.0f) {
        shot->fade_progress += 4.0f;
    }
    if (Model_FadeVertexColoursNear((int)trigger->model, shot, steps, targets, shot->fade_progress) == 0) {
        trigger->type = 1;
        shot->fade_progress = 0.0f;
    }
}

/* Signal hook of the fading kind: starts fading it in (step 1 to 2) or out (step 3 to 4) when it is at rest
   in the other state. */
void MapObject_SignalFadeObject(MapTrigger *trigger, int turn_on) {
    if (MAP_TRIGGER_FLAGS(trigger)->bits.type == 1 && !MAP_TRIGGER_FLAGS(trigger)->bits.flag_20000 && turn_on) {
        MAP_TRIGGER_FLAGS(trigger)->bits.flag_20000 = TRUE;
        MAP_TRIGGER_FLAGS(trigger)->bits.type = 2;
    } else if (MAP_TRIGGER_FLAGS(trigger)->bits.type == 3 && MAP_TRIGGER_FLAGS(trigger)->bits.flag_20000 && !turn_on) {
        MAP_TRIGGER_FLAGS(trigger)->bits.flag_20000 = FALSE;
        MAP_TRIGGER_FLAGS(trigger)->bits.type = 4;
    }
}

/* Start handler of the pulsing kind (kind 29): a random period of 120 to 180 frames and a random phase
   within it. */
void MapObject_StartPulse(MapTrigger *trigger) {
    MapTriggerShotKind122E88 *shot = (MapTriggerShotKind122E88 *)&trigger->shot;
    ModelInstance *model = trigger->model;

    model->draw_flags |= 0x200;
    model->fade_enabled = 1;
    trigger->model->fade_alpha = 0;
    /* 4.656613e-10f is 1 / 2^31: rand() scaled to 0..1 */
    shot->period = (int)((float)rand() * 61.0f * 4.656613e-10f) + 0x78;
    shot->phase = (int)((float)(shot->period + 1) * (float)rand() * 4.656613e-10f);
    trigger->type = 1;
}

/* Update handler of the pulsing kind: steps the phase through the period and sets
   the model's unk146 on a triangle wave that peaks at 32 half-way through (with unk145 set, as the start
   handler leaves it, Model_DrawPosedWithAlpha draws the model with unk146 as its fixed alpha). */
int MapObject_UpdatePulse(MapTrigger *trigger) {
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
    trigger->model->fade_alpha = value;
    return value;
}

/* Shows "... Key used" when the item a door or chest needed is one of the keys. */
void MapObject_ShowKeyUsed(int item) {
    if (item >= ITEM_KEY_FIRST && item <= ITEM_KEY_LAST) {
        Message_ShowNotice(0, 0, 0, 0, 0x2D, item + (MSG_KEY_USED_FIRST - ITEM_KEY_FIRST));
    }
}
