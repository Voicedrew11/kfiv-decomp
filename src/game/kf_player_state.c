/* player_state: the player himself, PlayerState in kf_player.h; player_previous the player as of the frame before. */
#include "common.h"
#include "kf_player.h"

PlayerState player_state = { 0 }; /* 0x413F80 Player_ResetForNewGame clears and copies all 0x300 bytes of it */
PlayerState player_previous = { 0 }; /* 0x414280 the player as of the frame before, which Player_Move reads */

/* Fields several files still read by their old splat names. */
asm(".globl player_rotation\nplayer_rotation = player_state + 0x10\n");   /* rotation */
asm(".globl player_weapon\nplayer_weapon = player_state + 0x3C\n");   /* weapon */
asm(".globl player_hp_max\nplayer_hp_max = player_state + 0x58\n");   /* hp_max */
asm(".globl D_00414184\nD_00414184 = player_state + 0x204\n");  /* unk204 */
asm(".globl D_00414188\nD_00414188 = player_state + 0x208\n");  /* unk208 */
asm(".globl D_004141D0\nD_004141D0 = player_state + 0x250\n");  /* unk250 */
asm(".globl D_004141D8\nD_004141D8 = player_state + 0x258\n");  /* unk258 */
asm(".globl player_status_bits\nplayer_status_bits = player_previous + 0x1C8\n");  /* unk1C8 status bits */
asm(".globl D_00414524\nD_00414524 = player_previous + 0x2A4\n");  /* the fade block at pad2A4 */
