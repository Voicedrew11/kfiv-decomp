/* The game module: new-game state, event flags, the save image, the screen fade and GS screen effects. */
#include "kf_file.h"
#include "kf_game.h"
#include "kf_player.h"
#include "kf_map.h"
#include "kf_gs.h"
#include "kf_entity.h"
#include "libvu0.h"

extern u8 D_00349680[0x10];             /* the options (func_00102930 sets their defaults) */
extern u8 D_00349690[0x2000];
extern u8 D_0034B690[0x40];
extern u8 D_0034B6D0[0xC80];
extern AreaFlagBits D_0034C350[50];     /* 400 flags per area, numbered 3000 + 50 * area */
extern u8 D_0034C4E0[50];               /* nonzero: the area has a flag set */
extern u8 D_0034C518[0x400];
extern u8 D_0034C918[0x10];             /* 128 flags, one bit each */
extern u8 D_0034C928[0x8000];           /* four flag bits per nibble, 0x100 bytes per map */
extern u8 D_00354928[0x80][0x40];       /* a 4-bit counter per nibble, 128 per map */
extern u8 D_00356928[0x2000];
extern u8 D_00358928[0xA000];
extern u8 D_00362928[0x5000];
extern TriggerItemRecord D_00367928[TRIGGER_ITEM_RECORD_COUNT];
extern u16 spell_experience[6][5];
extern u8 spell_levels[6][5];
extern LevelRecord D_003679A0[70];
extern LevelRecord D_003682F0[160];
extern DefinitionByteSet D_00369830;
extern DefinitionByteSet D_003699B0;
extern DefinitionByteSet D_00369B30;
extern DefinitionByteSet D_00369CB0;
extern DefinitionByteSet D_00369E30;
extern DefinitionByteSet D_00369FB0;
extern u8 D_0036A130[0x20000];          /* the save image */
extern char D_0038A130[0x28];
extern MapSlot D_0038CD20[];
extern const DefinitionByteSet D_0043D9B0;
extern const DefinitionByteSet D_0043DB30;
extern const DefinitionByteSet D_0043DCB0;
extern const DefinitionByteSet D_0043DE30;
extern const DefinitionByteSet D_0043DFB0;
extern PlayerState D_00413F80;
extern GameDefinition D_004147F8[380];
extern TriggerTypeDef D_0041A6F8[150];
extern u8 D_00421050[0x30];
extern s16 D_0044692C, D_0044692E, D_00446930, D_00446932, D_00446934, D_00446936, D_00446938;
extern s16 D_0044693A, D_0044693C, D_0044693E, D_00446940, D_00446942, D_00446944, D_00446946;
extern s32 D_00446738;
extern volatile int D_00446770;         // match: volatile keeps the load where the save writer reads it
extern s32 D_004469D0;
extern char D_00446A28[8];
extern int D_00447010;                  /* frames of the fade left */
extern int D_00447014;                  /* frames the fade takes */
extern int D_00468180[3];               /* the fade colour */

void *memcpy(void *, const void *, u32);
void func_0010A4E8(int, int, int, int, int);
void func_0011CF88(void);
void func_001DC0E0(void);
extern int D_0038A178[8];               /* the screen depth of each haze layer */
extern u32 D_0038A158[8];               /* the distances of the haze layers, nearest last */
extern float D_00420D00[];
extern float D_00420D30[];
extern float D_00420D40[];
extern sceVu0FMATRIX D_00431AF0;
extern sceVu0FMATRIX D_00432320;
extern sceVu0FMATRIX D_004323E0;
extern GsColor D_0043E450;
extern LayerOffsets D_0043E460;
extern char D_0043E470[];
extern ScreenRect D_0043E480;
extern ScreenRect D_0043E490;
extern int D_0044673C;
extern int D_00446740;                  /* nonzero: func_00108020 lays its grid out afresh */
extern u32 D_00446744;                  /* the number of haze layers */
extern u32 D_00446ED4;
extern int D_00446F78;
extern int D_00467680[8][11][2];
extern int D_00467940[8][11][2];
extern int D_00467C00[8][11][2];
extern float D_00467EC0[8][11][2];

float sqrtf(float);
int rand(void);
u32 fptoui(float);
void sceVu0TransMatrix(void *, void *, void *);
void sceVu0ViewScreenMatrix(void *, float, float, float, float, float, float, float, float, float);
void KF_Vu0_LoadMatrix(void *);
void KF_Vu0_ProjectToScreen(int *, void *);
void KF_Vector_MultiplyComponents(float *, float *, float *);
void KF_Vector_Scale(float *, float *, float);
void KF_Vector_Subtract(float *, float *, float *);
void func_00216778(void *, void *);
void func_00216C48(void *, void *, void *);
void func_00109650(u32 *, u32 *);
u32 *func_00109990(int);
void func_00109A50(void);
void func_00109A80(int, s64);
void func_00109AD0(void);
void func_00109E18(void);
void func_00109E88(void);
void func_0010A100(void *);
u16 *func_00217760(int, int);
static int func_001036F8(int map, int entry, int bit);
static void func_00103778(int map, int entry, int bit, int value);

/* sets the options to their defaults */
void func_00102930(void) {
    memset(D_00349680, 0, sizeof(D_00349680));
    D_00349680[0] = 1;
    D_00349680[1] = 0x40;
    D_00349680[2] = 0x40;
    D_00349680[3] = 0x40;
    D_00349680[4] = 1;
    D_00349680[5] = 1;
    D_00349680[6] = 1;
    D_00349680[7] = 1;
    D_00349680[8] = 0;
    D_00349680[9] = 1;
    D_00349680[10] = 0x88;
}

/* clears the state a new game starts from */
void func_001029A8(void) {
    int i;

    memset(D_00349690, 0, sizeof(D_00349690));
    memset(D_0034B690, 0, sizeof(D_0034B690));
    memset(D_0034B6D0, 0, sizeof(D_0034B6D0));
    memset(D_0034C350, 0, sizeof(D_0034C350));
    memset(D_0034C4E0, 0, sizeof(D_0034C4E0));
    D_0034C4E0[0] = 1;
    memset(D_0034C518, 0, sizeof(D_0034C518));
    memset(D_0034C918, 0, sizeof(D_0034C918));
    memset(D_0034C928, 0, sizeof(D_0034C928));
    memset(D_00354928, 0, sizeof(D_00354928));
    memset(D_00356928, 0, sizeof(D_00356928));
    memset(D_00358928, 0, sizeof(D_00358928));
    memset(D_00362928, 0, sizeof(D_00362928));
    memset(D_00367928, 0, sizeof(D_00367928));
    D_00367928[0].map_tag = 0x66;
    D_00367928[0].trigger_id = 200;
    D_00367928[0].item = 0x87;
    D_00367928[1].map_tag = 0xD;
    D_00367928[1].trigger_id = 200;
    D_00367928[1].item = 0x88;
    memset(D_003679A0, 0, sizeof(D_003679A0));
    memset(D_003682F0, 0, sizeof(D_003682F0));
    for (i = 0; i < 70; i++) {
        D_003679A0[i].level = D_004147F8[150 + i].unk3C;
    }
    for (i = 0; i < 160; i++) {
        D_003682F0[i].level = D_004147F8[220 + i].unk3C;
    }
    memset(&D_00369830, 0, sizeof(D_00369830));
    D_003699B0 = D_0043D9B0;
    D_00369B30 = D_0043DB30;
    D_00369CB0 = D_0043DCB0;
    D_00369E30 = D_0043DE30;
    D_00369FB0 = D_0043DFB0;
    memset(spell_experience, 0, sizeof(spell_experience));
    memset(spell_levels, 0, sizeof(spell_levels));
    func_001DC0E0();
    memset(D_0038A130, 0, sizeof(D_0038A130));
}

