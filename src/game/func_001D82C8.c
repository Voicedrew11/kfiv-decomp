#include "kf_menu.h"

/* Takes item out of its list and links it in right after the item `after`. */
void func_001D82C8(MenuItem *item, MenuItem *after) {
    item->next->prev = item->prev;
    item->prev->next = item->next;
    item->next = after->next;
    item->prev = after;
    after->next->prev = item;
    after->next = item;
}
