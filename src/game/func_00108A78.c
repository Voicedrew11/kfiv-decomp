#include "kf_model.h"

/* Whether the model's motion has reached its last frame. */
int func_00108A78(ModelInstance *model) {
    ModelChannel *channel = MODEL_CHANNEL(model);

    return channel->param >= ((ModelTableHeader *)channel->tables[channel->index] - 1)->unk00;
}