/* marks what the player has equipped as found and tidies the spell tables */
void func_001030B8(void) {
    int i;
    int j;
    int level;

    if (D_00413F80.unk3C != 0) {
        D_00369830.bytes[D_00413F80.unk3C + 150] = 1;
    }
    if (D_00413F80.equipment[0] != 0) {
        D_00369830.bytes[D_00413F80.equipment[0] + 220] = 1;
    }
    if (D_00413F80.equipment[1] != 30) {
        D_00369830.bytes[D_00413F80.equipment[1] + 220] = 1;
    }
    if (D_00413F80.equipment[2] != 55) {
        D_00369830.bytes[D_00413F80.equipment[2] + 220] = 1;
    }
    if (D_00413F80.equipment[3] != 80) {
        D_00369830.bytes[D_00413F80.equipment[3] + 220] = 1;
    }
    if (D_00413F80.equipment[4] != 105) {
        D_00369830.bytes[D_00413F80.equipment[4] + 220] = 1;
    }
    if (D_00413F80.equipment[5] != 130) {
        D_00369830.bytes[D_00413F80.equipment[5] + 220] = 1;
    }
    if (D_00413F80.equipment[6] != 140) {
        D_00369830.bytes[D_00413F80.equipment[6] + 220] = 1;
    }
    if (D_00413F80.equipment[7] != 150) {
        D_00369830.bytes[D_00413F80.equipment[7] + 220] = 1;
    }
    for (i = 0; i < 150; i++) {
        if (D_00369830.bytes[i] >= 2 && (D_0041A6F8[i].flags >> 1 & 1)) {
            D_00369830.bytes[i] = 1;
        }
    }
    for (i = 150; i < 220; i++) {
        if (D_00369830.bytes[i] >= 2) {
            D_00369830.bytes[i] = 1;
        }
    }
    for (i = 220; i < 380; i++) {
        if (D_00369830.bytes[i] >= 2) {
            D_00369830.bytes[i] = 1;
        }
    }
    D_00369830.bytes[0] = 1;
    D_00369830.bytes[150] = 1;
    D_00369830.bytes[220] = 1;
    D_00369830.bytes[250] = 1;
    D_00369830.bytes[275] = 1;
    D_00369830.bytes[300] = 1;
    D_00369830.bytes[325] = 1;
    D_00369830.bytes[350] = 1;
    D_00369830.bytes[360] = 1;
    D_00369830.bytes[370] = 1;
    for (i = 0; i < 6; i++) {
        for (j = 0; j < 5; j++) {
            if (spell_levels[i][j] == 0) {
                for (; j < 5; j++) {
                    spell_levels[i][j] = 0;
                }
            }
        }
    }
    // match: through an int; a bare byte compare is narrowed and gives bnez, not bgtz
    if (D_00413F80.unk3D == 0 || (level = spell_levels[(D_00413F80.unk3D - 1) / 15][(D_00413F80.unk3D - 1) / 3 % 5]) <= 0) {
        D_00413F80.unk3D = 0;
    }
    for (i = 0; i < 6; i++) {
        for (j = 0; j < 5; j++) {
            if (spell_experience[i][j] == 0) {
                switch (spell_levels[i][j]) {
                case 0:
                    spell_experience[i][j] = 0;
                    break;
                case 1:
                    spell_experience[i][j] = 0;
                    break;
                case 2:
                    spell_experience[i][j] = 50;
                    break;
                case 3:
                    spell_experience[i][j] = 150;
                    break;
                default:
                    spell_experience[i][j] = 0;
                    spell_levels[i][j] = 0;
                    break;
                }
            }
        }
    }
    D_00413F80.unkB4 = D_00413F80.unk20;
    D_00413F80.unkB8 = D_00413F80.unk24;
}

/* one of the 128 flags in D_0034C918 */
int func_001034A8(int flag) {
    if ((u32)flag < 0x80U) {
        return D_0034C918[flag / 8] >> (flag % 8) & 1;
    }
    return 0;
}

void func_001034F8(int flag) {
    if ((u32)flag < 0x80U) {
        D_0034C918[flag / 8] |= 1 << (flag % 8);
    }
}

/* the 4-bit counter of an entry on a map */
int func_00103548(int map, int entry) {
    int count;

    if ((u32)map < 0x80U && entry >= 0 && entry < 0x80) {
        count = D_00354928[map][entry / 2];
        if (entry & 1) {
            count >>= 4;
        } else {
            count &= 0xF;
        }
        return count;
    }
    return 0;
}

/* adds one to the 4-bit counter of an entry on a map */
void func_001035A8(u32 map, int entry) {
    u32 shifted;
    u32 old_byte;
    u32 low_nibble;
    u32 count;
    int merged;

    if (map < 0x80 && entry >= 0 && entry < 0x80) {
        count = (old_byte = ((u8 *)D_00354928)[(entry / 2) + (map << 6)]);
        if (entry & 1) {
            count >>= 4;
            count++;
            shifted = count << 4;
            ((u8 *)D_00354928)[(entry / 2) + (map << 6)] = (low_nibble = old_byte & 0xF);
            count = shifted & 0xFF;
        } else {
            count &= 0xF;
            count++;
            ((u8 *)D_00354928)[(entry / 2) + (map << 6)] = old_byte & 0xF0;
        }
        merged = count | ((u8 *)D_00354928)[(entry / 2) + (map << 6)]; // match: merged in an int
        ((u8 *)D_00354928)[(entry / 2) + (map << 6)] = merged;
    }
}

void func_00103648(int map, int entry, u32 bit, int value) {
    if (bit < 2) {
        func_00103778(map, entry, bit, value);
    }
}

int func_00103668(int map, int entry, u32 bit) {
    if (bit < 2) {
        return func_001036F8(map, entry, bit);
    }
    return 0;
}

int func_00103698(int map, int entry) {
    return func_001036F8(map, entry, 2);
}

void func_001036B8(int map, int entry, int value) {
    func_00103778(map, entry, 2, value);
}

int func_001036C8(int map, int entry) {
    return func_001036F8(map, entry, 3);
}

void func_001036E8(int map, int entry, int value) {
    func_00103778(map, entry, 3, value);
}

/* one of the four flag bits of an entry on a map */
static int func_001036F8(int map, int entry, int bit) {
    u32 mask = 1;

    if ((u32)map >= 0x80) {
        return 0;
    }
    if ((u32)entry >= 0x200) {
        return 0;
    }
    if ((u32)bit >= 4) {
        return 0;
    }
    mask = (mask << bit) & 0xFF;
    mask = (mask << ((entry % 2) * 4)) & 0xFF;
    return (D_0034C928[entry / 2 + map * 0x100] & mask) != 0;
}

static void func_00103778(int map, int entry, int bit, int value) {
    if ((u32)map < 0x80 && (u32)entry < 0x200 && (u32)bit < 4) {
        u8 mask = (u8)(1 << bit);
        u8 *flag_byte;

        mask = (u8)(mask << ((entry % 2) * 4));
        flag_byte = &D_0034C928[entry / 2 + map * 0x100];
        *flag_byte = *flag_byte & ~mask;
        if (value != 0) {
            *flag_byte = *flag_byte | mask;
        }
    }
}

/* one of the area flags, numbered from 3000 */
int func_001037F8(int flag) {
    int bit = flag % 50;
    int area = (flag - 3000) / 50;

    return (D_0034C350[area].bits[bit / 8] >> (bit % 8)) & 1;
}

