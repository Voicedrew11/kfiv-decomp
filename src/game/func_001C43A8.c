#include "kf_collision.h"

void Collision_PointTriangle(f32 *, f32 *, f32 *, f32 *, f32);

/* Tests a point (with a reach) against the quad of the object's model points 0 to 3, as two triangles, if the
   object has flag 0x10000. */
void func_001C43A8(MapObject *object, f32 *point, f32 reach) {
    f32 *corner_0;
    f32 *corner_1;
    f32 *corner_2;
    f32 *corner_3;

    if (!(object->unk10 & 0x10000)) {
        return;
    }
    corner_0 = (f32 *)object->model->get_point(object->model, 0);
    corner_1 = (f32 *)object->model->get_point(object->model, 1);
    corner_2 = (f32 *)object->model->get_point(object->model, 2);
    corner_3 = (f32 *)object->model->get_point(object->model, 3);
    Collision_PointTriangle(corner_0, corner_1, corner_2, point, reach);
    if (COLLISION_RESULT->hit == 0) {
        Collision_PointTriangle(corner_0, corner_2, corner_3, point, reach);
    }
}
