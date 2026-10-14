#include "kf_model.h"

void ModelMatrix_Build(ModelInstance *, Vector4 *, Vector4 *, f32 *);
void ModelMatrix_BuildPartLocal(ModelInstance *, ModelPart *, ModelPart *);
void func_0010B5D0(ModelPart *);
void Vector_Copy(void *, void *);
void sceVu0CopyMatrix(void *, void *);

/* Rebuilds the model matrices of a kind 6 object with its second part put where
   it was when the object started, then puts the part back. */
void func_0011F980(MapObject *object) {
    Vector4 saved;
    int i;
    ModelPart *part;
    int part_count;

    // match: it returns a value
    ((int (*)(ModelInstance *, Vector4 *, Vector4 *, f32 *))ModelMatrix_Build)(object->model, &object->model->position, &object->model->rotation, &object->model->unkE0);
    Vector_Copy(&saved, object->model->parts[1].unk40);
    Vector_Copy(object->model->parts[1].unk40, &MAP_OBJECT_WORK(object)->unk10);
    object->model->parts[1].unk40[3] = 1.0f;
    part_count = object->model->part_count;
    part = object->model->parts;
    for (i = 0; i < part_count; i++) {
        ModelMatrix_BuildPartLocal(object->model, part, part);
        part++;
    }
    Vector_Copy(object->model->parts[1].unk40, &saved);
    if (object->model->unk100 & 1) {
        part = object->model->parts;
        for (i = 0; i < part_count; i++) {
            func_0010B5D0(part);
            part++;
        }
    }
    sceVu0CopyMatrix(object->model->unk80, object->model->unk40);
    object->model->unk1B4(object->model);
}
