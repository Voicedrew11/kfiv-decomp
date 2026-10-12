#include "kf_texture.h"

u32 *func_00109990(int);
void func_00109650(u32 *, u32 *);
void func_00109E18(void);

/* Queue every chunk of a texture table for sending. */
void func_0010B238(TextureTableHeader *table) {
    TextureTableEntry *entries = (TextureTableEntry *)(table + 1);
    u32 *packet_start;
    u32 *packet;
    u32 *chunk;
    u32 i;

    func_00109E18();
    packet_start = packet = func_00109990(table->entry_count + 1);
    for (i = 0; i < table->entry_count; i++) {
        chunk = (u32 *)((char *)table + entries[i].offset);
        *packet++ = (*chunk & 0x8FFFFFFF) | 0x30000000;
        *packet++ = (u32)(chunk + 4);
        *packet++ = 0;
        *packet++ = 0;
    }
    func_00109650(packet_start, packet);
}
