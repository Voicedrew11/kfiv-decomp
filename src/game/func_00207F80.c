#include "common.h"

void Ini_SetMagicSchool(int, char *);

/* ini/NNNN.ini, a "magic_..." key: magic of element 1 (Ini_SetMagicSchool). */
void Ini_SetMagicEarth(char *text) {
    Ini_SetMagicSchool(1, text);
}
