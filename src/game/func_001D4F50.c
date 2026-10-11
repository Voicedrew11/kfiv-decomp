#include "common.h"

int sprintf(char *, char *, ...);
extern char D_00426498[];
extern char D_004264E8[];
extern char D_00426538[];
extern char D_0043F768[];
extern char *D_005CE880[];

void func_001D4F50(void) {
    sprintf(D_00426498, D_0043F768, *(s16 *)(D_005CE880[0] + 0xC));
    sprintf(D_00426538, D_0043F768, *(s16 *)(*(char **)(D_005CE880[0] + 8) + 0xC));
    sprintf(D_004264E8, D_0043F768, *(s16 *)(*(char **)(D_005CE880[0] + 4) + 0xC));
}
