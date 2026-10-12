#include "kf_map.h"

extern TriggerItemRecord D_00367928[TRIGGER_ITEM_RECORD_COUNT];

void *memset(void *, int, u32);

/* Frees the record of a trigger; returns whether there was one. */
int func_00103B38(int map_tag, int trigger_id) {
    TriggerItemRecord *record;
    int i;

    if ((u32)(map_tag - 0x11) < 3) {
        map_tag = 0x11;
    }
    record = D_00367928;
    for (i = 0; i < TRIGGER_ITEM_RECORD_COUNT; i++, record++) {
        if (record->map_tag == map_tag && record->trigger_id == trigger_id) {
            memset(record, 0, sizeof(TriggerItemRecord));
            return TRUE;
        }
    }
    return FALSE;
}
