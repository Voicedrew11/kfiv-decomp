#include "kf_game.h"
#include "kf_map_object.h"

extern GameStatus D_0038CC50;

/* Step handler: counts the timer down and runs the animation; at its end starts
   animation 5 at step 4 and clears bit 0 of D_0038CC50.unk30. */
void func_00121228(MapObject *object) {
    MapObjectWork *work = MAP_OBJECT_WORK(object);

    if (work->timer > 0) {
        work->timer--;
    }
    if (MODEL_CHANNEL(object->model)->param != 0) {
        MapObject_StepAnim(object);
        return;
    }
    MapObject_StartAnim(object, 5);
    MAP_OBJECT_BYTES(object)->step = 4;
    D_0038CC50.unk30 &= ~1U;
}
