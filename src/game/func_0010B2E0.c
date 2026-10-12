#include "kf_texture.h"

u32 *func_00109990(int);
void func_00109650(u32 *, u32 *);
void func_00109E18(void);

/* Queue the chunks of a texture table that are neither switched off nor in skip_mask. */
void func_0010B2E0(TextureTableHeader *table_arg, s32 skip_mask) {
    TextureTableHeader *table = table_arg; // match: a copy of the parameter keeps the count reload at the loop end
    TextureTableEntry *entries;
    u32 *packet_start = NULL;
    u32 *packet;
    u32 *chunk;
    s32 i;

    func_00109E18();
    entries = (TextureTableEntry *)(table + 1);
    for (i = 0; i < table->entry_count; i++) {
        if (entries[i].unk0C == 0 && (i >= 32 || !((skip_mask >> i) & 1))) {
            packet = func_00109990(1);
            if (packet_start == NULL) {
                packet_start = packet;
            }
            chunk = (u32 *)((char *)table + entries[i].offset);
            *packet++ = (*chunk & 0x8FFFFFFF) | 0x30000000;
            *packet++ = (u32)(chunk + 4);
            *packet++ = 0;
            *packet++ = 0;
        }
    }
    if (packet_start != NULL) {
        func_00109650(packet_start, func_00109990(1));
    }
}
