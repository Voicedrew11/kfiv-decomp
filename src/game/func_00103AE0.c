#include "kf_map.h"

extern TriggerItemRecord D_00367928[TRIGGER_ITEM_RECORD_COUNT];

/* Returns the item recorded for a trigger, 0 if there is none. */
int func_00103AE0(int map_tag, int trigger_id) {
    TriggerItemRecord *record;
    int i;

    if ((u32)(map_tag - 0x11) < 3) {
        map_tag = 0x11;
    }
    record = D_00367928;
    for (i = 0; i < TRIGGER_ITEM_RECORD_COUNT; i++, record++) {
        if (record->map_tag == map_tag && record->trigger_id == trigger_id) {
            return record->item;
        }
    }
    return 0;
}
