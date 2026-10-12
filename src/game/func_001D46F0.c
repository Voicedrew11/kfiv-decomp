#include "kf_menu.h"

extern MenuNameHandlers D_0043F780;
extern MenuCategory *D_005CE840[];
extern char D_00426498[];
extern char D_004264E8[];
extern char D_00426538[];
extern char D_00426588[];
extern char D_004265D8[];

/* Writes the model file names of the five items on show: the current one, those of the next and
   previous categories, and the next and previous items of this category. */
void func_001D46F0(void) {
    MenuNameHandlers handlers = D_0043F780;
    MenuItem *item;
    void (*handler)(char *buffer, MenuItem *item);

    item = D_005CE840[0]->item;
    handler = handlers.handler[item->group];
    handler(D_00426498, item);
    item = (MenuItem *)D_005CE840[0]; // match: the category passes through the item's register
    item = ((MenuCategory *)item)->next->item;
    handler = handlers.handler[item->group];
    handler(D_004264E8, item);
    item = (MenuItem *)D_005CE840[0]; // match: as above
    item = ((MenuCategory *)item)->prev->item;
    handler = handlers.handler[item->group];
    handler(D_00426538, item);
    item = (MenuItem *)D_005CE840[0]; // match: as above
    item = ((MenuCategory *)item)->item->next;
    handler = handlers.handler[item->group];
    handler(D_00426588, item);
    item = (MenuItem *)D_005CE840[0]; // match: as above
    item = ((MenuCategory *)item)->item->prev;
    handler = handlers.handler[item->group];
    handler(D_004265D8, item);
}
