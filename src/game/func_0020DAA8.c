#include "kf_texture.h"

char *func_0020D0B0(char *, int);

/* Walks a chunk table, handing each entry's chunk to func_0020D0B0; returns the read cursor. */
char *func_0020DAA8(char *base, int chunk_arg) {
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
        cursor = func_0020D0B0((char *)(entry[i].offset + (int)base), chunk_arg);
    }
    return cursor;
}
