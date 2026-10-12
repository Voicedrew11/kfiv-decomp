#include "kf_map.h"

extern MapSlot D_0038CD20[];

/* Returns packet index of the map slot. */
u32 *func_00116BD0(int slot, int index) {
    MapSlot *map_slot = &D_0038CD20[slot];
    MapSlotPackets *packets = &map_slot->unk40;

    if (packets != NULL) {
        return packets->items[index];
    }
    return NULL;
}
