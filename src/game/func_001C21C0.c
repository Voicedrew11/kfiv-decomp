#include "kf_collision.h"

int func_001C3DF8(MapObject *, f32 *, f32);
int func_001C3FA0(MapObject *, f32 *, f32);
void func_001C43A8(MapObject *, f32 *, f32); // match: the definition names a struct of its own file for the same object
void func_001C4498(MapObject *, f32 *, f32);
void func_001C3D00(CollisionOwner *, int, float);

/* Tests a point (with a reach) against a map object: first the object's own reach around its
   model, then the test that fits the object's kind. The result is left in COLLISION_RESULT. */
void func_001C21C0(MapObject *object, f32 *point, f32 reach) {
    ModelInstance *model = object->model;
    f32 delta_x;
    f32 delta_z;
    f32 object_reach;

    COLLISION_RESULT->unk18 = 6;
    COLLISION_RESULT->hit = 0;
    delta_x = ABS(model->position.x - point[0]);
    object_reach = object->unk14;
    if (delta_x > object_reach) {
        return;
    }
    delta_z = ABS(model->position.z - point[2]);
    if (delta_z > object_reach) {
        return;
    }
    if (delta_x * delta_x + delta_z * delta_z > object_reach * object_reach) {
        return;
    }
    switch (object->unk04->kind) {
    case 1:
        func_001C3DF8(object, point, reach);
        break;
    case 2:
        func_001C3FA0(object, point, reach);
        break;
    case 17:
        func_001C43A8(object, point, reach);
        break;
    case 18:
        func_001C4498(object, point, reach);
        break;
    default:
        if (model->strips != NULL) {
            func_001C3D00((CollisionOwner *)object, (int)point, reach);
        }
        break;
    }
}
