#include "kf_map.h"

extern MapSlot D_0038CD20[];

u32 *func_00109990(int);
void func_00109650(u32 *, u32 *);

/* Queues packet index of the map slot for sending. */
void func_00116AD8(int slot, int index) {
    MapSlot *map_slot = &D_0038CD20[slot];
    u32 *packet_start;
    u32 *packet;

    packet_start = packet = func_00109990(3);
    *packet++ = *map_slot->unk40.items[index] | 0x30000000;
    *packet++ = (u32)map_slot->unk40.items[index] + 0x10;
    *packet++ = 0;
    *packet++ = 0;
    func_00109650(packet_start, packet);
}
