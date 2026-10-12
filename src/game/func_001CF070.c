#include "common.h"

int sceClose(int);
int sceLseek(int, int, int);
int sceOpen(const char *, int);

/* Opens the file (retrying until it works), takes its size, and rounds it up to a multiple of 64. */
int func_001CF070(const char *name) {
    int handle;
    int size;

    do {
        do {
            handle = sceOpen(name, 1);
        } while (handle < 0);
        size = sceLseek(handle, 0, 2);
        sceClose(handle);
    } while (size < 0);
    if (size & 0x3F) {
        do {
            size += 1;
        } while (size & 0x3F);
    }
    return size;
}
