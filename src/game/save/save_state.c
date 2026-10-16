/* What a save file holds: every block Game_WriteSaveImage writes and Game_ReadSaveImage reads back. */
#include "core/game.h"
#include "map/map.h"
#include "entity/entity.h"
#include "player/player.h"

/* 0x349680 the options; Game_SetDefaultOptions sets their defaults, the code names single bytes of them:
   [0] stereo (1) or mono, [1] music_volume, [2] sfx_volume, [3] a level the options screen draws as a third bar
   but has no line for, [5] head_bob_option, [6] rumble_option, [7] the picture option of a menu screen,
   [8] menu_backdrop_option (0 to 2), [9] hud_option, [0xA] brightness_option */
u8 game_options[0x10] = { 0 };
asm(".globl music_volume\nmusic_volume = game_options + 1\n"
    ".globl sfx_volume\nsfx_volume = game_options + 2\n"
    ".globl D_00349683\nD_00349683 = game_options + 3\n"
    ".globl head_bob_option\nhead_bob_option = game_options + 5\n"
    ".globl rumble_option\nrumble_option = game_options + 6\n"
    ".globl menu_backdrop_option\nmenu_backdrop_option = game_options + 8\n"
    ".globl hud_option\nhud_option = game_options + 9\n"
    ".globl brightness_option\nbrightness_option = game_options + 0xA\n");

/* 0x349690 a 0x80-by-0x40 byte grid the map scripts, NPCs and set-ups record their state in: one row of 0x40
   per map number, byte 0 of a row counting the visits. The single bytes the code names, each set by its
   map's script:
   - ocean_shore_icon_of_fire_placed (map 027 [5]): 1 once the Icon of Fire is put on the Altar of Fire
   - guardians_room_water_purified (map 035 [6]): nonzero once the Icon of Water is put on the Altar of Water;
     the room's water then stops hurting and is no longer tinted green
   - dark_sanctuary_fog_lifted (map 038 [7]): 1 once the fog has drawn back, the first time the player
     stands in a certain part of the map
   - dark_succubus_fight_started (map 038 [8]): set the first time the player gets past x 6500, which wakes the
     Dark Succubus and her Dark Archers
   - dark_sanctuary_idol_of_sorrow_placed (map 038 [9]): 1 once the Idol of Sorrow is put on the Altar of
     Darkness, which opens the way to the Dark Reality
   - craftsmans_room_door_unlocked (map 104 [8]): set when a breakable map object is broken (once [7] is set);
     until then one of the room's doors only says that it will not open
   - master_fadin_killed (map 110 [7]): set when the Master Fadin dies; the Fadins die with it
   - dark_sorcerer_fight_started (map 110 [0xC]): set when the player comes through the door to the Dark
     Sorcerer, which wakes him */
u8 map_script_flags[0x2000] = { 0 };
asm(".globl ocean_shore_icon_of_fire_placed\nocean_shore_icon_of_fire_placed = map_script_flags + 0x6C5\n"
    ".globl guardians_room_water_purified\nguardians_room_water_purified = map_script_flags + 0x8C6\n"
    ".globl dark_sanctuary_fog_lifted\ndark_sanctuary_fog_lifted = map_script_flags + 0x987\n"
    ".globl dark_succubus_fight_started\ndark_succubus_fight_started = map_script_flags + 0x988\n"
    ".globl dark_sanctuary_idol_of_sorrow_placed\ndark_sanctuary_idol_of_sorrow_placed = map_script_flags + 0x989\n"
    ".globl D_0034A0D5\nD_0034A0D5 = map_script_flags + 0xA45\n"
    ".globl D_0034AFD6\nD_0034AFD6 = map_script_flags + 0x1946\n"
    ".globl craftsmans_room_door_unlocked\ncraftsmans_room_door_unlocked = map_script_flags + 0x1A08\n"
    ".globl master_fadin_killed\nmaster_fadin_killed = map_script_flags + 0x1B87\n"
    ".globl dark_sorcerer_fight_started\ndark_sorcerer_fight_started = map_script_flags + 0x1B8C\n");

