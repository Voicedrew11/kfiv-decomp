#include "common.h"

u32 *func_00109990(int);
void func_00109650(u32 *, u32 *);

void func_00109E18(void) {
    u32 *packet_start;
    u32 *packet;

    packet_start = packet = func_00109990(3);
    *packet++ = 0x10000001;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = 0x13000000;
    func_00109650(packet_start, packet);
}
