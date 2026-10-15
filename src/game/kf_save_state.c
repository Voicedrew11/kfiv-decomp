/* What a save file holds: every block Game_WriteSaveImage writes and Game_ReadSaveImage reads back. */
#include "kf_game.h"
#include "kf_map.h"
#include "kf_entity.h"
#include "kf_player.h"

/* 0x349680 the options; Game_SetDefaultOptions sets their defaults, the code names single bytes of them */
u8 game_options[0x10] = { 0 };
asm(".globl music_volume\nmusic_volume = game_options + 1\n"
    ".globl sfx_volume\nsfx_volume = game_options + 2\n"
    ".globl D_00349683\nD_00349683 = game_options + 3\n"
    ".globl D_00349685\nD_00349685 = game_options + 5\n"
    ".globl D_00349686\nD_00349686 = game_options + 6\n"
    ".globl D_00349688\nD_00349688 = game_options + 8\n"
    ".globl D_00349689\nD_00349689 = game_options + 9\n"
    ".globl D_0034968A\nD_0034968A = game_options + 0xA\n");

/* 0x349690 a 0x80-by-0x40 byte grid the map scripts, NPCs and set-ups record their state in */
u8 map_script_flags[0x2000] = { 0 };
asm(".globl D_00349D55\nD_00349D55 = map_script_flags + 0x6C5\n"
    ".globl D_00349F56\nD_00349F56 = map_script_flags + 0x8C6\n"
    ".globl D_0034A017\nD_0034A017 = map_script_flags + 0x987\n"
    ".globl D_0034A018\nD_0034A018 = map_script_flags + 0x988\n"
    ".globl D_0034A019\nD_0034A019 = map_script_flags + 0x989\n"
    ".globl D_0034A0D5\nD_0034A0D5 = map_script_flags + 0xA45\n"
    ".globl D_0034AFD6\nD_0034AFD6 = map_script_flags + 0x1946\n"
    ".globl D_0034B098\nD_0034B098 = map_script_flags + 0x1A08\n"
    ".globl D_0034B217\nD_0034B217 = map_script_flags + 0x1B87\n"
    ".globl D_0034B21C\nD_0034B21C = map_script_flags + 0x1B8C\n");

/* 0x34B690 a 0x40-byte block of state a new game clears and a save keeps */
u8 D_0034B690[0x40] = { 0 };
asm(".globl D_0034B696\nD_0034B696 = D_0034B690 + 6\n"
    ".globl D_0034B69C\nD_0034B69C = D_0034B690 + 0xC\n");

/* 0x34B6D0 the state a group of entities shares (EntityController.unk43A picks one) */
EntityGroupRecord entity_group_state[0x32] = { { 0 } };

/* 0x34C350 a Recollection page: bits 0-2 the description shown, then a bit per message heard */
RecollectionFlags recollection_flags[50] = { { 0 } };

/* 0x34C4E0 per Recollection page: 0 not met yet, else which description it shows */
u8 recollection_pages[50] = { 0 };

/* 0x34C518 0x400 bytes a save keeps whole; the f32 map_record_height at +0xC is a record's height */
u8 D_0034C518[0x400] = { 0 };
asm(".globl map_record_height\nmap_record_height = D_0034C518 + 0xC\n");

/* 0x34C918 a bit per map tag: the maps visited (Game_IsMapVisited, Game_MarkMapVisited) */
u8 map_visited_flags[0x10] = { 0 };

/* 0x34C928 four flag bits per nibble, 0x100 bytes per map */
u8 map_entry_flags[0x8000] = { 0 };

/* 0x354928 a 4-bit counter per map entry: how often the entity there has died */
u8 map_entry_counters[0x80][0x40] = { { 0 } };

/* 0x356928 a byte per map object counting its item's pickups (kf_model.h's required_flag) */
u8 map_item_pickup_counts[0x2000] = { 0 };

/* 0x358928 the items lying on the maps: a row of 16 per map tag (kf_map_item.c) */
u8 dropped_item_records[0xA000] = { 0 };

/* 0x362928 where the map objects are marked, an ObjectMarkRow per map tag (kf_gold.c) */
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
