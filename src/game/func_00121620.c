#include "kf_model.h"

/* Fires a resting object of this kind (step 1, not fired yet) when the item is
   the one it waits for; returns whether it fired. */
int func_00121620(MapObject *object, int item) {
    MapObjectWorkSlide *work = (MapObjectWorkSlide *)object->work;
    u8 step = MAP_OBJECT_BYTES(object)->step;

    if (step == 1 && work->fired == 0 && item == MAP_OBJECT_WORDS(object)->unk44) {
        work->fired = step;
        return TRUE;
    }
    return FALSE;
}
