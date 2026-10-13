#include "kf_game.h"
#include "kf_map_object.h"

extern GameStatus D_0038CC50;

void func_00216C48(void *, void *, void *);
void func_00122600(void *, int);

/* The same for the kind of func_00121DA8, which also resets its count through func_00122600. */
void func_00122480(MapObject *object) {
    MapObjectWorkKind121DA8 *work = (MapObjectWorkKind121DA8 *)object->work;
    ModelPart *part = &object->model->parts[work->part];

    if (work->timer > 0) {
        work->timer--;
    }
    func_00216C48(part->unk40, part->unk40, work);
    if (--work->frames <= 0) {
        func_00122600(object, 0);
        MapObject_StartAnim(object, 4);
        MAP_OBJECT_BYTES(object)->step = 1;
        D_0038CC50.unk30 &= ~1U;
    }
}
