#include "kf_model.h"

void sceVu0CopyMatrix(void *, void *);
void sceVu0DivVectorXYZ(void *, void *, float);

/* Makes the two matrices a model is drawn with: the first with its rows divided by the model's scale;
   the second, for models flagged 0x400, emptied but for the model's colour times its alpha. */
void func_0020CBC0(ModelInstance *model, s32 normal_src, s32 light_src, void *normal_dest, void *light_dest) {
    f32 (*normal)[4] = normal_dest;
    f32 (*light)[4] = light_dest;

    sceVu0CopyMatrix(normal, (void *)normal_src);
    sceVu0DivVectorXYZ(normal[0], normal[0], model->unkE0);
    sceVu0DivVectorXYZ(normal[1], normal[1], model->unkE4);
    sceVu0DivVectorXYZ(normal[2], normal[2], model->unkE8);
    sceVu0CopyMatrix(light, (void *)light_src);
    if (model->unk100 & 0x400) {
        light[0][0] = 0.0f;
        light[0][1] = 0.0f;
        light[0][2] = 0.0f;
        light[1][0] = 0.0f;
        light[1][1] = 0.0f;
        light[1][2] = 0.0f;
        light[2][0] = 0.0f;
        light[2][1] = 0.0f;
        light[2][2] = 0.0f;
        light[3][0] = model->unk150 / 255.0f * model->alpha / 255.0f;
        light[3][1] = model->unk154 / 255.0f * model->alpha / 255.0f;
        light[3][2] = model->unk158 / 255.0f * model->alpha / 255.0f;
    }
}
