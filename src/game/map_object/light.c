#include "model/model.h"
#include "model/linked_point.h"
#include "math/angle.h"

/* MapObjectUnk08.kind_param_6: the light's sort (LIGHT_SORT_* flicker or steady) */
#define LIGHT_SORT_FAINT_FLICKER    0
#define LIGHT_SORT_STRONG_FLICKER   1
#define LIGHT_SORT_STEADY           2

/* MapObject.step of a light (kind 9) */
#define LIGHT_STEP_ACTIVE           1       /* the update flickers it (sorts 0 and 1) */
/* MapObject.step of a status-on-touch object (kind 10) */
#define STATUS_ON_TOUCH_STEP_IDLE   1       /* waiting to be touched */

/* Start handler of the light kind: makes a light of the colour in its source at
   a point of the model, and for the flickering sorts (0, 1) rolls the two
   flicker periods and where in them it starts. */
void MapObject_StartLight(MapObject *object) {
    s16 *work = (s16 *)object->work;
    Vector4 colour;
    MapObjectUnk08Words *source = MAP_OBJECT_WORDS(object);
    void *point;
    int sort;
    MapObjectUnk08Words *params;

    colour.x = source->kind_param_0;
    colour.y = source->kind_param_1;
    colour.z = source->kind_param_2;
    colour.w = 1.0f;
    point = (void *)object->model->get_point(object->model, source->kind_param_7);
    params = MAP_OBJECT_WORDS(object);
    work[0] = LinkedPoint_Allocate(params->kind_param_3, point, &colour, 0, MAP_OBJECT_BYTES(object)->map_slot & 1, params->kind_param_4 != 0 ? params->kind_param_5 : -1, 1);
    sort = MAP_OBJECT_WORDS(object)->kind_param_6;
    if (sort >= 0) {
        if (sort <= 1) {
            work[2] = (int)(rand() * 6.0f * RAND_SCALE) + 0x14;
            work[1] = work[2] * (f32)rand() * RAND_SCALE;
            work[4] = (int)(rand() * 6.0f * RAND_SCALE) + 5;
            work[3] = work[4] * (f32)rand() * RAND_SCALE;
        }
    }
    MAP_OBJECT_BYTES(object)->step = LIGHT_STEP_ACTIVE;
}

/* Update handler of the light kind: the flickering sorts (0 faintly, 1 strongly)
   step their two periods and scale the colour by the product of the two waves
   read from the table sine_table (0xA00 entries a period). */
void MapObject_UpdateLight(MapObject *object) {
    s16 *work = (s16 *)object->work;
    MapObjectUnk08Words *source = MAP_OBJECT_WORDS(object);
    Vector4 colour;
    f32 level;

    switch (source->kind_param_6) {
        case LIGHT_SORT_FAINT_FLICKER: {
            // match: the depth and base are locals of each case
            f32 depth = 0.1f;
            f32 base = 0.9f;

            work[1] = (work[1] + 1) % work[2];
            work[3] = (work[3] + 1) % work[4];
            level = sine_table[work[1] * 0xA00 / work[2]] * sine_table[work[3] * 0xA00 / work[4]];
            depth *= level;
            level = depth + base;
            break;
        }
        case LIGHT_SORT_STRONG_FLICKER: {
            f32 depth = 0.25f;
            f32 base = 0.75f;

            work[1] = (work[1] + 1) % work[2];
            work[3] = (work[3] + 1) % work[4];
            level = sine_table[work[1] * 0xA00 / work[2]] * sine_table[work[3] * 0xA00 / work[4]];
            depth *= level;
            level = depth + base;
            break;
        }
        case LIGHT_SORT_STEADY: // match: the steady sort is named, which decides the order of the tests
        default:
            level = 1.0f;
            break;
    }
    colour.x = level * source->kind_param_0;
    colour.y = level * source->kind_param_1;
    colour.z = level * source->kind_param_2;
    colour.w = 1.0f;
    LinkedPoint_Retarget(source->kind_param_3, work[0], &colour);
}

/* End hook of the light kind (9): releases its light (the LinkedPoint handle in its work area), if it has
   one. */
void MapObject_ReleaseLight(MapObject *object) {
    s16 *handle = (s16 *)object->work;

    if (*handle >= 0) {
        LinkedPoint_ReleaseIndex(*handle);
        *handle = -1;
    }
}

/* Step 0 of kind 10: step 1, where nothing happens. Its effect comes from touching it:
   Collision_PushOutOfMeshObj notes the object the player touches and Player_CollideWithMapObjects starts the
   timed status its source names (touch_status_flag: PlayerState.timed_flags[touch_status_flag - 1], [0] the poison) and the red
   damage flash. */
void MapObject_StartStatusOnTouch(MapObject *object) {
    MAP_OBJECT_BYTES(object)->step = STATUS_ON_TOUCH_STEP_IDLE;
}

/* Does nothing: the step a kind 10 object stays in once started (its effect comes from touching it). */
void MapObject_IdleStatusOnTouch(MapObject *object) {
}
