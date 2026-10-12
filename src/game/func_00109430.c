#include "common.h"
#include <eeregs.h>
#include <libdma.h>

/* the VIF1 packets that upload the VU1 microprograms */
extern u32 D_0043B1D0[];
extern u32 D_0043BB20[];
extern u32 D_0043BB90[];
extern u32 D_0043CD40[];
extern u32 D_0043D290[];

/* sends a packet to VIF1 and waits for the transfer to end */
#define SEND_TO_VIF1(packet)                    \
    channel = sceDmaGetChan(1);                 \
    channel->chcr |= 0x40;                      \
    sceDmaSend(channel, (u32)(packet));         \
    while (*D1_CHCR & 0x100) {                  \
    }

/* Uploads the five VU1 microprograms. */
void func_00109430(void) {
    DmaChan *channel;

    SEND_TO_VIF1(D_0043B1D0);
    SEND_TO_VIF1(D_0043BB20);
    SEND_TO_VIF1(D_0043BB90);
    SEND_TO_VIF1(D_0043CD40);
    SEND_TO_VIF1(D_0043D290);
}
