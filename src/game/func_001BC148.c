#include "kf_map.h"

extern u8 D_00369830[];
extern TriggerTypeDef D_0041A6F8[];

int func_001BC148(int idx) {
    TriggerTypeDef *entry;

    if (idx < 0) {
        return 0;
    }
    entry = D_0041A6F8 + idx;
    if (entry->flags >> 1 & 1) {
        if (D_00369830[idx] != 0) {
            return 1;
        }
    } else {
        if (D_00369830[idx] >= 0x63) {
            return 1;
        }
    }
    return 0;
}
