#include "common.h"
#include <eeregs.h>

int FlushCache(int);
int sceGsSyncPath(int, int);

int func_00209CD8(u32 packet, u32 qword_count) {
    FlushCache(0);
    while (*D2_CHCR & 0x100);
    *D2_QWC = qword_count;
    *D2_MADR = packet;
    *D2_CHCR = 0x100;
    sceGsSyncPath(0, 0);
    return 0;
}
