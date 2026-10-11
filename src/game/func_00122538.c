#include "kf_map.h"

extern MapSlot D_0038CD20[];
extern TriggerTypeDef D_0041A6F8[];

int func_00103BA8(int, int, int);
void func_00122600(void *, int);

/* Fires the trigger's one-shot when its type matches; returns whether it fired. */
int func_00122538(MapTrigger *trigger, int type_index) {
    u8 type;
    MapSlot *slot;
    int tag;
    MapTriggerShot *shot = &trigger->shot;

    type = D_0041A6F8[type_index].type;
    slot = &D_0038CD20[trigger->unk10 & 1];
    tag = slot->tag;
    if (trigger->type == 1 && shot->fired == 0 && type == trigger->type &&
        func_00103BA8(tag, (u8)trigger->unk08[0x3A], type_index)) {
        func_00122600(trigger, type_index);
        shot->fired = type;
        return TRUE;
    }
    return FALSE;
}
