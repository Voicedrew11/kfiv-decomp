/* The saved game state: options, story flags and per-map records (save/save_state.c). */
#ifndef KF_SAVE_STATE_H
#define KF_SAVE_STATE_H

#include "common.h"
#include "core/game.h"
#include "entity/entity.h"
#include "map/map.h"
#include "player/player.h"

/* save/save_state.c */
extern u8 music_volume[];
extern u8 sfx_volume[];
extern s8 head_bob_option[];
extern s8 rumble_option[];
extern s8 menu_backdrop_option[];
extern s8 hud_option[];
extern u8 brightness_option[];
extern u8 game_options[];  /* the options block, a byte per setting */
extern u8 game_option_third_bar[];
extern u8 ocean_shore_icon_of_fire_placed[];
extern u8 guardians_room_water_purified[];
extern u8 dark_sanctuary_fog_lifted[];
extern u8 dark_succubus_fight_started[];
extern u8 dark_sanctuary_idol_of_sorrow_placed[];
extern u8 craftsmans_room_door_unlocked[];
extern u8 master_fadin_killed[];
extern u8 dark_sorcerer_fight_started[];
extern u8 map_script_flags[];
extern u8 kings_throne_alcoves_story_flag[];
extern u8 loop_effect_stop_state[];
extern u8 forge_stage[];
extern u8 forge_door_open[];
extern u8 story_flags[];
extern EntityGroupRecord entity_group_state[];
extern RecollectionFlags recollection_flags[50];
extern u8 recollection_pages[50];
extern f32 map_record_height[];
extern u8 story_values[];
extern u8 map_visited_flags[0x10];
extern u8 map_entry_flags[0x8000];
extern u8 map_entry_counters[0x80][0x40];
extern u8 map_item_pickup_counts[0x2000];
extern u8 dropped_item_records[0xA000];
extern u8 map_object_marks[];
extern TriggerItemRecord trigger_items[TRIGGER_ITEM_RECORD_COUNT];
extern u16 spell_experience[6][5];
extern u8 spell_levels[][5];
extern LevelRecord weapon_level_records[];  /* the weapons' records */
extern LevelRecord worn_item_level_records[];  /* the worn items' records */
extern DefinitionByteSet item_counts;
extern DefinitionByteSet trade_stock_1;
extern DefinitionByteSet trade_stock_2;
extern DefinitionByteSet trade_stock_3;
extern DefinitionByteSet trade_stock_4;
extern DefinitionByteSet trade_stock_5;
extern PlayTime save_play_time[];  /* the play time in the loaded save (save_image + 8) */
extern u8 save_image[];

#endif
