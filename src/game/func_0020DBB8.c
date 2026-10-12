#include "kf_texture.h"

void *func_0020DB48(TextureHeader *, int, int);

/* Moves the TEX0 base pointers of every texture of a chunk table by the deltas; returns the read cursor. */
char *func_0020DBB8(char *base, int delta_a, int delta_b) {
    char *cursor;
    TextureTableHeader *header;
    TextureTableEntry *entry;
    u32 i;

    cursor = base;
    header = (TextureTableHeader *)cursor;
    cursor += sizeof(TextureTableHeader);
    entry = (TextureTableEntry *)cursor;
    cursor += header->entry_count * sizeof(TextureTableEntry);
    for (i = 0; i < header->entry_count; i++) {
        cursor = func_0020DB48((TextureHeader *)(entry[i].offset + (int)base), delta_a, delta_b);
    }
    return cursor;
}
