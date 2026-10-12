#include "kf_menu.h"

int sprintf(char *, char *, ...);
extern char D_0043F768[];

/* Builds the file name of a menu item's model ("menu/select/m%%04d.mod"). */
void func_001D46B8(char *dest, MenuItem *item) {
    sprintf(dest, D_0043F768, item->unk0C);
}
