#include "common.h"

extern u8 D_0034C350[0x190];   // 50 areas x 8 bytes of bits

int func_001037F8(int id) {
    int r = id % 50;
    int area = (id - 3000) / 50;
    return (D_0034C350[r / 8 + area * 8] >> (r % 8)) & 1;
}
