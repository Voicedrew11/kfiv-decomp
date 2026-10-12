#include "kf_game.h"
#include "kf_player.h"
#include "kf_map.h"

extern u8 D_0036A130[0x20000];
extern u8 D_00349680[0x10];
extern s16 D_0044692C, D_0044692E, D_00446930, D_00446932, D_00446934, D_00446936, D_00446938;
extern s16 D_0044693A, D_0044693C, D_0044693E, D_00446940, D_00446942, D_00446944, D_00446946;
extern u8 D_00349690[0x2000];
extern u8 D_0034B690[0x40];
extern u8 D_0034B6D0[0xC80];
extern u8 D_0034C350[0x190];
extern u8 D_0034C4E0[0x32];
extern u8 D_0034C518[0x400];
extern u8 D_0034C918[0x10];
extern u8 D_0034C928[0x8000];
extern u8 D_00354928[0x2000];
extern u8 D_00356928[0x2000];
extern u8 D_00358928[0xA000];
extern u8 D_00362928[0x5000];
extern u8 D_00367928[0x14];
extern u8 spell_experience[0x3C];
extern u8 spell_levels[0x1E];
extern u8 D_003679A0[0x940];
extern u8 D_003682F0[0x1540];
extern u8 D_00369830[0x17C];
extern u8 D_003699B0[0x17C];
extern u8 D_00369B30[0x17C];
extern u8 D_00369CB0[0x17C];
extern u8 D_00369E30[0x17C];
extern u8 D_00369FB0[0x17C];
extern u8 D_00421050[0x30];
extern s32 D_00446738;
extern s32 D_004469D0;
extern PlayerState D_00413F80;
extern GameDefinitionFlags D_004147F8[380];
extern char D_00446A28[8];
extern MapSlot D_0038CD20[];
extern volatile int D_00446770;
void func_0011CF88(void);

