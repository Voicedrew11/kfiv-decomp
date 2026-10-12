#include "kf_entity.h"

extern EntityRangeWeights D_0043EB68;

int rand(void);
float sinf(float);
float cosf(float);
void func_00216C48(void *, void *, void *);

/* func_0018B778 for a caller that already knows how far from `centre` it is */
void func_0018BA10(f32 *point, f32 *centre, f32 distance, f32 range) {
    EntityRangeWeights table = D_0043EB68;
    f32 angle;
    s32 total;
    s32 roll;
    s32 i;

    if ((s32)(distance * 100.0f / range) > 50) {
        for (i = 0; i < 5; i++) {
            table.weights[i + 5] *= 2;
        }
    } else {
        for (i = 0; i < 5; i++) {
            table.weights[i] *= 2;
        }
    }
    total = 0;
    for (i = 0; i < 10; i++) {
        total += table.weights[i];
    }
    roll = rand() % total;
    for (i = 0; i < 10; i++) {
        if (roll < table.weights[i]) {
            break;
        }
    }
    range *= (10.0f - i) / 10.0f - (rand() % 10) / 10.0f;
    angle = (rand() % 360) * 0.017453292f;
    if (angle >= 3.1415927f) {
        angle -= 6.2831855f;
    }
    point[0] = -sinf(angle) * range;
    point[1] = 0.0f;
    point[2] = -cosf(angle) * range;
    func_00216C48(point, point, centre);
}
