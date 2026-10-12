#include "kf_model.h"

extern f32 D_00413F80[];
extern char D_004681A0[];

void KF_Vector_Subtract(void *, void *, void *);
float sqrtf(float);

/* Put a model in the draw list bucket for its distance from the player. */
void func_0010B018(ModelInstance *model, s32 layer, s32 list) {
    f32 delta[4];
    ModelInstance **bucket;
    ModelInstance *next;
    s32 depth;

    if (model->part_count != 0 && (u32)layer < 5 && (u32)list < 2) {
        KF_Vector_Subtract(delta, &model->position, D_00413F80);
        depth = (sqrtf(delta[0] * delta[0] + delta[1] * delta[1] + delta[2] * delta[2]) * 1000.0f) / 5000.0f;
        if (depth < 0) {
            depth = 0;
        } else if (depth >= 1000) {
            depth = 999;
        }
        bucket = (ModelInstance **)(D_004681A0 + (depth * 4 + list * 0xFA0 + layer * 0x1F40));
        next = *bucket;
        *bucket = model;
        model->draw_next[list] = next;
    }
}
