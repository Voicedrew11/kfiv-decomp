#include "kf_model.h"

extern float D_00413F80[];
extern f32 D_00431AF0[16];

int func_0011E3A0(void *);
void func_00122530(char *);
void func_00216BC0(void *, void *, void *);
void func_00216778(void *, void *);
void KF_Vector_Subtract(void *, void *, void *);
float sqrtf(float);
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void KF_Vector_Scale(float *, float *, float);

/* Update handler, step 5: once the animation has ended, works out the steps that
   carry the part from point 0 of the model to a point 115 below the player. */
void func_001222C0(MapObject *object) {
    MapObjectWorkKind121DA8 *work = (MapObjectWorkKind121DA8 *)object->work;
    f32 matrix[16];
    Vector4 origin;
    Vector4 target;
    f32 distance;
    int frames;
    void *point;

    if (work->timer > 0) {
        work->timer--;
    }
    if (MODEL_CHANNEL(object->model)->param != 0) {
        func_0011E3A0((char *)object);
        return;
    }
    MAP_OBJECT_BYTES(object)->step = 6;
    func_00122530((char *)object);
    object->model->unk1B4(object->model);
    point = (void *)object->model->get_point(object->model, 0);
    func_00216778(&origin, point);
    func_00216778(&target, D_00413F80);
    target.y -= 115.0f;
    KF_Vector_Subtract(work, &target, &origin);
    distance = sqrtf(((MapObjectWorkKind121DA8 *)object->work)->step.x * ((MapObjectWorkKind121DA8 *)object->work)->step.x + work->step.y * work->step.y + work->step.z * work->step.z);
    sceVu0RotMatrixX(matrix, D_00431AF0, object->model->rotation.x);
    sceVu0RotMatrixY(matrix, D_00431AF0, -object->model->rotation.y);
    func_00216BC0(work, matrix, work);
    frames = distance / 6.6666665f;
    work->frames = frames;
    KF_Vector_Scale(&work->step.x, &work->step.x, 1.0f / frames);
}
