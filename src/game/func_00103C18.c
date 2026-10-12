#include "kf_game.h"
#include "kf_map.h"
#include "kf_player.h"

extern u8 D_00349680[0x10];
extern char D_00349690[0x2000];
extern char D_0034B690[0x40];
extern char D_0034B6D0[0xC80];
extern char D_0034C350[0x190];
extern char D_0034C4E0[0x32];
extern char D_0034C518[0x400];
extern char D_0034C918[0x10];
extern char D_0034C928[0x8000];
extern char D_00354928[0x2000];
extern char D_00356928[0x2000];
extern char D_00358928[0xA000];
extern char D_00362928[0x5000];
extern TriggerItemRecord D_00367928[TRIGGER_ITEM_RECORD_COUNT];
extern char D_00367980[0x1E];
extern char D_00367940[0x3C];
extern LevelRecord D_003679A0[70];
extern LevelRecord D_003682F0[160];
extern u8 D_00369830[380];
extern u8 D_003699B0[380];
extern u8 D_00369B30[380];
extern u8 D_00369CB0[380];
extern u8 D_00369E30[380];
extern u8 D_00369FB0[380];
extern char D_0036A130[];
extern PlayerState D_00413F80;
extern GameDefinition D_004147F8[380];
extern char D_00421050[0x30];
extern int D_00446770;
extern MapSlot D_0038CD20[];
extern PlayTime D_00446A28;
extern u16 D_0044692C;
extern u16 D_0044692E;
extern u16 D_00446930;
extern u16 D_00446932;
extern u16 D_00446934;
extern u16 D_00446936;
extern u16 D_00446938;
extern u16 D_0044693A;
extern u16 D_0044693C;
extern u16 D_0044693E;
extern u16 D_00446940;
extern u16 D_00446942;
extern u16 D_00446944;
extern u16 D_00446946;
extern s32 D_004469D0;

void *memcpy(void *, const void *, u32);
void *memset(void *, int, u32);
void func_0011CF88(void);

/* copies `size` bytes to the save buffer and returns where the next go (a real call to memcpy: inside the
   helper the size is not a constant, where the sizeof copies below are expanded in place) */
static inline char *save_block(char *cursor, void *source, int size) {
    memcpy(cursor, source, size);
    cursor += size;
    return cursor;
}

/* Packs the game's state into the save buffer D_0036A130 (func_00104EC8 takes it out in the same order),
   then adds one checksum byte for every SAVE_BLOCK_SIZE bytes (func_001060C0 checks them). */
