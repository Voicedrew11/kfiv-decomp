#include "common.h"
#include "kf_model.h"

#define SCRATCH_FLAG (*(s32 *)0x70000010)

s32 Collision_RayTriangle(s32, s32, s32, s32, s32, f32); // match: retail returns what the second test leaves in $v0; the definition returns nothing

/* Tests a ray against the quad of the object's model points 0 to 3 as two triangles, if the object has flag
   0x10000; the second triangle only when the first is missed. */
s32 func_001C3910(AnimObject *object, s32 param1, s32 param2, f32 param3) {
    s32 point0;
    s32 point1;
    s32 point2;
    s32 point3;
    s32 result;

    result = ANIM_OBJECT_FLAGS(object) & 0x10000;
    if (result) {
        point0 = object->model->get_point(object->model, 0);
        point1 = object->model->get_point(object->model, 1);
        point2 = object->model->get_point(object->model, 2);
        point3 = object->model->get_point(object->model, 3);
        Collision_RayTriangle(point0, point1, point2, param1, param2, param3);
        result = SCRATCH_FLAG;
        if (result == 0) {
            result = Collision_RayTriangle(point0, point2, point3, param1, param2, param3);
        }
    }
    return result;
}
