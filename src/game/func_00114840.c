#include "kf_map.h"
#include "kf_entity.h"

extern MapSlot D_0038CD20[];
extern MapObject D_003A1A30[][0x200];
extern Entity D_003BEDF0[][0x80];
extern MapSlotMemory D_004321F0[2];
extern int D_004467B0[2];
extern EntitySpawnInfo *D_004467D0[2];
extern MapItemDef *D_004469B8[2];
extern s16 *D_004469C0[2];
extern u8 *D_004469C8[2];

void *memset(void *, int, u32);

/* the address rounded up to a multiple of 16 */
static inline u32 align_16(u32 address) {
    u32 remainder = address & 0xF;
    u32 aligned = address;

    if (remainder != 0) {
        aligned = remainder - 0x10;
        aligned = address - aligned;
    }
    return aligned;
}

/* Finds the parts of a map file that was just read into a slot's memory, and shares out the
   slot's working memory. */
void func_00114840(int map) {
    MapSlotMemory *memory = &D_004321F0[map];
    // match: set twice, the compiler no longer knows the slot is not the memory block, so the store
    // in the loop below makes it read memory->unk1C again afterwards
    MapSlot *slot = NULL;
    MapSlotUnk08 *header;
    char *cursor;
    char *work;
    u32 *packet;
    int i;
    int offset;
    int size;
    u32 address;

    slot = &D_0038CD20[map];
    memset(D_003A1A30[map], 0, sizeof(D_003A1A30[map]));
    memset(D_003BEDF0[map], 0, sizeof(D_003BEDF0[map]));
    header = (MapSlotUnk08 *)memory->unk1C;
    work = memory->unk18;

    /* the working memory: the models, then the two tables of the slot */
    // match: stored as a model pointer (it is room for models); as a char * it is scheduled earlier
    *(ModelInstance **)&memory->unk34 = (ModelInstance *)work;
    work += header->record_count * 0x1C0;
    memory->unk3C = work;
    work += header->unk40 * 0x1C0;
    memory->unk44 = (ModelInstance *)work;
    work += header->unk42 * 0x1C0;
    memory->unk48 = work;
    work += header->unk3A * 0x1C0;
    slot->unk24 = work;
    work += header->unk2C * 4;
    slot->unk28 = work;
    work += header->unk2C;
    // match: stored as a u8 *; as a char * the read of header->unk4C below comes one instruction late
    *(u8 **)&memory->unk18 = (u8 *)work;

    /* the file: its fixed parts, then the tables whose sizes the header gives */
    cursor = (char *)header;
    slot->header = (MapHeader *)cursor;
    cursor = (char *)header + 0x80;
    slot->unk0C = (MapSlotUnk0C *)cursor;
    cursor = (char *)header + 0x480;
    slot->unk10 = cursor;
    cursor = (char *)header + 0x1480;
    slot->unk38 = cursor;
    cursor = (char *)header + 0x1540;
    slot->unk3C = cursor;
    cursor = (char *)header + 0x17C0;
    slot->records = cursor;
    cursor += header->record_count * 0x80;
    slot->flag_blocks = (MapFlagBlock *)cursor;
    cursor += header->unk3E * 0x40;
    slot->origins = (Vector4 *)cursor;
    cursor += 0x200;
    D_004467B0[map] = (int)cursor;
    cursor += header->unk40 * 0x80;
    D_004467D0[map] = (EntitySpawnInfo *)cursor;
    cursor += header->unk42 * 0x60;
    D_004469B8[map] = (MapItemDef *)cursor;
    cursor += header->unk44 * 0x40;
    D_004469C0[map] = (s16 *)cursor;
    cursor += 0x40;
    D_004469C8[map] = (u8 *)cursor;

    /* the parts the header gives offsets for */
    cursor = (char *)header;
    cursor += header->unk4C;
    slot->unk5C = (s32)cursor;
    offset = header->unk28;
    cursor = (char *)header;
    cursor += header->unk24;
    slot->unk30 = cursor;
    size = header->unk00;
    packet = (u32 *)cursor;
    for (i = 0; i < 6; i++) {
        // match: stored as a char *, the type memory->unk1C has (see the note on slot)
        ((char **)slot->unk40.items)[i] = (char *)packet;
        packet += (*packet + 1) * 4;
    }
    cursor = (char *)header;
    cursor += offset;
    slot->unk34 = (s32 *)cursor;
    /* two offsets in that part become addresses */
    ((char **)cursor)[1] += (s32)memory->unk1C;
    ((char **)cursor)[3] += (s32)memory->unk1C;
    address = (u32)memory->unk1C;
    address += size;
    memory->unk14 = (char *)align_16(address);
}
