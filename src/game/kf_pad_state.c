/* D_00413DA8: the copy of port 0's controller the game tests; D_00413DB8 the two controllers. */
#include "common.h"
#include "kf_game.h"

PadState D_00413DA8 = { 0 };                                         /* 0x413DA8 */
PadState D_00413DB8[2] = { 0 };                                      /* 0x413DB8 */
u32 D_00413DD8[2] __attribute__((section(".data"))) = { 0 };         /* 0x413DD8 */

/* D_00413DAA is D_00413DA8.pressed: several files still read it by that name. */
asm(".globl D_00413DAA\nD_00413DAA = D_00413DA8 + 2\n");
