#include "kf_collision.h"

void func_00216778(void *, void *);
void func_0020E918(f32 *, f32 *, f32 *, f32 *, f32 *, f32);

/* Tests a ray against the four walls of a box-like map object (kind 1), two triangles a wall.
   Model points 0 to 3 are the top corners; the bottom ones lie below them by the height that
   point 0x1F gives. Stops at the first triangle hit. */
void func_001C2F30(void *target, int from, int direction, float distance) {
    MapObject *object = target;
    Vector4 bottom[4];
    f32 *corner0;
    f32 *corner1;
    f32 *corner2;
    f32 *corner3;
    f32 *top;
    f32 height;

    if (!(object->unk10 & 0x10000)) {
        return;
    }
    corner0 = (f32 *)object->model->get_point(object->model, 0);
    top = (f32 *)object->model->get_point(object->model, 0x1F);
    height = corner0[1] - top[1];
    corner1 = (f32 *)object->model->get_point(object->model, 1);
    corner2 = (f32 *)object->model->get_point(object->model, 2);
    corner3 = (f32 *)object->model->get_point(object->model, 3);
    func_00216778(&bottom[0], corner0);
    bottom[0].y -= height;
    func_0020E918((int)corner0, (int)corner1, (int)&bottom[0], from, direction, distance);
    if (COLLISION_RESULT->hit != 0) {
        return;
    }
    func_00216778(&bottom[1], corner1);
    bottom[1].y -= height;
    func_0020E918((int)corner1, (int)&bottom[0], (int)&bottom[1], from, direction, distance);
    if (COLLISION_RESULT->hit != 0) {
        return;
    }
    func_00216778(&bottom[2], corner2);
    bottom[2].y -= height;
    func_0020E918((int)corner2, (int)corner3, (int)&bottom[2], from, direction, distance);
    if (COLLISION_RESULT->hit != 0) {
        return;
    }
    func_00216778(&bottom[3], corner3);
    bottom[3].y -= height;
    func_0020E918((int)corner3, (int)&bottom[2], (int)&bottom[3], from, direction, distance);
    if (COLLISION_RESULT->hit != 0) {
        return;
    }
    func_0020E918((int)corner1, (int)corner2, (int)&bottom[1], from, direction, distance);
    if (COLLISION_RESULT->hit != 0) {
        return;
    }
    func_0020E918((int)corner2, (int)&bottom[1], (int)&bottom[2], from, direction, distance);
    if (COLLISION_RESULT->hit != 0) {
        return;
    }
    func_0020E918((int)corner3, (int)corner0, (int)&bottom[3], from, direction, distance);
    if (COLLISION_RESULT->hit != 0) {
        return;
    }
    func_0020E918((int)corner0, (int)&bottom[3], (int)&bottom[0], from, direction, distance);
}
