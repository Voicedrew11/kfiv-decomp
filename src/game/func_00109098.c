#include "kf_model.h"

void func_00108A08(ModelInstance *, int);

/* Switches the model to a motion and sets its speed. */
int func_00109098(ModelInstance *model, u8 index, u16 speed) {
    func_00108A08(model, index);
    MODEL_CHANNEL(model)->value = speed << 8;
    return 0;
}
