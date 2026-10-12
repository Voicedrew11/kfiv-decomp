#include "common.h"
#include <eeregs.h>

void func_0021CDB8(int);
int DeleteSema(int);

/* Resets DMA channel 4 registers, then deletes the semaphore kept at +0x40. */
int func_0021D818(int *block) {
    func_0021CDB8(5);
    *D4_QWC = 0;
    *D4_MADR = 0;
    *D4_TADR = 0;
    DeleteSema(block[0x40 / 4]);
    return 1;
}
