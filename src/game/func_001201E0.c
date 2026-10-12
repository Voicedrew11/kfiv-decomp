#include "kf_model.h"

s16 func_0011E340(char *, int);

/* Start handler of the kind that fades with the distance to the player: takes
   the two distances and the two alphas from its source and starts invisible. */
void func_001201E0(MapObject *object) {
    MapObjectWorkFade *work = (MapObjectWorkFade *)object->work;
    MapObjectUnk08Words *source;

    if (object->model->unk100 & 1) {
        func_0011E340((char *)object, 4);
    }
    source = MAP_OBJECT_WORDS(object);
    ((MapObjectWorkFade *)object->work)->near_distance = source->unk44;
    work->far_distance = source->unk46;
    work->near_alpha = source->unk48;
    work->far_alpha = source->unk4A;
    object->model->alpha = 0.0f;
    MAP_OBJECT_BYTES(object)->step = 1;
}
