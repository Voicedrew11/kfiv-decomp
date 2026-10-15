/* D_00413F80: the player himself, PlayerState in kf_player.h; D_00414280 the player as of the frame before. */
#include "common.h"
#include "kf_player.h"

PlayerState D_00413F80 = { 0 }; /* 0x413F80 func_001B5810 clears and copies all 0x300 bytes of it */
PlayerState D_00414280 = { 0 }; /* 0x414280 the player as of the frame before, which Player_Move reads */

/* Fields several files still read by their old splat names. */
asm(".globl D_00413F90\nD_00413F90 = D_00413F80 + 0x10\n");   /* rotation */
asm(".globl D_00413FBC\nD_00413FBC = D_00413F80 + 0x3C\n");   /* weapon */
asm(".globl D_00413FD8\nD_00413FD8 = D_00413F80 + 0x58\n");   /* hp_max */
asm(".globl D_00414184\nD_00414184 = D_00413F80 + 0x204\n");  /* unk204 */
asm(".globl D_00414188\nD_00414188 = D_00413F80 + 0x208\n");  /* unk208 */
asm(".globl D_004141D0\nD_004141D0 = D_00413F80 + 0x250\n");  /* unk250 */
asm(".globl D_004141D8\nD_004141D8 = D_00413F80 + 0x258\n");  /* unk258 */
asm(".globl D_00414448\nD_00414448 = D_00414280 + 0x1C8\n");  /* unk1C8 status bits */
asm(".globl D_00414524\nD_00414524 = D_00414280 + 0x2A4\n");  /* the fade block at pad2A4 */
