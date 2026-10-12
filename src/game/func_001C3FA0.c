#include "kf_collision.h"

void func_00210018(void *, void *, f32, void *, f32);

// match: the last test is called for its value, which keeps it an ordinary call
typedef int (*WallTest)(void *, void *, f32, void *, f32);

/* Tests a point against the walls of a two-box map object (kind 2). Model points 0 to 3 and
   4 to 7 are the corners of its two halves and point 0x1F gives its height. While the object
   is at the start of animation 4 the halves are tested as one box (corners 0, 1, 6, 7).
   Stops at the first wall hit. */
int func_001C3FA0(MapObject *object, f32 *point, f32 reach) {
    f32 *corner0;
    f32 *corner1;
    f32 *corner2;
    f32 *corner3;
    f32 *corner4;
    f32 *corner5;
    f32 *corner6;
    f32 *corner7;
    f32 *top;
    f32 height;
    int hit;

    if (!(object->unk10 & 0x10000)) {
        return;
    }
    if (MODEL_CHANNEL(object->model)->index == object->unk04->anims[4].anim && MODEL_CHANNEL(object->model)->param < 2) {
        corner0 = (f32 *)object->model->get_point(object->model, 0);
        if (corner0 == NULL) {
            return;
        }
        corner1 = (f32 *)object->model->get_point(object->model, 1);
        if (corner1 == NULL) {
            return;
        }
        corner2 = (f32 *)object->model->get_point(object->model, 6);
        if (corner2 == NULL) {
            return;
        }
        corner3 = (f32 *)object->model->get_point(object->model, 7);
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
    corner4 = (f32 *)object->model->get_point(object->model, 4);
    if (corner4 == NULL) {
        return;
    }
    corner5 = (f32 *)object->model->get_point(object->model, 5);
    if (corner5 == NULL) {
        return;
    }
    corner6 = (f32 *)object->model->get_point(object->model, 6);
    if (corner6 == NULL) {
        return;
    }
    corner7 = (f32 *)object->model->get_point(object->model, 7);
    if (corner7 == NULL) {
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
    func_00210018(corner3, corner0, height, point, reach);
    hit = COLLISION_RESULT->hit;
    if (hit != 0) {
        return hit;
    }
    func_00210018(corner4, corner5, height, point, reach);
    hit = COLLISION_RESULT->hit;
    if (hit != 0) {
        return hit;
    }
    func_00210018(corner5, corner6, height, point, reach);
    hit = COLLISION_RESULT->hit;
    if (hit != 0) {
        return hit;
    }
    func_00210018(corner6, corner7, height, point, reach);
    hit = COLLISION_RESULT->hit;
    if (hit != 0) {
        return hit;
    }
    return ((WallTest)func_00210018)(corner7, corner4, height, point, reach);
}
