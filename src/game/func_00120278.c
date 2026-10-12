#include "kf_model.h"

extern float D_00413F80[];

s16 func_0011E340(char *, int);
void func_0011E3A0(char *);
void KF_Vector_Subtract(void *, void *, void *);
float sqrtf(float);

/* Update handler of the fading kind: the alpha goes from near_alpha to far_alpha
   as the distance to the player goes from near_distance to far_distance. */
void func_00120278(MapObject *object) {
    MapObjectWorkFade *work = (MapObjectWorkFade *)object->work;
    Vector4 delta;
    f32 distance;

    if (object->model->unk100 & 1) {
        func_0011E340((char *)object, 4);
        func_0011E3A0((char *)object);
    }
    if (work->near_alpha == 0.0f && work->far_alpha == 0.0f) {
        object->model->alpha = 127.0f;
        return;
    }
    KF_Vector_Subtract(&delta, &object->model->position, D_00413F80);
    distance = sqrtf(delta.x * delta.x + delta.z * delta.z);
    if (distance < work->near_distance) {
        object->model->alpha = work->near_alpha;
    } else if (work->far_distance < distance) {
        object->model->alpha = work->far_alpha;
    } else {
        object->model->alpha = work->near_alpha + (work->far_alpha - work->near_alpha) * (distance - work->near_distance) / (work->far_distance - work->near_distance);
    }
    if (object->model->alpha < 0.0f) {
        object->model->alpha = 0.0f;
    } else if (object->model->alpha > 255.0f) {
        object->model->alpha = 255.0f;
    }
}
