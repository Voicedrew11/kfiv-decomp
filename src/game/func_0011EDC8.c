#include "kf_model.h"

void Vector_Add(void *, void *, void *);

/* Moves a map object and what its unk08 points at by an offset. */
void func_0011EDC8(MapObject *object, void *offset) {
    Vector_Add(&object->model->position, &object->model->position, offset);
    Vector_Add(object->unk08, object->unk08, offset);
}
