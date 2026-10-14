#include "kf_model.h"

extern float D_004325A0[];
extern f32 D_00431B30[4][4];

void *memset(void *, int, u32);
void func_0020C300(ModelInstance *, Vector4 *, Vector4 *, f32 *);
void func_0020B8B0(ModelInstance *, ModelPart *, ModelPart *);
void func_0010B5D0(ModelPart *);
void sceVu0CopyMatrix(void *, void *);
void Matrix_Multiply(void *, void *, void *);
void Vector_ApplyMatrix(void *, void *, void *);
void Vector_Subtract(void *, void *, void *);
float sqrtf(float);
float atan2f(float, float);
void Vector_MultiplyComponents(float *, float *, float *);

/* Rebuilds the model matrices of an object whose parts always face the camera:
   each part is put where it sits in the world and turned toward D_004325A0. */
void func_001203F0(MapObject *object) {
    f32 matrix[16];
    Vector4 world;
    Vector4 delta;
    Vector4 unit;
    int i = 0;
    ModelPart *part;
    int part_count;
    f32 pitch;
    f32 yaw;

    memset(&unit, 0, sizeof(unit));
    unit.w = 1.0f;
    func_0020C300(object->model, &object->model->position, &object->model->rotation, &object->model->unkE0);
    part_count = object->model->part_count;
    part = object->model->parts;
    for (; i < part_count; i++) {
        func_0020B8B0(object->model, part, part);
        part++;
    }
    if (object->model->unk100 & 1) {
        part = object->model->parts;
        for (i = 0; i < part_count; i++) {
            func_0010B5D0(part);
            part++;
        }
    }
    i = 0;
    sceVu0CopyMatrix(object->model->unk80, object->model->unk40);
    part = object->model->parts;
    for (; i < part_count; i++) {
        Matrix_Multiply(matrix, object->model->unk40, part);
        Vector_ApplyMatrix(&world, matrix, &unit);
        Vector_Subtract(&delta, D_004325A0, &world);
        pitch = atan2f(delta.y, sqrtf(delta.x * delta.x + delta.z * delta.z));
        yaw = atan2f(-delta.x, -delta.z);
        part->unk40[0] = world.x;
        part->unk40[1] = world.y;
        part->unk40[2] = world.z;
        part->unk40[3] = 1.0f;
        part->unk50 = pitch;
        part->unk54 = yaw;
        part->unk58 = 0.0f;
        part->unk5C = 1.0f;
        Vector_MultiplyComponents(part->unk60, part->unk60, &object->model->unkE0);
        part++;
    }
    part = object->model->parts;
    for (i = 0; i < part_count; i++) {
        func_0020B8B0(object->model, part, part);
        part++;
    }
    sceVu0CopyMatrix(object->model->unk40, D_00431B30);
    object->model->unk1B4(object->model);
}
