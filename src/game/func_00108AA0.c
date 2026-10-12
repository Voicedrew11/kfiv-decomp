#include "kf_model.h"

/* Whether the model's motion is at the given frame. */
int func_00108AA0(ModelInstance *model, int frame) {
    ModelChannel *channel = MODEL_CHANNEL(model);

    return channel->unk8E != 0 && channel->param == frame;
}
