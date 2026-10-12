#include "common.h"
#include "kf_entity.h"

/* 1 if point lies outside the circle of radius around the model in x/z, else 0 */
int func_0020F058(EntityModel *model, f32 *point, f32 radius) {
    f32 delta_x;
    f32 delta_z;

    delta_x = point[0] - model->position[0];
    delta_z = point[2] - model->position[2];
    if (radius < delta_x) {
        return 1;
    }
    if (delta_x < -radius) {
        return 1;
    }
    if (radius < delta_z) {
        return 1;
    }
    if (delta_z < -radius) {
        return 1;
    }
    return radius * radius < delta_x * delta_x + delta_z * delta_z;
}
