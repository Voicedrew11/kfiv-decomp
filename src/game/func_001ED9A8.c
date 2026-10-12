#include "kf_player.h"
#include "kf_game.h"
#include "kf_menu.h"

extern u8 D_00369830[];
extern LevelDef D_004147F8[380];
extern LevelRecordTimed D_003679A0[70];
extern LevelRecordTimed D_003682F0[160];
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
extern MenuItem *D_005CF390[4];
extern MenuItem *D_005CF3A0[4];
extern MenuItem *D_005CF3B0[4];
extern MenuItem *D_00446C1C;
extern MenuItem *D_00446C20;
extern MenuItem *D_00446C24;
extern MenuItem *D_00446C28;
extern s32 D_00446C34;
extern s32 D_00446C38;
extern ClockTime D_00446A28;

void func_001D20E0(void);
int func_001D2258(ClockTime, ClockTime);

/* Adds a node to the end of the ring being built; `seventh` remembers the seventh one added. */
#define APPEND(node, seventh) \
    count++; \
    if (count == 7) { \
        seventh = (node); \
    } \
    if (last != NULL) { \
        (node)->next = last; \
        last->prev = (node); \
    } else { \
        first = (node); \
    } \
    last = (node);

/* The owned, usable items of one run of definitions whose record is idle and below level 100. */
#define APPEND_UPGRADABLE(nodes, records, base, first_item, limit) \
    for (i = 1; i < limit; i++) { \
        if (D_00369830[first_item + i] != 0 && !(D_004147F8[first_item + i].state & 2) && \
            records[base + i].unk18 == 0 && records[base + i].level < 100) { \
            APPEND(&nodes[i], D_00426130[0]) \
        } \
    }

/* The owned, usable items of one run of definitions whose record is idle. */
#define APPEND_IDLE(nodes, records, base, first_item, limit) \
    for (i = 1; i < limit; i++) { \
        if (D_00369830[first_item + i] != 0 && !(D_004147F8[first_item + i].state & 2) && \
            records[base + i].unk18 == 0) { \
            APPEND(&nodes[i], D_00426130[0]) \
        } \
    }

/* Builds the two item rings of the trade screen: the items that can be handed in (mode 0: those that can still
   gain a level, mode 1: any idle one) at D_00426110, and at D_005CF390 the items handed in earlier whose time is
   up. Each ring's count goes to D_00446C34 / D_00446C38. */
void func_001ED9A8(s32 mode) {
    MenuItem *last = NULL;
    MenuItem *first = NULL;
    s32 count = 0;
    s32 i;

    D_00426120[0] = NULL;
    D_00426130[0] = NULL;
    D_00446C1C = NULL;
    D_00446C20 = NULL;
    // match: the three are cleared a second time; retail's `first` and `count` are copies of `last` because of it
    last = NULL;
    first = NULL;
    count = 0;
    if (mode == 0) {
        APPEND_UPGRADABLE(D_004231F0, D_003679A0, 0, 150, 70)
        APPEND_UPGRADABLE(D_00423650, D_003682F0, 0, 220, 30)
        APPEND_UPGRADABLE(D_00423830, D_003682F0, 30, 250, 25)
        APPEND_UPGRADABLE(D_004239C0, D_003682F0, 55, 275, 25)
        APPEND_UPGRADABLE(D_00423B50, D_003682F0, 80, 300, 25)
        APPEND_UPGRADABLE(D_00423CE0, D_003682F0, 105, 325, 25)
    } else if (mode == 1) {
        APPEND_IDLE(D_004231F0, D_003679A0, 0, 150, 70)
        APPEND_IDLE(D_00423650, D_003682F0, 0, 220, 30)
        APPEND_IDLE(D_00423830, D_003682F0, 30, 250, 25)
        APPEND_IDLE(D_004239C0, D_003682F0, 55, 275, 25)
        APPEND_IDLE(D_00423B50, D_003682F0, 80, 300, 25)
        APPEND_IDLE(D_00423CE0, D_003682F0, 105, 325, 25)
        APPEND_IDLE(D_00423E70, D_003682F0, 130, 350, 10)
        APPEND_IDLE(D_00423F10, D_003682F0, 140, 360, 10)
        APPEND_IDLE(D_00423FB0, D_003682F0, 150, 370, 10)
    }
    D_00446C34 = count;
    if (first != NULL) {
        last->prev = first;
        D_00426120[0] = first;
        D_00426110[0] = first;
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

    D_005CF390[0] = NULL;
    D_005CF3A0[0] = NULL;
    last = NULL;
    first = NULL;
    count = 0;
    D_00446C24 = NULL;
    D_00446C28 = NULL;
    func_001D20E0();
    for (i = 1; i < 380; i++) {
        if (i >= 150) {
            if (i < 220) {
                if (i != 150 && D_003679A0[i - 150].unk18 != 0 && !(D_004147F8[i].state & 2) &&
                    func_001D2258(D_003679A0[i - 150].unk19, D_00446A28) >= D_004147F8[i].unk36 * 60) {
                    // match: written out; through APPEND's `(node)->next` the item pointer is reduced differently
                    count++;
                    if (count == 7) {
                        D_005CF3A0[0] = &D_00421A30[i];
                    }
                    if (last != NULL) {
                        D_00421A30[i].next = last;
                        last->prev = &D_00421A30[i];
                    } else {
                        first = &D_00421A30[i];
                    }
                    last = &D_00421A30[i];
                }
            } else {
                if (i != 220 && D_003682F0[i - 220].unk18 != 0 && !(D_004147F8[i].state & 2) &&
                    func_001D2258(D_003682F0[i - 220].unk19, D_00446A28) >= D_004147F8[i].unk36 * 60) {
                    count++;
                    if (count == 7) {
                        D_005CF3A0[0] = &D_00421A30[i];
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
    }
    D_00446C38 = count;
    if (first != NULL) {
        last->prev = first;
        D_005CF390[0] = first;
        D_005CF3B0[0] = first;
        first->next = last;
        if (D_005CF3A0[0] == NULL) {
            D_005CF3A0[0] = last->next;
        } else {
            D_005CF3A0[0] = D_005CF3A0[0]->next;
        }
        D_00446C24 = first;
        D_00446C28 = last;
        for (i = 5; i >= 0; i--) {
            D_00446C28 = D_00446C28->next;
        }
    } else {
        D_005CF3B0[0] = NULL;
    }
}