void func_00103858(int flag) {
    int area;
    u8 mask;

    if (flag == 0x138B) {
        func_00103858(0x138C);
        func_00103858(0x138D);
        func_00103858(0x138E);
        return;
    }
    if (flag == 0x13F0) {
        func_00103858(0x13F1);
        func_00103858(0x13F2);
        func_00103858(0x13F3);
        return;
    }
    if (flag == 0x1421) {
        func_00103858(0x1422);
        func_00103858(0x1423);
        func_00103858(0x1424);
        return;
    }
    area = (flag - 3000) / 50;
    flag %= 50;
    mask = 1 << (flag % 8);
    if (flag < 3) {
        D_0034C350[area].bits[0] = (D_0034C350[area].bits[0] & 0xF8) | mask;
    } else {
        D_0034C350[area].bits[flag / 8] |= mask;
    }
    if (D_0034C4E0[area] == 0) {
        D_0034C4E0[area] = 1;
    }
}

/* moves an area's flags to another area */
void func_00103990(u32 source_area, u32 dest_area) {
    if (source_area < 50 && dest_area < 50) {
        D_0034C4E0[dest_area] = D_0034C4E0[source_area];
        D_0034C350[dest_area] = D_0034C350[source_area];
        memset(&D_0034C350[source_area], 0, sizeof(AreaFlagBits));
        D_0034C4E0[source_area] = 0;
    }
}

/* finds where an item was placed; returns whether it was */
int func_00103A18(int item, int *map_tag, int *trigger_id) {
    TriggerItemRecord *record;
    int i;

    if (item == 0) {
        return 0;
    }
    record = D_00367928;
    for (i = 0; i < 5; i++, record++) {
        if (record->item == item) {
            u8 tag;

            *trigger_id = record->trigger_id;
            tag = record->map_tag;
            *map_tag = tag;
            if ((u32)(tag - 0x11) < 3) {
                switch ((u8)D_0034B690[6]) {
                case 0:
                    *map_tag = 0x11;
                    break;
                case 1:
                    *map_tag = 0x12;
                    break;
                case 2:
                    *map_tag = 0x13;
                    break;
                }
            }
            return 1;
        }
    }
    return 0;
}

/* the item recorded for a trigger, 0 if there is none */
int func_00103AE0(int map_tag, int trigger_id) {
    TriggerItemRecord *record;
    int i;

    if ((u32)(map_tag - 0x11) < 3) {
        map_tag = 0x11;
    }
    record = D_00367928;
    for (i = 0; i < TRIGGER_ITEM_RECORD_COUNT; i++, record++) {
        if (record->map_tag == map_tag && record->trigger_id == trigger_id) {
            return record->item;
        }
    }
    return 0;
}

/* frees the record of a trigger; returns whether there was one */
int func_00103B38(int map_tag, int trigger_id) {
    TriggerItemRecord *record;
    int i;

    if ((u32)(map_tag - 0x11) < 3) {
        map_tag = 0x11;
    }
    record = D_00367928;
    for (i = 0; i < TRIGGER_ITEM_RECORD_COUNT; i++, record++) {
        if (record->map_tag == map_tag && record->trigger_id == trigger_id) {
            memset(record, 0, sizeof(TriggerItemRecord));
            return TRUE;
        }
    }
    return FALSE;
}

/* records an item for a trigger in the first free record; returns whether there was room */
int func_00103BA8(int map_tag, int trigger_id, int item) {
    TriggerItemRecord *record;
    int i;

    if ((u32)(map_tag - 0x11) < 3) {
        map_tag = 0x11;
    }
    record = D_00367928;
    for (i = 0; i < TRIGGER_ITEM_RECORD_COUNT; i++, record++) {
        if (record->map_tag == 0) {
            if (record->trigger_id == 0 && record->item == 0) {
                record->map_tag = map_tag;
                record->trigger_id = trigger_id;
                record->item = item;
                return TRUE;
            }
        }
    }
    return FALSE;
}

/* writes the game into the save image, then the checksum table */
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
    memcpy(save_cursor, D_00354928[0x5A], copy_size); save_cursor += copy_size;
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
    memcpy(save_cursor, &D_00369830, 0x17C); save_cursor += 0x17C;
    memcpy(save_cursor, &D_003699B0, 0x17C); save_cursor += 0x17C;
    memcpy(save_cursor, &D_00369B30, 0x17C); save_cursor += 0x17C;
    memcpy(save_cursor, &D_00369CB0, 0x17C); save_cursor += 0x17C;
    memcpy(save_cursor, &D_00369E30, 0x17C); save_cursor += 0x17C;
    memcpy(save_cursor, &D_00369FB0, 0x17C); save_cursor += 0x17C;
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

/* reads the game back from the save image */
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
    memcpy(D_00354928[0x5A], save_cursor, copy_size); save_cursor += copy_size;
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
    memcpy(&D_00369830, save_cursor, 0x17C); save_cursor += 0x17C;
    memcpy(&D_003699B0, save_cursor, 0x17C); save_cursor += 0x17C;
    memcpy(&D_00369B30, save_cursor, 0x17C); save_cursor += 0x17C;
    memcpy(&D_00369CB0, save_cursor, 0x17C); save_cursor += 0x17C;
    memcpy(&D_00369E30, save_cursor, 0x17C); save_cursor += 0x17C;
    memcpy(&D_00369FB0, save_cursor, 0x17C); save_cursor += 0x17C;
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

/* checks the save image: the magic word and a byte sum per 128-byte block */
int func_001060C0(void) {
    UnalignedInt header[2];
    int block_count;
    int i;
    u8 *base;
    u8 *cursor;
    u8 *sum_table;
    u8 *block_byte;
    int sum;
    int byte_index;

    cursor = D_0036A130;
    header[0] = *(UnalignedInt *)D_0036A130;
    base = cursor;
    cursor = base + 4;
    if (header[0].value != 0x15400) {
        return 0;
    }
    header[1] = *(UnalignedInt *)cursor;
    block_count = (u32)header[1].value >> 7;
    block_byte = base;
    sum_table = base + header[1].value;
    for (i = 0; i < block_count; i++) {
        sum = 0;
        for (byte_index = 0x7F; byte_index >= 0; byte_index--) {
            sum += *block_byte++;
        }
        sum &= 0xFF;
        if (sum != *sum_table) {
            return 0;
        }
        sum_table++;
    }
    return 1;
}

/* starts a fade from a colour to the picture over some frames */
void func_00106178(int red, int green, int blue, int frames) {
    int *color = D_00468180;

    D_00447000[3] = 1;
    D_00468180[0] = red;
    D_00447010 = frames;
    color[1] = green;
    D_00447014 = (frames > 0) ? frames : 1;
    color[2] = blue;
}

/* starts a fade from the picture to a colour over some frames */
void func_001061B0(int red, int green, int blue, int frames) {
    int *color = D_00468180;

    color[0] = red;
    D_00447000[3] = -1;
    color[1] = green;
    color[2] = blue;
    D_00447014 = (frames > 0) ? frames : 1;
    D_00447010 = frames;
}

/* counts the fade down a frame */
void func_001061E8(void) {
    if (D_00447000[3] != 0 && D_00447010 > 0) {
        D_00447010--;
    }
}

/* draws the fade */
void func_00106208(void) {
    char unused_stack[0x40]; // match: retail reserves the 0x40 bytes
    int alpha;

    switch (D_00447000[3]) {
    case 0:
        return;
    case 1:
        alpha = (D_00447010 << 7) / D_00447014;
        break;
    case -1:
        alpha = 0x80 - (D_00447010 << 7) / D_00447014;
        break;
    default:
        return;
    }
    if (alpha > 0) {
        func_00109A50();
        func_00109A80(GS_REG_TEST_1, 0x30000);
        func_00109A80(GS_REG_ZBUF_1, 0x10A000118L);
        func_00109AD0();
        func_0010A4E8(D_00468180[0], D_00468180[1], D_00468180[2], alpha, 0x44);
        func_00109A50();
        func_00109A80(GS_REG_ZBUF_1, 0xA000118);
        func_00109AD0();
    }
}

