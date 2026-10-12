#include "kf_model.h"

void func_00216C48(void *, void *, void *);

/* Moves a map object and what its unk08 points at by an offset. */
void func_0011EDC8(MapObject *object, void *offset) {
    func_00216C48(&object->model->position, &object->model->position, offset);
    func_00216C48(object->unk08, object->unk08, offset);
}
