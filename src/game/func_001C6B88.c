#include "kf_collision.h"
#include "kf_player.h"

/* moves the centre out of what was hit, to the given distance from it */
#define PUSH_OUT(distance) \
    centre[0] = COLLISION_RESULT->point[0] + COLLISION_RESULT->normal[0] * (distance); \
    centre[2] = COLLISION_RESULT->point[2] + COLLISION_RESULT->normal[2] * (distance)

extern PlayerState D_00413F80;

void func_00210088(void *, void *, f32, void *, f32, f32);
void func_0020F0D8(void *, f32, f32, void *, f32, f32);
float sqrtf(float);

/* whether the circle does not fit between the two quads; the player is told by the object instead */
static inline s32 gap_is_closed(MapObject *object, f32 *centre, f32 push, f32 first_gap, f32 second_gap) {
    s32 closed = FALSE;

    if (centre == (f32 *)&D_00413F80) {
        closed = -(object->unk60 & 1);
    } else if (first_gap <= push * 2.25f || second_gap <= push * 2.25f) {
        closed = TRUE;
    }
    return closed;
}

/* Pushes a circle out of the walls and corner posts of a map object's outline: points 0, 1, 6, 7 while its fifth animation
   is on frame 1, otherwise the two quads 0-3 and 4-7. When the gap between the quads is too narrow for the circle (or, for
   the player, when the object says so), the gap is walled off instead of the quads' facing sides. */
void func_001C6B88(MapObject *object, f32 *centre, f32 radius, f32 push) {
    f32 *point0, *point1, *point2, *point3, *point4, *point5, *point6, *point7;
    f32 *top;
    f32 height;
    f32 delta_x, delta_z;
    f32 first_gap, second_gap;

    if (object->unk10 & 0x10000) {
        object->model->unk1B4(object->model);
        if (MODEL_CHANNEL(object->model)->index == object->unk04->anims[4].anim && MODEL_CHANNEL(object->model)->param == 1) {
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
            func_00210088(point0, point1, height, centre, radius, push);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_OUT(push);
            }
            func_00210088(point1, point2, height, centre, radius, push);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_OUT(push);
            }
            func_00210088(point2, point3, height, centre, radius, push);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_OUT(push);
            }
            func_00210088(point3, point0, height, centre, radius, push);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_OUT(push);
            }
            func_0020F0D8(centre, push, radius, point0, 0.0f, height);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_OUT(push + 0.001f);
            }
            func_0020F0D8(centre, push, radius, point1, 0.0f, height);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_OUT(push + 0.001f);
            }
            func_0020F0D8(centre, push, radius, point2, 0.0f, height);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_OUT(push + 0.001f);
            }
            func_0020F0D8(centre, push, radius, point3, 0.0f, height);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_OUT(push + 0.001f);
            }
        } else {
            point0 = (f32 *)object->model->get_point(object->model, 0);
            point1 = (f32 *)object->model->get_point(object->model, 1);
            point2 = (f32 *)object->model->get_point(object->model, 2);
            point3 = (f32 *)object->model->get_point(object->model, 3);
            point4 = (f32 *)object->model->get_point(object->model, 4);
            point5 = (f32 *)object->model->get_point(object->model, 5);
            point6 = (f32 *)object->model->get_point(object->model, 6);
            point7 = (f32 *)object->model->get_point(object->model, 7);
            top = (f32 *)object->model->get_point(object->model, 0x1F);
            height = point0[1] - top[1];
            delta_x = point2[0] - point5[0];
            delta_z = point2[2] - point5[2];
            first_gap = sqrtf(delta_x * delta_x + delta_z * delta_z);
            delta_x = point3[0] - point4[0];
            delta_z = point3[2] - point4[2];
            second_gap = sqrtf(delta_x * delta_x + delta_z * delta_z);
            if (!(MODEL_CHANNEL(object->model)->index == object->unk04->anims[5].anim &&
                  MODEL_CHANNEL(object->model)->param == 1) &&
                gap_is_closed(object, centre, push, first_gap, second_gap)) {
                func_00210088(point2, point5, height, centre, radius, push);
                if (COLLISION_RESULT->hit != 0) {
                    PUSH_OUT(push);
                }
                func_00210088(point4, point3, height, centre, radius, push);
                if (COLLISION_RESULT->hit != 0) {
                    PUSH_OUT(push);
                }
                func_00210088(point0, point1, height, centre, radius, push);
                if (COLLISION_RESULT->hit != 0) {
                    PUSH_OUT(push);
                }
                func_00210088(point1, point2, height, centre, radius, push);
                if (COLLISION_RESULT->hit != 0) {
                    PUSH_OUT(push);
                }
                func_00210088(point3, point0, height, centre, radius, push);
                if (COLLISION_RESULT->hit != 0) {
                    PUSH_OUT(push);
                }
            } else {
                func_00210088(point0, point1, height, centre, radius, push);
                if (COLLISION_RESULT->hit != 0) {
                    PUSH_OUT(push);
                }
                func_00210088(point1, point2, height, centre, radius, push);
                if (COLLISION_RESULT->hit != 0) {
                    PUSH_OUT(push);
                }
                func_00210088(point2, point3, height, centre, radius, push);
                if (COLLISION_RESULT->hit != 0) {
                    PUSH_OUT(push);
                }
                func_00210088(point3, point0, height, centre, radius, push);
                if (COLLISION_RESULT->hit != 0) {
                    PUSH_OUT(push);
                }
                func_00210088(point4, point5, height, centre, radius, push);
                if (COLLISION_RESULT->hit != 0) {
                    PUSH_OUT(push);
                }
            }
            func_00210088(point5, point6, height, centre, radius, push);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_OUT(push);
            }
            func_00210088(point6, point7, height, centre, radius, push);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_OUT(push);
            }
            func_00210088(point7, point4, height, centre, radius, push);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_OUT(push);
            }
            func_0020F0D8(centre, push, radius, point0, 0.0f, height);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_OUT(push + 0.001f);
            }
            func_0020F0D8(centre, push, radius, point1, 0.0f, height);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_OUT(push + 0.001f);
            }
            func_0020F0D8(centre, push, radius, point2, 0.0f, height);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_OUT(push + 0.001f);
            }
            func_0020F0D8(centre, push, radius, point3, 0.0f, height);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_OUT(push + 0.001f);
            }
            func_0020F0D8(centre, push, radius, point4, 0.0f, height);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_OUT(push + 0.001f);
            }
            func_0020F0D8(centre, push, radius, point5, 0.0f, height);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_OUT(push + 0.001f);
            }
            func_0020F0D8(centre, push, radius, point6, 0.0f, height);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_OUT(push + 0.001f);
            }
            func_0020F0D8(centre, push, radius, point7, 0.0f, height);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_OUT(push + 0.001f);
            }
        }
    }
}
