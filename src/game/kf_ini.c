#include "kf_ini.h"

void *memset(void *, int, u32);

static char *ini_cursor;          /* 0x447350 start of the next line */
static int ini_remaining;         /* 0x447354 bytes left after ini_cursor */
static char ini_line[0x400];      /* 0x5F9700 the line Ini_ReadLine returns */

/* Sets the text Ini_ReadLine reads lines from. */
void Ini_SetText(char *text, int length) {
    ini_cursor = text;
    ini_remaining = length;
}

/* Copies the next line of the text to ini_line, without its CR LF; NULL at the end of the text. */
char *Ini_ReadLine(void) {
    char *dest;
    char *source;
    char current;
    char previous;

    memset(ini_line, 0, sizeof(ini_line));
    if (ini_remaining <= 0) {
        return NULL;
    }
    source = ini_cursor;
    dest = ini_line;
    current = 0;
    for (;;) {
        previous = current;
        current = *source;
        ini_remaining--;
        if (previous == '\r' && current == '\n') {
            dest[-1] = 0;
            break;
        }
        if (ini_remaining <= 0) {
            *dest = 0;
            break;
        }
        *dest = *source;
        source++;
        dest++;
    }
    ini_cursor = source + 1;
    return ini_line;
}
