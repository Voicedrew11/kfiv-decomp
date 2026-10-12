#include "kf_menu.h"
#include "kf_game.h"
#include "kf_map.h"
#include "kf_player.h"
#include "kf_effect.h"
#include "kf_texture.h"

extern FxTableEntry D_00402DA8[];
extern TriggerTypeDef D_0041A6F8[];
extern PlayerItemDef D_0041B9B8[];
extern LevelRecordDef D_0041E118[];
extern MenuItem D_004210D0[150];
extern MenuItem D_00421A30[380];
extern MenuItem D_004231F0[70];
extern MenuItem D_00423650[30];
extern MenuItem D_00423830[25];
extern MenuItem D_004239C0[25];
extern MenuItem D_00423B50[25];
extern MenuItem D_00423CE0[25];
extern MenuItem D_00423E70[10];
extern MenuItem D_00423F10[10];
extern MenuItem D_00423FB0[10];
extern MenuItem D_00424050[];
extern MenuItem D_00424600[];
extern GsDoubleBufferDc D_00426140;
extern LoadedTexture D_0042A8D0;
extern char D_0043FBD0[];
extern char D_0043FBE0[];
extern char D_0043FBF0[];
extern MenuItemList D_005CE6A0;
extern MenuItemList D_005CE6B0;
extern MenuItemList D_005CE6C0;
extern MenuItemList D_005CE6D0;
extern MenuItemList D_005CE6E0;
extern MenuItemList D_005CE6F0;
extern MenuItemList D_005CE700;
extern MenuItemList D_005CE710;
extern MenuItemList D_005CE720;
extern MenuItemList D_005CE730;
extern MenuItemList D_005CE740;
extern MenuItemList D_005CE760;
extern MenuItemList D_005CE770;
extern MenuItemList D_005CE780;
extern MenuItemList D_005CE790;
extern MenuItemList D_005CE7A0;
extern MenuItemList D_005CE7B0;
extern MenuItemList D_005CE7C0;
extern MenuItemList D_005CE7D0;
extern MenuItemList D_005CE7E0;
extern MenuItemList D_005CE7F0;
extern MenuItemList D_005CE800;
extern MenuItemList D_005CE810;
extern RelocFile *D_005CEE60[20];

int sceGsResetGraph(int, int, int, int);
void sceGsSetDefDBuffDc(void *, s16, s16, s16, s16, s16, s16);
void func_001FBD70(void);
void func_001D5740(void);
void *func_001FBD90(int);
int func_00203FC8(char *, void *);
void func_00204128(char *);
void func_001FBD80(void);
void func_00204D60(TextureHeader *, u64 *, u64 *);
void func_001FBDC8(void);
s32 func_00200820(RelocFile *);
int sprintf(char *, const char *, ...);

/* Sets the menus up: the frame buffers, the menu and font textures, the twenty equipment pictures, then every
   list of menu items (one MenuItem per definition, numbered) and the ring of the eleven lists. */
