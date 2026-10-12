#include "kf_collision.h"

/* moves the centre out of what was hit, to the given distance from it */
#define PUSH_OUT(distance) \
    centre[0] = COLLISION_RESULT->point[0] + COLLISION_RESULT->normal[0] * (distance); \
    centre[2] = COLLISION_RESULT->point[2] + COLLISION_RESULT->normal[2] * (distance)

void func_00210088(void *, void *, f32, void *, f32, f32);
void func_0020F0D8(void *, f32, f32, void *, f32, f32);

/* Pushes a circle out of the four walls of a map object's outline (points 0-3), or else out of
   the first corner post it touches. */
void func_001C68B0(MapObject *object, f32 *centre, f32 radius, f32 push) {
    f32 *point0, *point1, *point2, *point3;
    f32 *top;
    f32 height;

    if (object->unk10 & 0x10000) {
        point0 = (f32 *)object->model->get_point(object->model, 0);
        if (point0 == NULL) {
            return;
        }
        point1 = (f32 *)object->model->get_point(object->model, 1);
        if (point1 == NULL) {
            return;
        }
        point2 = (f32 *)object->model->get_point(object->model, 2);
        if (point2 == NULL) {
            return;
        }
        point3 = (f32 *)object->model->get_point(object->model, 3);
        if (point3 == NULL) {
            return;
        }
        top = (f32 *)object->model->get_point(object->model, 0x1F);
        if (top == NULL) {
            return;
        }
        height = point0[1] - top[1];
        func_00210088(point0, point1, height, centre, radius, push);
        if (COLLISION_RESULT->hit != 0) {
            PUSH_OUT(push);
            return;
        }
        func_00210088(point1, point2, height, centre, radius, push);
        if (COLLISION_RESULT->hit != 0) {
            PUSH_OUT(push);
            return;
        }
        func_00210088(point2, point3, height, centre, radius, push);
        if (COLLISION_RESULT->hit != 0) {
            PUSH_OUT(push);
            return;
        }
        func_00210088(point3, point0, height, centre, radius, push);
        if (COLLISION_RESULT->hit != 0) {
            PUSH_OUT(push);
            return;
        }
        func_0020F0D8(centre, push, radius, point0, 0.0f, height);
        if (COLLISION_RESULT->hit != 0) {
            PUSH_OUT(push + 0.1f);
            return;
        }
        func_0020F0D8(centre, push, radius, point1, 0.0f, height);
        if (COLLISION_RESULT->hit != 0) {
            PUSH_OUT(push + 0.1f);
            return;
        }
        func_0020F0D8(centre, push, radius, point2, 0.0f, height);
        if (COLLISION_RESULT->hit != 0) {
            PUSH_OUT(push + 0.1f);
            return;
        }
        func_0020F0D8(centre, push, radius, point3, 0.0f, height);
        if (COLLISION_RESULT->hit != 0) {
            PUSH_OUT(push + 0.1f);
        }
    }
}
