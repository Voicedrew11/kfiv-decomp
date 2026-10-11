#include "kf_game.h"

int vsprintf(char *, const char *, char *);
u32 strlen(char *);
void *memcpy(void *, const void *, u32);
extern TextLog D_01E0E9C0;

/* printf-style append to the text log, cut off at its 0x800 bytes. */
void func_00217EE0(char *format, char *args) {
    char line[0x400];
    TextLog *log;
    char *text;
    int length;
    int used;

    vsprintf(line, format, args);
    length = strlen(line);
    log = &D_01E0E9C0;
    used = log->used;
    if (length + used >= 0x801) {
        length = 0x800 - used;
    }
    text = log->text;
    memcpy(text + used, line, length);
    log->used += length;
}
