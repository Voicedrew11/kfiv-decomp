#include "kf_map.h"

int rand(void);

/* Starts this kind of trigger: a random period and a random phase within it. */
void func_00122E88(MapTrigger *trigger) {
    MapTriggerShotKind122E88 *shot = (MapTriggerShotKind122E88 *)&trigger->shot;
    ModelInstance *model = trigger->model;

    model->unk102 |= 0x200;
    model->unk145 = 1;
    trigger->model->unk146 = 0;
    /* 4.656613e-10f is 1 / 2^31: rand() scaled to 0..1 */
    shot->period = (int)((float)rand() * 61.0f * 4.656613e-10f) + 0x78;
    shot->phase = (int)((float)(shot->period + 1) * (float)rand() * 4.656613e-10f);
    trigger->type = 1;
}
