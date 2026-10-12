#include "kf_collision.h"

void func_0020E1F8(f32 *, f32 *, f32 *, f32 *, f32, f32);

/* notes on which side of the lines from the origin to the two ends the point lies */
#define TEST_SIDES(point) \
    if ((first_x - origin->x) * ((point)[2] - origin->z) - (first_z - origin->z) * ((point)[0] - origin->x) > 0.0f) { \
        first_sides |= 1; \
    } \
    if ((second_x - origin->x) * ((point)[2] - origin->z) - (second_z - origin->z) * ((point)[0] - origin->x) < 0.0f) { \
        second_sides |= 1; \
    }

/* Tests the capsule at `origin` against the two triangles of a map object's face (points 1, 2,
   0, 3 as a strip), leaving out a triangle whose corners all lie outside the wedge from the
   origin to (first_x, first_z) and (second_x, second_z). Stops at the first hit. */
void func_001C6360(MapObject *object, Vector4 *origin, f32 radius, f32 push, f32 first_x, f32 first_z, f32 second_x,
                   f32 second_z) {
    f32 *points[4];
    s32 second_sides;
    s32 first_sides;
    s32 i;

    if (object->unk10 & 0x10000) {
        second_sides = 0;
        first_sides = 0;
        points[0] = (f32 *)object->model->get_point(object->model, 1);
        points[1] = (f32 *)object->model->get_point(object->model, 2);
        points[2] = (f32 *)object->model->get_point(object->model, 0);
        points[3] = (f32 *)object->model->get_point(object->model, 3);
        TEST_SIDES(points[0]);
        first_sides <<= 1;
        second_sides <<= 1;
        TEST_SIDES(points[1]);
        first_sides <<= 1;
        second_sides <<= 1;
        for (i = 2; i < 4; i++) {
            // match: the notes are not moved up a bit here (func_001C5280 does), so on the second triangle bit 0 is point 0 or point 3
            TEST_SIDES(points[i]);
            if (second_sides != 7 && first_sides != 7) {
                func_0020E1F8((int)points[i - 2], (int)points[i - 1], (int)points[i], (int)origin, push, radius);
                if (COLLISION_RESULT->hit != 0) {
                    return;
                }
            }
        }
    }
}