/* 0x34B690 0x40 story bytes a new game clears and a save keeps (block 11 of the script variables):
   [6] forge_stage, which of the Forge maps 017-019 is in use (0-2; the set-ups of the Earth Folks' Village,
   the Cavern of Lava and the Fire Lord's Domain follow it too), [0xC] forge_door_open (door 0x65 of the Forge
   starts open), [0xD] set when the player kills an NPC */
u8 story_flags[0x40] = { 0 };
asm(".globl forge_stage\nforge_stage = story_flags + 6\n"
    ".globl forge_door_open\nforge_door_open = story_flags + 0xC\n");

/* 0x34B6D0 the state a group of entities shares (EntityController.entity_group_index picks one) */
EntityGroupRecord entity_group_state[0x32] = { { 0 } };

/* 0x34C350 a Recollection page: bits 0-2 the description shown, then a bit per message heard */
RecollectionFlags recollection_flags[50] = { { 0 } };

/* 0x34C4E0 per Recollection page: 0 not met yet, else which description it shows */
u8 recollection_pages[50] = { 0 };

/* 0x34C518 0x400 bytes of story values a save keeps whole (block 15 of the script variables); the f32
   map_record_height at +0xC is a record's height */
u8 story_values[0x400] = { 0 };
asm(".globl map_record_height\nmap_record_height = story_values + 0xC\n");

/* 0x34C918 a bit per map tag: the maps visited (Game_IsMapVisited, Game_MarkMapVisited) */
u8 map_visited_flags[0x10] = { 0 };

/* 0x34C928 four flag bits per nibble, 0x100 bytes per map */
u8 map_entry_flags[0x8000] = { 0 };

/* 0x354928 a 4-bit counter per map entry: how often the entity there has died */
u8 map_entry_counters[0x80][0x40] = { { 0 } };

/* 0x356928 a byte per map object counting its item's pickups (model/model.h's required_flag) */
u8 map_item_pickup_counts[0x2000] = { 0 };

/* 0x358928 the items lying on the maps: a row of 16 per map tag (item/map_item.c) */
u8 dropped_item_records[0xA000] = { 0 };

/* 0x362928 where the map objects are marked, an ObjectMarkRow per map tag (item/gold.c) */
u8 map_object_marks[0x5000] = { 0 };

/* 0x367928[5] which item each trigger holds */
TriggerItemRecord trigger_items[TRIGGER_ITEM_RECORD_COUNT] = { { 0 } };

/* 0x367940 the experience of each of the six schools' five spells */
u16 spell_experience[6][5] = { { 0 } };

/* 0x367980 the level of each of them */
u8 spell_levels[6][5] = { { 0 } };

/* 0x3679A0 what has been gained per weapon, a LevelRecord each */
LevelRecord weapon_level_records[70] = { { 0 } };

/* 0x3682F0 what has been gained per worn item, a LevelRecord each */
LevelRecord worn_item_level_records[160] = { { 0 } };

/* 0x369830 how many of each item the player has, one byte per item definition */
DefinitionByteSet item_counts = { { 0 } };

/* 0x3699B0 the five trade tables' stock, copied from 0x0043D9B0 on a new game */
DefinitionByteSet trade_stock_1 = { { 0 } };
DefinitionByteSet trade_stock_2 = { { 0 } };
DefinitionByteSet trade_stock_3 = { { 0 } };
DefinitionByteSet trade_stock_4 = { { 0 } };
DefinitionByteSet trade_stock_5 = { { 0 } };

/* 0x36A130 the save image; the play time save_play_time is at +8 */
u8 save_image[0x20000] = { 0 };
asm(".globl save_play_time\nsave_play_time = save_image + 8\n");
