#include "common.h"
#include "kf_packet.h"

extern s32 D_00446D50;
extern BufferPair D_00446D48[];

/* Flips between the two packet buffers and returns the one now current. */
void *func_001FBE70(void) {
    BufferPair pair = D_00446D48[0];

    D_00446D50 = D_00446D50 == 0;
    return pair.buffers[D_00446D50];
}
