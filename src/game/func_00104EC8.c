#include "kf_game.h"
#include "kf_player.h"

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

void func_00104EC8(void) {
    u8 *save_cursor;
    int magic;
    int stored_size;
    PlayTime play_time;
    u8 flag_bits;
    int i;
    u32 flags;
    int copy_size;

    save_cursor = D_0036A130;
    memcpy(&magic, save_cursor, 4); save_cursor += 4;
    memcpy(&stored_size, save_cursor, 4); save_cursor += 4;
    memcpy(&play_time, save_cursor, 8); save_cursor += 8;
    memcpy(D_00349680, save_cursor, 0x10); save_cursor += 0x10;
    memcpy(&D_0044692C, save_cursor, 2); save_cursor += 2;
    memcpy(&D_0044692E, save_cursor, 2); save_cursor += 2;
    memcpy(&D_00446930, save_cursor, 2); save_cursor += 2;
    memcpy(&D_00446932, save_cursor, 2); save_cursor += 2;
    memcpy(&D_00446934, save_cursor, 2); save_cursor += 2;
    memcpy(&D_00446936, save_cursor, 2); save_cursor += 2;
    memcpy(&D_00446938, save_cursor, 2); save_cursor += 2;
    memcpy(&D_0044693A, save_cursor, 2); save_cursor += 2;
    memcpy(&D_0044693C, save_cursor, 2); save_cursor += 2;
    memcpy(&D_0044693E, save_cursor, 2); save_cursor += 2;
    memcpy(&D_00446940, save_cursor, 2); save_cursor += 2;
    memcpy(&D_00446942, save_cursor, 2); save_cursor += 2;
    memcpy(&D_00446944, save_cursor, 2); save_cursor += 2;
    memcpy(&D_00446946, save_cursor, 2); save_cursor += 2;
    copy_size = 0xB40;
    memcpy(D_00349690, save_cursor, copy_size); save_cursor += copy_size;
    copy_size = 0x6C0;
    memcpy(D_00349690 + 0x1680, save_cursor, copy_size); save_cursor += copy_size;
    memcpy(D_0034B690, save_cursor, 0x40); save_cursor += 0x40;
    memcpy(D_0034B6D0, save_cursor, 0xC80); save_cursor += 0xC80;
    memcpy(D_0034C350, save_cursor, 0x190); save_cursor += 0x190;
    memcpy(D_0034C4E0, save_cursor, 0x32); save_cursor += 0x32;
    memcpy(D_0034C518, save_cursor, 0x400); save_cursor += 0x400;
    memcpy(D_0034C918, save_cursor, 0x10); save_cursor += 0x10;
    memcpy(D_00367928, save_cursor, 0x14); save_cursor += 0x14;
    copy_size = 0x2D00;
    memcpy(D_0034C928, save_cursor, copy_size); save_cursor += copy_size;
    copy_size = 0x1B00;
    memcpy(D_0034C928 + 0x5A00, save_cursor, copy_size); save_cursor += copy_size;
    copy_size = 0xB40;
    memcpy(D_00354928, save_cursor, copy_size); save_cursor += copy_size;
    copy_size = 0x6C0;
    memcpy(D_00354928 + 0x1680, save_cursor, copy_size); save_cursor += copy_size;
    copy_size = 0xB40;
    memcpy(D_00356928, save_cursor, copy_size); save_cursor += copy_size;
    copy_size = 0x6C0;
    memcpy(D_00356928 + 0x1680, save_cursor, copy_size); save_cursor += copy_size;
    copy_size = 0x3840;
    memcpy(D_00358928, save_cursor, copy_size); save_cursor += copy_size;
    copy_size = 0x21C0;
    memcpy(D_00358928 + 0x7080, save_cursor, copy_size); save_cursor += copy_size;
    copy_size = 0x1C20;
    memcpy(D_00362928, save_cursor, copy_size); save_cursor += copy_size;
    copy_size = 0x10E0;
    memcpy(D_00362928 + 0x3840, save_cursor, copy_size); save_cursor += copy_size;
    memcpy(spell_experience, save_cursor, 0x3C); save_cursor += 0x3C;
    memcpy(spell_levels, save_cursor, 0x1E); save_cursor += 0x1E;
    memcpy(D_003679A0, save_cursor, 0x94C); save_cursor += 0x94C;
    memcpy(D_003682F0, save_cursor, 0x1540); save_cursor += 0x1540;
    memcpy(D_00369830, save_cursor, 0x17C); save_cursor += 0x17C;
    memcpy(D_003699B0, save_cursor, 0x17C); save_cursor += 0x17C;
    memcpy(D_00369B30, save_cursor, 0x17C); save_cursor += 0x17C;
    memcpy(D_00369CB0, save_cursor, 0x17C); save_cursor += 0x17C;
    memcpy(D_00369E30, save_cursor, 0x17C); save_cursor += 0x17C;
    memcpy(D_00369FB0, save_cursor, 0x17C); save_cursor += 0x17C;
    memcpy(&D_00413F80, save_cursor, 0x10); save_cursor += 0x10;
    memcpy((u8 *)&D_00413F80 + 0x10, save_cursor, 0x10); save_cursor += 0x10;
    memcpy((u8 *)&D_00413F80 + 0x20, save_cursor, 4); save_cursor += 4;
    memcpy((u8 *)&D_00413F80 + 0x24, save_cursor, 4); save_cursor += 4;
    memcpy((u8 *)&D_00413F80 + 0x28, save_cursor, 4); save_cursor += 4;
    memcpy((u8 *)&D_00413F80 + 0x2C, save_cursor, 4); save_cursor += 4;
    memcpy((u8 *)&D_00413F80 + 0x30, save_cursor, 0xC); save_cursor += 0xC;
    memcpy((u8 *)&D_00413F80 + 0x3C, save_cursor, 0x10); save_cursor += 0x10;
    memcpy((u8 *)&D_00413F80 + 0x4C, save_cursor, 4); save_cursor += 4;
    memcpy((u8 *)&D_00413F80 + 0x50, save_cursor, 4); save_cursor += 4;
    memcpy((u8 *)&D_00413F80 + 0x54, save_cursor, 4); save_cursor += 4;
    memcpy((u8 *)&D_00413F80 + 0xB4, save_cursor, 4); save_cursor += 4;
    memcpy((u8 *)&D_00413F80 + 0xB8, save_cursor, 4); save_cursor += 4;
    memcpy((u8 *)&D_00413F80 + 0xD4, save_cursor, 4); save_cursor += 4;
    memcpy((u8 *)&D_00413F80 + 0xD8, save_cursor, 4); save_cursor += 4;
    memcpy(&D_00446738, save_cursor, 4); save_cursor += 4;
    memcpy(D_00421050, save_cursor, 0x30); save_cursor += 0x30;
    memcpy(&D_004469D0, save_cursor, 4); save_cursor += 4;

    for (i = 0; i < 380; i++) {
        if (i & 7) {
            flag_bits >>= 1;
        } else {
            memcpy(&flag_bits, save_cursor, 1);
            save_cursor++;
        }
        flags = ((GameDefinitionFlags *)D_004147F8)[i].flags;
        if (((flags >> 18) & 3) < 2) {
            if (flag_bits & 1) {
                ((GameDefinitionFlags *)D_004147F8)[i].flags = (flags & 0xFFF3FFFF) | 0x40000;
            } else {
                ((GameDefinitionFlags *)D_004147F8)[i].flags = flags & 0xFFF3FFFF;
            }
        }
    }
}
