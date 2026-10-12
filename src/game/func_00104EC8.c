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
    u8 *p;
    int magic;
    int stored_size;
    PlayTime play_time;
    u8 flag_bits;
    int i;
    u32 flags;
    int copy_size;

    p = D_0036A130;
    memcpy(&magic, p, 4); p += 4;
    memcpy(&stored_size, p, 4); p += 4;
    memcpy(&play_time, p, 8); p += 8;
    memcpy(D_00349680, p, 0x10); p += 0x10;
    memcpy(&D_0044692C, p, 2); p += 2;
    memcpy(&D_0044692E, p, 2); p += 2;
    memcpy(&D_00446930, p, 2); p += 2;
    memcpy(&D_00446932, p, 2); p += 2;
    memcpy(&D_00446934, p, 2); p += 2;
    memcpy(&D_00446936, p, 2); p += 2;
    memcpy(&D_00446938, p, 2); p += 2;
    memcpy(&D_0044693A, p, 2); p += 2;
    memcpy(&D_0044693C, p, 2); p += 2;
    memcpy(&D_0044693E, p, 2); p += 2;
    memcpy(&D_00446940, p, 2); p += 2;
    memcpy(&D_00446942, p, 2); p += 2;
    memcpy(&D_00446944, p, 2); p += 2;
    memcpy(&D_00446946, p, 2); p += 2;
    copy_size = 0xB40;
    memcpy(D_00349690, p, copy_size); p += copy_size;
    copy_size = 0x6C0;
    memcpy(D_00349690 + 0x1680, p, copy_size); p += copy_size;
    memcpy(D_0034B690, p, 0x40); p += 0x40;
    memcpy(D_0034B6D0, p, 0xC80); p += 0xC80;
    memcpy(D_0034C350, p, 0x190); p += 0x190;
    memcpy(D_0034C4E0, p, 0x32); p += 0x32;
    memcpy(D_0034C518, p, 0x400); p += 0x400;
    memcpy(D_0034C918, p, 0x10); p += 0x10;
    memcpy(D_00367928, p, 0x14); p += 0x14;
    copy_size = 0x2D00;
    memcpy(D_0034C928, p, copy_size); p += copy_size;
    copy_size = 0x1B00;
    memcpy(D_0034C928 + 0x5A00, p, copy_size); p += copy_size;
    copy_size = 0xB40;
    memcpy(D_00354928, p, copy_size); p += copy_size;
    copy_size = 0x6C0;
    memcpy(D_00354928 + 0x1680, p, copy_size); p += copy_size;
    copy_size = 0xB40;
    memcpy(D_00356928, p, copy_size); p += copy_size;
    copy_size = 0x6C0;
    memcpy(D_00356928 + 0x1680, p, copy_size); p += copy_size;
    copy_size = 0x3840;
    memcpy(D_00358928, p, copy_size); p += copy_size;
    copy_size = 0x21C0;
    memcpy(D_00358928 + 0x7080, p, copy_size); p += copy_size;
    copy_size = 0x1C20;
    memcpy(D_00362928, p, copy_size); p += copy_size;
    copy_size = 0x10E0;
    memcpy(D_00362928 + 0x3840, p, copy_size); p += copy_size;
    memcpy(spell_experience, p, 0x3C); p += 0x3C;
    memcpy(spell_levels, p, 0x1E); p += 0x1E;
    memcpy(D_003679A0, p, 0x94C); p += 0x94C;
    memcpy(D_003682F0, p, 0x1540); p += 0x1540;
    memcpy(D_00369830, p, 0x17C); p += 0x17C;
    memcpy(D_003699B0, p, 0x17C); p += 0x17C;
    memcpy(D_00369B30, p, 0x17C); p += 0x17C;
    memcpy(D_00369CB0, p, 0x17C); p += 0x17C;
    memcpy(D_00369E30, p, 0x17C); p += 0x17C;
    memcpy(D_00369FB0, p, 0x17C); p += 0x17C;
    memcpy(&D_00413F80, p, 0x10); p += 0x10;
    memcpy((u8 *)&D_00413F80 + 0x10, p, 0x10); p += 0x10;
    memcpy((u8 *)&D_00413F80 + 0x20, p, 4); p += 4;
    memcpy((u8 *)&D_00413F80 + 0x24, p, 4); p += 4;
    memcpy((u8 *)&D_00413F80 + 0x28, p, 4); p += 4;
    memcpy((u8 *)&D_00413F80 + 0x2C, p, 4); p += 4;
    memcpy((u8 *)&D_00413F80 + 0x30, p, 0xC); p += 0xC;
    memcpy((u8 *)&D_00413F80 + 0x3C, p, 0x10); p += 0x10;
    memcpy((u8 *)&D_00413F80 + 0x4C, p, 4); p += 4;
    memcpy((u8 *)&D_00413F80 + 0x50, p, 4); p += 4;
    memcpy((u8 *)&D_00413F80 + 0x54, p, 4); p += 4;
    memcpy((u8 *)&D_00413F80 + 0xB4, p, 4); p += 4;
    memcpy((u8 *)&D_00413F80 + 0xB8, p, 4); p += 4;
    memcpy((u8 *)&D_00413F80 + 0xD4, p, 4); p += 4;
    memcpy((u8 *)&D_00413F80 + 0xD8, p, 4); p += 4;
    memcpy(&D_00446738, p, 4); p += 4;
    memcpy(D_00421050, p, 0x30); p += 0x30;
    memcpy(&D_004469D0, p, 4); p += 4;

    for (i = 0; i < 380; i++) {
        if (i & 7) {
            flag_bits >>= 1;
        } else {
            memcpy(&flag_bits, p, 1);
            p++;
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
