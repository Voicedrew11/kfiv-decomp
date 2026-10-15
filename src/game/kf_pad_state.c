/* pad_state: the copy of port 0's controller the game tests; pad_states the two controllers. */
#include "common.h"
#include "kf_game.h"

PadState pad_state = { 0 };                                         /* 0x413DA8 */
PadState pad_states[2] = { 0 };                                      /* 0x413DB8 */
u32 D_00413DD8[2] __attribute__((section(".data"))) = { 0 };         /* 0x413DD8 */

/* pad_pressed is pad_state.pressed: several files still read it by that name. */
asm(".globl pad_pressed\npad_pressed = pad_state + 2\n");
