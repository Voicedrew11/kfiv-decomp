#include "common.h"

void Ini_SetMagicSchool(int, char *);

/* ini/NNNN.ini, a "magic_..." key: magic of element 0 (Ini_SetMagicSchool). */
void Ini_SetMagicFire(char *text) {
    Ini_SetMagicSchool(0, text);
}
