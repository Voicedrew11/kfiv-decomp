#include "kf_map.h"

extern TriggerItemRecord D_00367928[TRIGGER_ITEM_RECORD_COUNT];

/* Records an item for a trigger in the first free record; returns whether there was room. */
int func_00103BA8(int map_tag, int trigger_id, int item) {
    TriggerItemRecord *record;
    int i;

    if ((u32)(map_tag - 0x11) < 3) {
        map_tag = 0x11;
    }
    record = D_00367928;
    for (i = 0; i < TRIGGER_ITEM_RECORD_COUNT; i++, record++) {
        if (record->map_tag == 0) {
            if (record->trigger_id == 0 && record->item == 0) {
                record->map_tag = map_tag;
                record->trigger_id = trigger_id;
                record->item = item;
                return TRUE;
            }
        }
    }
    return FALSE;
}
