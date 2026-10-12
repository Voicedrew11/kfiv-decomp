#include "kf_collision.h"

void func_00216C48(void *, void *, void *);

/* Moves a map record and its model by an offset. */
void func_00115550(MapRecordBounds *record, f32 *offset) {
    if (record != NULL) {
        func_00216C48(&record->model->position, &record->model->position, offset);
        func_00216C48(record, record, offset);
        func_00216C48(record->unk40, record->unk40, offset);
        func_00216C48(record->unk50, record->unk50, offset);
        func_00216C48(record->lowest, record->lowest, offset);
        func_00216C48(record->highest, record->highest, offset);
    }
}
