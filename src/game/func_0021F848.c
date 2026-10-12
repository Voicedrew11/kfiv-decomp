#include "common.h"

extern char D_004421D0[];

int sceSifLoadModule(const char *, int, const char *);
void Exit(int);

/* Loads an IOP module with no arguments; a failure ends the program. */
void func_0021F848(const char *path) {
    if (sceSifLoadModule(path, 0, D_004421D0) < 0) {
        Exit(1);
    }
}
