#include "common.h"

extern u32 *D_00446754;         /* the register list being built, NULL: none */
extern int D_00446758;          /* registers in it */

u32 *func_00109990(int);
void func_00109650(u32 *, u32 *);

/* Ends the list of GS register writes: fills in its DMA, VIF and GIF tags and queues it. */
void func_00109AD0(void) {
    u32 *packet;
    int i;
    int count;
    int blocks;
    u64 descriptors = 0xE;

    if (D_00446754 != NULL) {
        packet = D_00446754;
        i = 0;
        *packet++ = (D_00446758 + 4) | 0x10000000;
        *packet++ = 0;
        *packet++ = 0;
        *packet++ = 0;
        *packet++ = 0;
        *packet++ = 0;
        *packet++ = 0;
        *packet++ = ((D_00446758 + 1) << 16) | 0x6C00C000;
        count = D_00446758 / 16;
        blocks = count;
        count = D_00446758 - blocks * 16;
        *(u64 *)packet = (u64)((blocks + 1) | 0x8000) | ((u64)count << 60);
        packet += 2;
        *(u64 *)packet = 0;
        /* one "A+D" descriptor per register */
        if (count > 0) {
            // match: a for loop is turned into a count-down
            do {
                // match: volatile keeps the load and the store inside the loop
                *(volatile u64 *)packet |= descriptors;
                descriptors <<= 4;
                i++;
            } while (i < count);
        }
        packet = func_00109990(2);
        *packet++ = 0;
        *packet++ = 0;
        *packet++ = 0;
        *packet++ = 0x15000276;
        *packet++ = 0;
        *packet++ = 0;
        *packet++ = 0;
        *packet++ = 0x13000000;
        func_00109650(D_00446754, func_00109990(1));
        D_00446754 = NULL;
        D_00446758 = 0;
    }
}
