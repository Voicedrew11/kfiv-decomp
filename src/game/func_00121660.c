#include "kf_map.h"

/* Starts this kind of trigger: stopped, with its state cleared. */
void func_00121660(MapTrigger *trigger) {
    MapTriggerShotKind121660 *shot = (MapTriggerShotKind121660 *)&trigger->shot;

    shot->unk00 = 0.0f;
    shot->unk08 = 0;
    shot->unk0C = 0;
    trigger->type = 1;
}
