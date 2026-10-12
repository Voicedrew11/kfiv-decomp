#include "kf_map.h"

void func_0010E6A8(ModelInstance *, s64, s64);
void func_0010E820(ModelInstance *, s64);
void func_0010E8E0(void *, int, int, int, int);

/* Starts this kind of trigger: at rest in the position its flag gives. */
void func_00122BE8(MapTrigger *trigger) {
    MapTriggerShotKind122BE8 *shot = (MapTriggerShotKind122BE8 *)&trigger->shot;

    func_0010E6A8(trigger->model, 0x40, 0x40);
    func_0010E820(trigger->model, 0x44);
    shot->unk00[0] = 0.0f;
    shot->unk00[1] = -700.0f;
    shot->unk00[2] = 0.0f;
    shot->unk00[3] = 0.0f;
    shot->unk10 = 0.0f;
    if (!(MAP_TRIGGER_FLAGS(trigger)->word & MAP_TRIGGER_FLAG_20000)) {
        trigger->type = 1;
        func_0010E8E0(trigger->model, -1, -1, -1, 0);
    } else {
        trigger->type = 3;
        func_0010E8E0(trigger->model, -1, -1, -1, 0x80);
    }
}
