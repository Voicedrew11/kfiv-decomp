#include "kf_collision.h"
#include "kf_player.h"

extern int D_00446770;
extern s32 D_004467A8[2];
extern s32 D_004467B8[2];
extern MapObject D_003A1A30[];
extern MapObject D_003AFA30[];
extern PlayerState D_00413F80;
extern f32 D_004469A8[]; // match: the largest float; declared without a size, it is not reached through $gp

float sqrtf(float);
s32 func_001C7C60(MapObject *, f32 *, f32);
s32 func_001C7E48(MapObject *, f32 *, f32);
s32 func_001C7AF8(MapObject *, f32 *, f32);

/* the objects of the current map (map 0) or the other loaded one (map 1), which the game inlines */
static inline MapObject *map_objects(s32 map) {
    if (D_004467A8[D_00446770 == map ? 0 : 1] != 0) {
        if (D_00446770 != map) {
            return D_003AFA30;
        }
        return D_003A1A30;
    }
    return NULL;
}

static inline s32 map_object_count(s32 map) {
    if (D_004467A8[D_00446770 == map ? 0 : 1] != 0) {
        return D_004467B8[D_00446770 == map ? 0 : 1];
    }
    return 0;
}

/* what each kind's test does when it finds something: the answer is TRUE, and the player state
   notes the object when the point tested is the player's position */
#define NOTE_FOUND(object)     result = TRUE;     if (centre == (f32 *)&D_00413F80) {         ((PlayerState *)centre)->unk208 = (s32)(object);     }

/* Tests a point against every map object of both loaded maps that is near enough and that can
   be stood on (kinds 1 and 2 are left out); TRUE if any test says so. When the point is the
   player's position the object is noted in the player state. `measure` is as in func_001C2758. */
s32 func_001C2A38(f32 *centre, s32 measure, f32 radius, f32 push) {
    MapObject *object;
    f32 reach;
    f32 delta_x;
    f32 delta_z;
    f32 distance;
    s32 map;
    s32 count;
    s32 i;
    s32 result = FALSE;
    f32 farthest = D_004469A8[0];

    COLLISION_RESULT->hit = 0;
    for (map = 0; map < 2; map++) {
        object = map_objects(map);
        count = map_object_count(map);
        for (i = 0; i < count; i++, object++) {
            if (object->unk14 <= 0.0f) {
                continue;
            }
            reach = object->unk14 + push;
            if (measure) {
                object->unk18 = farthest;
                delta_x = ABS(object->model->position.x - centre[0]);
                if (delta_x > reach) {
                    continue;
                }
                delta_z = ABS(object->model->position.z - centre[2]);
                if (delta_z > reach) {
                    continue;
                }
                distance = sqrtf(delta_x * delta_x + delta_z * delta_z);
                if (distance > reach) {
                    continue;
                }
                object->unk18 = distance;
            } else if (object->unk18 > reach) {
                continue;
            }
            switch (object->unk04->kind) {
            case 1:
            case 2:
                break;
            case 17:
                if (func_001C7C60(object, centre, radius)) {
                    NOTE_FOUND(object);
                }
                break;
            case 18:
                if (func_001C7E48(object, centre, radius)) {
                    NOTE_FOUND(object);
                }
                break;
            default:
                if (object->model->strips != NULL && func_001C7AF8(object, centre, radius)) {
                    NOTE_FOUND(object);
                }
                break;
            }
        }
    }
    return result;
}
