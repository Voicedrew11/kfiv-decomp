#include "common.h"

extern int D_00420B40[64];

char *func_001CFC68(s32 arg0) {
    return (arg0 * 0x10) + ((char *)&D_00420B40);
}
