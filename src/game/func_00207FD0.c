#include "common.h"

extern u8 spell_levels[6][5];
extern u16 spell_experience[6][5];

int atoi(const char *);
char *strchr(const char *, int);

/* ini/NNNN.ini, keys "magic_<school>_1=" to "_5=": the level of that magic and, after a comma, its experience. */
void Ini_SetMagicSchool(int school, char *text) {
    int idx;
    char *comma;

    if ((u8)(text[-2] - 0x31) < 5) {
        idx = text[-2] - 0x31;
        atoi(text);
        spell_levels[school][idx] = atoi(text);
        comma = strchr(text, 0x2C);
        if (comma != NULL) {
            spell_experience[school][idx] = atoi(comma + 1);
        }
    }
}
