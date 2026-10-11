#include "common.h"

extern u8 D_00369830[];
extern u8 D_0041A6F8[];

typedef struct {
    char pad00[0xE];
    u16 f0E;
    char pad10[0x10];
} Ent;

int func_001CA5E0(int arg0) {
    if (D_00369830[arg0] != 0 && (arg0 >= 0x96 || ((((Ent *)D_0041A6F8) + arg0)->f0E >> 1 & 1))) {
        return 0;
    }
    return 1;
}
