#include "kf_model.h"

void KF_Vector_Subtract(void *, void *, void *);
float atan2f(float, float);
float func_0019B838(float);

/* The turn that would face a map object toward a point (wrapped by func_0019B838). */
f32 func_0011E708(MapObject *object, Vector4 *point) {
    Vector4 delta;

    KF_Vector_Subtract(&delta, &object->model->position, point);
    return func_0019B838(atan2f(delta.x, delta.z) - object->model->rotation.y);
}
