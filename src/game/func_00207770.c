#include "common.h"
#include <eeregs.h>

void func_00207770(s32 src_addr, s32 qword_count) {
    while ((*D2_CHCR & 0x100) || (*GIF_STAT & 0xC00));
    *D2_QWC = qword_count;
    *D2_MADR = (src_addr & 0x7FFFFFFF) | 0x80000000;
    *D2_CHCR = 0x105;
}