/* loads the screen matrix and projects the haze layer distances D_0038A158 to screen depths */
void func_00106318(void) {
    sceVu0FMATRIX screen_matrix;
    sceVu0FMATRIX work_matrix;
    sceVu0FMATRIX view_matrix;
    sceVu0FVECTOR translation;
    VuIntVector screen_xyz;
    int i;

    memset(translation, 0, sizeof(translation));
    translation[3] = 1.0f;
    sceVu0TransMatrix(work_matrix, D_00431AF0, translation);
    sceVu0RotMatrixY(work_matrix, D_00432320, 0.0f);
    sceVu0RotMatrixX(work_matrix, D_00432320, 0.0f);
    sceVu0ViewScreenMatrix(view_matrix, 512.0f, 1.0f, 1.0f, 2048.0f, 2048.0f, 1.0f, 1048560.0f, 1.0f, 65535.0f);
    sceVu0MulMatrix(screen_matrix, view_matrix, work_matrix);
    sceVu0MulMatrix(screen_matrix, screen_matrix, D_00431AF0);
    KF_Vu0_LoadMatrix(screen_matrix);
    D_00446744 = 0;
    for (i = 0; i < 8; i++) {
        if (D_0038A158[i] == 0) {
            break;
        }
        if (i > 0 && D_0038A158[i] >= D_0038A158[i - 1]) {
            break;
        }
        translation[2] = D_0038A158[i];
        KF_Vu0_ProjectToScreen(screen_xyz, translation);
        screen_xyz[2] /= 16;
        D_0038A178[i] = screen_xyz[2];
        D_00446744++;
    }
}

/* draws the D_00446744 small sprites whose screen depths are in D_0038A178 */
void func_00106520(void) {
    GsColor color = D_0043E450;
    int texture_base;
    int texture_height_log2;
    u32 i;
    u64 tex0_low;
    u32 *packet_start;
    u32 *cursor;

    texture_base = 0x1180;
    if (D_00446F78 != 0) {
        texture_base = 0;
    }
    if (D_00446744 == 0) {
        return;
    }
    func_00109A50();
    func_00109A80(GS_REG_ALPHA_1, 0x80000000A8L);
    func_00109A80(GS_REG_TEST_1, 0x51001);
    func_00109A80(GS_REG_CLAMP_1, 5);
    func_00109AD0();

    for (i = 0; i < D_00446744; i++) {
        packet_start = cursor = func_00109990(0xD);
        *cursor++ = 0x1000000B;
        *cursor++ = 0;
        *cursor++ = 0;
        *cursor++ = 0;
        *cursor++ = 0;
        *cursor++ = 0;
        *cursor++ = 0;
        *cursor++ = 0x6C09C000;
        *(u64 *)cursor = 0x80AF400000008001L;
        cursor += 2;
        *(u64 *)cursor = 0x513513EEL;
        cursor += 2;
        texture_height_log2 = 9;
        tex0_low = texture_base | 0x28028000L;
        *(u64 *)cursor = tex0_low | ((u64)texture_height_log2 << 30);
        cursor += 2;
        *cursor++ = 6;
        *cursor++ = 0;
        *(u64 *)cursor = 0x60;
        cursor += 2;
        *cursor++ = 0x14;
        *cursor++ = 0;
        *cursor++ = 0;
        *cursor++ = 0;
        *cursor++ = 0;
        *cursor++ = 0;
        *(Uint128 *)cursor = *(Uint128 *)&color;
        cursor += 4;
        *cursor++ = 0x6BF8;
        *cursor++ = 0x7200;
        *cursor++ = D_0038A178[i];
        *cursor++ = 0;
        *cursor++ = 0x2800;
        *cursor++ = 0x1C00;
        *cursor++ = 0;
        *cursor++ = 0;
        *(Uint128 *)cursor = *(Uint128 *)&color;
        cursor += 4;
        *cursor++ = 0x93F8;
        *cursor++ = 0x8E00;
        *cursor++ = D_0038A178[i];
        *cursor++ = 0;
        *cursor++ = 0;
        *cursor++ = 0;
        *cursor++ = 0;
        *cursor++ = 0x15000276;
        func_00109650(packet_start, cursor);
    }

    func_00109A50();
    func_00109A80(GS_REG_CLAMP_1, 0);
    func_00109AD0();
}

/* draws the D_00446744 sprites in a strip, each at a column offset from the table D_0043E460 */
void func_001067E0(void) {
    LayerOffsets offset_table = D_0043E460;
    GsSpritePacket sprite;
    int texture_base;
    int texture_height_log2;
    u64 tex0_low;
    u32 i;
    u32 offset_index;
    int x_offset;
    int depth;

    texture_base = 0x1180;
    if (D_00446F78 != 0) {
        texture_base = 0;
    }
    func_00109A50();
    func_00109A80(GS_REG_ALPHA_1, 0x80000000a8L);
    func_00109A80(GS_REG_TEST_1, 0x51001);
    func_00109A80(GS_REG_CLAMP_1, 5);
    func_00109AD0();
    texture_height_log2 = 9;
    tex0_low = texture_base | 0x28028000L;
    sprite.tex0 = tex0_low | ((u64)texture_height_log2 << 30);
    sprite.unk08 = 0x60L;
    sprite.prim = 0x156;
    sprite.rgba[0] = 0x80;
    sprite.rgba[1] = 0x80;
    sprite.rgba[2] = 0x80;
    sprite.rgba[3] = 0x80;
    i = 0;
    if (D_00446744 != 0) {
        do {
            offset_index = D_00446ED4 + i;
            x_offset = offset_table.x[offset_index % 3];
            depth = D_0038A178[i];
            sprite.corner[1].texel_x = 0x2800;
            sprite.corner[0].x = x_offset + 0x6bf8;
            sprite.corner[1].texel_y = 0x1c00;
            sprite.corner[0].texel_x = 0;
            sprite.corner[0].texel_y = 0;
            sprite.corner[0].z = depth;
            sprite.corner[0].y = 0x7200;
            sprite.corner[1].y = 0x8e00;
            sprite.corner[1].x = x_offset - 0x6c08;
            sprite.corner[1].z = depth;
            func_0010A100(&sprite);
            i++;
        } while (i < D_00446744);
    }
    func_00109A50();
    func_00109A80(GS_REG_CLAMP_1, 0);
    func_00109AD0();
}

/* draws one sprite at the depth in D_0044673C; the pad's up and down buttons step that depth */
void func_00106980(void) {
    u32 *cursor;
    int texture_height_log2;
    u32 *packet_start;
    GsColor color = D_0043E450;
    int texture_base;
    u16 pad_buttons;
    u64 tex0_low;

    texture_base = 0x1180;
    if (D_00446F78 != 0) {
        texture_base = 0;
    }
    pad_buttons = func_00217760(1, 0)[1];
    if (pad_buttons & 0x400) {
        D_0044673C += 0xFF;
        printf(D_0043E470, D_0044673C);
    }
    if (pad_buttons & 0x100) {
        D_0044673C -= 0xFF;
        printf(D_0043E470, D_0044673C);
    }
    func_00109A50();
    func_00109A80(GS_REG_ALPHA_1, 0x80000000A8L);
    func_00109A80(GS_REG_TEST_1, 0x51001);
    func_00109A80(GS_REG_CLAMP_1, 5);
    func_00109AD0();

    packet_start = cursor = func_00109990(0xD);
    *cursor++ = 0x1000000B;
    *cursor++ = 0;
    *cursor++ = 0;
    *cursor++ = 0;
    *cursor++ = 0;
    *cursor++ = 0;
    *cursor++ = 0;
    *cursor++ = 0x6C09C000;
    *(u64 *)cursor = 0x80AF400000008001L;
    cursor += 2;
    *(u64 *)cursor = 0x513513EEL;
    cursor += 2;
    texture_height_log2 = 9;
    tex0_low = texture_base | 0x28028000L;
    *(u64 *)cursor = tex0_low | ((u64)texture_height_log2 << 30);
    cursor += 2;
    *cursor++ = 6;
    *cursor++ = 0;
    *(u64 *)cursor = 0x60;
    cursor += 2;
    *cursor++ = 0x14;
    *cursor++ = 0;
    *cursor++ = 0;
    *cursor++ = 0;
    *cursor++ = 0;
    *cursor++ = 0;
    *(Uint128 *)cursor = *(Uint128 *)&color;
    cursor += 4;
    *cursor++ = 0x6BF8;
    *cursor++ = 0x7200;
    *cursor++ = D_0044673C;
    *cursor++ = 0;
    *cursor++ = 0x2800;
    *cursor++ = 0x1C00;
    *cursor++ = 0;
    *cursor++ = 0;
    *(Uint128 *)cursor = *(Uint128 *)&color;
    cursor += 4;
    *cursor++ = 0x93F8;
    *cursor++ = 0x8E00;
    *cursor++ = D_0044673C;
    *cursor++ = 0;
    *cursor++ = 0;
    *cursor++ = 0;
    *cursor++ = 0;
    *cursor++ = 0x15000276;
    cursor[3] = 0;
    cursor[2] = 0;
    cursor[1] = 0;
    cursor[0] = 0;
    func_00109650(packet_start, cursor);

    func_00109A50();
    func_00109A80(GS_REG_CLAMP_1, 0);
    func_00109AD0();
}

