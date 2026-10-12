#include "kf_model.h"
#include "kf_game.h"

extern GameStatus D_0038CC50;

int func_0011E3A0(void *);
s16 func_0011E340(char *, int);

/* Update handler: runs the animation; at its end the object is at step 4 and
   D_0038CC50.unk30 bit 0 is cleared. */
void func_00122138(MapObject *object) {
    MapObjectWorkKind121DA8 *work = (MapObjectWorkKind121DA8 *)object->work;

    if (work->timer > 0) {
        work->timer--;
    }
    if (MODEL_CHANNEL(object->model)->param != 0) {
        func_0011E3A0((char *)object);
    } else {
        func_0011E340((char *)object, 5);
        MAP_OBJECT_BYTES(object)->step = 4;
        D_0038CC50.unk30 &= ~1U;
    }
}
