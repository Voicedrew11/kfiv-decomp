#include "kf_game.h"
#include "kf_map_object.h"

extern GameStatus D_0038CC50;

void func_00216C48(void *, void *, void *);

/* Update handler of a sliding kind: moves the part one step; after the last one it
   starts animation 4 at step 1 and clears bit 0 of D_0038CC50.unk30. */
void func_00121578(MapObject *object) {
    MapObjectWorkSlide *work = (MapObjectWorkSlide *)object->work;
    ModelPart *part = &object->model->parts[work->part];

    if (work->timer > 0) {
        work->timer--;
    }
    func_00216C48(part->unk40, part->unk40, work);
    if (--work->frames <= 0) {
        MapObject_StartAnim(object, 4);
        MAP_OBJECT_BYTES(object)->step = 1;
        D_0038CC50.unk30 &= ~1U;
    }
}
