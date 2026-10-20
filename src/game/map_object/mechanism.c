#include "map/map.h"
#include "player/magic_ladder.h"
#include "player/player.h"
#include "entity/entity.h"
#include "effect/shot.h"
#include "model/model.h"
#include "map_object/map_object.h"

void Vector_Subtract(void *, void *, void *);
float atan2f(float, float);
float Angle_Wrap(float);
float sqrtf(float);
int Attack_LaunchMagicEffect(u16, void *, void *, void *, void *);
extern void *object_under_player[];
extern u32 game_status_flags[];
extern PlayerState player_state;
extern u32 frame_counter;

/* Step 0 of kind 0xE, a platform that sinks under the player: at rest, with its state cleared; step 1. */
void MapObject_StartSinkingPlatform(MapTrigger *trigger) {
    MapTriggerShotKind121660 *shot = (MapTriggerShotKind121660 *)&trigger->shot;

    shot->speed = 0.0f;
    shot->player_on = 0;
    shot->player_was_on = 0;
    trigger->type = 1;
}

/* Step 1 of kind 0xE: while the player stands on it (after a wait of unk4E frames) it sinks, at most unk44
   below where it started (y grows downward), and when he does not it rises back; unk46 and unk48 are the
   speeds down and up, or accelerations when unk4A and unk4C are set. */
void MapObject_UpdateSinkingPlatform(MapObject *object) {
    MapTriggerShotKind121660 *shot = (MapTriggerShotKind121660 *)object->work;
    MapObjectUnk08Words *source;
    ModelInstance *model;
    f32 top;

    shot->player_was_on = shot->player_on;
    shot->player_on = object_under_player[0] == object;
    if (shot->player_on != shot->player_was_on) {
        shot->frames_to_wait = MAP_OBJECT_WORDS(object)->kind_param_5;
    }
    switch (shot->player_on) {
        case 0:
            source = MAP_OBJECT_WORDS(object);
            if (source->kind_param_4 == 0) {
                ((MapTriggerShotKind121660 *)object->work)->speed = source->kind_param_2;
            } else {
                ((MapTriggerShotKind121660 *)object->work)->speed -= source->kind_param_2;
            }
            model = object->model;
            model->position.y -= shot->speed;
            if (model->position.y < object->source->position.y) {
                model->position.y = object->source->position.y;
            }
            break;
        case 1:
            if (shot->frames_to_wait > 0) {
                shot->frames_to_wait--;
                return;
            }
            source = MAP_OBJECT_WORDS(object);
            if (source->kind_param_3 == 0) {
                ((MapTriggerShotKind121660 *)object->work)->speed = source->kind_param_1;
            } else {
                ((MapTriggerShotKind121660 *)object->work)->speed += source->kind_param_1;
            }
            model = object->model;
            model->position.y += shot->speed;
            top = object->source->position.y + source->kind_param_0;
            if (top < model->position.y) {
                model->position.y = top;
            }
            break;
    }
}

/* Start handler of the ladder kind: step 1. */
void MapObject_StartLadder(MapObject *object) {
    MAP_OBJECT_BYTES(object)->step = 1;
}

/* Update handler: when the player (using it, status bit 36 clear) is within
   75 in height, within the radius and inside the angle of one of its two
   points, Ladder_Start is called for that point. */