/* draws a column of 10 gradient sprites, with the Z buffer off for the run */
void func_00106C58(void) {
    GsSpritePacket sprite;
    int texture_base;
    int remaining;
    int texture_height_log2;
    u64 tex0_low;

    texture_base = 0x1180;
    if (D_00446F78 != 0) {
        texture_base = 0;
    }
    func_00109A50();
    remaining = 9;
    func_00109A80(GS_REG_TEXFLUSH, 0xffffffffL);
    func_00109A80(GS_REG_ALPHA_1, 0x44);
    func_00109A80(GS_REG_TEST_1, 0x30000);
    func_00109A80(GS_REG_ZBUF_1, 0x10a000118L);
    func_00109AD0();
    texture_height_log2 = 9;
    tex0_low = texture_base | 0x28028000L;
    sprite.unk08 = 0;
    sprite.tex0 = tex0_low | ((u64)texture_height_log2 << 30);
    sprite.rgba[0] = 0x80;
    sprite.rgba[1] = 0x80;
    sprite.corner[1].z = 0xFFFFFFFF;
    sprite.prim = 0x156;
    sprite.rgba[2] = 0x80;
    sprite.corner[1].texel_x = 0;
    sprite.rgba[3] = 0x40;
    sprite.corner[1].texel_y = 0;
    sprite.corner[1].x = 0x6c00;
    sprite.corner[1].y = 0x7210;
    sprite.corner[0].x = 0x6ff8;
    sprite.corner[0].y = 0x8e08;
    sprite.corner[0].z = 0xFFFFFFFF;
    sprite.corner[0].texel_x = 0x400;
    sprite.corner[0].texel_y = 0x1c00;
    do {
        remaining--;
        func_0010A100(&sprite);
        sprite.corner[0].x += 0x400;
        sprite.corner[1].x += 0x400;
        sprite.corner[0].texel_x += 0x400;
        sprite.corner[1].texel_x += 0x400;
    } while (remaining >= 0);

    func_00109A50();
    func_00109A80(GS_REG_ZBUF_1, 0xa000118L);
    func_00109AD0();
}

/* draws two columns of 10 gradient sprites each, with the Z buffer off for the run */
void func_00106DC8(void) {
    GsSpritePacket sprite;
    int texture_base;
    int remaining;
    int texture_height_log2;
    u64 tex0_low;

    texture_base = 0x1180;
    if (D_00446F78 != 0) {
        texture_base = 0;
    }
    func_00109A50();
    remaining = 9;
    func_00109A80(GS_REG_TEXFLUSH, 0xffffffffL);
    func_00109A80(GS_REG_ALPHA_1, 0x44);
    func_00109A80(GS_REG_TEST_1, 0x30000);
    func_00109A80(GS_REG_ZBUF_1, 0x10a000118L);
    func_00109AD0();

    texture_height_log2 = 9;
    tex0_low = texture_base | 0x28028000L;
    sprite.unk08 = 0;
    sprite.tex0 = tex0_low | ((u64)texture_height_log2 << 30);
    sprite.rgba[0] = 0x80;
    sprite.rgba[1] = 0x80;
    sprite.corner[1].z = 0xFFFFFFFF;
    sprite.prim = 0x156;
    sprite.rgba[2] = 0x80;
    sprite.corner[1].texel_x = 0;
    sprite.rgba[3] = 0x40;
    sprite.corner[1].texel_y = 0;
    sprite.corner[1].x = 0x6c00;
    sprite.corner[1].y = 0x7210;
    sprite.corner[0].x = 0x6ff8;
    sprite.corner[0].y = 0x8e08;
    sprite.corner[0].z = 0xFFFFFFFF;
    sprite.corner[0].texel_x = 0x400;
    sprite.corner[0].texel_y = 0x1c00;

    do {
        remaining--;
        func_0010A100(&sprite);
        sprite.corner[0].x += 0x400;
        sprite.corner[1].x += 0x400;
        sprite.corner[0].texel_x += 0x400;
        sprite.corner[1].texel_x += 0x400;
    } while (remaining >= 0);

    sprite.corner[1].x = 0x7000;
    remaining = 9;
    sprite.corner[0].x = 0x6bf8;
    sprite.corner[1].y = 0x8df0;
    sprite.corner[0].y = 0x71e8;
    sprite.corner[1].texel_x = 0x400;
    sprite.corner[1].texel_y = 0x1c00;
    sprite.corner[0].texel_x = 0;
    sprite.corner[0].texel_y = 0;
    do {
        remaining--;
        func_0010A100(&sprite);
        sprite.corner[0].x += 0x400;
        sprite.corner[1].x += 0x400;
        sprite.corner[0].texel_x += 0x400;
        sprite.corner[1].texel_x += 0x400;
    } while (remaining >= 0);

    func_00109A50();
    func_00109A80(GS_REG_ZBUF_1, 0xa000118L);
    func_00109AD0();
}

/* draws one sprite from the screen rectangle D_0043E480 at the given alpha */
void func_00106FB8(int alpha) {
    ScreenRect screen_rect = D_0043E480;
    ScreenRect texel_rect = D_0043E480;
    int vertex_color[4] = { 0x80, 0x80, 0x80, alpha };
    int texture_base;
    int texture_height_log2;
    u64 tex0_low;
    u32 *packet_start;
    u32 *cursor;

    texture_base = 0x1180;
    if (D_00446F78 == 0) {
        texture_base = 0;
    }
    func_00109A50();
    func_00109A80(GS_REG_ALPHA_1, 0x44);
    func_00109A80(GS_REG_TEST_1, 0x51001);
    func_00109AD0();

    packet_start = cursor = func_00109990(0xD);
    *cursor++ = 0x1000000B;
    *cursor++ = 0;
    *cursor++ = 0;
    *cursor++ = 0;
    *cursor++ = 0;
    *cursor++ = 0;
    *cursor++ = 0;
    *cursor++ = 0x6C09C000;
    *(u64 *)cursor = 0x80AF400000008001L;
    cursor += 2;
    *(u64 *)cursor = 0x513513EEL;
    cursor += 2;
    texture_height_log2 = 9;
    tex0_low = texture_base | 0x28028000L;
    *(u64 *)cursor = tex0_low | ((u64)texture_height_log2 << 30);
    cursor += 2;
    *cursor++ = 6;
    *cursor++ = 0;
    *(u64 *)cursor = 0;
    cursor += 2;
    *cursor++ = 0x14;
    *cursor++ = 0;
    *cursor++ = texel_rect.left << 4;
    *cursor++ = texel_rect.top << 4;
    *cursor++ = 0;
    *cursor++ = 0;
    *(Uint128 *)cursor = *(Uint128 *)vertex_color;
    cursor += 4;
    *cursor++ = (screen_rect.left + 0x6C0) << 4;
    *cursor++ = (screen_rect.top + 0x720) << 4;
    *cursor++ = 0xFFFFFFFF;
    *cursor++ = 0;
    *cursor++ = texel_rect.right << 4;
    *cursor++ = texel_rect.bottom << 4;
    *cursor++ = 0;
    *cursor++ = 0;
    *(Uint128 *)cursor = *(Uint128 *)vertex_color;
    cursor += 4;
    *cursor++ = (screen_rect.right << 4) + 0x6BF8;
    *cursor++ = (screen_rect.bottom << 4) + 0x71F8;
    *cursor++ = 0xFFFFFFFF;
    *cursor++ = 0;
    *cursor++ = 0;
    *cursor++ = 0;
    *cursor++ = 0;
    *cursor++ = 0x15000276;
    cursor[3] = 0;
    cursor[2] = 0;
    cursor[1] = 0;
    cursor[0] = 0;
    func_00109650(packet_start, cursor);
}

