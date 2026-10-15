#include "kf_map.h"
#include "kf_magic_ladder.h"
#include "kf_player.h"
#include "kf_entity.h"
#include "kf_shot.h"
#include "kf_model.h"
#include "kf_map_object.h"

void Vector_Subtract(void *, void *, void *);
float atan2f(float, float);
float Angle_Wrap(float);
float sqrtf(float);
int func_001B4F30(u16, void *, void *, void *, void *);
extern void *D_00414188[];
extern u32 D_0038CC74[];
extern PlayerState player_state;
extern u32 D_00446ED4;

/* Starts this kind of trigger: stopped, with its state cleared. */
void func_00121660(MapTrigger *trigger) {
    MapTriggerShotKind121660 *shot = (MapTriggerShotKind121660 *)&trigger->shot;

    shot->unk00 = 0.0f;
    shot->unk08 = 0;
    shot->unk0C = 0;
    trigger->type = 1;
}

/* Update handler of the kind of func_00121660 (a platform): it rises while the
   player stands on it, after a wait of unk4E frames, and sinks back when he does
   not. unk46 and unk48 are the speeds up and down, or accelerations when unk4A
   and unk4C are set; unk44 is how far it rises. */
void func_00121680(MapObject *object) {
    MapTriggerShotKind121660 *shot = (MapTriggerShotKind121660 *)object->work;
    MapObjectUnk08Words *source;
    ModelInstance *model;
    f32 top;

    shot->unk0C = shot->unk08;
    shot->unk08 = D_00414188[0] == object;
    if (shot->unk08 != shot->unk0C) {
        shot->unk04 = MAP_OBJECT_WORDS(object)->unk4E;
    }
    switch (shot->unk08) {
        case 0:
            source = MAP_OBJECT_WORDS(object);
            if (source->unk4C == 0) {
                ((MapTriggerShotKind121660 *)object->work)->unk00 = source->unk48;
            } else {
                ((MapTriggerShotKind121660 *)object->work)->unk00 -= source->unk48;
            }
            model = object->model;
            model->position.y -= shot->unk00;
            if (model->position.y < object->unk08->position.y) {
                model->position.y = object->unk08->position.y;
            }
            break;
        case 1:
            if (shot->unk04 > 0) {
                shot->unk04--;
                return;
            }
            source = MAP_OBJECT_WORDS(object);
            if (source->unk4A == 0) {
                ((MapTriggerShotKind121660 *)object->work)->unk00 = source->unk46;
            } else {
                ((MapTriggerShotKind121660 *)object->work)->unk00 += source->unk46;
            }
            model = object->model;
            model->position.y += shot->unk00;
            top = object->unk08->position.y + source->unk44;
            if (top < model->position.y) {
                model->position.y = top;
            }
            break;
    }
}

/* Start handler of the ladder kind: step 1. */
void func_001217B0(MapObject *object) {
    MAP_OBJECT_BYTES(object)->step = 1;
}

/* Update handler: when the player (using it, status bit 36 clear) is within
   75 in height, within the radius and inside the angle of one of its two
   points, Ladder_Start is called for that point. */
void func_001217C0(MapObject *object) {
    Vector4 delta;
    int i;
    f32 turn;
    f32 half_angle;

    if (!((D_0038CC74[0] >> 1) & 1)) {
        return;
    }
    if ((s32)(player_state.unk1C8 >> 36) & 1) {
        return;
    }
    for (i = 0; i < 2; i++) {
        Vector_Subtract(&delta, (void *)object->model->get_point(object->model, i), &player_state);
        if (delta.y < -75.0f) {
            continue;
        }
        if (delta.y > 75.0f) {
            continue;
        }
        if ((f32)MAP_OBJECT_WORD_ARRAY(object)[i * 3] * (f32)MAP_OBJECT_WORD_ARRAY(object)[i * 3] < delta.x * delta.x + delta.z * delta.z) {
            continue;
        }
        turn = Angle_Wrap(atan2f(delta.x, delta.z) - object->model->rotation.y);
        half_angle = MAP_OBJECT_WORD_ARRAY(object)[i * 3 + 1];
        half_angle = half_angle * 0.017453292f * 0.5f;
        if (turn < -half_angle) {
            continue;
        }
        if (half_angle < turn) {
            continue;
        }
        Ladder_Start((ObjectReference *)object, i);
        return;
    }
}

/* Start handler of the shooting kind: step 1. */
void func_00121958(MapObject *object) {
    MAP_OBJECT_BYTES(object)->step = 1;
}

/* Update handler of the shooting kind: every so many frames (or all the time
   when its model is animated), while it is on and the player is in range, it
   launches something from a point of the model, turned by two angles of its source. */
