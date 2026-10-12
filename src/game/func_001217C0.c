#include "kf_player.h"

extern u32 D_0038CC74[];
extern PlayerState D_00413F80;

void KF_Vector_Subtract(void *, void *, void *);
float atan2f(float, float);
float func_0019B838(float);
void func_001BD420(MapObject *, int);

/* Update handler: when the player (using it, status bit 36 clear) is within
   75 in height, within the radius and inside the angle of one of its two
   points, func_001BD420 is called for that point. */
void func_001217C0(MapObject *object) {
    Vector4 delta;
    int i;
    f32 turn;
    f32 half_angle;

    if (!((D_0038CC74[0] >> 1) & 1)) {
        return;
    }
    if ((s32)(D_00413F80.unk1C8 >> 36) & 1) {
        return;
    }
    for (i = 0; i < 2; i++) {
        KF_Vector_Subtract(&delta, (void *)object->model->get_point(object->model, i), &D_00413F80);
        if (delta.y < -75.0f) {
            continue;
        }
        if (delta.y > 75.0f) {
            continue;
        }
        if ((f32)MAP_OBJECT_WORD_ARRAY(object)[i * 3] * (f32)MAP_OBJECT_WORD_ARRAY(object)[i * 3] < delta.x * delta.x + delta.z * delta.z) {
            continue;
        }
        turn = func_0019B838(atan2f(delta.x, delta.z) - object->model->rotation.y);
        half_angle = MAP_OBJECT_WORD_ARRAY(object)[i * 3 + 1];
        half_angle = half_angle * 0.017453292f * 0.5f;
        if (turn < -half_angle) {
            continue;
        }
        if (half_angle < turn) {
            continue;
        }
        func_001BD420(object, i);
        return;
    }
}
