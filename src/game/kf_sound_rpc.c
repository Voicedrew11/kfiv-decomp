/* The sound driver's RPC buffers: the argument block func_0010E050 fills and the file name it sends. */
#include "common.h"

u32 D_00420918[10] = { 0 };     /* 0x420918 the ten words before the argument block */
int D_00420940[0x40] = { 0 };   /* 0x420940 the arguments of the sound RPC */
char D_00420A40[0x100] = { 0 }; /* 0x420A40 the file name the sound RPC sends */
