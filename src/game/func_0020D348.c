#include "kf_texture.h"

void func_0020D478(char **, char **);

/* Builds, at *packets, a table of GS upload packets from the texture table at *textures: the same
   header and entries, then one packet per texture (func_0020D478). Leaves *packets after the last
   packet, and the size of the whole in the new header's first word. */
void func_0020D348(char **textures, char **packets) {
    char *source_cursor;
    char *target_cursor;
    TextureTableHeader *source;
    TextureTableHeader *target;
    TextureTableEntry *source_entry;
    TextureTableEntry *target_entry;
    u32 i;

    source_cursor = *textures;
    target_cursor = *packets;
    source = (TextureTableHeader *)source_cursor;
    source_cursor += sizeof(TextureTableHeader);
    source_entry = (TextureTableEntry *)source_cursor;
    source_cursor += source->entry_count * sizeof(TextureTableEntry);
    target = (TextureTableHeader *)target_cursor;
    target->unk00 = 0;
    target_cursor += sizeof(TextureTableHeader);
    target_entry = (TextureTableEntry *)target_cursor;
    target->entry_count = source->entry_count;
    target->unk08 = 0;
    target->unk0C = 0;
    target_cursor += target->entry_count * sizeof(TextureTableEntry);
    for (i = 0; i < source->entry_count; i++) {
        source_cursor = *textures + source_entry[i].offset;
        target_entry[i].offset = target_cursor - *packets;
        target_entry[i].unk0C = 0;
        target_entry[i].unk08 = 0;
        target_entry[i].unk04 = 0;
        func_0020D478(&source_cursor, &target_cursor);
    }
    target->unk00 = target_cursor - *packets;
    *packets = target_cursor;
}