void func_00103C18(void) {
    u8 *save_cursor;
    u8 bit_byte;
    int map_tag;
    int i;
    int j;
    int k;
    int copy_size;
    int mask;
    u32 flags;
    int used;
    u32 size;
    int block_count;
    u8 *cursor;
    u8 *table;
    int sum;

    save_cursor = D_0036A130;
    func_0011CF88();
    memset(save_cursor, 0, sizeof(D_0036A130));
    save_cursor += 8;
    *(int *)D_0036A130 = 0x15400;
    memcpy(save_cursor, &D_00446A28, 8); save_cursor += 8;
    memcpy(save_cursor, D_00349680, 0x10); save_cursor += 0x10;
    memcpy(save_cursor, &D_0044692C, 2); save_cursor += 2;
    memcpy(save_cursor, &D_0044692E, 2); save_cursor += 2;
    memcpy(save_cursor, &D_00446930, 2); save_cursor += 2;
    memcpy(save_cursor, &D_00446932, 2); save_cursor += 2;
    memcpy(save_cursor, &D_00446934, 2); save_cursor += 2;
    memcpy(save_cursor, &D_00446936, 2); save_cursor += 2;
    memcpy(save_cursor, &D_00446938, 2); save_cursor += 2;
    memcpy(save_cursor, &D_0044693A, 2); save_cursor += 2;
    memcpy(save_cursor, &D_0044693C, 2); save_cursor += 2;
    memcpy(save_cursor, &D_0044693E, 2); save_cursor += 2;
    memcpy(save_cursor, &D_00446940, 2); save_cursor += 2;
    memcpy(save_cursor, &D_00446942, 2); save_cursor += 2;
    memcpy(save_cursor, &D_00446944, 2); save_cursor += 2;
    memcpy(save_cursor, &D_00446946, 2); save_cursor += 2;
    copy_size = 0xB40;
    memcpy(save_cursor, D_00349690, copy_size); save_cursor += copy_size;
    copy_size = 0x6C0;
    memcpy(save_cursor, D_00349690 + 0x1680, copy_size); save_cursor += copy_size;
    memcpy(save_cursor, D_0034B690, 0x40); save_cursor += 0x40;
    memcpy(save_cursor, D_0034B6D0, 0xC80); save_cursor += 0xC80;
    memcpy(save_cursor, D_0034C350, 0x190); save_cursor += 0x190;
    memcpy(save_cursor, D_0034C4E0, 0x32); save_cursor += 0x32;
    memcpy(save_cursor, D_0034C518, 0x400); save_cursor += 0x400;
    memcpy(save_cursor, D_0034C918, 0x10); save_cursor += 0x10;
    memcpy(save_cursor, D_00367928, 0x14); save_cursor += 0x14;
    copy_size = 0x2D00;
    memcpy(save_cursor, D_0034C928, copy_size); save_cursor += copy_size;
    copy_size = 0x1B00;
    memcpy(save_cursor, D_0034C928 + 0x5A00, copy_size); save_cursor += copy_size;
    copy_size = 0xB40;
    memcpy(save_cursor, D_00354928, copy_size); save_cursor += copy_size;
    copy_size = 0x6C0;
    memcpy(save_cursor, D_00354928 + 0x1680, copy_size); save_cursor += copy_size;
    copy_size = 0xB40;
    memcpy(save_cursor, D_00356928, copy_size); save_cursor += copy_size;
    copy_size = 0x6C0;
    memcpy(save_cursor, D_00356928 + 0x1680, copy_size); save_cursor += copy_size;
    copy_size = 0x3840;
    memcpy(save_cursor, D_00358928, copy_size); save_cursor += copy_size;
    copy_size = 0x21C0;
    memcpy(save_cursor, D_00358928 + 0x7080, copy_size); save_cursor += copy_size;
    copy_size = 0x1C20;
    memcpy(save_cursor, D_00362928, copy_size); save_cursor += copy_size;
    copy_size = 0x10E0;
    memcpy(save_cursor, D_00362928 + 0x3840, copy_size); save_cursor += copy_size;
    memcpy(save_cursor, spell_experience, 0x3C); save_cursor += 0x3C;
    memcpy(save_cursor, spell_levels, 0x1E); save_cursor += 0x1E;
    memcpy(save_cursor, D_003679A0, 0x94C); save_cursor += 0x94C;
    memcpy(save_cursor, D_003682F0, 0x1540); save_cursor += 0x1540;
    memcpy(save_cursor, D_00369830, 0x17C); save_cursor += 0x17C;
    memcpy(save_cursor, D_003699B0, 0x17C); save_cursor += 0x17C;
    memcpy(save_cursor, D_00369B30, 0x17C); save_cursor += 0x17C;
    memcpy(save_cursor, D_00369CB0, 0x17C); save_cursor += 0x17C;
    memcpy(save_cursor, D_00369E30, 0x17C); save_cursor += 0x17C;
    memcpy(save_cursor, D_00369FB0, 0x17C); save_cursor += 0x17C;
    memcpy(save_cursor, &D_00413F80, 0x10); save_cursor += 0x10;
    memcpy(save_cursor, (u8 *)&D_00413F80 + 0x10, 0x10); save_cursor += 0x10;
    memcpy(save_cursor, (u8 *)&D_00413F80 + 0x20, 4); save_cursor += 4;
    memcpy(save_cursor, (u8 *)&D_00413F80 + 0x24, 4); save_cursor += 4;
    memcpy(save_cursor, (u8 *)&D_00413F80 + 0x28, 4); save_cursor += 4;
    memcpy(save_cursor, (u8 *)&D_00413F80 + 0x2C, 4); save_cursor += 4;
    memcpy(save_cursor, (u8 *)&D_00413F80 + 0x30, 0xC); save_cursor += 0xC;
    memcpy(save_cursor, (u8 *)&D_00413F80 + 0x3C, 0x10); save_cursor += 0x10;
    memcpy(save_cursor, (u8 *)&D_00413F80 + 0x4C, 4); save_cursor += 4;
    memcpy(save_cursor, (u8 *)&D_00413F80 + 0x50, 4); save_cursor += 4;
    memcpy(save_cursor, (u8 *)&D_00413F80 + 0x54, 4); save_cursor += 4;
    memcpy(save_cursor, (u8 *)&D_00413F80 + 0xB4, 4); save_cursor += 4;
    memcpy(save_cursor, (u8 *)&D_00413F80 + 0xB8, 4); save_cursor += 4;
    memcpy(save_cursor, (u8 *)&D_00413F80 + 0xD4, 4); save_cursor += 4;
    memcpy(save_cursor, (u8 *)&D_00413F80 + 0xD8, 4); save_cursor += 4;
    map_tag = D_0038CD20[D_00446770].tag;
    memcpy(save_cursor, &map_tag, 4); save_cursor += 4;
    memcpy(save_cursor, D_00421050, 0x30); save_cursor += 0x30;
    memcpy(save_cursor, &D_004469D0, 4); save_cursor += 4;

    bit_byte = 0;
    mask = 1;
    for (i = 0; i < 380; i++) {
        flags = ((GameDefinitionFlags *)D_004147F8)[i].flags >> 18;
        if ((flags & 3) < 2 && (flags & 1)) {
            bit_byte |= mask;
        }
        if (i % 8 != 7) {
            mask = (mask << 1) & 0xFF;
        } else {
            memcpy(save_cursor, &bit_byte, 1);
            save_cursor++;
            bit_byte = 0;
            mask = 1;
        }
    }
    memcpy(save_cursor, &bit_byte, 1);
    save_cursor++;

    used = save_cursor - D_0036A130;
    size = (u32)(used + 0x7F) / 128 * 128;
    table = save_cursor + (size - used);
    *(int *)(D_0036A130 + 4) = size; // match: the header word is stored straight into the buffer
    block_count = size / 128;
    cursor = D_0036A130;
    for (j = 0; j < block_count; j++) {
        sum = 0;
        for (k = 0; k < 128; k++) {
            sum += *cursor++;
        }
        *table++ = sum;
    }
}
