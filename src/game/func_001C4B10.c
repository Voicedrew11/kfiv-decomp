#include "kf_collision.h"

void func_00210088(void *, void *, f32, void *, f32, f32);
void func_0020F0D8(void *, f32, f32, void *, f32, f32);

/* tests the circle against the wall from one point to the next; the function ends on a hit */
#define TEST_WALL(from, to)     func_00210088(from, to, height, centre, radius, push);     if (COLLISION_RESULT->hit != 0) {         return;     }

/* the same against the post standing at a point */
#define TEST_POST(point)     func_0020F0D8(centre, push, radius, point, 0.0f, height);     if (COLLISION_RESULT->hit != 0) {         return;     }

/* Tests a circle against the walls and corner posts of a map object's outline and stops at the
   first hit: points 0, 1, 6, 7 while its fifth animation is on frame 0 or 1, otherwise the two
   quads 0-3 and 4-7. The result is left in the scratchpad. */
void func_001C4B10(MapObject *object, f32 *centre, f32 radius, f32 push) {
    f32 *point0, *point1, *point2, *point3, *point4, *point5, *point6, *point7;
    f32 *top;
    f32 height;

    if (object->unk10 & 0x10000) {
        if (MODEL_CHANNEL(object->model)->index == object->unk04->anims[4].anim && MODEL_CHANNEL(object->model)->param < 2) {
            point0 = (f32 *)object->model->get_point(object->model, 0);
            if (point0 == NULL) {
                return;
            }
            point1 = (f32 *)object->model->get_point(object->model, 1);
            if (point1 == NULL) {
                return;
            }
            point2 = (f32 *)object->model->get_point(object->model, 6);
            if (point2 == NULL) {
                return;
            }
            point3 = (f32 *)object->model->get_point(object->model, 7);
            if (point3 == NULL) {
                return;
            }
            top = (f32 *)object->model->get_point(object->model, 0x1F);
            if (top == NULL) {
                return;
            }
            height = point0[1] - top[1];
            TEST_WALL(point0, point1);
            TEST_WALL(point1, point2);
            TEST_WALL(point2, point3);
            TEST_WALL(point3, point0);
            TEST_POST(point0);
            TEST_POST(point1);
            TEST_POST(point2);
            func_0020F0D8(centre, push, radius, point3, 0.0f, height);
        } else {
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
            point4 = (f32 *)object->model->get_point(object->model, 4);
            if (point4 == NULL) {
                return;
            }
            point5 = (f32 *)object->model->get_point(object->model, 5);
            if (point5 == NULL) {
                return;
            }
            point6 = (f32 *)object->model->get_point(object->model, 6);
            if (point6 == NULL) {
                return;
            }
            point7 = (f32 *)object->model->get_point(object->model, 7);
            if (point7 == NULL) {
                return;
            }
            top = (f32 *)object->model->get_point(object->model, 0x1F);
            if (top == NULL) {
                return;
            }
            height = point0[1] - top[1];
            TEST_WALL(point0, point1);
            TEST_WALL(point1, point2);
            TEST_WALL(point2, point3);
            TEST_WALL(point3, point0);
            TEST_WALL(point4, point5);
            TEST_WALL(point5, point6);
            TEST_WALL(point6, point7);
            TEST_WALL(point7, point4);
            TEST_POST(point0);
            TEST_POST(point1);
            TEST_POST(point2);
            TEST_POST(point3);
            TEST_POST(point4);
            TEST_POST(point5);
            TEST_POST(point6);
            func_0020F0D8(centre, push, radius, point7, 0.0f, height);
        }
    }
}
