#include "common.h"

void Ini_SetMagicSchool(int, char *);

/* ini/NNNN.ini, a "magic_..." key: magic of element 2 (Ini_SetMagicSchool). */
void Ini_SetMagicWind(char *text) {
    Ini_SetMagicSchool(2, text);
}
