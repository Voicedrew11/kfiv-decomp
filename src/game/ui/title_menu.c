#include "ui/title.h"
#include "ui/menu_screens.h"
#include "item/item.h"

extern u8 item_counts[];
extern u8 spell_levels[6][5];
extern char fx_table[];

/* The title screen's loop: runs Title_Run and the menus it leads to; 0 for a new game, 1 once a game was loaded. */
int TitleMenu_Run(void) {
    int screen = 0;

    do {
        switch (Title_Run(screen)) {
        case 1:
            return 0;
        case 2:
            if (Menu_Run(1) != 0) {
                return 1;
            }
            screen = 2;
            break;
        case 3:
            Menu_Run(2);
            screen = 2;
            break;
        case 4:
            Menu_Run(3);
            screen = 2;
            break;
        }
    } while (screen != -1);
}

/* Whether the player has any item other than the keys and the "none" items (the main menu's item screen). */
int Menu_OwnsAnyItem(void) {
    int i;

    for (i = 0; i < ITEM_COUNT; i++) {
        if (item_counts[i] != 0 && (i < ITEM_KEY_FIRST || i == ITEM_KEY_UNUSED || i > ITEM_KEY_LAST) && i != 0
            && i != ITEM_WEAPON_FIRST && i != ITEM_SHIELD_NONE && i != ITEM_HELM_NONE && i != ITEM_ARMOR_NONE
            && i != ITEM_GAUNTLET_NONE && i != ITEM_BOOT_NONE && i != ITEM_RING_NONE && i != ITEM_BRACELET_NONE
            && i != ITEM_AMULET_NONE && i != 0) {
            return TRUE;
        }
    }
    return FALSE;
}

/* Whether the player knows a magic cast from the menu (fx_table unk102 == 1; the main menu's spell screen). */
int Menu_KnowsCastableSpell(void) {
    int i;
    int j;

    for (i = 0; i < 6; i++) {
        for (j = 0; j < 5; j++) {
            if (spell_levels[i][j] != 0) {
                if (fx_table[(15 * i + 3 * j + spell_levels[i][j]) * 0x110 + 0x102] == 1) {
                    return 1;
                }
            }
        }
    }
    return 0;
}
