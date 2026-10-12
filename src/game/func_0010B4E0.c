#include "kf_texture.h"

/* Switch one chunk of a texture table on or off for func_0010B2E0. */
void func_0010B4E0(TextureTableHeader *table, s32 index, s32 enable) {
    TextureTableEntry *entries = (TextureTableEntry *)(table + 1);

    if (index >= 0 && index < table->entry_count) {
        entries[index].unk0C = enable == 0;
    }
}
