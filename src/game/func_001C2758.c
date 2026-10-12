#include "kf_collision.h"

extern int D_00446770;
extern s32 D_004467A8[2];
extern s32 D_004467B8[2];
extern MapObject D_003A1A30[];
extern MapObject D_003AFA30[];
extern f32 D_004469A4[]; // match: the largest float; declared without a size, it is not reached through $gp

float sqrtf(float);
void func_001C68B0(MapObject *, f32 *, f32, f32);
void func_001C6B88(MapObject *, f32 *, f32, f32);
void func_001C7960(MapObject *, f32 *, f32, f32);
void func_001C7AF0(void);
void func_001C65F0(MapObject *, f32 *, f32, f32);

typedef void (*MapObjectCircleTest)(MapObject *, f32 *, f32, f32);

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

/* Collides a circle with every map object of both loaded maps that is near enough. With
   `measure` set the distance to each object is worked out and kept in the object (unk18);
   otherwise the distances kept by an earlier call are used. */
void func_001C2758(f32 *centre, s32 measure, f32 radius, f32 push) {
    MapObject *object;
    f32 reach;
    f32 delta_x;
    f32 delta_z;
    f32 distance;
    s32 map;
    s32 count;
    s32 i;

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
                object->unk18 = D_004469A4[0];
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
                func_001C68B0(object, centre, radius, push);
                break;
            case 2:
                func_001C6B88(object, centre, radius, push);
                break;
            case 17:
                func_001C7960(object, centre, radius, push);
                break;
            case 18:
                // match: the pitfall test is an empty function, but it is passed everything the others are
                ((MapObjectCircleTest)func_001C7AF0)(object, centre, radius, push);
                break;
            default:
                if (object->model->strips != NULL) {
                    func_001C65F0(object, centre, radius, push);
                }
                break;
            }
        }
    }
}
