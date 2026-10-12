#include "kf_collision.h"

void func_00210258(Vector4 *, Vector4 *, f32, Vector4 *, f32, f32);

/* whether the point lies outside the line from the origin to the first end */
#define OUTSIDE_FIRST(point) \
    ((first_x - origin->x) * ((point)->z - origin->z) - (first_z - origin->z) * ((point)->x - origin->x) > 0.0f)

/* whether the point lies outside the line from the origin to the second end */
#define OUTSIDE_SECOND(point) \
    ((second_x - origin->x) * ((point)->z - origin->z) - (second_z - origin->z) * ((point)->x - origin->x) < 0.0f)

/* Tests the capsule at `origin` against the four walls of a map object's outline (points 0-3),
   leaving out a wall whose two ends both lie outside the wedge from the origin to
   (first_x, first_z) and (second_x, second_z). Stops at the first hit. */
void func_001C5538(MapObject *object, Vector4 *origin, f32 radius, f32 push, f32 first_x, f32 first_z, f32 second_x,
                   f32 second_z) {
    Vector4 *point0, *point1, *point2, *point3;
    Vector4 *top;
    f32 height;
    s32 first0, first1, first2, first3;
    s32 second0, second1, second2, second3;
    s32 hit;

    if (object->unk10 & 0x10000) {
        point0 = (Vector4 *)object->model->get_point(object->model, 0);
        point1 = (Vector4 *)object->model->get_point(object->model, 1);
        point2 = (Vector4 *)object->model->get_point(object->model, 2);
        point3 = (Vector4 *)object->model->get_point(object->model, 3);
        top = (Vector4 *)object->model->get_point(object->model, 0x1F);
        height = point0->y - top->y;
        first0 = OUTSIDE_FIRST(point0);
        second0 = OUTSIDE_SECOND(point0);
        first1 = OUTSIDE_FIRST(point1);
        second1 = OUTSIDE_SECOND(point1);
        first2 = OUTSIDE_FIRST(point2);
        second2 = OUTSIDE_SECOND(point2);
        first3 = OUTSIDE_FIRST(point3);
        second3 = OUTSIDE_SECOND(point3);
        if (!(first0 && first1) && !(second0 && second1)) {
            func_00210258(point0, point1, height, origin, radius, push);
        }
        // match: read even when the wall was left out, whatever the last test before this left there
        hit = COLLISION_RESULT->hit;
        if (hit != 0) {
            return;
        }
        if (!(first1 && first2) && !(second1 && second2)) {
            func_00210258(point1, point2, height, origin, radius, push);
            hit = COLLISION_RESULT->hit;
            if (hit != 0) {
                return;
            }
        } else if (hit != 0) {    // match: always false here; the repeated test (one copy in the binary) orders the saved registers
            return;
        }
        if (!(first2 && first3) && !(second2 && second3)) {
            func_00210258(point2, point3, height, origin, radius, push);
            hit = COLLISION_RESULT->hit;
            if (hit != 0) {
                return;
            }
        } else if (hit != 0) {    // match: as above
            return;
        }
        if (!(first3 && first0) && !(second3 && second0)) {
            func_00210258(point3, point0, height, origin, radius, push);
        }
    }
}
