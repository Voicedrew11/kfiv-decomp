#include "kf_menu.h"
#include "kf_map.h"
#include "kf_player.h"
#include "kf_effect.h"

extern LevelRecord D_003679A0[70];
extern LevelRecord D_003682F0[160];
extern u8 D_00367980[6][5];
extern u8 D_00369830[380];
extern FxTableEntry D_00402DA8[];
extern PlayerState D_00413F80;
extern LevelDef D_004147F8[0x17C];
extern TriggerTypeDef D_0041A6F8[];
extern MenuCategory *D_00420D90[];
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
extern MenuItem *D_00426110[4];
extern MenuItem *D_00426120[4];
extern MenuItem *D_00426130[4];
extern MenuItem *D_00446C24;
extern MenuItem *D_00446C28;
extern s32 D_00446C38;
extern MenuItem *D_00446C40;
extern MenuItem *D_00446C44;
extern s32 D_00446C48;
extern s32 D_004472E0;
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
extern MenuItem *D_005CE830[4];
extern MenuCategory *D_005CE840[4];
extern MenuItem *D_005CE860[4];
extern MenuItem *D_005CE850[4];
extern MenuItem *D_005CE870[4];
extern MenuItem *D_005CE880[4];
extern MenuItem *D_005CE890[4];
extern MenuItem *D_005CE8A0[4];

// match: D_005CE860[0] reached from D_005CE840 (the same address). gcse numbers its hoisted address registers by a
// hash of the symbol's name, and with the name D_005CE860 two of them ($19, $20) come out the other way round.
#define D_005CE860_FIRST (((MenuItem **)D_005CE840)[8])

/* Appends node to the ring being built. */
#define LINK_ITEM(node)                     \
    if (last != NULL) {                     \
        (node)->next = last;                \
        last->prev = (node);                \
    } else {                                \
        first = (node);                     \
    }                                       \
    last = (node);

/* The same, counting: the seventh one becomes the page mark. */
#define LINK_COUNTED_ITEM(node, page_mark)  \
    count++;                                \
    if (count == 7) {                       \
        (page_mark)[0] = (node);            \
    }                                       \
    LINK_ITEM(node)

/* Rings the owned, usable items of one equipment group whose record is not in use, and puts the list on the one worn.
   The ring is closed without a test for an empty one. */
#define LINK_GROUP(nodes, records, record_base, item_base, item_count, list, start_list, worn)            \
    last = NULL;                                                                                         \
    first = NULL;                                                                                        \
    for (i = 0; i < (item_count); i++) {                                                                 \
        if (D_00369830[(item_base) + i] != 0 && (D_004147F8[(item_base) + i].state & 1) &&               \
            (records)[(record_base) + i].unk18 == 0) {                                                   \
            LINK_ITEM(&(nodes)[i])                                                                       \
        }                                                                                                \
    }                                                                                                    \
    first->next = last;                                                                                  \
    last->prev = first;                                                                                  \
    (list).item = (start_list).item = &(nodes)[(worn) - (record_base)];

/* Whether the spell is the one its slot of D_00367980 holds (three spells to a slot, numbered from 1). */
#define SPELL_IS_SET(spell) (D_00367980[((spell) - 1) / 15][(((spell) - 1) / 3) % 5] == ((spell) - 1) % 3 + 1)

/* Builds every ring the menus walk from what the player owns: the items, everything, the usable items and spells,
   one ring per equipment group, the spells known and the spells that can be cast. */
