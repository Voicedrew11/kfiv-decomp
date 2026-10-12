#include "kf_collision.h"

void func_00210018(void *, void *, f32, void *, f32);

// match: the last test is called for its value, which keeps it an ordinary call
typedef int (*WallTest)(void *, void *, f32, void *, f32);

/* Tests a point against the four walls of a box-like map object (kind 1): model points 0 to 3
   are its corners and point 0x1F gives its height. Stops at the first wall hit. */
int func_001C3DF8(MapObject *object, f32 *point, f32 reach) {
    f32 *corner0;
    f32 *corner1;
    f32 *corner2;
    f32 *corner3;
    f32 *top;
    f32 height;
    int hit;

    if (!(object->unk10 & 0x10000)) {
        return;
    }
    if (object->model->get_point(object->model, 0x1F) == 0) {
        return;
    }
    corner0 = (f32 *)object->model->get_point(object->model, 0);
    if (corner0 == NULL) {
        return;
    }
    corner1 = (f32 *)object->model->get_point(object->model, 1);
    if (corner1 == NULL) {
        return;
    }
    corner2 = (f32 *)object->model->get_point(object->model, 2);
    if (corner2 == NULL) {
        return;
    }
    corner3 = (f32 *)object->model->get_point(object->model, 3);
    if (corner3 == NULL) {
        return;
    }
    top = (f32 *)object->model->get_point(object->model, 0x1F);
    if (top == NULL) {
        return;
    }
    height = corner0[1] - top[1];
    func_00210018(corner0, corner1, height, point, reach);
    hit = COLLISION_RESULT->hit;
    if (hit != 0) {
        return hit;
    }
    func_00210018(corner1, corner2, height, point, reach);
    hit = COLLISION_RESULT->hit;
    if (hit != 0) {
        return hit;
    }
    func_00210018(corner2, corner3, height, point, reach);
    hit = COLLISION_RESULT->hit;
    if (hit != 0) {
        return hit;
    }
    return ((WallTest)func_00210018)(corner3, corner0, height, point, reach);
}
