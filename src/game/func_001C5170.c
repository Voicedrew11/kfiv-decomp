#include "kf_model.h"

int func_0020E1F8(int, int, int, int, float, float);

/* Draws up to two segments through three or four model points. */
int func_001C5170(CollisionOwner *owner, int context, float first_value, float second_value) {
    int point_a, point_b, point_c, point_d, result;

    if (owner->flags & 0x10000) {
        point_a = owner->model->get_point(owner->model, 0);
        point_b = owner->model->get_point(owner->model, 1);
        point_c = owner->model->get_point(owner->model, 2);
        point_d = owner->model->get_point(owner->model, 3);
        func_0020E1F8(point_a, point_b, point_c, context, second_value, first_value);
        result = *(int *)0x70000010;
        if (result == 0) {
            return func_0020E1F8(point_a, point_c, point_d, context, second_value, first_value);
        }
        return result;
    }
}
