#include "common.h"
#include "kf_effect.h"

int rand(void);

// On the first frame, turns the model to a random heading within +-pi, then runs func_001A4480.
void func_001A4410(EffectActor *effect) {
    if (effect->frame == 0) {
        effect->model->rotation.z = ((float)(rand() & 0xFFFF) * 0.000030517578125f - 1.0f) * 3.1415f;
    }
    func_001A4480(effect);
}