/* draws one sprite from the screen rectangle D_0043E480, textured from D_0043E490, at the given alpha */
void func_00107270(int alpha) {
    ScreenRect screen_rect = D_0043E480;
    ScreenRect texel_rect = D_0043E490;
    int vertex_color[4] = { 0x80, 0x80, 0x80, alpha };
    int texture_base;
    int texture_height_log2;
    u64 tex0_low;
    u32 *packet_start;
    u32 *cursor;

    texture_base = 0x1180;
    if (D_00446F78 == 0) {
        texture_base = 0;
    }
    func_00109A50();
    func_00109A80(GS_REG_ALPHA_1, 0x44);
    func_00109A80(GS_REG_TEST_1, 0x51001);
    func_00109AD0();

    packet_start = cursor = func_00109990(0xD);
    *cursor++ = 0x1000000B;
    *cursor++ = 0;
    *cursor++ = 0;
    *cursor++ = 0;
    *cursor++ = 0;
    *cursor++ = 0;
    *cursor++ = 0;
    *cursor++ = 0x6C09C000;
    *(u64 *)cursor = 0x80AF400000008001L;
    cursor += 2;
    *(u64 *)cursor = 0x513513EEL;
    cursor += 2;
    texture_height_log2 = 9;
    tex0_low = texture_base | 0x28028000L;
    *(u64 *)cursor = tex0_low | ((u64)texture_height_log2 << 30);
    cursor += 2;
    *cursor++ = 6;
    *cursor++ = 0;
    *(u64 *)cursor = 0;
    cursor += 2;
    *cursor++ = 0x14;
    *cursor++ = 0;
    *cursor++ = texel_rect.left << 4;
    *cursor++ = texel_rect.top << 4;
    *cursor++ = 0;
    *cursor++ = 0;
    *(Uint128 *)cursor = *(Uint128 *)vertex_color;
    cursor += 4;
    *cursor++ = (screen_rect.left + 0x6C0) << 4;
    *cursor++ = (screen_rect.top + 0x720) << 4;
    *cursor++ = 0xFFFFFFFF;
    *cursor++ = 0;
    *cursor++ = texel_rect.right << 4;
    *cursor++ = texel_rect.bottom << 4;
    *cursor++ = 0;
    *cursor++ = 0;
    *(Uint128 *)cursor = *(Uint128 *)vertex_color;
    cursor += 4;
    *cursor++ = (screen_rect.right << 4) + 0x6BF8;
    *cursor++ = (screen_rect.bottom << 4) + 0x71F8;
    *cursor++ = 0xFFFFFFFF;
    *cursor++ = 0;
    *cursor++ = 0;
    *cursor++ = 0;
    *cursor++ = 0;
    *cursor++ = 0x15000276;
    cursor[3] = 0;
    cursor[2] = 0;
    cursor[1] = 0;
    cursor[0] = 0;
    func_00109650(packet_start, cursor);
}

/* draws the sight marker: a 20-strip sprite whose position follows the projected target */
void func_00107528(void) {
    float view_matrix[16];
    float scaled_vector[4];
    int screen_xy[2];
    GsSpritePacket sprite_packet;
    GsSpritePacket *strip_packet;
    int texture_base;
    int offset_x, offset_y;
    int strip;

    texture_base = D_00446F78 ? 0 : 0x1180;

    func_00109A50();
    func_00109A80(GS_REG_ALPHA_1, 10);
    func_00109A80(GS_REG_TEST_1, 0x30000);
    func_00109A80(GS_REG_ZBUF_1, 0x10A000118L);
    func_00109A80(GS_REG_SCANMSK, 3);
    func_00109AD0();

    sceVu0MulMatrix(view_matrix, D_004323E0, D_00431AF0);
    KF_Vu0_LoadMatrix(view_matrix);
    KF_Vector_Scale(scaled_vector, D_00420D30, 1000.0f);
    func_00216C48(scaled_vector, scaled_vector, D_00420D30 - 4);
    scaled_vector[3] = 1.0f;
    KF_Vu0_ProjectToScreen(screen_xy, scaled_vector);

    offset_x = screen_xy[0] - 0x8000;
    if (offset_x < 0) {
        offset_x = screen_xy[0] - 0x7FFF;
    }
    offset_y = screen_xy[1] - 0x8000;
    if (offset_y < 0) {
        offset_y = screen_xy[1] - 0x7FFF;
    }
    offset_x /= 2;
    offset_y /= 2;
    if (offset_x > 0x80) {
        offset_x = 0x80;
    } else if (offset_x < -0x80) {
        offset_x = -0x80;
    }
    if (offset_y > 0x80) {
        offset_y = 0x80;
    } else if (offset_y < -0x80) {
        offset_y = -0x80;
    }
    if (D_00420D40[0] < 125.0f) {
        offset_y = 0;
    }

    sprite_packet.prim = 0x116;
    sprite_packet.tex0 = 0x28028000L | texture_base;
    sprite_packet.tex0 |= 0xA400L << 20;
    sprite_packet.unk08 = 0;
    sprite_packet.rgba[0] = 0x80;
    sprite_packet.rgba[1] = 0x80;
    sprite_packet.rgba[2] = 0x80;
    sprite_packet.rgba[3] = 0x80;
    sprite_packet.corner[0].z = 0xFFFFFFFF;
    sprite_packet.corner[1].z = 0xFFFFFFFF;

    if (offset_x < 1) {
        sprite_packet.corner[0].x = 0x6BF8;
        sprite_packet.corner[1].x = 0x6E00;
        sprite_packet.corner[0].texel_x = 0;
        sprite_packet.corner[1].texel_x = 0x200;
    } else {
        sprite_packet.corner[0].x = 0x9400;
        sprite_packet.corner[1].x = 0x91F8;
        sprite_packet.corner[0].texel_x = 0x2800;
        sprite_packet.corner[1].texel_x = 0x2600;
    }
    if (offset_y < 1) {
        sprite_packet.corner[0].y = 0x71F8;
        sprite_packet.corner[1].y = 0x8E00;
        sprite_packet.corner[0].texel_y = 0;
        sprite_packet.corner[1].texel_y = 0x1C00;
    } else {
        sprite_packet.corner[0].y = 0x8E00;
        sprite_packet.corner[1].y = 0x71F8;
        sprite_packet.corner[0].texel_y = 0x1C00;
        sprite_packet.corner[1].texel_y = 0;
    }

    sprite_packet.corner[0].x += offset_x;
    sprite_packet.corner[1].x += offset_x;
    sprite_packet.corner[0].y += offset_y;
    sprite_packet.corner[1].y += offset_y;

    if (offset_x < 1) {
        offset_x = 0x200;
    } else {
        offset_x = -0x200;
    }

    strip_packet = &sprite_packet;
    strip = 0x13;
    do {
        strip--;
        func_0010A100(strip_packet);
        sprite_packet.corner[0].x += offset_x;
        sprite_packet.corner[1].x += offset_x;
        sprite_packet.corner[0].texel_x += offset_x;
        sprite_packet.corner[1].texel_x += offset_x;
    } while (strip >= 0);

    func_00109A50();
    func_00109A80(GS_REG_SCANMSK, 0);
    func_00109A80(GS_REG_ZBUF_1, 0xA000118L);
    func_00109AD0();
}

