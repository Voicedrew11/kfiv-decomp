#include "kf_model.h"

int Collision_CapsuleTriangle(int, int, int, int, float, float); // match: retail returns what the second test leaves in $v0; the definition returns nothing

/* Tests an upright capsule against the quad of the owner's model points 0 to 3 as two triangles, if the owner
   has flag 0x10000; the second triangle only when the first is missed. */
int func_001C5170(CollisionOwner *owner, int context, float first_value, float second_value) {
    int point_a, point_b, point_c, point_d, result;

    if (owner->flags & 0x10000) {
        point_a = owner->model->get_point(owner->model, 0);
        point_b = owner->model->get_point(owner->model, 1);
        point_c = owner->model->get_point(owner->model, 2);
        point_d = owner->model->get_point(owner->model, 3);
        Collision_CapsuleTriangle(point_a, point_b, point_c, context, second_value, first_value);
        result = *(int *)0x70000010;
        if (result == 0) {
            return Collision_CapsuleTriangle(point_a, point_c, point_d, context, second_value, first_value);
        }
        return result;
    }
}
