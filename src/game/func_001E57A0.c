#include "kf_menu.h"
#include "kf_player.h"

extern const TradeValueTables D_00440300;
extern const TradeStockTables D_00440318;
extern u8 D_00369830[380];
extern LevelDef D_004147F8[0x17C];
extern LevelRecord D_003679A0[70];
extern LevelRecord D_003682F0[160];
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
extern MenuItem *D_00426110[4];
extern MenuItem *D_00426120[4];
extern MenuItem *D_00426130[4];
extern MenuItem *D_005CF360[4];
extern MenuItem *D_005CF370[4];
extern MenuItem *D_005CF380[4];
extern MenuItem *D_00446C1C;
extern MenuItem *D_00446C20;
extern MenuItem *D_00446C24;
extern MenuItem *D_00446C28;
extern s32 D_00446C34;
extern s32 D_00446C38;

/* Appends node to the ring being built; the seventh one becomes the page mark. */
#define LINK_ITEM(node, page_mark)          \
    count++;                                \
    if (count == 7) {                       \
        (page_mark)[0] = (node);            \
    }                                       \
    if (last != NULL) {                     \
        (node)->next = last;                \
        last->prev = (node);                \
    } else {                                \
        first = (node);                     \
    }                                       \
    last = (node);

/* Lists the owned, unlocked items of one group that the table gives a value for and whose record is not in use. */
#define LINK_OWNED_GROUP(nodes, records, record_base, item_base, item_count)                                       \
    for (i = 1; i < (item_count); i++) {                                                                           \
        if (D_00369830[(item_base) + i] != 0 && !(D_004147F8[(item_base) + i].state & 2) &&                        \
            values.values[table][(item_base) + i].give_value != 0 && (records)[(record_base) + i].unk18 == 0) {    \
            LINK_ITEM(&(nodes)[i], D_00426130)                                                                     \
        }                                                                                                          \
    }

/* Builds the two item rings of trade table `table`: what is owned and may be given, and what is in stock. */
void func_001E57A0(s32 table) {
    TradeValueTables values = D_00440300;
    TradeStockTables stock = D_00440318;
    MenuItem *last = NULL;
    MenuItem *first = NULL;
    s32 count = 0;
    s32 i;
    u8 *stock_counts;

    D_00426120[0] = NULL;
    D_00426130[0] = NULL;
    D_00446C1C = NULL;
    D_00446C20 = NULL;
    // match: the three are cleared a second time; retail's `first` and `count` are copies of `last` because of it
    last = NULL;
    first = NULL;
    count = 0;
    for (i = 1; i < 150; i++) {
        if (D_00369830[i] != 0 && !(D_004147F8[i].state & 2) && values.values[table][i].give_value != 0) {
            LINK_ITEM(&D_004210D0[i], D_00426130)
        }
    }
    LINK_OWNED_GROUP(D_004231F0, D_003679A0, 0, 150, 70)
    LINK_OWNED_GROUP(D_00423650, D_003682F0, 0, 220, 30)
    LINK_OWNED_GROUP(D_00423830, D_003682F0, 30, 250, 25)
    LINK_OWNED_GROUP(D_004239C0, D_003682F0, 55, 275, 25)
    LINK_OWNED_GROUP(D_00423B50, D_003682F0, 80, 300, 25)
    LINK_OWNED_GROUP(D_00423CE0, D_003682F0, 105, 325, 25)
    LINK_OWNED_GROUP(D_00423E70, D_003682F0, 130, 350, 10)
    LINK_OWNED_GROUP(D_00423F10, D_003682F0, 140, 360, 10)
    LINK_OWNED_GROUP(D_00423FB0, D_003682F0, 150, 370, 10)
    D_00446C34 = count;
    if (first != NULL) {
        last->prev = first;
        D_00426110[0] = D_00426120[0] = first;
        first->next = last;
        if (D_00426130[0] == NULL) {
            D_00426130[0] = last->next;
        } else {
            D_00426130[0] = D_00426130[0]->next;
        }
        D_00446C1C = first;
        D_00446C20 = last;
        for (i = 5; i >= 0; i--) {
            D_00446C20 = D_00446C20->next;
        }
    } else {
        D_00426110[0] = NULL;
    }

    D_005CF360[0] = NULL;
    D_005CF370[0] = NULL;
    D_00446C24 = NULL;
    D_00446C28 = NULL;
    stock_counts = stock.counts[table];
    last = NULL;
    first = NULL;
    count = 0;
    for (i = 1; i < 0x17C; i++) {
        if (stock_counts[i] != 0 && values.values[table][i].take_value != 0 && !(D_004147F8[i].state & 2)) {
            LINK_ITEM(&D_00421A30[i], D_005CF370)
        }
    }
    D_00446C38 = count;
    if (first != NULL) {
        last->prev = first;
        D_005CF380[0] = D_005CF360[0] = first;
        first->next = last;
        if (D_005CF370[0] == NULL) {
            D_005CF370[0] = last->next;
        } else {
            D_005CF370[0] = D_005CF370[0]->next;
        }
        D_00446C24 = first;
        D_00446C28 = last;
        for (i = 5; i >= 0; i--) {
            D_00446C28 = D_00446C28->next;
        }
    } else {
        D_005CF380[0] = NULL;
    }
}
