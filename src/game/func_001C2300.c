#include "kf_collision.h"

float sqrtf(float);
int func_001C48A8(MapObject *, f32 *, f32, f32);
void func_001C4B10(MapObject *, f32 *, f32, f32);
int func_001C5170(CollisionOwner *, int, float, float);
void func_001C5278(void);

typedef void (*MapObjectSweepTest)(MapObject *, f32 *, f32, f32);
void func_001C47A0(void *, int, float, float);

/* Tests a point (with two reaches) against a map object: first the object's own reach around
   its model, then the test that fits the object's kind. The result is left in COLLISION_RESULT. */
void func_001C2300(MapObject *object, f32 *point, f32 reach, f32 height) {
    f32 delta_x;
    f32 delta_z;

    COLLISION_RESULT->hit = 0;
    delta_x = ABS(object->model->position.x - point[0]);
    if (delta_x > object->unk14) {
        return;
    }
    delta_z = ABS(object->model->position.z - point[2]);
    if (delta_z > object->unk14) {
        return;
    }
    if (sqrtf(delta_x * delta_x + delta_z * delta_z) > object->unk14) {
        return;
    }
    switch (object->unk04->kind) {
    case 1:
        func_001C48A8(object, point, reach, height);
        break;
    case 2:
        func_001C4B10(object, point, reach, height);
        break;
    case 17:
        func_001C5170((CollisionOwner *)object, (int)point, reach, height);
        break;
    case 18:
        // match: the pitfall test is an empty function, but it is passed everything the others are
        ((MapObjectSweepTest)func_001C5278)(object, point, reach, height);
        break;
    default:
        if (object->model->strips != NULL) {
            func_001C47A0(object, (int)point, reach, height);
        }
        break;
    }
}