void MapObject_UpdateLadder(MapObject *object) {
    Vector4 delta;
    int i;
    f32 turn;
    f32 half_angle;

    if (!GAME_STATUS_FLAG_BITS(game_status_flags)->using_faced_object) {
        return;
    }
    if ((s32)(player_state.status_bits >> 36) & 1) {
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
        half_angle = half_angle * DEG_TO_RAD(1) * 0.5f;
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

/* Step 0 of kind 0x10, which shoots at the player: step 1. */
void MapObject_StartShooter(MapObject *object) {
    MAP_OBJECT_BYTES(object)->step = 1;
}

/* Update handler of the shooting kind: every so many frames (or all the time
   when its model is animated), while it is on and the player is in range, it
   launches something from a point of the model, turned by two angles of its source. */
void MapObject_UpdateShooter(MapObject *object) {
    Vector4 aim;
    f32 distance;
    void *point;
    EntityUnk1B1390 *shot;

    if (!(object->model->model_flags & 1)) {
        if (frame_counter % MAP_OBJECT_WORDS(object)->kind_param_1 != 0) {
            return;
        }
    }
    if (!(object->state_flags & MAP_OBJECT_ON)) {
        return;
    }
    Vector_Subtract(&aim, &object->model->position, &player_state);
    distance = sqrtf(aim.x * aim.x + aim.y * aim.y + aim.z * aim.z);
    if (MAP_OBJECT_WORDS(object)->kind_param_5 < distance) {
        return;
    }
    point = (void *)object->model->get_point(object->model, MAP_OBJECT_WORDS(object)->kind_param_2);
    aim.x = Angle_Wrap(object->model->rotation.x + DEG_TO_RAD(MAP_OBJECT_WORDS(object)->kind_param_3));
    aim.y = Angle_Wrap(object->model->rotation.y + DEG_TO_RAD(MAP_OBJECT_WORDS(object)->kind_param_4) + PI);
    aim.z = object->model->rotation.z;
    aim.w = 0.0f;
    shot = Shot_Alloc();
    shot->player_power = 0;
    shot->is_magic_shot = 1;
    shot->player_magic = 0;
    // match: the id is passed as 16 bits
    ((void (*)(u16, void *, void *, void *, void *))Attack_LaunchMagicEffect)(MAP_OBJECT_WORDS(object)->kind_param_0, shot, point, &aim, object);
}

/* Signal hook of kind 0x10: turns its shooting on or off (bit 17). */
void MapObject_SetShooterOn(MapObject *object, int enabled) {
    MAP_OBJECT_BITS(object)->bit17 = enabled != 0;
}

/* Step 0 of the pitfalls (kind 0x12, the name the game's own debug print gives in
   Collision_HitSpherePitfallObj): closed (animation 4, step 1) or open (animation 5, step 3), with the wait
   that goes with it. */
void MapObject_StartPitfall(MapObject *object) {
    if (!(object->state_flags & MAP_OBJECT_ON)) {
        MapObject_StartAnim(object, 4);
        MAP_OBJECT_BYTES(object)->step = 1;
        object->work[0] = MAP_OBJECT_WORDS(object)->kind_param_0;
    } else {
        MapObject_StartAnim(object, 5);
        MAP_OBJECT_BYTES(object)->step = 3;
        object->work[0] = MAP_OBJECT_WORDS(object)->kind_param_1;
    }
}

/* Step 1 of a pitfall: stepped on by the player, it opens (step 2) unless its first wait (unk44) is negative. */
void MapObject_UpdateClosedPitfall(MapObject *object) {
    MapObject *stood_on = object_under_player[0];

    if (stood_on == object && MAP_OBJECT_WORDS(stood_on)->kind_param_0 >= 0) {
        stood_on->state_flags |= MAP_OBJECT_ON;
        MAP_OBJECT_BYTES(stood_on)->step = 2;
    }
}

/* Step 2 of a pitfall: waits out its delay (counted down only while unk44 is positive), then runs the opening
   animation; at its end it is open (animation 5, step 3) with the delay reloaded from unk46. */
void MapObject_UpdateOpeningPitfall(MapObject *object) {
    int delay = object->work[0];
    char reserved[0x10]; // match: unused, but retail reserves the 0x10 bytes

    if (delay != 0) {
        if (MAP_OBJECT_WORDS(object)->kind_param_0 > 0) {
            object->work[0] = delay - 1;
        }
    } else {
        if (MODEL_CHANNEL(object->model)->frame != 0) {
            MapObject_StepAnim(object);
        } else {
            MapObject_StartAnim(object, 5);
            MAP_OBJECT_BYTES(object)->step = 3;
            object->work[0] = MAP_OBJECT_WORDS(object)->kind_param_1;
        }
    }
}

/* Step 3 of a pitfall: closes again (step 4) unless its second wait (unk46) is negative. */
void MapObject_UpdateOpenPitfall(MapObject *object) {
    if (MAP_OBJECT_WORDS(object)->kind_param_1 >= 0) {
        object->state_flags &= ~MAP_OBJECT_ON;
        MAP_OBJECT_BYTES(object)->step = 4;
    }
}

/* Step 4 of a pitfall: waits, then runs the closing animation; at its end it is closed again (animation 4,
   step 1). */
void MapObject_UpdateClosingPitfall(MapObject *object) {
    if (object->work[0] != 0) {
        if (MAP_OBJECT_WORDS(object)->kind_param_1 > 0) {
            object->work[0]--;
        }
    } else if (MODEL_CHANNEL(object->model)->frame != 0) {
        MapObject_StepAnim(object);
    } else {
        MapObject_StartAnim(object, 4);
        MAP_OBJECT_BYTES(object)->step = 1;
        object->work[0] = MAP_OBJECT_WORDS(object)->kind_param_0;
    }
}

/* Signal hook of the pitfalls: opens a closed one (step 1 to 2) or closes an open one (3 to 4) at once, with
   no wait. */
void MapObject_SetPitfallOpen(MapObject *object, int turn_on) {
    s32 *wait = object->work;
    int flags = object->state_flags;

    if ((flags & MAP_OBJECT_STEP_AND_ON_MASK) == 0x100) {
        if (turn_on != 0) {
            object->work[0] = 0;
            object->state_flags = flags | MAP_OBJECT_ON;
            MAP_OBJECT_BYTES(object)->step = 2;
            return;
        }
    }
    if ((flags & MAP_OBJECT_STEP_AND_ON_MASK) == 0x20300 && turn_on == 0) {
        *wait = 0;
        object->state_flags &= ~MAP_OBJECT_ON;
        MAP_OBJECT_BYTES(object)->step = 4;
    }
}
