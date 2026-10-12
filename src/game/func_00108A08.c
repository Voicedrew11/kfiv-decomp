#include "kf_model.h"

/* Switches the model to one of its loaded motions, at frame 0. */
void func_00108A08(ModelInstance *model, int index) {
    int i;

    MODEL_CHANNEL(model)->index = index;
    MODEL_CHANNEL(model)->param = 0;
    MODEL_CHANNEL(model)->param_shifted = 0;
    for (i = 0; i < model->part_count; i++) {
        model->parts[i].level = 0;
    }
}
