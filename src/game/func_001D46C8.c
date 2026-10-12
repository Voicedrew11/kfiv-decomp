#include "kf_game.h"
#include "kf_menu.h"

int sprintf(char *, char *, ...);
extern GameDefinitionWords D_004147F8[380];
extern char D_0043F750[];

/* Formats the file number of a menu item's definition into buffer. */
void func_001D46C8(char *buffer, MenuItem *item) {
    sprintf(buffer, D_0043F750, D_004147F8[item->unk0C].file_number);
}
