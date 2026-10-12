#include "kf_collision.h"

float sqrtf(float);
float sinf(float);
float cosf(float);
float func_0019B838(float);
void func_001C5538(MapObject *, Vector4 *, f32, f32, f32, f32, f32, f32);
void func_001C58F0(MapObject *, Vector4 *, f32, f32, f32, f32, f32, f32);
void func_001C6360(MapObject *, Vector4 *, f32, f32, f32, f32, f32, f32);
void func_001C65E8(void);
void func_001C5280(MapObject *, Vector4 *, f32, f32, f32, f32, f32, f32);

typedef void (*MapObjectFanTest)(MapObject *, Vector4 *, f32, f32, f32, f32, f32, f32);

/* Tests a fan (a sector of a circle around `origin`, `spread` to each side of `heading`)
   against a map object: first the object's own reach around its model, then the test that fits
   the object's kind, which is given the fan's two edge points. The result is left in
   COLLISION_RESULT. */
void func_001C24C0(MapObject *object, Vector4 *origin, f32 radius, f32 push, f32 spread, f32 heading) {
    f32 first_angle;
    f32 second_angle;
    f32 first_x;
    f32 first_z;
    f32 second_x;
    f32 second_z;
    f32 delta_x;
    f32 delta_z;
    f32 distance;

    COLLISION_RESULT->hit = 0;
    first_angle = func_0019B838(-heading + spread);
    second_angle = func_0019B838(-heading - spread);
    first_x = -sinf(first_angle);
    first_z = cosf(first_angle);
    second_x = -sinf(second_angle);
    second_z = cosf(second_angle);
    first_x += origin->x;
    first_z += origin->z;
    second_x += origin->x;
    second_z += origin->z;
    delta_x = ABS(object->model->position.x - origin->x);
    if (delta_x > object->unk14 + push) {
        return;
    }
    delta_z = ABS(object->model->position.z - origin->z);
    if (delta_z > object->unk14 + push) {
        return;
    }
    distance = sqrtf(delta_x * delta_x + delta_z * delta_z);
    if (distance > object->unk14 + push) {
        return;
    }
    switch (object->unk04->kind) {
    case 1:
        func_001C5538(object, origin, radius, push, first_x, first_z, second_x, second_z);
        break;
    case 2:
        func_001C58F0(object, origin, radius, push, first_x, first_z, second_x, second_z);
        break;
    case 17:
        func_001C6360(object, origin, radius, push, first_x, first_z, second_x, second_z);
        break;
    case 18:
        // match: the pitfall test is an empty function, but it is passed everything the others are
        ((MapObjectFanTest)func_001C65E8)(object, origin, radius, push, first_x, first_z, second_x, second_z);
        break;
    default:
        if (object->model->strips != NULL) {
            func_001C5280(object, origin, radius, push, first_x, first_z, second_x, second_z);
        }
        break;
    }
}
