#include "common.h"

extern u8 item_counts[];

/* Takes one from the count at the index, if there is any. */
void func_001BC2A0(u32 index) {
    if (index < 0x17C && item_counts[index] != 0) {
        item_counts[index]--;
    }
}