void func_00121968(MapObject *object) {
    Vector4 aim;
    f32 distance;
    void *point;
    EntityUnk1B1390 *shot;

    if (!(object->model->unk100 & 1)) {
        if (D_00446ED4 % MAP_OBJECT_WORDS(object)->unk46 != 0) {
            return;
        }
    }
    if (!(object->unk10 & 0x20000)) {
        return;
    }
    Vector_Subtract(&aim, &object->model->position, &player_state);
    distance = sqrtf(aim.x * aim.x + aim.y * aim.y + aim.z * aim.z);
    if (MAP_OBJECT_WORDS(object)->unk4E < distance) {
        return;
    }
    point = (void *)object->model->get_point(object->model, MAP_OBJECT_WORDS(object)->unk48);
    aim.x = Angle_Wrap(object->model->rotation.x + MAP_OBJECT_WORDS(object)->unk4A * 0.017453292f);
    aim.y = Angle_Wrap(object->model->rotation.y + MAP_OBJECT_WORDS(object)->unk4C * 0.017453292f + 3.1415927f);
    aim.z = object->model->rotation.z;
    aim.w = 0.0f;
    shot = Shot_Alloc();
    shot->unk20 = 0;
    shot->unk00 = 1;
    shot->unk22 = 0;
    // match: the id is passed as 16 bits
    ((void (*)(u16, void *, void *, void *, void *))func_001B4F30)(MAP_OBJECT_WORDS(object)->unk44, shot, point, &aim, object);
}

/* Sets or clears bit 17 of a map object. */
void func_00121B10(MapObject *object, int enabled) {
    MAP_OBJECT_BITS(object)->bit17 = enabled != 0;
}

/* Start handler: off (animation 4, step 1) or on (animation 5, step 3), with
   the wait that goes with it. */
void func_00121B38(MapObject *object) {
    if (!(object->unk10 & 0x20000)) {
        MapObject_StartAnim(object, 4);
        MAP_OBJECT_BYTES(object)->step = 1;
        object->work[0] = MAP_OBJECT_WORDS(object)->unk44;
    } else {
        MapObject_StartAnim(object, 5);
        MAP_OBJECT_BYTES(object)->step = 3;
        object->work[0] = MAP_OBJECT_WORDS(object)->unk46;
    }
}

/* Update handler, step 1: stepped on by the player, it turns on (unless its
   first wait is negative). */
void func_00121BA8(MapObject *object) {
    MapObject *stood_on = D_00414188[0];

    if (stood_on == object && MAP_OBJECT_WORDS(stood_on)->unk44 >= 0) {
        stood_on->unk10 |= 0x20000;
        MAP_OBJECT_BYTES(stood_on)->step = 2;
    }
}

/* Step handler: waits out a delay (counted only while unk44 is positive), then runs the
   animation and, once it ends, starts animation 5 at step 3 with the delay reloaded from unk46. */
void func_00121BE8(MapObject *object) {
    int delay = object->work[0];
    char reserved[0x10]; // match: unused, but retail reserves the 0x10 bytes

    if (delay != 0) {
        if (MAP_OBJECT_WORDS(object)->unk44 > 0) {
            object->work[0] = delay - 1;
        }
    } else {
        if (MODEL_CHANNEL(object->model)->param != 0) {
            MapObject_StepAnim(object);
        } else {
            MapObject_StartAnim(object, 5);
            MAP_OBJECT_BYTES(object)->step = 3;
            object->work[0] = MAP_OBJECT_WORDS(object)->unk46;
        }
    }
}

/* Update handler, step 3: turns off again (unless its second wait is negative). */
void func_00121C68(MapObject *object) {
    if (MAP_OBJECT_WORDS(object)->unk46 >= 0) {
        object->unk10 &= ~0x20000;
        MAP_OBJECT_BYTES(object)->step = 4;
    }
}

/* Update handler, step 4: waits, then runs the animation back; at its end the
   object is off again. */
void func_00121CA0(MapObject *object) {
    if (object->work[0] != 0) {
        if (MAP_OBJECT_WORDS(object)->unk46 > 0) {
            object->work[0]--;
        }
    } else if (MODEL_CHANNEL(object->model)->param != 0) {
        MapObject_StepAnim(object);
    } else {
        MapObject_StartAnim(object, 4);
        MAP_OBJECT_BYTES(object)->step = 1;
        object->work[0] = MAP_OBJECT_WORDS(object)->unk44;
    }
}

/* Turns a pressure plate on (step 1 to 2) or off (3 to 4) from outside, with no wait. */
void func_00121D20(MapObject *object, int turn_on) {
    s32 *wait = object->work;
    int flags = object->unk10;

    if ((flags & 0x2FF00) == 0x100) {
        if (turn_on != 0) {
            object->work[0] = 0;
            object->unk10 = flags | 0x20000;
            MAP_OBJECT_BYTES(object)->step = 2;
            return;
        }
    }
    if ((flags & 0x2FF00) == 0x20300 && turn_on == 0) {
        *wait = 0;
        object->unk10 &= ~0x20000;
        MAP_OBJECT_BYTES(object)->step = 4;
    }
}