void func_00103C18(void) {
    u8 bits;
    s32 map_tag;
    char *cursor = D_0036A130;
    GameDefinitionFlags *definition;
    char *source;
    char *sums;
    u32 state;
    u32 total;
    u32 rounded;
    s32 blocks;
    s32 sum;
    u8 mask;
    int i;
    int block;
    int byte;

    func_0011CF88();
    memset(cursor, 0, 0x20000);
    ((SaveHeader *)D_0036A130)->magic = SAVE_MAGIC;
    cursor += sizeof(SaveHeader);
    memcpy(cursor, &D_00446A28, sizeof(D_00446A28));
    cursor += sizeof(D_00446A28);
    memcpy(cursor, D_00349680, sizeof(D_00349680));
    cursor += sizeof(D_00349680);
    memcpy(cursor, &D_0044692C, sizeof(D_0044692C));
    cursor += sizeof(D_0044692C);
    memcpy(cursor, &D_0044692E, sizeof(D_0044692E));
    cursor += sizeof(D_0044692E);
    memcpy(cursor, &D_00446930, sizeof(D_00446930));
    cursor += sizeof(D_00446930);
    memcpy(cursor, &D_00446932, sizeof(D_00446932));
    cursor += sizeof(D_00446932);
    memcpy(cursor, &D_00446934, sizeof(D_00446934));
    cursor += sizeof(D_00446934);
    memcpy(cursor, &D_00446936, sizeof(D_00446936));
    cursor += sizeof(D_00446936);
    memcpy(cursor, &D_00446938, sizeof(D_00446938));
    cursor += sizeof(D_00446938);
    memcpy(cursor, &D_0044693A, sizeof(D_0044693A));
    cursor += sizeof(D_0044693A);
    memcpy(cursor, &D_0044693C, sizeof(D_0044693C));
    cursor += sizeof(D_0044693C);
    memcpy(cursor, &D_0044693E, sizeof(D_0044693E));
    cursor += sizeof(D_0044693E);
    memcpy(cursor, &D_00446940, sizeof(D_00446940));
    cursor += sizeof(D_00446940);
    memcpy(cursor, &D_00446942, sizeof(D_00446942));
    cursor += sizeof(D_00446942);
    memcpy(cursor, &D_00446944, sizeof(D_00446944));
    cursor += sizeof(D_00446944);
    memcpy(cursor, &D_00446946, sizeof(D_00446946));
    cursor += sizeof(D_00446946);
    cursor = save_block(cursor, D_00349690, 0xB40);
    cursor = save_block(cursor, &D_00349690[0x1680], 0x6C0);
    memcpy(cursor, D_0034B690, sizeof(D_0034B690));
    cursor += sizeof(D_0034B690);
    memcpy(cursor, D_0034B6D0, sizeof(D_0034B6D0));
    cursor += sizeof(D_0034B6D0);
    memcpy(cursor, D_0034C350, sizeof(D_0034C350));
    cursor += sizeof(D_0034C350);
    memcpy(cursor, D_0034C4E0, sizeof(D_0034C4E0));
    cursor += sizeof(D_0034C4E0);
    memcpy(cursor, D_0034C518, sizeof(D_0034C518));
    cursor += sizeof(D_0034C518);
    memcpy(cursor, D_0034C918, sizeof(D_0034C918));
    cursor += sizeof(D_0034C918);
    memcpy(cursor, D_00367928, sizeof(D_00367928));
    cursor += sizeof(D_00367928);
    cursor = save_block(cursor, D_0034C928, 0x2D00);
    cursor = save_block(cursor, &D_0034C928[0x5A00], 0x1B00);
    cursor = save_block(cursor, D_00354928, 0xB40);
    cursor = save_block(cursor, &D_00354928[0x1680], 0x6C0);
    cursor = save_block(cursor, D_00356928, 0xB40);
    cursor = save_block(cursor, &D_00356928[0x1680], 0x6C0);
    cursor = save_block(cursor, D_00358928, 0x3840);
    cursor = save_block(cursor, &D_00358928[0x7080], 0x21C0);
    cursor = save_block(cursor, D_00362928, 0x1C20);
    cursor = save_block(cursor, &D_00362928[0x3840], 0x10E0);
    memcpy(cursor, D_00367940, sizeof(D_00367940));
    cursor += sizeof(D_00367940);
    memcpy(cursor, D_00367980, sizeof(D_00367980));
    cursor += sizeof(D_00367980);
    memcpy(cursor, D_003679A0, sizeof(D_003679A0));
    cursor += sizeof(D_003679A0);
    memcpy(cursor, D_003682F0, sizeof(D_003682F0));
    cursor += sizeof(D_003682F0);
    memcpy(cursor, D_00369830, sizeof(D_00369830));
    cursor += sizeof(D_00369830);
    memcpy(cursor, D_003699B0, sizeof(D_003699B0));
    cursor += sizeof(D_003699B0);
    memcpy(cursor, D_00369B30, sizeof(D_00369B30));
    cursor += sizeof(D_00369B30);
    memcpy(cursor, D_00369CB0, sizeof(D_00369CB0));
    cursor += sizeof(D_00369CB0);
    memcpy(cursor, D_00369E30, sizeof(D_00369E30));
    cursor += sizeof(D_00369E30);
    memcpy(cursor, D_00369FB0, sizeof(D_00369FB0));
    cursor += sizeof(D_00369FB0);
    memcpy(cursor, &D_00413F80.position, sizeof(D_00413F80.position));
    cursor += sizeof(D_00413F80.position);
    memcpy(cursor, &D_00413F80.rotation, sizeof(D_00413F80.rotation));
    cursor += sizeof(D_00413F80.rotation);
    memcpy(cursor, &D_00413F80.unk20, sizeof(D_00413F80.unk20));
    cursor += sizeof(D_00413F80.unk20);
    memcpy(cursor, &D_00413F80.unk24, sizeof(D_00413F80.unk24));
    cursor += sizeof(D_00413F80.unk24);
    memcpy(cursor, &D_00413F80.unk28, sizeof(D_00413F80.unk28));
    cursor += sizeof(D_00413F80.unk28);
    memcpy(cursor, &D_00413F80.unk2C, sizeof(D_00413F80.unk2C));
    cursor += sizeof(D_00413F80.unk2C);
    memcpy(cursor, D_00413F80.timed_flags, sizeof(D_00413F80.timed_flags));
    cursor += sizeof(D_00413F80.timed_flags);
    memcpy(cursor, &D_00413F80.unk3C, 0x10);
    cursor += 0x10;
    memcpy(cursor, &D_00413F80.unk4C, sizeof(D_00413F80.unk4C));
    cursor += sizeof(D_00413F80.unk4C);
    memcpy(cursor, &D_00413F80.unk50, sizeof(D_00413F80.unk50));
    cursor += sizeof(D_00413F80.unk50);
    memcpy(cursor, &D_00413F80.unk54, sizeof(D_00413F80.unk54));
    cursor += sizeof(D_00413F80.unk54);
    memcpy(cursor, &D_00413F80.unkB4, sizeof(D_00413F80.unkB4));
    cursor += sizeof(D_00413F80.unkB4);
    memcpy(cursor, &D_00413F80.unkB8, sizeof(D_00413F80.unkB8));
    cursor += sizeof(D_00413F80.unkB8);
    memcpy(cursor, &D_00413F80.unkD4, sizeof(D_00413F80.unkD4));
    cursor += sizeof(D_00413F80.unkD4);
    memcpy(cursor, &D_00413F80.unkD8, sizeof(D_00413F80.unkD8));
    cursor += sizeof(D_00413F80.unkD8);
    map_tag = D_0038CD20[D_00446770].tag;
    memcpy(cursor, &map_tag, sizeof(map_tag));
    cursor += sizeof(map_tag);
    memcpy(cursor, D_00421050, sizeof(D_00421050));
    cursor += sizeof(D_00421050);
    memcpy(cursor, &D_004469D0, sizeof(D_004469D0));
    cursor += sizeof(D_004469D0);
    bits = 0;
    mask = 1;
    for (i = 0; i < 380; i++) {
        definition = &((GameDefinitionFlags *)D_004147F8)[i];
        /* one bit for every definition: whether its state is 1 (states 2 and 3 are not saved) */
        state = definition->flags >> 18;
        if ((state & 3) < 2) {
            if (state & 1) {
                bits |= mask;
            }
        }
        if (i % 8 != 7) {
            mask <<= 1;
        } else {
            memcpy(cursor, &bits, sizeof(bits));
            cursor += sizeof(bits);
            bits = 0;
            mask = 1;
        }
    }
    memcpy(cursor, &bits, sizeof(bits));
    cursor += sizeof(bits);
    total = cursor - D_0036A130;
    rounded = (total + (SAVE_BLOCK_SIZE - 1)) / SAVE_BLOCK_SIZE * SAVE_BLOCK_SIZE;
    sums = cursor + (rounded - total);
    blocks = rounded / SAVE_BLOCK_SIZE;
    ((SaveHeader *)D_0036A130)->size = rounded;
    source = D_0036A130;
    for (block = 0; block < blocks; block++) {
        sum = 0;
        for (byte = 0; byte < SAVE_BLOCK_SIZE; byte++) {
            sum += (u8)*source++;
        }
        *sums++ = sum;
    }
    // match: a dead assignment, but one that is not `cursor += n`: with only those gcse knows that cursor never
    // leaves the save buffer and moves the read of D_00446770 up to the first copy loop
    cursor = sums;
}