void func_001D2680(void) {
    MenuItem *last = NULL;
    MenuItem *first = NULL;
    s32 count = 0;
    s32 i;
    s32 spell;

    D_00426120[0] = NULL;
    D_00426130[0] = NULL;
    D_00446C40 = NULL;
    D_00446C44 = NULL;
    // match: the three are cleared a second time; retail's `first` and `count` are copies of `last` because of it
    last = NULL;
    first = NULL;
    count = 0;
    for (i = 1; i < 150; i++) {
        if (D_00369830[i] != 0 && (D_004147F8[i].state & 1)) {
            LINK_COUNTED_ITEM(&D_004210D0[i], D_00426130)
        }
    }
    D_00446C48 = count;
    if (first != NULL) {
        last->prev = first;
        D_00426110[0] = D_00426120[0] = D_005CE760.item = first;
        first->next = last;
        if (D_00426130[0] == NULL) {
            D_00426130[0] = last->next;
        } else {
            D_00426130[0] = D_00426130[0]->next;
        }
        D_00446C40 = first;
        D_00446C44 = last;
        for (i = 5; i >= 0; i--) {
            D_00446C44 = D_00446C44->next;
        }
    } else {
        D_00426110[0] = NULL;
    }

    D_005CE860_FIRST = NULL;
    D_005CE870[0] = NULL;
    D_00446C24 = NULL;
    D_00446C28 = NULL;
    last = NULL;
    first = NULL;
    count = 0;
    for (i = 1; i < 380; i++) {
        if (D_00369830[i] != 0 && !(D_004147F8[i].state & 2)) {
            if ((i >= 0 && i < 150) || (i >= 150 && i < 220 && D_003679A0[i - 150].unk18 == 0) ||
                (i >= 220 && i < 250 && D_003682F0[i - 220].unk18 == 0) ||
                (i >= 250 && i < 275 && D_003682F0[i - 220].unk18 == 0) ||
                (i >= 275 && i < 300 && D_003682F0[i - 220].unk18 == 0) ||
                (i >= 300 && i < 325 && D_003682F0[i - 220].unk18 == 0) ||
                (i >= 325 && i < 350 && D_003682F0[i - 220].unk18 == 0) ||
                (i >= 350 && i < 360 && D_003682F0[i - 220].unk18 == 0) ||
                (i >= 360 && i < 370 && D_003682F0[i - 220].unk18 == 0) ||
                (i >= 370 && D_003682F0[i - 220].unk18 == 0)) {
                count++;
                if (count == 7) {
                    D_005CE870[0] = &D_00421A30[i];
                }
                if (last != NULL) {
                    D_00421A30[i].next = last;
                    last->prev = &D_00421A30[i];
                } else {
                    first = &D_00421A30[i];
                }
                last = &D_00421A30[i];
            }
        }
    }
    D_00446C38 = count;
    if (first != NULL) {
        last->prev = first;
        D_005CE850[0] = D_005CE860_FIRST = first;
        first->next = last;
        if (D_005CE870[0] == NULL) {
            D_005CE870[0] = last->next;
        } else {
            D_005CE870[0] = D_005CE870[0]->next;
        }
        D_00446C24 = first;
        D_00446C28 = last;
        for (i = 5; i >= 0; i--) {
            D_00446C28 = D_00446C28->next;
        }
    } else {
        D_005CE850[0] = NULL;
    }

    last = NULL;
    first = NULL;
    for (i = 0; i < 150; i++) {
        if (D_00369830[i] != 0 && (D_004147F8[i].state & 1) && !((u8)D_0041A6F8[i].flags & 1) && !(i >= 145 && i < 148)) {
            LINK_ITEM(&D_00424600[i])
        }
    }
    for (i = 150, spell = 0; i < 241; i++, spell++) {
        if (SPELL_IS_SET(spell) && D_00402DA8[spell].unk102 == 1) {
            LINK_ITEM(&D_00424600[i])
        }
    }
    // match: closed without the `first != NULL` test of the counted rings (so are the rings below); an empty one
    // would be written through NULL
    first->next = last;
    last->prev = first;
    switch (D_00413F80.unk48) {
    case 0:
        D_005CE810.item = D_005CE6A0.item = D_00424600;
        // match: no break, case 0 goes on into case 1, which sets both again
    case 1:
        if (D_00369830[D_00413F80.unk46] != 0) {
            D_005CE810.item = D_005CE6A0.item = &D_00424600[D_00413F80.unk46];
        } else {
            D_00413F80.unk46 = 0;
            D_005CE810.item = D_005CE6A0.item = D_00424600;
        }
        break;
    case 2:
        D_005CE810.item = D_005CE6A0.item = &D_00424600[D_00413F80.unk46 + 150];
        break;
    }

    last = NULL;
    first = NULL;
    for (i = 0; i < 70; i++) {
        if (D_00369830[150 + i] != 0 && (D_004147F8[150 + i].state & 1) && D_003679A0[i].unk18 == 0) {
            LINK_ITEM(&D_004231F0[i])
        }
    }
    first->next = last;
    last->prev = first;
    D_005CE770.item = D_005CE6B0.item = &D_004231F0[D_00413F80.unk3C];

    LINK_GROUP(D_00423650, D_003682F0, 0, 220, 30, D_005CE780, D_005CE6C0, D_00413F80.equipment[0])
    LINK_GROUP(D_00423830, D_003682F0, 30, 250, 25, D_005CE790, D_005CE6D0, D_00413F80.equipment[1])
    LINK_GROUP(D_004239C0, D_003682F0, 55, 275, 25, D_005CE7A0, D_005CE6E0, D_00413F80.equipment[2])
    LINK_GROUP(D_00423B50, D_003682F0, 80, 300, 25, D_005CE7B0, D_005CE6F0, D_00413F80.equipment[3])
    LINK_GROUP(D_00423CE0, D_003682F0, 105, 325, 25, D_005CE7C0, D_005CE700, D_00413F80.equipment[4])
    LINK_GROUP(D_00423E70, D_003682F0, 130, 350, 10, D_005CE7D0, D_005CE710, D_00413F80.equipment[5])
    LINK_GROUP(D_00423F10, D_003682F0, 140, 360, 10, D_005CE7E0, D_005CE720, D_00413F80.equipment[6])
    LINK_GROUP(D_00423FB0, D_003682F0, 150, 370, 10, D_005CE7F0, D_005CE730, D_00413F80.equipment[7])

    last = NULL;
    first = NULL;
    for (i = 0; i < 91; i++) {
        if (i == 0 || (SPELL_IS_SET(i) && D_00402DA8[i].unk102 == 0)) {
            LINK_ITEM(&D_00424050[i])
        }
    }
    first->next = last;
    last->prev = first;
    D_005CE890[0] = NULL;
    D_005CE800.item = D_005CE740.item = &D_00424050[D_00413F80.unk3D];
    D_005CE8A0[0] = NULL;

    last = NULL;
    first = NULL;
    count = 0;
    for (i = 1; i < 91; i++) {
        if (SPELL_IS_SET(i) && D_00402DA8[i].unk102 == 1) {
            LINK_COUNTED_ITEM(&D_00424050[i], D_005CE8A0)
        }
    }
    if (first != NULL) {
        first->next = last;
        D_005CE830[0] = first;
        D_005CE890[0] = D_005CE880[0] = first;
        last->prev = first;
        if (D_005CE8A0[0] == NULL) {
            D_005CE8A0[0] = last->next;
        } else {
            D_005CE8A0[0] = D_005CE8A0[0]->next;
        }
    } else {
        D_005CE880[0] = NULL;
    }
    D_005CE840[0] = D_00420D90[D_004472E0];
}