/* draws the target marker for the nearest entity in front of the camera, if any */
void func_00107840(void) {
    sceVu0FMATRIX view_matrix;
    sceVu0FVECTOR offset_to_entity;
    sceVu0FVECTOR scratch_vector;
    VuIntVector screen_position;
    GsSpritePacket sprite_packet;
    int texture_base;
    Entity *entity;
    float focus_distance, nearest_distance_squared, distance_squared;
    int offset_x, offset_y, strip;
    int texture_height_log2;
    u64 tex0_low;

    texture_base = 0x1180;
    if (D_00446F78 != 0) texture_base = 0;
    focus_distance = D_00420D00[18] > 1000.0f ? 1000.0f : D_00420D00[18];
    nearest_distance_squared = focus_distance * focus_distance;
    for (entity = D_004467E0; entity != NULL; entity = entity->next) {
        if (((int)(entity->flags >> 1) & 1) == 0) continue;
        if (focus_distance < entity->distance_to_player) continue;
        func_00216778(offset_to_entity, entity->model->position);
        offset_to_entity[1] -= entity->height * 0.5f;
        KF_Vector_Subtract(offset_to_entity, offset_to_entity, D_00420D00);
        KF_Vector_MultiplyComponents(scratch_vector, offset_to_entity, offset_to_entity);
        distance_squared = scratch_vector[0] + scratch_vector[1] + scratch_vector[2];
        if (nearest_distance_squared < distance_squared) continue;
        sceVu0Normalize(scratch_vector, offset_to_entity);
        if (sceVu0InnerProduct(scratch_vector, D_00420D00 + 4) < 0.86602497f) continue; /* about cos 30 degrees */
        nearest_distance_squared = distance_squared;
        focus_distance = sqrtf(distance_squared);
    }

    sceVu0MulMatrix(view_matrix, D_004323E0, D_00431AF0);
    KF_Vu0_LoadMatrix(view_matrix);
    KF_Vector_Scale(scratch_vector, D_00420D00 + 12, focus_distance);
    func_00216C48(scratch_vector, scratch_vector, D_00420D00 + 8);
    scratch_vector[3] = 1.0f;
    KF_Vu0_ProjectToScreen(screen_position, scratch_vector);

    offset_x = screen_position[0] - 0x8000;
    if (offset_x < 0) offset_x = screen_position[0] - 0x7FFF;
    offset_y = screen_position[1] - 0x8000;
    if (offset_y < 0) offset_y = screen_position[1] - 0x7FFF;
    offset_x /= 2;
    offset_y /= 2;
    if (offset_x > 0x80) offset_x = 0x80;
    else if (offset_x < -0x80) offset_x = -0x80;
    if (offset_y > 0x80) offset_y = 0x80;
    else if (offset_y < -0x80) offset_y = -0x80;

    func_00109A50();
    func_00109A80(GS_REG_TEST_1, 0x30000);
    func_00109A80(GS_REG_ZBUF_1, 0x10A000118L);
    func_00109A80(GS_REG_SCANMSK, 3);
    func_00109AD0();

    sprite_packet.prim = 0x116;
    tex0_low = texture_base | 0x28028000L;
    sprite_packet.unk08 = 0;
    sprite_packet.rgba[0] = 0x80;
    sprite_packet.rgba[1] = 0x80;
    sprite_packet.rgba[2] = 0x80;
    texture_height_log2 = 0x29;
    sprite_packet.corner[0].z = 0xFFFFFFFF;
    sprite_packet.tex0 = tex0_low | ((u64)texture_height_log2 << 30);
    sprite_packet.rgba[3] = 0x80;
    sprite_packet.corner[1].z = 0xFFFFFFFF;

    if (offset_x <= 0) {
        sprite_packet.corner[0].x = 0x6BF8;
        sprite_packet.corner[1].x = 0x6E00;
        sprite_packet.corner[1].texel_x = 0x200;
        sprite_packet.corner[0].texel_x = 0;
    } else {
        sprite_packet.corner[0].x = 0x9400;
        sprite_packet.corner[1].x = 0x91F8;
        sprite_packet.corner[0].texel_x = 0x2800;
        sprite_packet.corner[1].texel_x = 0x2600;
    }
    if (offset_y <= 0) {
        sprite_packet.corner[0].y = 0x71F8;
        sprite_packet.corner[1].y = 0x8E00;
        sprite_packet.corner[1].texel_y = 0x1C00;
        sprite_packet.corner[0].texel_y = 0;
    } else {
        sprite_packet.corner[0].y = 0x8E00;
        sprite_packet.corner[1].y = 0x71F8;
        sprite_packet.corner[0].texel_y = 0x1C00;
        sprite_packet.corner[1].texel_y = 0;
    }
    sprite_packet.corner[0].x += offset_x;
    sprite_packet.corner[1].x += offset_x;
    sprite_packet.corner[0].y += offset_y;
    sprite_packet.corner[1].y += offset_y;
    if (offset_x <= 0) offset_x = 0x200;
    else offset_x = -0x200;
    for (strip = 19; strip >= 0; strip--) {
        func_0010A100(&sprite_packet);
        sprite_packet.corner[0].x += offset_x;
        sprite_packet.corner[1].x += offset_x;
        sprite_packet.corner[0].texel_x += offset_x;
        sprite_packet.corner[1].texel_x += offset_x;
    }

    func_00109A50();
    func_00109A80(GS_REG_SCANMSK, 0);
    func_00109A80(GS_REG_ZBUF_1, 0xA000118L);
    func_00109AD0();
}

/* draws a column of 10 grey sprites at the given brightness, scrolling across the screen */
void func_00107CD8(int brightness) {
    char unused_stack[0x60]; // match: retail reserves the 0x60 bytes
    GsSpritePacket sprite;
    int texture_base;
    int remaining;
    int texture_height_log2;
    u64 tex0_low;

    texture_base = D_00446F78 ? 0 : 0x1180;
    remaining = 9;
    func_00109E88();
    func_00109E18();
    func_00109A50();
    func_00109A80(GS_REG_ALPHA_1, 10);
    func_00109A80(GS_REG_TEST_1, 0x30000);
    func_00109A80(GS_REG_ZBUF_1, 0x10A000118L);
    func_00109AD0();
    sprite.unk08 = 0;
    tex0_low = texture_base | 0x28028000L;
    texture_height_log2 = 9;
    sprite.tex0 = tex0_low | ((u64)texture_height_log2 << 30);
    sprite.prim = 0x116;
    sprite.rgba[1] = sprite.rgba[2] = brightness;
    sprite.rgba[0] = brightness;
    sprite.rgba[3] = 0x80;
    sprite.corner[0].z = 0xFFFFFFFF;
    sprite.corner[0].x = 0x6C00;
    sprite.corner[0].y = 0x7200;
    sprite.corner[1].x = 0x7000;
    sprite.corner[0].texel_x = 0;
    sprite.corner[1].y = 0x8DF8;
    sprite.corner[0].texel_y = 0;
    sprite.corner[1].z = 0xFFFFFFFF;
    sprite.corner[1].texel_x = 0x400;
    sprite.corner[1].texel_y = 0x1C00;
    do {
        remaining--;
        func_0010A100(&sprite);
        sprite.corner[0].x += 0x400;
        sprite.corner[1].x += 0x400;
        sprite.corner[0].texel_x += 0x400;
        sprite.corner[1].texel_x += 0x400;
    } while (remaining >= 0);

    func_00109A50();
    func_00109A80(GS_REG_ZBUF_1, 0xA000118L);
    func_00109AD0();
    func_00109E18();
    func_00109E88();
}

