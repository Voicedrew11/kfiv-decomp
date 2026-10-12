#include "common.h"

u32 *func_00109990(int);
void func_00109650(u32 *, u32 *);
void func_00109E18(void);
void func_00109E88(void);
extern int *D_004470F8;

void func_001A2108(void) {
    u32 *packet_start;
    u32 *packet;

    func_00109E18();
    packet_start = packet = func_00109990(2);
    *packet++ = *D_004470F8 | 0x30000000;
    *packet++ = (u32)D_004470F8 + 0x10;
    *packet++ = 0;
    *packet++ = 0;
    func_00109650(packet_start, packet);
    func_00109E88();
}
