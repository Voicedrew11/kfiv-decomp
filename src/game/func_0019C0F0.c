#include "common.h"

int FlushCache(int);
int sceClose(int);
int sceOpen(const char *, int);
int sceWrite(int, const void *, int);
int sceGsSyncPath(int, int);
void sceGsSetDefStoreImage(void *, s16, s16, s16, s16, s16, s16, s16);
void sceGsExecStoreImage(void *, void *);
int sprintf(char *, const char *, ...);
extern int D_00446F78;
extern char D_0043EC20[];

/* Stores the screen into a 16-byte aligned buffer and writes it to the first unused numbered screenshot file. */
void func_0019C0F0(u8 *buffer) {
    char store_image[0x70];
    char name[0x100];
    int file;
    int i;
    int in_range;
    s16 base;
    u32 misalign;
    u8 *aligned;

    base = D_00446F78 != 0 ? 0 : 0x1180;
    i = 0;
    // match: the test stays at the top only in a do loop; while/for get rotated
    do {
        in_range = i < 0x400;
        if (!in_range) {
            break;
        }
        sprintf(name, D_0043EC20, i);
        file = sceOpen(name, 2);
        if (file < 0) {
            break;
        }
        sceClose(file);
        i++;
    } while (TRUE);
    if (in_range) {
        aligned = buffer;
        misalign = (u32)aligned & 0xF;
        if (misalign != 0) {
            aligned -= misalign - 0x10;
        }
        sceGsSetDefStoreImage(store_image, base, 0xA, 1, 0, 0, 0x280, 0x1C0);
        FlushCache(0);
        sceGsExecStoreImage(store_image, aligned);
        sceGsSyncPath(0, 0);
        file = sceOpen(name, 0x202);
        if (file >= 0) {
            sceWrite(file, aligned, 0xD2000);
            sceClose(file);
        }
    }
}
