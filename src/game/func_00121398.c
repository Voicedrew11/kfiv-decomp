#include "kf_map_object.h"
#include "kf_model.h"

extern float D_00413F80[];
extern f32 D_00431AF0[16];

void func_0011F148(char *);
void Vector_ApplyMatrix(void *, void *, void *);
void Vector_Copy(void *, void *);
void Vector_Subtract(void *, void *, void *);
float sqrtf(float);
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void Vector_Scale(float *, float *, float);

/* Update handler of a sliding kind, step 5: once the animation has ended, works
   out the steps that carry the part to a point 115 below the player (6.67 a frame). */
void func_00121398(MapObject *object) {
    MapObjectWorkSlide *work = (MapObjectWorkSlide *)object->work;
    ModelPart *part = &object->model->parts[work->part];
    f32 matrix[16];
    Vector4 origin;
    Vector4 target;
    f32 distance;
    int frames;

    if (work->timer > 0) {
        work->timer--;
    }
    if (MODEL_CHANNEL(object->model)->param != 0) {
        MapObject_StepAnim(object);
        return;
    }
    MAP_OBJECT_BYTES(object)->step = 6;
    func_0011F148((char *)object);
    origin.w = 1.0f;
    origin.x = origin.y = origin.z = 0.0f;
    Vector_ApplyMatrix(&origin, part, &origin);
    Vector_ApplyMatrix(&origin, object->model->unk40, &origin);
    Vector_Copy(&target, D_00413F80);
    target.y -= 115.0f;
    Vector_Subtract(work, &target, &origin);
    distance = sqrtf(((MapObjectWorkSlide *)object->work)->step.x * ((MapObjectWorkSlide *)object->work)->step.x + work->step.y * work->step.y + work->step.z * work->step.z);
    sceVu0RotMatrixX(matrix, D_00431AF0, object->model->rotation.x);
    sceVu0RotMatrixY(matrix, D_00431AF0, -object->model->rotation.y);
    Vector_ApplyMatrix(work, matrix, work);
    frames = distance / 6.6666665f;
    work->frames = frames;
    Vector_Scale(&work->step.x, &work->step.x, 1.0f / frames);
}
