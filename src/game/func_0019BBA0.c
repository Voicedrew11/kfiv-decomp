#include "common.h"

void *memset(void *, int, u32);
int FlushCache(int);
int sceClose(int);
int sceOpen(const char *, int);
int sceWrite(int, const void *, int);
void sceGsSyncPath(int, int);
void sceGsSetDefStoreImage(void *, s16, s16, s16, s16, s16, s16, s16);
void sceGsExecStoreImage(void *, void *);
extern char D_0043EBB0[];

/* Dumps VRAM to a file, 32 rows at a time, from first_row through last_row. */
void func_0019BBA0(int first_row, int last_row) {
    char store_image[0x70];
    char buf[0x10000];
    int file;
    int row;

    file = sceOpen(D_0043EBB0, 0x202);
    if (file >= 0) {
        for (row = first_row; row <= last_row; row += 0x20) {
            memset(buf, 0, 0x10000);
            sceGsSetDefStoreImage(store_image, row, 1, 0, 0, 0, 0x40, 0x20);
            FlushCache(0);
            sceGsExecStoreImage(store_image, buf);
            sceGsSyncPath(0, 0);
            sceWrite(file, buf, 0x2000);
        }
        sceClose(file);
    }
}
