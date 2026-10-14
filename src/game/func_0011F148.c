#include "kf_model.h"

void ModelMatrix_Build(ModelInstance *, Vector4 *, Vector4 *, f32 *);
void ModelMatrix_BuildPartLocal(ModelInstance *, ModelPart *, ModelPart *);
void func_0010B5D0(ModelPart *);
void sceVu0CopyMatrix(void *, void *);

/* Rebuilds a map object's model matrices from its position and rotation. */
void func_0011F148(char *arg) {
    MapObject *object = (MapObject *)arg;
    int i;
    ModelPart *part;
    int part_count;

    ModelMatrix_Build(object->model, &object->model->position, &object->model->rotation, &object->model->unkE0);
    part_count = object->model->part_count;
    part = object->model->parts;
    for (i = 0; i < part_count; i++) {
        ModelMatrix_BuildPartLocal(object->model, part, part);
        part++;
    }
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