/* draws a column of 10 sprites in the given colour, scrolling across the screen */
void func_00107E70(int red, int green, int blue) {
    char unused_stack[0x60]; // match: retail reserves the 0x60 bytes
    GsSpritePacket sprite_packet;
    int texture_base;
    int remaining;

    texture_base = D_00446F78 ? 0 : 0x1180;
    remaining = 9;
    func_00109E88();
    func_00109E18();
    func_00109A50();
    func_00109A80(GS_REG_ALPHA_1, 10);
    func_00109A80(GS_REG_TEST_1, 0x30000);
    func_00109A80(GS_REG_ZBUF_1, 0x10A000118L);
    func_00109AD0();

    sprite_packet.tex0 = (0x28028000L | texture_base) | (0x9000L << 18);
    sprite_packet.unk08 = 0;
    sprite_packet.prim = 0x116;
    sprite_packet.rgba[0] = red;
    sprite_packet.rgba[1] = green;
    sprite_packet.rgba[2] = blue;
    sprite_packet.rgba[3] = 0x80;
    sprite_packet.corner[0].x = 0x6C00;
    sprite_packet.corner[0].y = 0x7200;
    sprite_packet.corner[1].x = 0x7000;
    {
        // match: the bottom y goes through a local of its own
        u32 bottom_y = 0x8DF8;
        sprite_packet.corner[1].y = bottom_y;
    }
    sprite_packet.corner[0].z = 0xFFFFFFFFU;
    sprite_packet.corner[1].z = 0xFFFFFFFFU;
    sprite_packet.corner[1].texel_x = 0x400;
    sprite_packet.corner[1].texel_y = 0x1C00;
    sprite_packet.corner[0].texel_x = 0;
    sprite_packet.corner[0].texel_y = 0;

    do {
        remaining--;
        func_0010A100(&sprite_packet);
        sprite_packet.corner[0].x += 0x400;
        sprite_packet.corner[1].x += 0x400;
        sprite_packet.corner[0].texel_x += 0x400;
        sprite_packet.corner[1].texel_x += 0x400;
    } while (remaining >= 0);

    func_00109A50();
    func_00109A80(GS_REG_ZBUF_1, 0xA000118L);
    func_00109AD0();
    func_00109E18();
    func_00109E88();
}

/* steps the wobbling grid (laid out afresh when D_00446740 is set) and draws its cells, blended by blend */
void func_00108020(int red, int green, int blue, int alpha, float blend) {
    char unused_stack[0x40]; // match: retail reserves the 0x40 bytes
    int j, i, k, corner_col;
    int origin_x, origin_y, target_x;
    s64 tex0_low;
    float force_x, force_y;
    u32 *packet_start;
    u32 *cursor;

    if (D_00446740 != 0) {
        for (i = 0; i < 8; i++) {
            for (j = 0; j < 11; j++) {
                D_00467680[i][j][0] = D_00467940[i][j][0] = j << 6;
                D_00467680[i][j][1] = D_00467940[i][j][1] = i << 6;
                if (j > 0 && j < 10) {
                    D_00467940[i][j][0] -= 8;
                    D_00467940[i][j][0] += (int)(rand() * 17.0f / 2147483648.0f);
                }
                if (i > 0 && i < 7) {
                    D_00467940[i][j][1] -= 8;
                    D_00467940[i][j][1] += (int)(rand() * 17.0f / 2147483648.0f);
                }
                D_00467940[i][j][0] <<= 4;
                D_00467940[i][j][1] <<= 4;
                D_00467680[i][j][0] <<= 4;
                D_00467680[i][j][1] <<= 4;
            }
        }
    }

    *(RippleGrid *)D_00467C00 = *(RippleGrid *)D_00467940; // match: copied whole, as quadwords

    for (i = 1; i < 7; i++) {
        for (j = 1; j < 10; j++) {
            force_x = force_y = 0.0f;
            force_x += D_00467C00[i - 1][j][0] * 0.1f;
            force_y += D_00467C00[i - 1][j][1] * 0.1f;
            force_x += D_00467C00[i + 1][j][0] * 0.1f;
            force_y += D_00467C00[i + 1][j][1] * 0.1f;
            force_x += D_00467C00[i][j - 1][0] * 0.1f;
            force_y += D_00467C00[i][j - 1][1] * 0.1f;
            force_x += D_00467C00[i][j + 1][0] * 0.1f;
            force_y += D_00467C00[i][j + 1][1] * 0.1f;
            force_x -= D_00467C00[i][j][0] * 0.4f;
            force_y -= D_00467C00[i][j][1] * 0.4f;
            D_00467EC0[i][j][0] += force_x;
            D_00467EC0[i][j][1] += force_y;
            D_00467940[i][j][0] += (int)(D_00467EC0[i][j][0] * 0.05f);
            D_00467940[i][j][1] += (int)(D_00467EC0[i][j][1] * 0.05f);
        }
    }

    func_00109E18();
    func_00109A50();
    func_00109A80(GS_REG_TEXFLUSH, 0xFFFFFFFF);
    func_00109A80(GS_REG_ALPHA_1, 0x44);
    func_00109A80(GS_REG_TEST_1, 0x30000);
    func_00109A80(GS_REG_ZBUF_1, 0x10A000118L);
    func_00109A80(GS_REG_TEX0_1, 0x26802ABC0L);
    func_00109A80(GS_REG_TEX1_1, 0);
    func_00109AD0();

    for (i = 0; i < 7; i++) {
        for (j = 0; j < 10; j++) {
            packet_start = cursor = func_00109990(0x11);
            *cursor++ = 0x1000000F;
            *cursor++ = 0;
            *cursor++ = 0;
            *cursor++ = 0;
            *cursor++ = 0;
            *cursor++ = 0;
            *cursor++ = 0;
            tex0_low = 0xC0AE400000008001L;
            *cursor++ = 0x6C0DC000;
            *(s64 *)cursor = tex0_low;
            cursor += 2;
            *(s64 *)cursor = 0x513513513513L;
            cursor += 2;
            for (k = 0; k < 2; k++) {
                for (corner_col = 0; corner_col < 2; corner_col++) {
                    target_x = D_00467940[i + k][j + corner_col][0];
                    origin_x = D_00467680[i + k][j + corner_col][0];
                    *cursor++ = origin_x + fptoui((target_x - origin_x) * blend);
                    origin_y = D_00467680[i + k][j + corner_col][1];
                    *cursor++ = origin_y + fptoui((D_00467940[i + k][j + corner_col][1] - origin_y) * blend);
                    *cursor++ = 0;
                    *cursor++ = 0;
                    *cursor++ = red;
                    *cursor++ = green;
                    *cursor++ = blue;
                    *cursor++ = alpha;
                    *cursor++ = (((j + corner_col) << 6) + 0x6C0) << 4;
                    *cursor++ = (((i + k) << 6) + 0x720) << 4;
                    *cursor++ = 0;
                    *cursor++ = 0;
                }
            }
            *cursor++ = 0;
            *cursor++ = 0;
            *cursor++ = 0;
            *cursor++ = 0x15000276;
            cursor[3] = 0;
            cursor[2] = 0;
            cursor[1] = 0;
            cursor[0] = 0;
            func_00109650(packet_start, cursor);
        }
    }

    D_00446740 = 0;
    func_00109A50();
    func_00109A80(GS_REG_ZBUF_1, 0xA000118);
    func_00109AD0();
    func_00109E18();
}
