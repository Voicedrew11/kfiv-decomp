#include "common.h"

int FlushCache(int);
int sceGsSyncPath(int, int);
void sceGsSetDefStoreImage(void *, s16, s16, s16, s16, s16, s16, s16);
void sceGsExecStoreImage(void *, void *);
extern u32 *D_004470F8;

/* Appends a GS store-image packet to the DMA buffer, then runs it and waits for the path to drain. */
void func_001A1868(void) {
    char store_image[0x70];
    u32 *packet;

    packet = D_004470F8;
    *packet++ = 0x7007;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = 0x51007006;
    *(u64 *)packet = 0x1000000000000004;
    packet += 2;
    *(u64 *)packet = 0xE;
    packet += 2;
    *(u64 *)packet = 0x8B90L << 34;
    packet += 2;
    *packet++ = 0x50;
    *packet++ = 0;
    *(u64 *)packet = 0;
    packet += 2;
    *packet++ = 0x51;
    *packet++ = 0;
    *(u64 *)packet = 0x38000000080;
    packet += 2;
    *packet++ = 0x52;
    *packet++ = 0;
    *(u64 *)packet = 0;
    packet += 2;
    *packet++ = 0x53;
    *packet++ = 0;
    *(u64 *)packet = 0x080000000000F000;
    packet += 2;
    *(u64 *)packet = 0;
    packet += 2;
    sceGsSetDefStoreImage(store_image, 0x2E40, 2, 0, 0, 0, 0x80, 0x380);
    FlushCache(0);
    sceGsExecStoreImage(store_image, packet);
    sceGsSyncPath(0, 0);
}
