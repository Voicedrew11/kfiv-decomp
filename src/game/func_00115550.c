#include "kf_collision.h"

void Vector_Add(void *, void *, void *);

/* Moves a map record and its model by an offset. */
void func_00115550(MapRecordBounds *record, f32 *offset) {
    if (record != NULL) {
        Vector_Add(&record->model->position, &record->model->position, offset);
        Vector_Add(record, record, offset);
        Vector_Add(record->unk40, record->unk40, offset);
        Vector_Add(record->unk50, record->unk50, offset);
        Vector_Add(record->lowest, record->lowest, offset);
        Vector_Add(record->highest, record->highest, offset);
    }
}
