#include "kf_effect.h"

typedef float Mat[4][4] __attribute__((aligned(16)));

extern Mat D_00431AF0;
extern int rand(void);
void sceVu0RotMatrixX(Mat, Mat, float);
void sceVu0RotMatrixY(Mat, Mat, float);
void sceVu0ApplyMatrix(void *, Mat, void *);
void Vector_Add(void *, void *, void *);
void Vector_Normalize(void *, void *);
void Vector_Scale(void *, void *, float);

/* Spawns an effect: a random rotation of the model, then a velocity along the rotated radius. */
void func_001A9B38(EffectActor *effect, EffectSlot *spawn_slot) {
    Mat rotation_matrix;
    Vector4 offset;
    EffectMotion *motion;
    f32 random_angle_y;

    func_001AF640(effect, spawn_slot);
    motion = &effect->motion;
    offset.x = offset.y = offset.w = 0.0f;
    offset.z = (float)effect->def->radius_half;
    effect->model->rotation.x = ((float)(rand() & 0xFFFF) * 3.0517578125e-05f - 1.0f) * 3.1415f;
    random_angle_y = ((float)(rand() & 0xFFFF) * 3.0517578125e-05f - 1.0f) * 3.1415f;
    effect->model->rotation.w = effect->model->rotation.z = 0.0f;
    effect->model->rotation.y = random_angle_y;
    sceVu0RotMatrixX(rotation_matrix, D_00431AF0, effect->model->rotation.x);
    sceVu0RotMatrixY(rotation_matrix, rotation_matrix, effect->model->rotation.y);
    sceVu0ApplyMatrix(&offset, rotation_matrix, &offset);
    Vector_Add(&effect->model->position, &effect->model->position, &offset);
    Vector_Normalize(&offset, &offset);
    Vector_Scale(motion, &offset, -effect->def->speed);
}
