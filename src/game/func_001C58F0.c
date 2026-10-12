#include "kf_collision.h"

/* whether a model point lies left of the line from the origin to (line_x, line_z), seen from above */
#define LEFT_OF(point, line_x, line_z) \
    (0.0f < ((line_x) - origin->x) * ((point)->z - origin->z) - ((line_z) - origin->z) * ((point)->x - origin->x))
/* whether it lies right of that line */
#define RIGHT_OF(point, line_x, line_z) \
    (((line_x) - origin->x) * ((point)->z - origin->z) - ((line_z) - origin->z) * ((point)->x - origin->x) < 0.0f)

void func_00210258(Vector4 *, Vector4 *, f32, Vector4 *, f32, f32);

/* Collides a circle with the outline of a map object, leaving out every edge that lies wholly left of the first line from
   the origin or wholly right of the second. The outline is points 0, 1, 6, 7 while the object's fifth animation is on its
   first frames, otherwise the two quads 0-3 and 4-7. Stops at the first edge that hits. */
void func_001C58F0(MapObject *object, Vector4 *origin, f32 radius, f32 push, f32 first_x, f32 first_z, f32 second_x,
                   f32 second_z) {
    Vector4 *point0, *point1, *point2, *point3, *point4, *point5, *point6, *point7;
    Vector4 *base;
    s32 left0, left1, left2, left3, left4, left5, left6, left7;
    s32 right0, right1, right2, right3, right4, right5, right6, right7;
    f32 height;
    CollisionResult *result = COLLISION_RESULT; // match: one pointer, so the hit flag is reloaded only after a call

    if (object->unk10 & 0x10000) {
        if (MODEL_CHANNEL(object->model)->index == object->unk04->anims[4].anim && MODEL_CHANNEL(object->model)->param < 2) {
            point0 = (Vector4 *)object->model->get_point(object->model, 0);
            if (point0 == NULL) {
                return;
            }
            point1 = (Vector4 *)object->model->get_point(object->model, 1);
            if (point1 == NULL) {
                return;
            }
            point2 = (Vector4 *)object->model->get_point(object->model, 6);
            if (point2 == NULL) {
                return;
            }
            point3 = (Vector4 *)object->model->get_point(object->model, 7);
            if (point3 == NULL) {
                return;
            }
            base = (Vector4 *)object->model->get_point(object->model, 0x1F);
            if (base == NULL) {
                return;
            }
            height = point0->y - base->y;
            left0 = LEFT_OF(point0, first_x, first_z);
            right0 = RIGHT_OF(point0, second_x, second_z);
            left1 = LEFT_OF(point1, first_x, first_z);
            right1 = RIGHT_OF(point1, second_x, second_z);
            left2 = LEFT_OF(point2, first_x, first_z);
            right2 = RIGHT_OF(point2, second_x, second_z);
            left3 = LEFT_OF(point3, first_x, first_z);
            right3 = RIGHT_OF(point3, second_x, second_z);
            if (!(left0 && left1) && !(right0 && right1)) {
                func_00210258(point0, point1, height, origin, radius, push);
            }
            if (result->hit != 0) {
                return;
            }
            if (!(left1 && left2) && !(right1 && right2)) {
                func_00210258(point1, point2, height, origin, radius, push);
            }
            if (result->hit != 0) {
                return;
            }
            if (!(left2 && left3) && !(right2 && right3)) {
                func_00210258(point2, point3, height, origin, radius, push);
            }
            if (result->hit != 0) {
                return;
            }
            if (!(left3 && left0) && !(right3 && right0)) {
                func_00210258(point3, point0, height, origin, radius, push);
            }
        } else {
            point0 = (Vector4 *)object->model->get_point(object->model, 0);
            if (point0 == NULL) {
                return;
            }
            point1 = (Vector4 *)object->model->get_point(object->model, 1);
            if (point1 == NULL) {
                return;
            }
            point2 = (Vector4 *)object->model->get_point(object->model, 2);
            if (point2 == NULL) {
                return;
            }
            point3 = (Vector4 *)object->model->get_point(object->model, 3);
            if (point3 == NULL) {
                return;
            }
            point4 = (Vector4 *)object->model->get_point(object->model, 4);
            if (point4 == NULL) {
                return;
            }
            point5 = (Vector4 *)object->model->get_point(object->model, 5);
            if (point5 == NULL) {
                return;
            }
            point6 = (Vector4 *)object->model->get_point(object->model, 6);
            if (point6 == NULL) {
                return;
            }
            point7 = (Vector4 *)object->model->get_point(object->model, 7);
            if (point7 == NULL) {
                return;
            }
            base = (Vector4 *)object->model->get_point(object->model, 0x1F);
            if (base == NULL) {
                return;
            }
            height = point0->y - base->y;
            left0 = LEFT_OF(point0, first_x, first_z);
            right0 = RIGHT_OF(point0, second_x, second_z);
            left1 = LEFT_OF(point1, first_x, first_z);
            right1 = RIGHT_OF(point1, second_x, second_z);
            left2 = LEFT_OF(point2, first_x, first_z);
            right2 = RIGHT_OF(point2, second_x, second_z);
            left3 = LEFT_OF(point3, first_x, first_z);
            right3 = RIGHT_OF(point3, second_x, second_z);
            left4 = LEFT_OF(point4, first_x, first_z);
            right4 = RIGHT_OF(point4, second_x, second_z);
            left5 = LEFT_OF(point5, first_x, first_z);
            right5 = RIGHT_OF(point5, second_x, second_z);
            left6 = LEFT_OF(point6, first_x, first_z);
            right6 = RIGHT_OF(point6, second_x, second_z);
            left7 = LEFT_OF(point7, first_x, first_z);
            right7 = RIGHT_OF(point7, second_x, second_z);
            if (!(left0 && left1) && !(right0 && right1)) {
                func_00210258(point0, point1, height, origin, radius, push);
            }
            if (result->hit != 0) {
                return;
            }
            if (!(left1 && left2) && !(right1 && right2)) {
                func_00210258(point1, point2, height, origin, radius, push);
            }
            if (result->hit != 0) {
                return;
            }
            if (!(left2 && left3) && !(right2 && right3)) {
                func_00210258(point2, point3, height, origin, radius, push);
            }
            if (result->hit != 0) {
                return;
            }
            if (!(left3 && left0) && !(right3 && right0)) {
                func_00210258(point3, point0, height, origin, radius, push);
            }
            if (result->hit != 0) {
                return;
            }
            if (!(left4 && left5) && !(right4 && right5)) {
                func_00210258(point4, point5, height, origin, radius, push);
            }
            if (result->hit != 0) {
                return;
            }
            if (!(left5 && left6) && !(right5 && right6)) {
                func_00210258(point5, point6, height, origin, radius, push);
            }
            if (result->hit != 0) {
                return;
            }
            if (!(left6 && left7) && !(right6 && right7)) {
                func_00210258(point6, point7, height, origin, radius, push);
            }
            if (result->hit != 0) {
                return;
            }
            if (!(left7 && left4) && !(right7 && right4)) {
                func_00210258(point7, point4, height, origin, radius, push);
            }
        }
    }
}
