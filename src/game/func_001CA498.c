#include "common.h"

extern s16 *D_004469C0[2];
extern char D_0057E040[];

/* Scans the slot's list of up to 32 ids for key; returns the matching 8-byte entry, or NULL. */
void *func_001CA498(int slot, int key) {
    s16 *cursor;
    s16 value;
    int i;

    cursor = D_004469C0[slot];
    for (i = 0; i < 0x20; i++) {
        value = cursor[i];
        if (value < 0) {
            return NULL;
        }
        if (value == key) {
            return D_0057E040 + (slot << 8) + i * 8;
        }
    }
    return NULL;
}
