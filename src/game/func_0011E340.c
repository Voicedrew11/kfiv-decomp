#include "kf_model.h"

void func_00108A08(ModelInstance *, int);
void func_0011E3A0(char *);

/* Starts one of a map object's animations from its first frame. */
s16 func_0011E340(char *arg, int anim) {
    MapObject *object = (MapObject *)arg;
    ModelChannel *channel;

    func_00108A08(object->model, object->unk04->anims[anim].anim);
    MAP_OBJECT_BYTES(object)->anim = anim;
    channel = MODEL_CHANNEL(object->model);
    channel->param = 0;
    channel->param_shifted = 0;
    func_0011E3A0(arg);
}
