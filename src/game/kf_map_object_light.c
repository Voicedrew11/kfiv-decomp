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

    colour.x = source->unk44;
    colour.y = source->unk46;
    colour.z = source->unk48;
    colour.w = 1.0f;
    point = (void *)object->model->get_point(object->model, source->unk52);
    params = MAP_OBJECT_WORDS(object);
    work[0] = LinkedPoint_Allocate(params->unk4A, point, &colour, 0, MAP_OBJECT_BYTES(object)->unk00 & 1, params->unk4C != 0 ? params->unk4E : -1, 1);
    sort = MAP_OBJECT_WORDS(object)->unk50;
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

    switch (source->unk50) {
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
    colour.x = level * source->unk44;
    colour.y = level * source->unk46;
    colour.z = level * source->unk48;
    colour.w = 1.0f;
    LinkedPoint_Retarget(source->unk4A, work[0], &colour);
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
   timed status its source names (unk52). */
void MapObject_StartTouchTrap(MapObject *object) {
    MAP_OBJECT_BYTES(object)->step = 1;
}

/* Does nothing: step 1 of kind 10. */
void func_00120D88(void) {
}
