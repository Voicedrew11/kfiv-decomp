#include "common.h"

int puts(char *);
extern char D_0043E4A0[];
extern int D_00446748;
extern u32 D_00447038;
extern int D_0044703C;

typedef struct { int w0, w1, w2, w3; } Blk;
extern void *D_00447018[2];
extern Blk *D_00447020[2];

void func_00109650(u32 *packet, u32 *packet_end) {
    void **packet_head;
    Blk **packet_tail;

    packet_head = &D_00447018[D_00446748];
    packet_tail = &D_00447020[D_00446748];
    if (D_0044703C == 0) {
        if ((u32)packet_end >= D_00447038) {
            puts(D_0043E4A0);
            while (1) {
            }
        } else {
            if (*packet_head != 0) {
                Blk *tail_tag = *packet_tail;
                tail_tag->w0 = 0x20000000;
                tail_tag->w1 = (int)packet;
                tail_tag->w3 = tail_tag->w2 = 0;
            } else {
                *packet_head = packet;
            }
            *packet_tail = (Blk *)packet_end;
        }
    }
}
