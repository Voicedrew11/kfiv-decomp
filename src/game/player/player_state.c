/* player_state: the player himself, PlayerState in player/player.h; player_previous the player as of the frame before. */
#include "common.h"
#include "player/player.h"

PlayerState player_state = { 0 }; /* 0x413F80 Player_ResetForNewGame clears and copies all 0x300 bytes of it */
PlayerState player_previous = { 0 }; /* 0x414280 the player as of the frame before, which Player_Move reads */

/* Fields several files still read by their old splat names. */
asm(".globl player_rotation\nplayer_rotation = player_state + 0x10\n");   /* rotation */
asm(".globl player_weapon\nplayer_weapon = player_state + 0x3C\n");   /* weapon */
asm(".globl player_hp_max\nplayer_hp_max = player_state + 0x58\n");   /* hp_max */
asm(".globl entity_under_player\nentity_under_player = player_state + 0x204\n");  /* stepped_on_entity the entity the player stands on */
asm(".globl object_under_player\nobject_under_player = player_state + 0x208\n");  /* floor_map_object the map object the player stands on */
asm(".globl player_floor_block\nplayer_floor_block = player_state + 0x250\n");  /* record_flag_block the flag block of the floor record under the player */
asm(".globl player_weapon_step\nplayer_weapon_step = player_state + 0x258\n");  /* item_action_step the step of the weapon's action, 0 while idle */
asm(".globl player_status_bits\nplayer_status_bits = player_previous + 0x1C8\n");  /* status_bits */
asm(".globl player_previous_fade_level\nplayer_previous_fade_level = player_previous + 0x2A4\n");  /* fade_area: the map record byte 0x4C under the player, the frame before */
