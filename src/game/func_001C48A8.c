#include "kf_collision.h"

void func_00210088(void *, void *, f32, void *, f32, f32);
void func_0020F0D8(void *, f32, f32, void *, f32, f32);

// match: the last test is called for its value, which keeps it an ordinary call
typedef int (*CornerTest)(void *, f32, f32, void *, f32, f32);

/* Tests a moving point against a box-like map object (kind 1): its four walls (model points
   0 to 3 are the corners, point 0x1F gives the height), then its four corner edges. Stops at
   the first hit. */
int func_001C48A8(MapObject *object, f32 *point, f32 radius, f32 push) {
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
    func_00210088(corner0, corner1, height, point, radius, push);
    hit = COLLISION_RESULT->hit;
    if (hit != 0) {
        return hit;
    }
    func_00210088(corner1, corner2, height, point, radius, push);
    hit = COLLISION_RESULT->hit;
    if (hit != 0) {
        return hit;
    }
    func_00210088(corner2, corner3, height, point, radius, push);
    hit = COLLISION_RESULT->hit;
    if (hit != 0) {
        return hit;
    }
    func_00210088(corner3, corner0, height, point, radius, push);
    hit = COLLISION_RESULT->hit;
    if (hit != 0) {
        return hit;
    }
    func_0020F0D8(point, push, radius, corner0, 0.0f, height);
    hit = COLLISION_RESULT->hit;
    if (hit != 0) {
        return hit;
    }
    func_0020F0D8(point, push, radius, corner1, 0.0f, height);
    hit = COLLISION_RESULT->hit;
    if (hit != 0) {
        return hit;
    }
    func_0020F0D8(point, push, radius, corner2, 0.0f, height);
    hit = COLLISION_RESULT->hit;
    if (hit != 0) {
        return hit;
    }
    return ((CornerTest)func_0020F0D8)(point, push, radius, corner3, 0.0f, height);
}
