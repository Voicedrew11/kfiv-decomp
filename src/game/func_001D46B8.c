#include "common.h"
#include "kf_menu.h"

int sprintf(char *, char *, ...);
extern char D_0043F768[];

/* Formats a menu item's value into buffer. */
void func_001D46B8(char *buffer, MenuItem *item) {
    sprintf(buffer, D_0043F768, item->unk0C);
}