void func_001D5A20(void) {
    char name[0x40];
    char *menu_file;
    s32 i;
    s32 number;

    sceGsResetGraph(0, 1, 2, 1);
    sceGsSetDefDBuffDc(&D_00426140, 0, 0x280, 0xE0, 3, 0x30, 1);
    D_00426140.clear1_rgbaq = D_00426140.clear0_rgbaq = 0x180000000L;
    func_001FBD70();
    func_001D5740();
    menu_file = func_001FBD90(0xE0000);
    func_00203FC8(D_0043FBD0, menu_file);
    func_00204128(menu_file);
    func_001FBD80();
    D_0042A8D0.header = func_001FBD90(0x23000);
    func_00203FC8(D_0043FBE0, D_0042A8D0.header);
    func_00204D60(D_0042A8D0.header, &D_0042A8D0.tex0, &D_0042A8D0.trailer);
    func_001FBDC8();
    for (i = 0; i < 20; i++) {
        D_005CEE60[i] = func_001FBD90(0x2800);
        sprintf(name, D_0043FBF0, i);
        func_00203FC8(name, D_005CEE60[i]);
        func_00200820(D_005CEE60[i]);
    }
    for (i = 0; i < 150; i++) {
        D_00424600[i].unk0C = i;
        D_00424600[i].unk00 = (int)&D_0041A6F8[i];
        D_00424600[i].group = 11;
    }
    for (i = 150; i < 241; i++) {
        D_00424600[i].unk00 = (int)&D_00402DA8[i - 150];
        D_00424600[i].unk0C = i - 150;
        D_00424600[i].group = 1;
    }
    number = 0;
    for (i = 0; i < 150; i++) {
        D_00421A30[number].unk00 = (int)&D_0041A6F8[i];
        D_00421A30[number].unk0C = number;
        D_00421A30[number].group = 12;
        number++;
    }
    for (i = 0; i < 70; i++) {
        D_00421A30[number].unk00 = (int)&D_0041B9B8[i];
        D_00421A30[number].unk0C = number;
        D_00421A30[number].group = 0;
        number++;
    }
    for (i = 0; i < 30; i++) {
        D_00421A30[number].unk00 = (int)&D_0041E118[i];
        D_00421A30[number].unk0C = number;
        D_00421A30[number].group = 2;
        number++;
    }
    for (i = 0; i < 25; i++) {
        D_00421A30[number].unk00 = (int)&D_0041E118[i + 30];
        D_00421A30[number].unk0C = number;
        D_00421A30[number].group = 3;
        number++;
    }
    for (i = 0; i < 25; i++) {
        D_00421A30[number].unk00 = (int)&D_0041E118[i + 55];
        D_00421A30[number].unk0C = number;
        D_00421A30[number].group = 4;
        number++;
    }
    for (i = 0; i < 25; i++) {
        D_00421A30[number].unk00 = (int)&D_0041E118[i + 80];
        D_00421A30[number].unk0C = number;
        D_00421A30[number].group = 5;
        number++;
    }
    for (i = 0; i < 25; i++) {
        D_00421A30[number].unk00 = (int)&D_0041E118[i + 105];
        D_00421A30[number].unk0C = number;
        D_00421A30[number].group = 6;
        number++;
    }
    for (i = 0; i < 10; i++) {
        D_00421A30[number].unk00 = (int)&D_0041E118[i + 130];
        D_00421A30[number].unk0C = number;
        D_00421A30[number].group = 7;
        number++;
    }
    for (i = 0; i < 10; i++) {
        D_00421A30[number].unk00 = (int)&D_0041E118[i + 140];
        D_00421A30[number].unk0C = number;
        D_00421A30[number].group = 8;
        number++;
    }
    for (i = 0; i < 10; i++) {
        D_00421A30[number].unk00 = (int)&D_0041E118[i + 130];
        D_00421A30[number].unk0C = number;
        D_00421A30[number].group = 9;
        number++;
    }
    number = 0;
    for (i = 0; i < 150; i++) {
        D_004210D0[i].unk00 = (int)&D_0041A6F8[i];
        D_004210D0[i].unk0C = number;
        D_004210D0[i].group = 11;
        number++;
    }
    for (i = 0; i < 70; i++) {
        D_004231F0[i].unk00 = (int)&D_0041B9B8[i];
        D_004231F0[i].unk0C = number;
        D_004231F0[i].group = 0;
        number++;
    }
    for (i = 0; i < 30; i++) {
        D_00423650[i].unk00 = (int)&D_0041E118[i];
        D_00423650[i].unk0C = number;
        D_00423650[i].group = 2;
        number++;
    }
    for (i = 0; i < 25; i++) {
        D_00423830[i].unk00 = (int)&D_0041E118[i + 30];
        D_00423830[i].unk0C = number;
        D_00423830[i].group = 3;
        number++;
    }
    for (i = 0; i < 25; i++) {
        D_004239C0[i].unk00 = (int)&D_0041E118[i + 55];
        D_004239C0[i].unk0C = number;
        D_004239C0[i].group = 4;
        number++;
    }
    for (i = 0; i < 25; i++) {
        D_00423B50[i].unk00 = (int)&D_0041E118[i + 80];
        D_00423B50[i].unk0C = number;
        D_00423B50[i].group = 5;
        number++;
    }
    for (i = 0; i < 25; i++) {
        D_00423CE0[i].unk00 = (int)&D_0041E118[i + 105];
        D_00423CE0[i].unk0C = number;
        D_00423CE0[i].group = 6;
        number++;
    }
    for (i = 0; i < 10; i++) {
        D_00423E70[i].unk00 = (int)&D_0041E118[i + 130];
        D_00423E70[i].unk0C = number;
        D_00423E70[i].group = 7;
        number++;
    }
    for (i = 0; i < 10; i++) {
        D_00423F10[i].unk00 = (int)&D_0041E118[i + 140];
        D_00423F10[i].unk0C = number;
        D_00423F10[i].group = 8;
        number++;
    }
    for (i = 0; i < 10; i++) {
        D_00423FB0[i].unk00 = (int)&D_0041E118[i + 130];
        D_00423FB0[i].unk0C = number;
        D_00423FB0[i].group = 9;
        number++;
    }
    for (i = 0; i < 91; i++) {
        D_00424050[i].unk0C = i;
        D_00424050[i].unk00 = (int)&D_00402DA8[i];
        D_00424050[i].group = 1;
    }
    D_005CE760.group = 11;
    D_005CE770.item = D_005CE6B0.item;
    D_005CE770.group = 0;
    D_005CE780.item = D_005CE6C0.item;
    D_005CE780.group = 2;
    D_005CE790.item = D_005CE6D0.item;
    D_005CE790.group = 3;
    D_005CE7A0.item = D_005CE6E0.item;
    D_005CE7A0.group = 4;
    D_005CE7B0.item = D_005CE6F0.item;
    D_005CE7B0.group = 5;
    D_005CE7C0.item = D_005CE700.item;
    D_005CE7C0.group = 6;
    D_005CE7D0.item = D_005CE710.item;
    D_005CE7D0.group = 7;
    D_005CE7E0.item = D_005CE720.item;
    D_005CE7E0.group = 8;
    D_005CE7F0.item = D_005CE730.item;
    D_005CE7F0.group = 9;
    D_005CE800.item = D_005CE740.item;
    D_005CE800.group = 1;
    D_005CE810.item = D_005CE6A0.item;
    D_005CE810.group = 10;
    D_005CE810.prev = &D_005CE770;
    D_005CE770.prev = &D_005CE800;
    D_005CE800.prev = &D_005CE780;
    D_005CE780.prev = &D_005CE790;
    D_005CE790.prev = &D_005CE7A0;
    D_005CE7A0.prev = &D_005CE7B0;
    D_005CE7B0.prev = &D_005CE7C0;
    D_005CE7C0.prev = &D_005CE7D0;
    D_005CE7D0.prev = &D_005CE7E0;
    D_005CE7E0.prev = &D_005CE7F0;
    D_005CE7F0.prev = &D_005CE810;
    D_005CE770.next = &D_005CE810;
    D_005CE800.next = &D_005CE770;
    D_005CE780.next = &D_005CE800;
    D_005CE790.next = &D_005CE780;
    D_005CE7A0.next = &D_005CE790;
    D_005CE7B0.next = &D_005CE7A0;
    D_005CE7C0.next = &D_005CE7B0;
    D_005CE7D0.next = &D_005CE7C0;
    D_005CE7E0.next = &D_005CE7D0;
    D_005CE7F0.next = &D_005CE7E0;
    D_005CE810.next = &D_005CE7F0;
    func_001FBD80();
}
