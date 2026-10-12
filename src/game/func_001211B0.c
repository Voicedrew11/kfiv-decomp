#include "kf_model.h"

void func_00216C48(void *, void *, void *);

/* Update handler of a sliding kind, step 2: moves the part one step; after the
   last one it is at step 3. */
void func_001211B0(MapObject *object) {
    MapObjectWorkSlide *work = (MapObjectWorkSlide *)object->work;
    ModelPart *part = &object->model->parts[work->part];

    if (work->timer > 0) {
        work->timer--;
    }
    func_00216C48(part->unk40, part->unk40, work);
    if (--work->frames <= 0) {
        MAP_OBJECT_BYTES(object)->step = 3;
    }
}
