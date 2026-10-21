/* The game status block and the loaded maps' state. */
#include "core/game.h"
#include "map/map.h"

GameStatus game_status = { 0 }; /* 0x38CC50 the status of the running game */
/* Field aliases other files still use by their own names. */
/* 0x38CC57 movement_input_enabled: 0 holds back the walk and step buttons; Player_Update sets it to 1 again */
asm(".globl player_can_walk\nplayer_can_walk = game_status + 0x7");
asm(".globl game_status_flags\ngame_status_flags = game_status + 0x24");  /* 0x38CC74 the flag word */
/* 0x38CC80 update_hold_flags: the updates held back (0x1 the player, 0x4 the entities, 0x8 the map objects' sway,
   0x10 falling items, ...) */
asm(".globl suspended_updates\nsuspended_updates = game_status + 0x30");
/* 0x38CC84 save_area: the message id of the area name the save list shows */
asm(".globl save_area_name_id\nsave_area_name_id = game_status + 0x34");

/* 0x38CCA0 default light block for map objects: one light (125, 110, 83) */
MapLightBlockLights map_object_default_lights __attribute__((aligned(16))) = {
    { { 0 } },
    { { 0 }, { 0 }, { 0 }, { 125.0f / 255.0f, 110.0f / 255.0f, 83.0f / 255.0f, 0.0f } },
};

MapSlot map_slots[2] = { 0 }; /* 0x38CD20 the two loaded maps */
asm(".globl map_slot0_header\nmap_slot0_header = map_slots + 0x8");  /* 0x38CD28 slot 0's header (sound/sound_game.c) */

/* 0x38CDE0 the prop of each prop id, per map slot */
MapProp *map_slot_props[2][0xFF] = { 0 };
