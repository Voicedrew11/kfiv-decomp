#include "kf_model.h"
#include "kf_linked_point.h"

extern f32 sine_table[];

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
    work[0] = LinkedPoint_Allocate(params->kind_param_3, point, &colour, 0, MAP_OBJECT_BYTES(object)->unk00 & 1, params->kind_param_4 != 0 ? params->kind_param_5 : -1, 1);
    sort = MAP_OBJECT_WORDS(object)->kind_param_6;
    if (sort >= 0) {
        if (sort <= 1) {
            work[2] = (int)(rand() * 6.0f * 4.656613e-10f) + 0x14;
            work[1] = work[2] * (f32)rand() * 4.656613e-10f;
            work[4] = (int)(rand() * 6.0f * 4.656613e-10f) + 5;
            work[3] = work[4] * (f32)rand() * 4.656613e-10f;
        }
    }
    MAP_OBJECT_BYTES(object)->step = 1;
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
        case 0: {
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
        case 1: {
            f32 depth = 0.25f;
            f32 base = 0.75f;

            work[1] = (work[1] + 1) % work[2];
            work[3] = (work[3] + 1) % work[4];
            level = sine_table[work[1] * 0xA00 / work[2]] * sine_table[work[3] * 0xA00 / work[4]];
            depth *= level;
            level = depth + base;
            break;
        }
        case 2: // match: the steady sort is named, which decides the order of the tests
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
   timed status its source names (unk52: PlayerState.timed_flags[unk52 - 1], [0] the poison) and the red
   damage flash. */
void MapObject_StartStatusOnTouch(MapObject *object) {
    MAP_OBJECT_BYTES(object)->step = 1;
}

/* Does nothing: step 1 of kind 10. */
void func_00120D88(void) {
}
