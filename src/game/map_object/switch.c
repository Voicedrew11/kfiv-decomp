#include "map_object/map_object.h"
#include "model/model.h"
#include "core/game.h"

/* MapObject.step of a toggle (kind 7) */
#define TOGGLE_STEP_IDLE            1       /* waiting for the player to use it */
#define TOGGLE_STEP_MOVING          2       /* its motion runs */
#define TOGGLE_MOTION_TURN_ON       0       /* ModelChannel.motion_index of the motion that turns it on */
/* MapObject.step of a switch (kind 8) */
#define SWITCH_STEP_OFF             1       /* off, waiting for the player */
#define SWITCH_STEP_TURNING_ON      2       /* the turning-on animation runs */
#define SWITCH_STEP_ON              3       /* on, waiting for the player */
#define SWITCH_STEP_TURNING_OFF     4       /* the turning-off animation runs */

/* Step 0 of kind 7 (no object of param/obj.prm has it): animation 4, step 1. */
void MapObject_StartToggle(MapObject *object) {
    MapObject_StartAnim(object, MAP_OBJECT_ANIM_OFF);
    MAP_OBJECT_BYTES(object)->step = TOGGLE_STEP_IDLE;
}

/* Step 1 of kind 7: used by the player, it flips its on bit (bit 17) and goes to step 2. */
void MapObject_UpdateIdleToggle(MapObject *object) {
    if (GAME_STATUS_FLAG_BITS(game_status_flags)->using_faced_object) {
        if (faced_object[0] == object) {
            MAP_OBJECT_BYTES(object)->step = TOGGLE_STEP_MOVING;
            object->state_flags = (object->state_flags & ~MAP_OBJECT_ON) | ((((s32)object->state_flags >> 17) ^ 1) & 1) << 17;
        }
    }
}

/* Step 2 of kind 7: runs the animation; while the model's channel is on motion 0 its end starts animation 5,
   on any other motion its end puts the object back at rest (animation 4, step 1). */
void MapObject_UpdateMovingToggle(MapObject *object) {
    ModelChannel *channel = MODEL_CHANNEL(object->model);

    if (channel->motion_index == TOGGLE_MOTION_TURN_ON) {
        if (channel->frame != 0) {
            MapObject_StepAnim(object);
        } else {
            MapObject_StartAnim(object, MAP_OBJECT_ANIM_ON);
        }
    } else {
        if (channel->frame == 0) {
            MapObject_StartAnim(object, MAP_OBJECT_ANIM_OFF);
            MAP_OBJECT_BYTES(object)->step = TOGGLE_STEP_IDLE;
        } else {
            MapObject_StepAnim(object);
        }
    }
}

/* Step 0 of the switches (kind 8): off (animation 4, step 1) or, with bit 17 set, on (animation 5, step 3). */
void MapObject_StartSwitch(MapObject *object) {
    if (!(object->state_flags & MAP_OBJECT_ON)) {
        MapObject_StartAnim(object, MAP_OBJECT_ANIM_OFF);
        MAP_OBJECT_BYTES(object)->step = SWITCH_STEP_OFF;
    } else {
        MapObject_StartAnim(object, MAP_OBJECT_ANIM_ON);
        MAP_OBJECT_BYTES(object)->step = SWITCH_STEP_ON;
    }
}

/* Update handler of a switch that is off: used by the player, it starts turning on. */
void MapObject_UpdateOffSwitch(MapObject *switch_object) {
    MapObject *object;

    if (GAME_STATUS_FLAG_BITS(game_status_flags)->using_faced_object && (object = faced_object[0]) == switch_object) {
        MAP_OBJECT_BYTES(object)->step = SWITCH_STEP_TURNING_ON;
        object->state_flags |= MAP_OBJECT_ON;
    }
}

/* Update handler of a switch turning on: runs the animation, then it is on. */
void MapObject_UpdateSwitchTurningOn(MapObject *object) {
    if (MODEL_CHANNEL(object->model)->frame != 0) {
        MapObject_StepAnim(object);
    } else {
        MapObject_StartAnim(object, MAP_OBJECT_ANIM_ON);
        MAP_OBJECT_BYTES(object)->step = SWITCH_STEP_ON;
    }
}

/* Update handler of a switch that is on: used by the player, it starts turning off. */
void MapObject_UpdateOnSwitch(MapObject *switch_object) {
    MapObject *object;

    if (GAME_STATUS_FLAG_BITS(game_status_flags)->using_faced_object && (object = faced_object[0]) == switch_object) {
        MAP_OBJECT_BYTES(object)->step = SWITCH_STEP_TURNING_OFF;
        object->state_flags &= ~MAP_OBJECT_ON;
    }
}

/* Update handler of a switch turning off: runs the animation, then it is off. */
void MapObject_UpdateSwitchTurningOff(MapObject *object) {
    if (MODEL_CHANNEL(object->model)->frame != 0) {
        MapObject_StepAnim(object);
    } else {
        MapObject_StartAnim(object, MAP_OBJECT_ANIM_OFF);
        MAP_OBJECT_BYTES(object)->step = SWITCH_STEP_OFF;
    }
}

/* Signal hook of the switches: turns one on (from step 1) or off (from step 3) by running its update as if
   the player had used it. */
void MapObject_SetSwitchOn(MapObject *object, int turn_on) {
    void *saved;
    int bit;

    if ((object->state_flags & MAP_OBJECT_STEP_AND_ON_MASK) == 0x100 && turn_on != 0) {
        saved = view_ray_status.current;
        bit = game_status.using_faced_object;
        game_status.using_faced_object = 1;
        view_ray_status.current = object;
        MapObject_UpdateOffSwitch(object);
        view_ray_status.current = saved;
        game_status.using_faced_object = bit;
    } else if ((object->state_flags & MAP_OBJECT_STEP_AND_ON_MASK) == 0x20300 && turn_on == 0) {
        saved = view_ray_status.current;
        bit = game_status.using_faced_object;
        game_status.using_faced_object = 1;
        view_ray_status.current = object;
        MapObject_UpdateOnSwitch(object);
        view_ray_status.current = saved;
        game_status.using_faced_object = bit;
    }
}
