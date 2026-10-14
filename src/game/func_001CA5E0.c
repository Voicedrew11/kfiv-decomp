#include "common.h"

extern u8 item_counts[];
extern u8 D_0041A6F8[];

typedef struct {
    char pad00[0xE];
    u16 flags;
    char pad10[0x10];
} Ent;

int func_001CA5E0(int item) {
    if (item_counts[item] != 0 && (item >= 0x96 || ((((Ent *)D_0041A6F8) + item)->flags >> 1 & 1))) {
        return 0;
    }
    return 1;
}
