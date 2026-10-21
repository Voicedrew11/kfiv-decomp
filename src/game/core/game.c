/* The game module: new-game state, event flags, the save image, the screen fade and GS screen effects. */
#include "core/file.h"
#include "core/game.h"
#include "input/pad.h"
#include "map_object/map_object.h"
#include "player/player.h"
#include "map/map.h"
#include "gfx/gs.h"
#include "entity/entity.h"
#include "libvu0.h"
#include "libgcc2.h"
#include "math.h"
#include "stdlib.h"
#include "ui/menu_screens.h"
#include "ui/message.h"
#include "item/item.h"
#include <string.h>
#include "gfx/gs_draw.h"
#include "math/vu0.h"
#include "math/camera.h"
#include "enemy/enemy.h"
#include "gfx/packet.h"
#include "save/save_state.h"
#include "sound/sound.h"

/* spell_levels entries: 0 not learned, else the spell's level (its experience starts at 0, 50 or 150) */
#define SPELL_LEVEL_NONE            0
#define SPELL_LEVEL_1               1
#define SPELL_LEVEL_2               2
#define SPELL_LEVEL_3               3

/* kf4_directory_words[3]: the fade running, 0 none (Game_StartFadeIn sets 1, Game_StartFadeOut -1) */
#define FADE_DIRECTION_NONE         0
#define FADE_DIRECTION_IN           1
#define FADE_DIRECTION_OUT          -1

/* cos 30 degrees: focus candidates must lie within this cone around the view ray */
#define FOCUS_CONE_MIN_COS 0.86602497f

static int wobble_grid_origin[8][11][2];     /* 0x467680 the grid as laid out */
static int wobble_grid_points[8][11][2];     /* 0x467940 the wobbling points */
static int wobble_grid_previous[8][11][2];     /* 0x467C00 the points of the frame before */
static float wobble_grid_forces[8][11][2];   /* 0x467EC0 the force on each point */
static int fade_color[3];            /* 0x468180 the fade colour */

static int Game_GetObjectBit(int map, int entry, int bit);
static void Game_SetObjectBit(int map, int entry, int bit, int value);

/* sets the options to their defaults */
void Game_SetDefaultOptions(void) {
    memset(game_options, 0, sizeof(game_options));
    game_options[0] = 1;
    game_options[1] = 0x40;
    game_options[2] = 0x40;
    game_options[3] = 0x40;
    game_options[4] = 1;
    game_options[5] = 1;
    game_options[6] = 1;
    game_options[7] = 1;
    game_options[8] = 0;
    game_options[9] = 1;
    game_options[10] = 0x88;
}

/* clears the state a new game starts from */
void Game_ResetProgress(void) {
    int i;

    memset(map_script_flags, 0, sizeof(map_script_flags));
    memset(story_flags, 0, sizeof(story_flags));
    memset(entity_group_state, 0, sizeof(entity_group_state));
    memset(recollection_flags, 0, sizeof(recollection_flags));
    memset(recollection_pages, 0, sizeof(recollection_pages));
    recollection_pages[0] = 1;
    memset(story_values, 0, sizeof(story_values));
    memset(map_visited_flags, 0, sizeof(map_visited_flags));
    memset(map_entry_flags, 0, sizeof(map_entry_flags));
    memset(map_entry_counters, 0, sizeof(map_entry_counters));
    memset(map_item_pickup_counts, 0, sizeof(map_item_pickup_counts));
    memset(dropped_item_records, 0, sizeof(dropped_item_records));
    memset(map_object_marks, 0, sizeof(map_object_marks));
    memset(trigger_items, 0, sizeof(trigger_items));
    trigger_items[0].map_tag = 0x66;
    trigger_items[0].trigger_id = 200;
    trigger_items[0].item = 0x87;
    trigger_items[1].map_tag = 0xD;
    trigger_items[1].trigger_id = 200;
    trigger_items[1].item = 0x88;
    memset(weapon_level_records, 0, sizeof(weapon_level_records));
    memset(worn_item_level_records, 0, sizeof(worn_item_level_records));
    for (i = 0; i < 70; i++) {
        weapon_level_records[i].level = item_name_defs[ITEM_WEAPON_FIRST + i].starting_level_percent;
    }
    for (i = 0; i < 160; i++) {
        worn_item_level_records[i].level = item_name_defs[ITEM_WORN_FIRST + i].starting_level_percent;
    }
    memset(&item_counts, 0, sizeof(item_counts));
    trade_stock_1 = initial_trade_stock_1;
    trade_stock_2 = initial_trade_stock_2;
    trade_stock_3 = initial_trade_stock_3;
    trade_stock_4 = initial_trade_stock_4;
    trade_stock_5 = initial_trade_stock_5;
    memset(spell_experience, 0, sizeof(spell_experience));
    memset(spell_levels, 0, sizeof(spell_levels));
    Menu_ClearKeyItems();
    memset(&sound_load_queue, 0, sizeof(sound_load_queue));
}

/* marks what the player has equipped as found and tidies the spell tables */
void Game_NormalizeStartState(void) {
    int i;
    int j;
    int level;

    if (player_state.weapon != 0) {
        item_counts.bytes[player_state.weapon + ITEM_WEAPON_FIRST] = 1;
    }
    if (player_state.equipment[EQUIP_SHIELD] != ITEM_WORN_INDEX(ITEM_SHIELD_NONE)) {
        item_counts.bytes[player_state.equipment[EQUIP_SHIELD] + ITEM_WORN_FIRST] = 1;
    }
    if (player_state.equipment[EQUIP_HELM] != ITEM_WORN_INDEX(ITEM_HELM_NONE)) {
        item_counts.bytes[player_state.equipment[EQUIP_HELM] + ITEM_WORN_FIRST] = 1;
    }
    if (player_state.equipment[EQUIP_ARMOR] != ITEM_WORN_INDEX(ITEM_ARMOR_NONE)) {
        item_counts.bytes[player_state.equipment[EQUIP_ARMOR] + ITEM_WORN_FIRST] = 1;
    }
    if (player_state.equipment[EQUIP_GAUNTLET] != ITEM_WORN_INDEX(ITEM_GAUNTLET_NONE)) {
        item_counts.bytes[player_state.equipment[EQUIP_GAUNTLET] + ITEM_WORN_FIRST] = 1;
    }
    if (player_state.equipment[EQUIP_BOOT] != ITEM_WORN_INDEX(ITEM_BOOT_NONE)) {
        item_counts.bytes[player_state.equipment[EQUIP_BOOT] + ITEM_WORN_FIRST] = 1;
    }
    if (player_state.equipment[EQUIP_RING] != ITEM_WORN_INDEX(ITEM_RING_NONE)) {
        item_counts.bytes[player_state.equipment[EQUIP_RING] + ITEM_WORN_FIRST] = 1;
    }
    if (player_state.equipment[EQUIP_BRACELET] != ITEM_WORN_INDEX(ITEM_BRACELET_NONE)) {
        item_counts.bytes[player_state.equipment[EQUIP_BRACELET] + ITEM_WORN_FIRST] = 1;
    }
    if (player_state.equipment[EQUIP_AMULET] != ITEM_WORN_INDEX(ITEM_AMULET_NONE)) {
        item_counts.bytes[player_state.equipment[EQUIP_AMULET] + ITEM_WORN_FIRST] = 1;
    }
    for (i = 0; i < ITEM_WEAPON_FIRST; i++) {
        if (item_counts.bytes[i] >= 2 && (item_common_defs[i].flags >> 1 & 1)) {
            item_counts.bytes[i] = 1;
        }
    }
    for (i = ITEM_WEAPON_FIRST; i < ITEM_WORN_FIRST; i++) {
        if (item_counts.bytes[i] >= 2) {
            item_counts.bytes[i] = 1;
        }
    }
    for (i = ITEM_WORN_FIRST; i < ITEM_COUNT; i++) {
        if (item_counts.bytes[i] >= 2) {
            item_counts.bytes[i] = 1;
        }
    }
    item_counts.bytes[0] = 1;
    item_counts.bytes[ITEM_WEAPON_FIRST] = 1;
    item_counts.bytes[ITEM_SHIELD_NONE] = 1;
    item_counts.bytes[ITEM_HELM_NONE] = 1;
    item_counts.bytes[ITEM_ARMOR_NONE] = 1;
    item_counts.bytes[ITEM_GAUNTLET_NONE] = 1;
    item_counts.bytes[ITEM_BOOT_NONE] = 1;
    item_counts.bytes[ITEM_RING_NONE] = 1;
    item_counts.bytes[ITEM_BRACELET_NONE] = 1;
    item_counts.bytes[ITEM_AMULET_NONE] = 1;
    for (i = 0; i < 6; i++) {
        for (j = 0; j < 5; j++) {
            if (spell_levels[i][j] == 0) {
                for (; j < 5; j++) {
                    spell_levels[i][j] = 0;
                }
            }
        }
    }
    // match: through an int; a bare byte compare is narrowed and gives bnez, not bgtz
    if (player_state.equipped_magic == 0 || (level = spell_levels[(player_state.equipped_magic - 1) / 15][(player_state.equipped_magic - 1) / 3 % 5]) <= 0) {
        player_state.equipped_magic = 0;
    }
    for (i = 0; i < 6; i++) {
        for (j = 0; j < 5; j++) {
            if (spell_experience[i][j] == 0) {
                switch (spell_levels[i][j]) {
                case SPELL_LEVEL_NONE:
                    spell_experience[i][j] = 0;
                    break;
                case SPELL_LEVEL_1:
                    spell_experience[i][j] = 0;
                    break;
                case SPELL_LEVEL_2:
                    spell_experience[i][j] = 50;
                    break;
                case SPELL_LEVEL_3:
                    spell_experience[i][j] = 150;
                    break;
                default:
                    spell_experience[i][j] = 0;
                    spell_levels[i][j] = 0;
                    break;
                }
            }
        }
    }
    player_state.hp_now = player_state.hp_base;
    player_state.mp_now = player_state.mp_base;
}

/* one of the 128 flags in map_visited_flags */
int Game_IsMapVisited(int flag) {
    if ((u32)flag < 0x80U) {
        return map_visited_flags[flag / 8] >> (flag % 8) & 1;
    }
    return 0;
}

/* marks map tag `flag` as visited (one of the 128 flags in map_visited_flags) */
void Game_MarkMapVisited(int flag) {
    if ((u32)flag < 0x80U) {
        map_visited_flags[flag / 8] |= 1 << (flag % 8);
    }
}

/* how many times the entity of spawn record `entry` of a map has died: a 4-bit counter (EntityAi_ShouldRespawn
   compares it with the spawn's limit) */
int Game_GetSpawnDeathCount(int map, int entry) {
    int count;

    if ((u32)map < 0x80U && entry >= 0 && entry < 0x80) {
        count = map_entry_counters[map][entry / 2];
        if (entry & 1) {
            count >>= SPAWN_COUNTER_HIGH_SHIFT;
        } else {
            count &= SPAWN_COUNTER_LOW_MASK;
        }
        return count;
    }
    return 0;
}

/* counts a death of the entity of spawn record `entry` of a map (EntityEvent_OnDeath); a nibble at 15 is not held */
void Game_CountSpawnDeath(u32 map, int entry) {
    u32 shifted;
    u32 old_byte;
    u32 low_nibble;
    u32 count;
    int merged;

    if (map < 0x80 && entry >= 0 && entry < 0x80) {
        count = (old_byte = ((u8 *)map_entry_counters)[(entry / 2) + (map << 6)]);
        if (entry & 1) {
            count >>= SPAWN_COUNTER_HIGH_SHIFT;
            count++;
            shifted = count << 4;
            ((u8 *)map_entry_counters)[(entry / 2) + (map << 6)] = (low_nibble = old_byte & SPAWN_COUNTER_LOW_MASK);
            count = shifted & 0xFF;
        } else {
            count &= SPAWN_COUNTER_LOW_MASK;
            count++;
            ((u8 *)map_entry_counters)[(entry / 2) + (map << 6)] = old_byte & SPAWN_COUNTER_HIGH_MASK;
        }
        merged = count | ((u8 *)map_entry_counters)[(entry / 2) + (map << 6)]; // match: merged in an int
        ((u8 *)map_entry_counters)[(entry / 2) + (map << 6)] = merged;
    }
}

/* map_entry_flags keeps four bits for each map object: 0 and 1 (MapObject bits 18 and 19), 2 whether the object
   is active (bit 16, MapObject_Activate) and 3 its state (bit 17: a chest opened, a mechanism switched on).
   Sets flag 0 or 1 of a map object; other bits are ignored. */
void Game_SetObjectFlag(int map, int entry, u32 bit, int value) {
    if (bit < 2) {
        Game_SetObjectBit(map, entry, bit, value);
    }
}

/* flag 0 or 1 of a map object; 0 for other bits */
int Game_GetObjectFlag(int map, int entry, u32 bit) {
    if (bit < 2) {
        return Game_GetObjectBit(map, entry, bit);
    }
    return 0;
}

/* whether a map object was active when its map was left (flag bit 2) */
int Game_GetObjectActive(int map, int entry) {
    return Game_GetObjectBit(map, entry, 2);
}

/* stores whether a map object is active (flag bit 2) */
void Game_SetObjectActive(int map, int entry, int value) {
    Game_SetObjectBit(map, entry, 2, value);
}

/* the state of a map object (flag bit 3: opened, switched on) */
int Game_GetObjectState(int map, int entry) {
    return Game_GetObjectBit(map, entry, 3);
}

/* stores the state of a map object (flag bit 3) */
void Game_SetObjectState(int map, int entry, int value) {
    Game_SetObjectBit(map, entry, 3, value);
}

/* one of the four flag bits of a map object; 0 when an index is out of range */
static int Game_GetObjectBit(int map, int entry, int bit) {
    u32 mask = 1;

    if ((u32)map >= 0x80) {
        return 0;
    }
    if ((u32)entry >= 0x200) {
        return 0;
    }
    if ((u32)bit >= 4) {
        return 0;
    }
    mask = (mask << bit) & 0xFF;
    mask = (mask << ((entry % 2) * 4)) & 0xFF;
    return (map_entry_flags[entry / 2 + map * 0x100] & mask) != 0;
}

/* sets or clears one of the four flag bits of a map object */
static void Game_SetObjectBit(int map, int entry, int bit, int value) {
    if ((u32)map < 0x80 && (u32)entry < 0x200 && (u32)bit < 4) {
        u8 mask = (u8)(1 << bit);
        u8 *flag_byte;

        mask = (u8)(mask << ((entry % 2) * 4));
        flag_byte = &map_entry_flags[entry / 2 + map * 0x100];
        *flag_byte = *flag_byte & ~mask;
        if (value != 0) {
            *flag_byte = *flag_byte | mask;
        }
    }
}

/* TRUE once message `flag` of a Recollection page (an id from MSG_RECOLLECTION_FIRST) has been shown */
int Game_IsMessageHeard(int flag) {
    int bit = flag % 50;
    int page = (flag - MSG_RECOLLECTION_FIRST) / 50;

    return (recollection_flags[page].bits[bit / 8] >> (bit % 8)) & 1;
}

/* Marks message `flag` of a Recollection page as shown (Message_OpenBox shows it next,
   ScriptCmd_ShowMessageBox); ids below 3 of the page pick its description, and marking a few ids marks the
   three after them too. */
void Game_MarkMessageHeard(int flag) {
    int page;
    u8 mask;

    if (flag == 0x138B) {
        Game_MarkMessageHeard(0x138C);
        Game_MarkMessageHeard(0x138D);
        Game_MarkMessageHeard(0x138E);
        return;
    }
    if (flag == 0x13F0) {
        Game_MarkMessageHeard(0x13F1);
        Game_MarkMessageHeard(0x13F2);
        Game_MarkMessageHeard(0x13F3);
        return;
    }
    if (flag == 0x1421) {
        Game_MarkMessageHeard(0x1422);
        Game_MarkMessageHeard(0x1423);
        Game_MarkMessageHeard(0x1424);
        return;
    }
    page = (flag - MSG_RECOLLECTION_FIRST) / 50;
    flag %= 50;
    mask = 1 << (flag % 8);
    if (flag < 3) {
        recollection_flags[page].bits[0] = (recollection_flags[page].bits[0] & ~RECOLLECTION_DESCRIPTION_MASK) | mask;
    } else {
        recollection_flags[page].bits[flag / 8] |= mask;
    }
    if (recollection_pages[page] == 0) {
        recollection_pages[page] = 1;
    }
}

/* moves a Recollection page's state to another page */
void Game_MoveRecollectionPage(u32 source_page, u32 dest_page) {
    if (source_page < 50 && dest_page < 50) {
        recollection_pages[dest_page] = recollection_pages[source_page];
        recollection_flags[dest_page] = recollection_flags[source_page];
        memset(&recollection_flags[source_page], 0, sizeof(RecollectionFlags));
        recollection_pages[source_page] = 0;
    }
}

/* finds the trigger an item was placed on (its map tag and trigger id); returns whether it was */
int Game_FindTriggerOfItem(int item, int *map_tag, int *trigger_id) {
    TriggerItemRecord *record;
    int i;

    if (item == 0) {
        return 0;
    }
    record = trigger_items;
    for (i = 0; i < 5; i++, record++) {
        if (record->item == item) {
            u8 tag;

            *trigger_id = record->trigger_id;
            tag = record->map_tag;
            *map_tag = tag;
            if ((u32)(tag - 0x11) < 3) {
                switch ((u8)story_flags[6]) {
                case 0:
                    *map_tag = 0x11;
                    break;
                case 1:
                    *map_tag = 0x12;
                    break;
                case 2:
                    *map_tag = 0x13;
                    break;
                }
            }
            return 1;
        }
    }
    return 0;
}

/* the item recorded for a trigger, 0 if there is none */
int Game_GetTriggerItem(int map_tag, int trigger_id) {
    TriggerItemRecord *record;
    int i;

    if ((u32)(map_tag - 0x11) < 3) {
        map_tag = 0x11;
    }
    record = trigger_items;
    for (i = 0; i < TRIGGER_ITEM_RECORD_COUNT; i++, record++) {
        if (record->map_tag == map_tag && record->trigger_id == trigger_id) {
            return record->item;
        }
    }
    return 0;
}

/* frees the record of a trigger; returns whether there was one */
int Game_ClearTriggerItem(int map_tag, int trigger_id) {
    TriggerItemRecord *record;
    int i;

    if ((u32)(map_tag - 0x11) < 3) {
        map_tag = 0x11;
    }
    record = trigger_items;
    for (i = 0; i < TRIGGER_ITEM_RECORD_COUNT; i++, record++) {
        if (record->map_tag == map_tag && record->trigger_id == trigger_id) {
            memset(record, 0, sizeof(TriggerItemRecord));
            return TRUE;
        }
    }
    return FALSE;
}

/* records an item for a trigger in the first free record; returns whether there was room */
int Game_AddTriggerItem(int map_tag, int trigger_id, int item) {
    TriggerItemRecord *record;
    int i;

    if ((u32)(map_tag - 0x11) < 3) {
        map_tag = 0x11;
    }
    record = trigger_items;
    for (i = 0; i < TRIGGER_ITEM_RECORD_COUNT; i++, record++) {
        if (record->map_tag == 0) {
            if (record->trigger_id == 0 && record->item == 0) {
                record->map_tag = map_tag;
                record->trigger_id = trigger_id;
                record->item = item;
                return TRUE;
            }
        }
    }
    return FALSE;
}

/* writes the game into the save image, then the checksum table */
void Game_WriteSaveImage(void) {
    u8 *save_cursor;
    u8 bit_byte;
    int map_tag;
    int i;
    int j;
    int k;
    int copy_size;
    int mask;
    u32 flags;
    int used;
    u32 size;
    int block_count;
    u8 *cursor;
    u8 *table;
    int sum;

    save_cursor = save_image;
    MapObject_SaveAllFlags();
    memset(save_cursor, 0, sizeof(save_image));
    save_cursor += 8;
    *(int *)save_image = 0x15400;
    memcpy(save_cursor, &current_play_time, 8); save_cursor += 8;
    memcpy(save_cursor, game_options, 0x10); save_cursor += 0x10;
    memcpy(save_cursor, &forward_button, 2); save_cursor += 2;
    memcpy(save_cursor, &back_button, 2); save_cursor += 2;
    memcpy(save_cursor, &turn_right_button, 2); save_cursor += 2;
    memcpy(save_cursor, &turn_left_button, 2); save_cursor += 2;
    memcpy(save_cursor, &step_right_button, 2); save_cursor += 2;
    memcpy(save_cursor, &step_left_button, 2); save_cursor += 2;
    memcpy(save_cursor, &look_down_button, 2); save_cursor += 2;
    memcpy(save_cursor, &look_up_button, 2); save_cursor += 2;
    memcpy(save_cursor, &attack_button, 2); save_cursor += 2;
    memcpy(save_cursor, &magic_button, 2); save_cursor += 2;
    memcpy(save_cursor, &action_button, 2); save_cursor += 2;
    memcpy(save_cursor, &menu_button, 2); save_cursor += 2;
    memcpy(save_cursor, &shortcut_button, 2); save_cursor += 2;
    memcpy(save_cursor, &pause_button, 2); save_cursor += 2;
    copy_size = 0xB40;
    memcpy(save_cursor, map_script_flags, copy_size); save_cursor += copy_size;
    copy_size = 0x6C0;
    memcpy(save_cursor, map_script_flags + 0x1680, copy_size); save_cursor += copy_size;
    memcpy(save_cursor, story_flags, 0x40); save_cursor += 0x40;
    memcpy(save_cursor, entity_group_state, 0xC80); save_cursor += 0xC80;
    memcpy(save_cursor, recollection_flags, 0x190); save_cursor += 0x190;
    memcpy(save_cursor, recollection_pages, 0x32); save_cursor += 0x32;
    memcpy(save_cursor, story_values, 0x400); save_cursor += 0x400;
    memcpy(save_cursor, map_visited_flags, 0x10); save_cursor += 0x10;
    memcpy(save_cursor, trigger_items, 0x14); save_cursor += 0x14;
    copy_size = 0x2D00;
    memcpy(save_cursor, map_entry_flags, copy_size); save_cursor += copy_size;
    copy_size = 0x1B00;
    memcpy(save_cursor, map_entry_flags + 0x5A00, copy_size); save_cursor += copy_size;
    copy_size = 0xB40;
    memcpy(save_cursor, map_entry_counters, copy_size); save_cursor += copy_size;
    copy_size = 0x6C0;
    memcpy(save_cursor, map_entry_counters[0x5A], copy_size); save_cursor += copy_size;
    copy_size = 0xB40;
    memcpy(save_cursor, map_item_pickup_counts, copy_size); save_cursor += copy_size;
    copy_size = 0x6C0;
    memcpy(save_cursor, map_item_pickup_counts[0x5A], copy_size); save_cursor += copy_size;
    copy_size = 0x3840;
    memcpy(save_cursor, dropped_item_records, copy_size); save_cursor += copy_size;
    copy_size = 0x21C0;
    memcpy(save_cursor, dropped_item_records[0x5A], copy_size); save_cursor += copy_size;
    copy_size = 0x1C20;
    memcpy(save_cursor, map_object_marks, copy_size); save_cursor += copy_size;
    copy_size = 0x10E0;
    memcpy(save_cursor, &map_object_marks[0x5A], copy_size); save_cursor += copy_size;
    memcpy(save_cursor, spell_experience, 0x3C); save_cursor += 0x3C;
    memcpy(save_cursor, spell_levels, 0x1E); save_cursor += 0x1E;
    memcpy(save_cursor, weapon_level_records, 0x94C); save_cursor += 0x94C;
    memcpy(save_cursor, worn_item_level_records, 0x1540); save_cursor += 0x1540;
    memcpy(save_cursor, &item_counts, 0x17C); save_cursor += 0x17C;
    memcpy(save_cursor, &trade_stock_1, 0x17C); save_cursor += 0x17C;
    memcpy(save_cursor, &trade_stock_2, 0x17C); save_cursor += 0x17C;
    memcpy(save_cursor, &trade_stock_3, 0x17C); save_cursor += 0x17C;
    memcpy(save_cursor, &trade_stock_4, 0x17C); save_cursor += 0x17C;
    memcpy(save_cursor, &trade_stock_5, 0x17C); save_cursor += 0x17C;
    memcpy(save_cursor, &player_state, 0x10); save_cursor += 0x10;
    memcpy(save_cursor, &player_state.rotation, 0x10); save_cursor += 0x10;
    memcpy(save_cursor, &player_state.hp_base, 4); save_cursor += 4;
    memcpy(save_cursor, &player_state.mp_base, 4); save_cursor += 4;
    memcpy(save_cursor, &player_state.power_base, 4); save_cursor += 4;
    memcpy(save_cursor, &player_state.magic_base, 4); save_cursor += 4;
    memcpy(save_cursor, &player_state.timed_flags, 0xC); save_cursor += 0xC;
    memcpy(save_cursor, &player_state.weapon, 0x10); save_cursor += 0x10;
    memcpy(save_cursor, &player_state.gold, 4); save_cursor += 4;
    memcpy(save_cursor, &player_state.experience, 4); save_cursor += 4;
    memcpy(save_cursor, &player_state.level, 4); save_cursor += 4;
    memcpy(save_cursor, &player_state.hp_now, 4); save_cursor += 4;
    memcpy(save_cursor, &player_state.mp_now, 4); save_cursor += 4;
    memcpy(save_cursor, &player_state.weapon_use_count, 4); save_cursor += 4;
    memcpy(save_cursor, &player_state.magic_use_count, 4); save_cursor += 4;
    map_tag = map_slots[*(volatile int *)&current_map_slot].tag; // match: a volatile load keeps it where the save writer reads it
    memcpy(save_cursor, &map_tag, 4); save_cursor += 4;
    memcpy(save_cursor, key_item_slots, 0x30); save_cursor += 0x30;
    memcpy(save_cursor, &item_drop_counter, 4); save_cursor += 4;

    bit_byte = 0;
    mask = 1;
    for (i = 0; i < 380; i++) {
        flags = ((GameDefinitionFlags *)item_name_defs)[i].flags >> 18;
        if ((flags & GAME_DEFINITION_STATE_VALUE_MASK) < 2 && (flags & 1)) {
            bit_byte |= mask;
        }
        if (i % 8 != 7) {
            mask = (mask << 1) & 0xFF;
        } else {
            memcpy(save_cursor, &bit_byte, 1);
            save_cursor++;
            bit_byte = 0;
            mask = 1;
        }
    }
    memcpy(save_cursor, &bit_byte, 1);
    save_cursor++;

    used = save_cursor - save_image;
    size = (u32)(used + 0x7F) / 128 * 128;
    table = save_cursor + (size - used);
    *(int *)(save_image + 4) = size; // match: the header word is stored straight into the buffer
    block_count = size / 128;
    cursor = save_image;
    for (j = 0; j < block_count; j++) {
        sum = 0;
        for (k = 0; k < 128; k++) {
            sum += *cursor++;
        }
        *table++ = sum;
    }
}

/* reads the game back from the save image */
void Game_ReadSaveImage(void) {
    u8 *save_cursor;
    int magic;
    int stored_size;
    ClockTime play_time;
    u8 flag_bits;
    int i;
    u32 flags;
    int copy_size;

    save_cursor = save_image;
    memcpy(&magic, save_cursor, 4); save_cursor += 4;
    memcpy(&stored_size, save_cursor, 4); save_cursor += 4;
    memcpy(&play_time, save_cursor, 8); save_cursor += 8;
    memcpy(game_options, save_cursor, 0x10); save_cursor += 0x10;
    memcpy(&forward_button, save_cursor, 2); save_cursor += 2;
    memcpy(&back_button, save_cursor, 2); save_cursor += 2;
    memcpy(&turn_right_button, save_cursor, 2); save_cursor += 2;
    memcpy(&turn_left_button, save_cursor, 2); save_cursor += 2;
    memcpy(&step_right_button, save_cursor, 2); save_cursor += 2;
    memcpy(&step_left_button, save_cursor, 2); save_cursor += 2;
    memcpy(&look_down_button, save_cursor, 2); save_cursor += 2;
    memcpy(&look_up_button, save_cursor, 2); save_cursor += 2;
    memcpy(&attack_button, save_cursor, 2); save_cursor += 2;
    memcpy(&magic_button, save_cursor, 2); save_cursor += 2;
    memcpy(&action_button, save_cursor, 2); save_cursor += 2;
    memcpy(&menu_button, save_cursor, 2); save_cursor += 2;
    memcpy(&shortcut_button, save_cursor, 2); save_cursor += 2;
    memcpy(&pause_button, save_cursor, 2); save_cursor += 2;
    copy_size = 0xB40;
    memcpy(map_script_flags, save_cursor, copy_size); save_cursor += copy_size;
    copy_size = 0x6C0;
    memcpy(map_script_flags + 0x1680, save_cursor, copy_size); save_cursor += copy_size;
    memcpy(story_flags, save_cursor, 0x40); save_cursor += 0x40;
    memcpy(entity_group_state, save_cursor, 0xC80); save_cursor += 0xC80;
    memcpy(recollection_flags, save_cursor, 0x190); save_cursor += 0x190;
    memcpy(recollection_pages, save_cursor, 0x32); save_cursor += 0x32;
    memcpy(story_values, save_cursor, 0x400); save_cursor += 0x400;
    memcpy(map_visited_flags, save_cursor, 0x10); save_cursor += 0x10;
    memcpy(trigger_items, save_cursor, 0x14); save_cursor += 0x14;
    copy_size = 0x2D00;
    memcpy(map_entry_flags, save_cursor, copy_size); save_cursor += copy_size;
    copy_size = 0x1B00;
    memcpy(map_entry_flags + 0x5A00, save_cursor, copy_size); save_cursor += copy_size;
    copy_size = 0xB40;
    memcpy(map_entry_counters, save_cursor, copy_size); save_cursor += copy_size;
    copy_size = 0x6C0;
    memcpy(map_entry_counters[0x5A], save_cursor, copy_size); save_cursor += copy_size;
    copy_size = 0xB40;
    memcpy(map_item_pickup_counts, save_cursor, copy_size); save_cursor += copy_size;
    copy_size = 0x6C0;
    memcpy(map_item_pickup_counts[0x5A], save_cursor, copy_size); save_cursor += copy_size;
    copy_size = 0x3840;
    memcpy(dropped_item_records, save_cursor, copy_size); save_cursor += copy_size;
    copy_size = 0x21C0;
    memcpy(dropped_item_records[0x5A], save_cursor, copy_size); save_cursor += copy_size;
    copy_size = 0x1C20;
    memcpy(map_object_marks, save_cursor, copy_size); save_cursor += copy_size;
    copy_size = 0x10E0;
    memcpy(&map_object_marks[0x5A], save_cursor, copy_size); save_cursor += copy_size;
    memcpy(spell_experience, save_cursor, 0x3C); save_cursor += 0x3C;
    memcpy(spell_levels, save_cursor, 0x1E); save_cursor += 0x1E;
    memcpy(weapon_level_records, save_cursor, 0x94C); save_cursor += 0x94C;
    memcpy(worn_item_level_records, save_cursor, 0x1540); save_cursor += 0x1540;
    memcpy(&item_counts, save_cursor, 0x17C); save_cursor += 0x17C;
    memcpy(&trade_stock_1, save_cursor, 0x17C); save_cursor += 0x17C;
    memcpy(&trade_stock_2, save_cursor, 0x17C); save_cursor += 0x17C;
    memcpy(&trade_stock_3, save_cursor, 0x17C); save_cursor += 0x17C;
    memcpy(&trade_stock_4, save_cursor, 0x17C); save_cursor += 0x17C;
    memcpy(&trade_stock_5, save_cursor, 0x17C); save_cursor += 0x17C;
    memcpy(&player_state, save_cursor, 0x10); save_cursor += 0x10;
    memcpy(&player_state.rotation, save_cursor, 0x10); save_cursor += 0x10;
    memcpy(&player_state.hp_base, save_cursor, 4); save_cursor += 4;
    memcpy(&player_state.mp_base, save_cursor, 4); save_cursor += 4;
    memcpy(&player_state.power_base, save_cursor, 4); save_cursor += 4;
    memcpy(&player_state.magic_base, save_cursor, 4); save_cursor += 4;
    memcpy(&player_state.timed_flags, save_cursor, 0xC); save_cursor += 0xC;
    memcpy(&player_state.weapon, save_cursor, 0x10); save_cursor += 0x10;
    memcpy(&player_state.gold, save_cursor, 4); save_cursor += 4;
    memcpy(&player_state.experience, save_cursor, 4); save_cursor += 4;
    memcpy(&player_state.level, save_cursor, 4); save_cursor += 4;
    memcpy(&player_state.hp_now, save_cursor, 4); save_cursor += 4;
    memcpy(&player_state.mp_now, save_cursor, 4); save_cursor += 4;
    memcpy(&player_state.weapon_use_count, save_cursor, 4); save_cursor += 4;
    memcpy(&player_state.magic_use_count, save_cursor, 4); save_cursor += 4;
    memcpy(&saved_map_tag, save_cursor, 4); save_cursor += 4;
    memcpy(key_item_slots, save_cursor, 0x30); save_cursor += 0x30;
    memcpy(&item_drop_counter, save_cursor, 4); save_cursor += 4;

    for (i = 0; i < 380; i++) {
        if (i & 7) {
            flag_bits >>= 1;
        } else {
            memcpy(&flag_bits, save_cursor, 1);
            save_cursor++;
        }
        flags = ((GameDefinitionFlags *)item_name_defs)[i].flags;
        if (((flags >> GAME_DEFINITION_STATE_SHIFT) & GAME_DEFINITION_STATE_VALUE_MASK) < 2) {
            if (flag_bits & 1) {
                ((GameDefinitionFlags *)item_name_defs)[i].flags = (flags & ~GAME_DEFINITION_STATE_MASK) | 0x40000;
            } else {
                ((GameDefinitionFlags *)item_name_defs)[i].flags = flags & ~GAME_DEFINITION_STATE_MASK;
            }
        }
    }
}

/* checks the save image: the magic word and a byte sum per 128-byte block */
int Game_CheckSaveImage(void) {
    UnalignedInt header[2];
    int block_count;
    int i;
    u8 *base;
    u8 *cursor;
    u8 *sum_table;
    u8 *block_byte;
    int sum;
    int byte_index;

    cursor = save_image;
    header[0] = *(UnalignedInt *)save_image;
    base = cursor;
    cursor = base + 4;
    if (header[0].value != 0x15400) {
        return 0;
    }
    header[1] = *(UnalignedInt *)cursor;
    block_count = (u32)header[1].value >> 7;
    block_byte = base;
    sum_table = base + header[1].value;
    for (i = 0; i < block_count; i++) {
        sum = 0;
        for (byte_index = 0x7F; byte_index >= 0; byte_index--) {
            sum += *block_byte++;
        }
        sum &= 0xFF;
        if (sum != *sum_table) {
            return 0;
        }
        sum_table++;
    }
    return 1;
}

/* starts a fade from a colour to the picture over some frames */
void Game_StartFadeIn(int red, int green, int blue, int frames) {
    int *color = fade_color;

    kf4_directory_words[3] = 1;
    fade_color[0] = red;
    fade_frames_left = frames;
    color[1] = green;
    fade_frames = (frames > 0) ? frames : 1;
    color[2] = blue;
}

/* starts a fade from the picture to a colour over some frames */
void Game_StartFadeOut(int red, int green, int blue, int frames) {
    int *color = fade_color;

    color[0] = red;
    kf4_directory_words[3] = -1;
    color[1] = green;
    color[2] = blue;
    fade_frames = (frames > 0) ? frames : 1;
    fade_frames_left = frames;
}

/* counts the fade down a frame */
void Game_StepFade(void) {
    if (kf4_directory_words[3] != 0 && fade_frames_left > 0) {
        fade_frames_left--;
    }
}

/* draws the fade */
void Game_DrawFade(void) {
    char unused_stack[0x40]; // match: retail reserves the 0x40 bytes
    int alpha;

    switch (kf4_directory_words[3]) {
    case FADE_DIRECTION_NONE:
        return;
    case FADE_DIRECTION_IN:
        alpha = (fade_frames_left << 7) / fade_frames;
        break;
    case FADE_DIRECTION_OUT:
        alpha = 0x80 - (fade_frames_left << 7) / fade_frames;
        break;
    default:
        return;
    }
    if (alpha > 0) {
        Packet_BeginGsRegisters();
        Packet_AddGsRegister(GS_REG_TEST_1, 0x30000);
        Packet_AddGsRegister(GS_REG_ZBUF_1, 0x10A000118L);
        Packet_EndGsRegisters();
        GsDraw_FillScreen(fade_color[0], fade_color[1], fade_color[2], alpha, 0x44);
        Packet_BeginGsRegisters();
        Packet_AddGsRegister(GS_REG_ZBUF_1, 0xA000118);
        Packet_EndGsRegisters();
    }
}

/* loads the screen matrix and projects the haze layer distances haze_layer_distances to screen depths */
void Game_ProjectHazeLayers(void) {
    sceVu0FMATRIX screen_matrix;
    sceVu0FMATRIX work_matrix;
    sceVu0FMATRIX view_matrix;
    sceVu0FVECTOR translation;
    VuIntVector screen_xyz;
    int i;

    memset(translation, 0, sizeof(translation));
    translation[3] = 1.0f;
    sceVu0TransMatrix(work_matrix, identity_matrix, translation);
    sceVu0RotMatrixY(work_matrix, world_view_matrix, 0.0f);
    sceVu0RotMatrixX(work_matrix, world_view_matrix, 0.0f);
    sceVu0ViewScreenMatrix(view_matrix[0], 512.0f, 1.0f, 1.0f, 2048.0f, 2048.0f, 1.0f, 1048560.0f, 1.0f, 65535.0f);
    sceVu0MulMatrix(screen_matrix, view_matrix, work_matrix);
    sceVu0MulMatrix(screen_matrix, screen_matrix, identity_matrix);
    Vu0_LoadMatrix(screen_matrix);
    haze_layer_count = 0;
    for (i = 0; i < 8; i++) {
        if (haze_layer_distances[i] == 0) {
            break;
        }
        if (i > 0 && haze_layer_distances[i] >= haze_layer_distances[i - 1]) {
            break;
        }
        translation[2] = haze_layer_distances[i];
        Vu0_ProjectToScreen(screen_xyz, translation);
        screen_xyz[2] /= 16;
        haze_layer_depths[i] = screen_xyz[2];
        haze_layer_count++;
    }
}

/* draws the haze_layer_count haze layers: a full-screen sprite at each depth in haze_layer_depths */
void Game_DrawHazeLayers(void) {
    GsColor color = haze_sprite_color;
    int texture_base;
    int texture_height_log2;
    u32 i;
    u64 tex0_low;
    u32 *packet_start;
    u32 *cursor;

    texture_base = 0x1180;
    if (context1_buffer_half != 0) {
        texture_base = 0;
    }
    if (haze_layer_count == 0) {
        return;
    }
    Packet_BeginGsRegisters();
    Packet_AddGsRegister(GS_REG_ALPHA_1, 0x80000000A8L);
    Packet_AddGsRegister(GS_REG_TEST_1, 0x51001);
    Packet_AddGsRegister(GS_REG_CLAMP_1, 5);
    Packet_EndGsRegisters();

    for (i = 0; i < haze_layer_count; i++) {
        packet_start = cursor = Packet_Alloc(0xD);
        *cursor++ = 0x1000000B;
        *cursor++ = 0;
        *cursor++ = 0;
        *cursor++ = 0;
        *cursor++ = 0;
        *cursor++ = 0;
        *cursor++ = 0;
        *cursor++ = 0x6C09C000;
        *(u64 *)cursor = 0x80AF400000008001L;
        cursor += 2;
        *(u64 *)cursor = 0x513513EEL;
        cursor += 2;
        texture_height_log2 = 9;
        tex0_low = texture_base | 0x28028000L;
        *(u64 *)cursor = tex0_low | ((u64)texture_height_log2 << 30);
        cursor += 2;
        *cursor++ = 6;
        *cursor++ = 0;
        *(u64 *)cursor = 0x60;
        cursor += 2;
        *cursor++ = 0x14;
        *cursor++ = 0;
        *cursor++ = 0;
        *cursor++ = 0;
        *cursor++ = 0;
        *cursor++ = 0;
        *(Uint128 *)cursor = *(Uint128 *)&color;
        cursor += 4;
        *cursor++ = 0x6BF8;
        *cursor++ = 0x7200;
        *cursor++ = haze_layer_depths[i];
        *cursor++ = 0;
        *cursor++ = 0x2800;
        *cursor++ = 0x1C00;
        *cursor++ = 0;
        *cursor++ = 0;
        *(Uint128 *)cursor = *(Uint128 *)&color;
        cursor += 4;
        *cursor++ = 0x93F8;
        *cursor++ = 0x8E00;
        *cursor++ = haze_layer_depths[i];
        *cursor++ = 0;
        *cursor++ = 0;
        *cursor++ = 0;
        *cursor++ = 0;
        *cursor++ = 0x15000276;
        Packet_AppendToChain(packet_start, cursor);
    }

    Packet_BeginGsRegisters();
    Packet_AddGsRegister(GS_REG_CLAMP_1, 0);
    Packet_EndGsRegisters();
}

/* Game_DrawHazeLayers with each layer moved sideways by an offset from haze_layer_x_offsets that cycles with the frame count.
   Nothing calls it. */
void Game_DrawHazeLayersShifted(void) {
    LayerOffsets offset_table = haze_layer_x_offsets;
    GsSpritePacket sprite;
    int texture_base;
    int texture_height_log2;
    u64 tex0_low;
    u32 i;
    u32 offset_index;
    int x_offset;
    int depth;

    texture_base = 0x1180;
    if (context1_buffer_half != 0) {
        texture_base = 0;
    }
    Packet_BeginGsRegisters();
    Packet_AddGsRegister(GS_REG_ALPHA_1, 0x80000000a8L);
    Packet_AddGsRegister(GS_REG_TEST_1, 0x51001);
    Packet_AddGsRegister(GS_REG_CLAMP_1, 5);
    Packet_EndGsRegisters();
    texture_height_log2 = 9;
    tex0_low = texture_base | 0x28028000L;
    sprite.tex0 = tex0_low | ((u64)texture_height_log2 << 30);
    sprite.gif_register_list = 0x60L;
    sprite.prim = 0x156;
    sprite.rgba[0] = 0x80;
    sprite.rgba[1] = 0x80;
    sprite.rgba[2] = 0x80;
    sprite.rgba[3] = 0x80;
    i = 0;
    if (haze_layer_count != 0) {
        do {
            offset_index = frame_counter + i;
            x_offset = offset_table.x[offset_index % 3];
            depth = haze_layer_depths[i];
            sprite.corner[1].texel_x = 0x2800;
            sprite.corner[0].x = x_offset + 0x6bf8;
            sprite.corner[1].texel_y = 0x1c00;
            sprite.corner[0].texel_x = 0;
            sprite.corner[0].texel_y = 0;
            sprite.corner[0].z = depth;
            sprite.corner[0].y = 0x7200;
            sprite.corner[1].y = 0x8e00;
            sprite.corner[1].x = x_offset - 0x6c08;
            sprite.corner[1].z = depth;
            GsDraw_QueueTexturedSprite(&sprite);
            i++;
        } while (i < haze_layer_count);
    }
    Packet_BeginGsRegisters();
    Packet_AddGsRegister(GS_REG_CLAMP_1, 0);
    Packet_EndGsRegisters();
}

/* debug: draws one haze sprite at the depth in depth_test_sprite_z; two buttons of pad 1 (0x400, 0x100) step that depth and print it
   ("z_value[%u]") */
void Game_DrawDepthTestSprite(void) {
    u32 *cursor;
    int texture_height_log2;
    u32 *packet_start;
    GsColor color = haze_sprite_color;
    int texture_base;
    u16 pad_buttons;
    u64 tex0_low;

    texture_base = 0x1180;
    if (context1_buffer_half != 0) {
        texture_base = 0;
    }
    pad_buttons = Pad_GetReport(1, 0)->held;
    if (pad_buttons & PAD_L1) {
        depth_test_sprite_z += 0xFF;
        printf(z_value_format, depth_test_sprite_z);
    }
    if (pad_buttons & PAD_L2) {
        depth_test_sprite_z -= 0xFF;
        printf(z_value_format, depth_test_sprite_z);
    }
    Packet_BeginGsRegisters();
    Packet_AddGsRegister(GS_REG_ALPHA_1, 0x80000000A8L);
    Packet_AddGsRegister(GS_REG_TEST_1, 0x51001);
    Packet_AddGsRegister(GS_REG_CLAMP_1, 5);
    Packet_EndGsRegisters();

    packet_start = cursor = Packet_Alloc(0xD);
    *cursor++ = 0x1000000B;
    *cursor++ = 0;
    *cursor++ = 0;
    *cursor++ = 0;
    *cursor++ = 0;
    *cursor++ = 0;
    *cursor++ = 0;
    *cursor++ = 0x6C09C000;
    *(u64 *)cursor = 0x80AF400000008001L;
    cursor += 2;
    *(u64 *)cursor = 0x513513EEL;
    cursor += 2;
    texture_height_log2 = 9;
    tex0_low = texture_base | 0x28028000L;
    *(u64 *)cursor = tex0_low | ((u64)texture_height_log2 << 30);
    cursor += 2;
    *cursor++ = 6;
    *cursor++ = 0;
    *(u64 *)cursor = 0x60;
    cursor += 2;
    *cursor++ = 0x14;
    *cursor++ = 0;
    *cursor++ = 0;
    *cursor++ = 0;
    *cursor++ = 0;
    *cursor++ = 0;
    *(Uint128 *)cursor = *(Uint128 *)&color;
    cursor += 4;
    *cursor++ = 0x6BF8;
    *cursor++ = 0x7200;
    *cursor++ = depth_test_sprite_z;
    *cursor++ = 0;
    *cursor++ = 0x2800;
    *cursor++ = 0x1C00;
    *cursor++ = 0;
    *cursor++ = 0;
    *(Uint128 *)cursor = *(Uint128 *)&color;
    cursor += 4;
    *cursor++ = 0x93F8;
    *cursor++ = 0x8E00;
    *cursor++ = depth_test_sprite_z;
    *cursor++ = 0;
    *cursor++ = 0;
    *cursor++ = 0;
    *cursor++ = 0;
    *cursor++ = 0x15000276;
    cursor[3] = 0;
    cursor[2] = 0;
    cursor[1] = 0;
    cursor[0] = 0;
    Packet_AppendToChain(packet_start, cursor);

    Packet_BeginGsRegisters();
    Packet_AddGsRegister(GS_REG_CLAMP_1, 0);
    Packet_EndGsRegisters();
}

/* blends the picture over itself one pixel lower at half strength, in ten 64-pixel strips (Z writes off) */
void Game_BlendScreenOffsetDown(void) {
    GsSpritePacket sprite;
    int texture_base;
    int remaining;
    int texture_height_log2;
    u64 tex0_low;

    texture_base = 0x1180;
    if (context1_buffer_half != 0) {
        texture_base = 0;
    }
    Packet_BeginGsRegisters();
    remaining = 9;
    Packet_AddGsRegister(GS_REG_TEXFLUSH, 0xffffffffL);
    Packet_AddGsRegister(GS_REG_ALPHA_1, 0x44);
    Packet_AddGsRegister(GS_REG_TEST_1, 0x30000);
    Packet_AddGsRegister(GS_REG_ZBUF_1, 0x10a000118L);
    Packet_EndGsRegisters();
    texture_height_log2 = 9;
    tex0_low = texture_base | 0x28028000L;
    sprite.gif_register_list = 0;
    sprite.tex0 = tex0_low | ((u64)texture_height_log2 << 30);
    sprite.rgba[0] = 0x80;
    sprite.rgba[1] = 0x80;
    sprite.corner[1].z = 0xFFFFFFFF;
    sprite.prim = 0x156;
    sprite.rgba[2] = 0x80;
    sprite.corner[1].texel_x = 0;
    sprite.rgba[3] = 0x40;
    sprite.corner[1].texel_y = 0;
    sprite.corner[1].x = 0x6c00;
    sprite.corner[1].y = 0x7210;
    sprite.corner[0].x = 0x6ff8;
    sprite.corner[0].y = 0x8e08;
    sprite.corner[0].z = 0xFFFFFFFF;
    sprite.corner[0].texel_x = 0x400;
    sprite.corner[0].texel_y = 0x1c00;
    do {
        remaining--;
        GsDraw_QueueTexturedSprite(&sprite);
        sprite.corner[0].x += 0x400;
        sprite.corner[1].x += 0x400;
        sprite.corner[0].texel_x += 0x400;
        sprite.corner[1].texel_x += 0x400;
    } while (remaining >= 0);

    Packet_BeginGsRegisters();
    Packet_AddGsRegister(GS_REG_ZBUF_1, 0xa000118L);
    Packet_EndGsRegisters();
}

/* blends the picture over itself one pixel lower, then again one pixel higher, at half strength (Z writes off) */
void Game_BlendScreenOffsetUpDown(void) {
    GsSpritePacket sprite;
    int texture_base;
    int remaining;
    int texture_height_log2;
    u64 tex0_low;

    texture_base = 0x1180;
    if (context1_buffer_half != 0) {
        texture_base = 0;
    }
    Packet_BeginGsRegisters();
    remaining = 9;
    Packet_AddGsRegister(GS_REG_TEXFLUSH, 0xffffffffL);
    Packet_AddGsRegister(GS_REG_ALPHA_1, 0x44);
    Packet_AddGsRegister(GS_REG_TEST_1, 0x30000);
    Packet_AddGsRegister(GS_REG_ZBUF_1, 0x10a000118L);
    Packet_EndGsRegisters();

    texture_height_log2 = 9;
    tex0_low = texture_base | 0x28028000L;
    sprite.gif_register_list = 0;
    sprite.tex0 = tex0_low | ((u64)texture_height_log2 << 30);
    sprite.rgba[0] = 0x80;
    sprite.rgba[1] = 0x80;
    sprite.corner[1].z = 0xFFFFFFFF;
    sprite.prim = 0x156;
    sprite.rgba[2] = 0x80;
    sprite.corner[1].texel_x = 0;
    sprite.rgba[3] = 0x40;
    sprite.corner[1].texel_y = 0;
    sprite.corner[1].x = 0x6c00;
    sprite.corner[1].y = 0x7210;
    sprite.corner[0].x = 0x6ff8;
    sprite.corner[0].y = 0x8e08;
    sprite.corner[0].z = 0xFFFFFFFF;
    sprite.corner[0].texel_x = 0x400;
    sprite.corner[0].texel_y = 0x1c00;

    do {
        remaining--;
        GsDraw_QueueTexturedSprite(&sprite);
        sprite.corner[0].x += 0x400;
        sprite.corner[1].x += 0x400;
        sprite.corner[0].texel_x += 0x400;
        sprite.corner[1].texel_x += 0x400;
    } while (remaining >= 0);

    sprite.corner[1].x = 0x7000;
    remaining = 9;
    sprite.corner[0].x = 0x6bf8;
    sprite.corner[1].y = 0x8df0;
    sprite.corner[0].y = 0x71e8;
    sprite.corner[1].texel_x = 0x400;
    sprite.corner[1].texel_y = 0x1c00;
    sprite.corner[0].texel_x = 0;
    sprite.corner[0].texel_y = 0;
    do {
        remaining--;
        GsDraw_QueueTexturedSprite(&sprite);
        sprite.corner[0].x += 0x400;
        sprite.corner[1].x += 0x400;
        sprite.corner[0].texel_x += 0x400;
        sprite.corner[1].texel_x += 0x400;
    } while (remaining >= 0);

    Packet_BeginGsRegisters();
    Packet_AddGsRegister(GS_REG_ZBUF_1, 0xa000118L);
    Packet_EndGsRegisters();
}

/* blends the other frame buffer (the frame shown before) over the whole screen at the given alpha */
void Game_BlendPreviousFrame(int alpha) {
    ScreenRect screen_rect = full_screen_rect;
    ScreenRect texel_rect = full_screen_rect;
    int vertex_color[4] = { 0x80, 0x80, 0x80, alpha };
    int texture_base;
    int texture_height_log2;
    u64 tex0_low;
    u32 *packet_start;
    u32 *cursor;

    texture_base = 0x1180;
    if (context1_buffer_half == 0) {
        texture_base = 0;
    }
    Packet_BeginGsRegisters();
    Packet_AddGsRegister(GS_REG_ALPHA_1, 0x44);
    Packet_AddGsRegister(GS_REG_TEST_1, 0x51001);
    Packet_EndGsRegisters();

    packet_start = cursor = Packet_Alloc(0xD);
    *cursor++ = 0x1000000B;
    *cursor++ = 0;
    *cursor++ = 0;
    *cursor++ = 0;
    *cursor++ = 0;
    *cursor++ = 0;
    *cursor++ = 0;
    *cursor++ = 0x6C09C000;
    *(u64 *)cursor = 0x80AF400000008001L;
    cursor += 2;
    *(u64 *)cursor = 0x513513EEL;
    cursor += 2;
    texture_height_log2 = 9;
    tex0_low = texture_base | 0x28028000L;
    *(u64 *)cursor = tex0_low | ((u64)texture_height_log2 << 30);
    cursor += 2;
    *cursor++ = 6;
    *cursor++ = 0;
    *(u64 *)cursor = 0;
    cursor += 2;
    *cursor++ = 0x14;
    *cursor++ = 0;
    *cursor++ = texel_rect.left << 4;
    *cursor++ = texel_rect.top << 4;
    *cursor++ = 0;
    *cursor++ = 0;
    *(Uint128 *)cursor = *(Uint128 *)vertex_color;
    cursor += 4;
    *cursor++ = (screen_rect.left + 0x6C0) << 4;
    *cursor++ = (screen_rect.top + 0x720) << 4;
    *cursor++ = 0xFFFFFFFF;
    *cursor++ = 0;
    *cursor++ = texel_rect.right << 4;
    *cursor++ = texel_rect.bottom << 4;
    *cursor++ = 0;
    *cursor++ = 0;
    *(Uint128 *)cursor = *(Uint128 *)vertex_color;
    cursor += 4;
    *cursor++ = (screen_rect.right << 4) + 0x6BF8;
    *cursor++ = (screen_rect.bottom << 4) + 0x71F8;
    *cursor++ = 0xFFFFFFFF;
    *cursor++ = 0;
    *cursor++ = 0;
    *cursor++ = 0;
    *cursor++ = 0;
    *cursor++ = 0x15000276;
    cursor[3] = 0;
    cursor[2] = 0;
    cursor[1] = 0;
    cursor[0] = 0;
    Packet_AppendToChain(packet_start, cursor);
}

/* blends the other frame buffer over the whole screen at the given alpha, its texels taken from the slightly
   smaller rectangle inset_screen_rect, so a little enlarged. Nothing calls it. */
void Game_BlendPreviousFrameZoomed(int alpha) {
    ScreenRect screen_rect = full_screen_rect;
    ScreenRect texel_rect = inset_screen_rect;
    int vertex_color[4] = { 0x80, 0x80, 0x80, alpha };
    int texture_base;
    int texture_height_log2;
    u64 tex0_low;
    u32 *packet_start;
    u32 *cursor;

    texture_base = 0x1180;
    if (context1_buffer_half == 0) {
        texture_base = 0;
    }
    Packet_BeginGsRegisters();
    Packet_AddGsRegister(GS_REG_ALPHA_1, 0x44);
    Packet_AddGsRegister(GS_REG_TEST_1, 0x51001);
    Packet_EndGsRegisters();

    packet_start = cursor = Packet_Alloc(0xD);
    *cursor++ = 0x1000000B;
    *cursor++ = 0;
    *cursor++ = 0;
    *cursor++ = 0;
    *cursor++ = 0;
    *cursor++ = 0;
    *cursor++ = 0;
    *cursor++ = 0x6C09C000;
    *(u64 *)cursor = 0x80AF400000008001L;
    cursor += 2;
    *(u64 *)cursor = 0x513513EEL;
    cursor += 2;
    texture_height_log2 = 9;
    tex0_low = texture_base | 0x28028000L;
    *(u64 *)cursor = tex0_low | ((u64)texture_height_log2 << 30);
    cursor += 2;
    *cursor++ = 6;
    *cursor++ = 0;
    *(u64 *)cursor = 0;
    cursor += 2;
    *cursor++ = 0x14;
    *cursor++ = 0;
    *cursor++ = texel_rect.left << 4;
    *cursor++ = texel_rect.top << 4;
    *cursor++ = 0;
    *cursor++ = 0;
    *(Uint128 *)cursor = *(Uint128 *)vertex_color;
    cursor += 4;
    *cursor++ = (screen_rect.left + 0x6C0) << 4;
    *cursor++ = (screen_rect.top + 0x720) << 4;
    *cursor++ = 0xFFFFFFFF;
    *cursor++ = 0;
    *cursor++ = texel_rect.right << 4;
    *cursor++ = texel_rect.bottom << 4;
    *cursor++ = 0;
    *cursor++ = 0;
    *(Uint128 *)cursor = *(Uint128 *)vertex_color;
    cursor += 4;
    *cursor++ = (screen_rect.right << 4) + 0x6BF8;
    *cursor++ = (screen_rect.bottom << 4) + 0x71F8;
    *cursor++ = 0xFFFFFFFF;
    *cursor++ = 0;
    *cursor++ = 0;
    *cursor++ = 0;
    *cursor++ = 0;
    *cursor++ = 0x15000276;
    cursor[3] = 0;
    cursor[2] = 0;
    cursor[1] = 0;
    cursor[0] = 0;
    Packet_AppendToChain(packet_start, cursor);
}

/* projects the point 1000 ahead along last frame's view (view_ray_status unk20/unk30) to find how far the view
   turned, and draws the picture again moved by that much (at most 8 pixels; not up or down while unk40 is under
   125) over every other line, in 20 strips */
void Game_ShiftScreenByViewTurn(void) {
    float view_matrix[16];
    float scaled_vector[4];
    int screen_xy[2];
    GsSpritePacket sprite_packet;
    GsSpritePacket *strip_packet;
    int texture_base;
    int offset_x, offset_y;
    int strip;

    texture_base = context1_buffer_half ? 0 : 0x1180;

    Packet_BeginGsRegisters();
    Packet_AddGsRegister(GS_REG_ALPHA_1, 10);
    Packet_AddGsRegister(GS_REG_TEST_1, 0x30000);
    Packet_AddGsRegister(GS_REG_ZBUF_1, 0x10A000118L);
    Packet_AddGsRegister(GS_REG_SCANMSK, 3);
    Packet_EndGsRegisters();

    sceVu0MulMatrix(view_matrix, world_screen_matrix, identity_matrix);
    Vu0_LoadMatrix(view_matrix);
    Vector_Scale(scaled_vector, view_ray_previous_direction, 1000.0f);
    Vector_Add(scaled_vector, scaled_vector, view_ray_previous_direction - 4);
    scaled_vector[3] = 1.0f;
    Vu0_ProjectToScreen(screen_xy, scaled_vector);

    offset_x = screen_xy[0] - 0x8000;
    if (offset_x < 0) {
        offset_x = screen_xy[0] - 0x7FFF;
    }
    offset_y = screen_xy[1] - 0x8000;
    if (offset_y < 0) {
        offset_y = screen_xy[1] - 0x7FFF;
    }
    offset_x /= 2;
    offset_y /= 2;
    if (offset_x > 0x80) {
        offset_x = 0x80;
    } else if (offset_x < -0x80) {
        offset_x = -0x80;
    }
    if (offset_y > 0x80) {
        offset_y = 0x80;
    } else if (offset_y < -0x80) {
        offset_y = -0x80;
    }
    if (view_ray_length[0] < 125.0f) {
        offset_y = 0;
    }

    sprite_packet.prim = 0x116;
    sprite_packet.tex0 = 0x28028000L | texture_base;
    sprite_packet.tex0 |= 0xA400L << 20;
    sprite_packet.gif_register_list = 0;
    sprite_packet.rgba[0] = 0x80;
    sprite_packet.rgba[1] = 0x80;
    sprite_packet.rgba[2] = 0x80;
    sprite_packet.rgba[3] = 0x80;
    sprite_packet.corner[0].z = 0xFFFFFFFF;
    sprite_packet.corner[1].z = 0xFFFFFFFF;

    if (offset_x < 1) {
        sprite_packet.corner[0].x = 0x6BF8;
        sprite_packet.corner[1].x = 0x6E00;
        sprite_packet.corner[0].texel_x = 0;
        sprite_packet.corner[1].texel_x = 0x200;
    } else {
        sprite_packet.corner[0].x = 0x9400;
        sprite_packet.corner[1].x = 0x91F8;
        sprite_packet.corner[0].texel_x = 0x2800;
        sprite_packet.corner[1].texel_x = 0x2600;
    }
    if (offset_y < 1) {
        sprite_packet.corner[0].y = 0x71F8;
        sprite_packet.corner[1].y = 0x8E00;
        sprite_packet.corner[0].texel_y = 0;
        sprite_packet.corner[1].texel_y = 0x1C00;
    } else {
        sprite_packet.corner[0].y = 0x8E00;
        sprite_packet.corner[1].y = 0x71F8;
        sprite_packet.corner[0].texel_y = 0x1C00;
        sprite_packet.corner[1].texel_y = 0;
    }

    sprite_packet.corner[0].x += offset_x;
    sprite_packet.corner[1].x += offset_x;
    sprite_packet.corner[0].y += offset_y;
    sprite_packet.corner[1].y += offset_y;

    if (offset_x < 1) {
        offset_x = 0x200;
    } else {
        offset_x = -0x200;
    }

    strip_packet = &sprite_packet;
    strip = 0x13;
    do {
        strip--;
        GsDraw_QueueTexturedSprite(strip_packet);
        sprite_packet.corner[0].x += offset_x;
        sprite_packet.corner[1].x += offset_x;
        sprite_packet.corner[0].texel_x += offset_x;
        sprite_packet.corner[1].texel_x += offset_x;
    } while (strip >= 0);

    Packet_BeginGsRegisters();
    Packet_AddGsRegister(GS_REG_SCANMSK, 0);
    Packet_AddGsRegister(GS_REG_ZBUF_1, 0xA000118L);
    Packet_EndGsRegisters();
}

/* Game_ShiftScreenByViewTurn measured at the nearest entity within 30 degrees ahead (or view_ray_status.unk48 away, at most
   1000) instead of 1000 ahead. Nothing calls it. */
void Game_ShiftScreenByFocusTurn(void) {
    sceVu0FMATRIX view_matrix;
    sceVu0FVECTOR offset_to_entity;
    sceVu0FVECTOR scratch_vector;
    VuIntVector screen_position;
    GsSpritePacket sprite_packet;
    int texture_base;
    Entity *entity;
    float focus_distance, nearest_distance_squared, distance_squared;
    int offset_x, offset_y, strip;
    int texture_height_log2;
    u64 tex0_low;

    texture_base = 0x1180;
    if (context1_buffer_half != 0) texture_base = 0;
    focus_distance = view_ray_status.nearest_entity_distance > 1000.0f ? 1000.0f : view_ray_status.nearest_entity_distance;
    nearest_distance_squared = focus_distance * focus_distance;
    for (entity = active_entities_head; entity != NULL; entity = entity->next) {
        if ((ENTITY_FLAG_BITS(entity)->live) == 0) continue;
        if (focus_distance < entity->distance_to_player) continue;
        Vector_Copy(offset_to_entity, entity->model->position);
        offset_to_entity[1] -= entity->height * 0.5f;
        Vector_Subtract(offset_to_entity, offset_to_entity, view_ray_status.view_origin);
        Vector_MultiplyComponents(scratch_vector, offset_to_entity, offset_to_entity);
        distance_squared = scratch_vector[0] + scratch_vector[1] + scratch_vector[2];
        if (nearest_distance_squared < distance_squared) continue;
        sceVu0Normalize(scratch_vector, offset_to_entity);
        if (sceVu0InnerProduct(scratch_vector, view_ray_status.view_direction) < FOCUS_CONE_MIN_COS) continue; /* about cos 30 degrees */
        nearest_distance_squared = distance_squared;
        focus_distance = sqrtf(distance_squared);
    }

    sceVu0MulMatrix(view_matrix, world_screen_matrix, identity_matrix);
    Vu0_LoadMatrix(view_matrix);
    Vector_Scale(scratch_vector, view_ray_status.prev_view_direction, focus_distance);
    Vector_Add(scratch_vector, scratch_vector, view_ray_status.prev_view_origin);
    scratch_vector[3] = 1.0f;
    Vu0_ProjectToScreen(screen_position, scratch_vector);

    offset_x = screen_position[0] - 0x8000;
    if (offset_x < 0) offset_x = screen_position[0] - 0x7FFF;
    offset_y = screen_position[1] - 0x8000;
    if (offset_y < 0) offset_y = screen_position[1] - 0x7FFF;
    offset_x /= 2;
    offset_y /= 2;
    if (offset_x > 0x80) offset_x = 0x80;
    else if (offset_x < -0x80) offset_x = -0x80;
    if (offset_y > 0x80) offset_y = 0x80;
    else if (offset_y < -0x80) offset_y = -0x80;

    Packet_BeginGsRegisters();
    Packet_AddGsRegister(GS_REG_TEST_1, 0x30000);
    Packet_AddGsRegister(GS_REG_ZBUF_1, 0x10A000118L);
    Packet_AddGsRegister(GS_REG_SCANMSK, 3);
    Packet_EndGsRegisters();

    sprite_packet.prim = 0x116;
    tex0_low = texture_base | 0x28028000L;
    sprite_packet.gif_register_list = 0;
    sprite_packet.rgba[0] = 0x80;
    sprite_packet.rgba[1] = 0x80;
    sprite_packet.rgba[2] = 0x80;
    texture_height_log2 = 0x29;
    sprite_packet.corner[0].z = 0xFFFFFFFF;
    sprite_packet.tex0 = tex0_low | ((u64)texture_height_log2 << 30);
    sprite_packet.rgba[3] = 0x80;
    sprite_packet.corner[1].z = 0xFFFFFFFF;

    if (offset_x <= 0) {
        sprite_packet.corner[0].x = 0x6BF8;
        sprite_packet.corner[1].x = 0x6E00;
        sprite_packet.corner[1].texel_x = 0x200;
        sprite_packet.corner[0].texel_x = 0;
    } else {
        sprite_packet.corner[0].x = 0x9400;
        sprite_packet.corner[1].x = 0x91F8;
        sprite_packet.corner[0].texel_x = 0x2800;
        sprite_packet.corner[1].texel_x = 0x2600;
    }
    if (offset_y <= 0) {
        sprite_packet.corner[0].y = 0x71F8;
        sprite_packet.corner[1].y = 0x8E00;
        sprite_packet.corner[1].texel_y = 0x1C00;
        sprite_packet.corner[0].texel_y = 0;
    } else {
        sprite_packet.corner[0].y = 0x8E00;
        sprite_packet.corner[1].y = 0x71F8;
        sprite_packet.corner[0].texel_y = 0x1C00;
        sprite_packet.corner[1].texel_y = 0;
    }
    sprite_packet.corner[0].x += offset_x;
    sprite_packet.corner[1].x += offset_x;
    sprite_packet.corner[0].y += offset_y;
    sprite_packet.corner[1].y += offset_y;
    if (offset_x <= 0) offset_x = 0x200;
    else offset_x = -0x200;
    for (strip = 19; strip >= 0; strip--) {
        GsDraw_QueueTexturedSprite(&sprite_packet);
        sprite_packet.corner[0].x += offset_x;
        sprite_packet.corner[1].x += offset_x;
        sprite_packet.corner[0].texel_x += offset_x;
        sprite_packet.corner[1].texel_x += offset_x;
    }

    Packet_BeginGsRegisters();
    Packet_AddGsRegister(GS_REG_SCANMSK, 0);
    Packet_AddGsRegister(GS_REG_ZBUF_1, 0xA000118L);
    Packet_EndGsRegisters();
}

/* draws the picture over itself in ten strips, its colour multiplied by brightness / 128: the brightness option
   (game_options[10]) */
void Game_ApplyBrightness(int brightness) {
    char unused_stack[0x60]; // match: retail reserves the 0x60 bytes
    GsSpritePacket sprite;
    int texture_base;
    int remaining;
    int texture_height_log2;
    u64 tex0_low;

    texture_base = context1_buffer_half ? 0 : 0x1180;
    remaining = 9;
    GsDraw_QueueTexFlush();
    GsDraw_QueueFlush();
    Packet_BeginGsRegisters();
    Packet_AddGsRegister(GS_REG_ALPHA_1, 10);
    Packet_AddGsRegister(GS_REG_TEST_1, 0x30000);
    Packet_AddGsRegister(GS_REG_ZBUF_1, 0x10A000118L);
    Packet_EndGsRegisters();
    sprite.gif_register_list = 0;
    tex0_low = texture_base | 0x28028000L;
    texture_height_log2 = 9;
    sprite.tex0 = tex0_low | ((u64)texture_height_log2 << 30);
    sprite.prim = 0x116;
    sprite.rgba[1] = sprite.rgba[2] = brightness;
    sprite.rgba[0] = brightness;
    sprite.rgba[3] = 0x80;
    sprite.corner[0].z = 0xFFFFFFFF;
    sprite.corner[0].x = 0x6C00;
    sprite.corner[0].y = 0x7200;
    sprite.corner[1].x = 0x7000;
    sprite.corner[0].texel_x = 0;
    sprite.corner[1].y = 0x8DF8;
    sprite.corner[0].texel_y = 0;
    sprite.corner[1].z = 0xFFFFFFFF;
    sprite.corner[1].texel_x = 0x400;
    sprite.corner[1].texel_y = 0x1C00;
    do {
        remaining--;
        GsDraw_QueueTexturedSprite(&sprite);
        sprite.corner[0].x += 0x400;
        sprite.corner[1].x += 0x400;
        sprite.corner[0].texel_x += 0x400;
        sprite.corner[1].texel_x += 0x400;
    } while (remaining >= 0);

    Packet_BeginGsRegisters();
    Packet_AddGsRegister(GS_REG_ZBUF_1, 0xA000118L);
    Packet_EndGsRegisters();
    GsDraw_QueueFlush();
    GsDraw_QueueTexFlush();
}

/* draws the picture over itself in ten strips, its colour multiplied by red, green and blue / 128 (under water) */
void Game_TintScreen(int red, int green, int blue) {
    char unused_stack[0x60]; // match: retail reserves the 0x60 bytes
    GsSpritePacket sprite_packet;
    int texture_base;
    int remaining;

    texture_base = context1_buffer_half ? 0 : 0x1180;
    remaining = 9;
    GsDraw_QueueTexFlush();
    GsDraw_QueueFlush();
    Packet_BeginGsRegisters();
    Packet_AddGsRegister(GS_REG_ALPHA_1, 10);
    Packet_AddGsRegister(GS_REG_TEST_1, 0x30000);
    Packet_AddGsRegister(GS_REG_ZBUF_1, 0x10A000118L);
    Packet_EndGsRegisters();

    sprite_packet.tex0 = (0x28028000L | texture_base) | (0x9000L << 18);
    sprite_packet.gif_register_list = 0;
    sprite_packet.prim = 0x116;
    sprite_packet.rgba[0] = red;
    sprite_packet.rgba[1] = green;
    sprite_packet.rgba[2] = blue;
    sprite_packet.rgba[3] = 0x80;
    sprite_packet.corner[0].x = 0x6C00;
    sprite_packet.corner[0].y = 0x7200;
    sprite_packet.corner[1].x = 0x7000;
    {
        // match: the bottom y goes through a local of its own
        u32 bottom_y = 0x8DF8;
        sprite_packet.corner[1].y = bottom_y;
    }
    sprite_packet.corner[0].z = 0xFFFFFFFFU;
    sprite_packet.corner[1].z = 0xFFFFFFFFU;
    sprite_packet.corner[1].texel_x = 0x400;
    sprite_packet.corner[1].texel_y = 0x1C00;
    sprite_packet.corner[0].texel_x = 0;
    sprite_packet.corner[0].texel_y = 0;

    do {
        remaining--;
        GsDraw_QueueTexturedSprite(&sprite_packet);
        sprite_packet.corner[0].x += 0x400;
        sprite_packet.corner[1].x += 0x400;
        sprite_packet.corner[0].texel_x += 0x400;
        sprite_packet.corner[1].texel_x += 0x400;
    } while (remaining >= 0);

    Packet_BeginGsRegisters();
    Packet_AddGsRegister(GS_REG_ZBUF_1, 0xA000118L);
    Packet_EndGsRegisters();
    GsDraw_QueueFlush();
    GsDraw_QueueTexFlush();
}

/* steps the wobbling grid (laid out afresh when wobble_grid_reset is set) and draws the grabbed picture through its
   cells in the colour given, the wobble scaled by blend: under water and while MP is drained */
void Game_DrawWobblingScreen(int red, int green, int blue, int alpha, float blend) {
    char unused_stack[0x40]; // match: retail reserves the 0x40 bytes
    int j, i, k, corner_col;
    int origin_x, origin_y, target_x;
    s64 tex0_low;
    float force_x, force_y;
    u32 *packet_start;
    u32 *cursor;

    if (wobble_grid_reset != 0) {
        for (i = 0; i < 8; i++) {
            for (j = 0; j < 11; j++) {
                wobble_grid_origin[i][j][0] = wobble_grid_points[i][j][0] = j << 6;
                wobble_grid_origin[i][j][1] = wobble_grid_points[i][j][1] = i << 6;
                if (j > 0 && j < 10) {
                    wobble_grid_points[i][j][0] -= 8;
                    wobble_grid_points[i][j][0] += (int)(rand() * 17.0f / 2147483648.0f);
                }
                if (i > 0 && i < 7) {
                    wobble_grid_points[i][j][1] -= 8;
                    wobble_grid_points[i][j][1] += (int)(rand() * 17.0f / 2147483648.0f);
                }
                wobble_grid_points[i][j][0] <<= 4;
                wobble_grid_points[i][j][1] <<= 4;
                wobble_grid_origin[i][j][0] <<= 4;
                wobble_grid_origin[i][j][1] <<= 4;
            }
        }
    }

    *(RippleGrid *)wobble_grid_previous = *(RippleGrid *)wobble_grid_points; // match: copied whole, as quadwords

    for (i = 1; i < 7; i++) {
        for (j = 1; j < 10; j++) {
            force_x = force_y = 0.0f;
            force_x += wobble_grid_previous[i - 1][j][0] * 0.1f;
            force_y += wobble_grid_previous[i - 1][j][1] * 0.1f;
            force_x += wobble_grid_previous[i + 1][j][0] * 0.1f;
            force_y += wobble_grid_previous[i + 1][j][1] * 0.1f;
            force_x += wobble_grid_previous[i][j - 1][0] * 0.1f;
            force_y += wobble_grid_previous[i][j - 1][1] * 0.1f;
            force_x += wobble_grid_previous[i][j + 1][0] * 0.1f;
            force_y += wobble_grid_previous[i][j + 1][1] * 0.1f;
            force_x -= wobble_grid_previous[i][j][0] * 0.4f;
            force_y -= wobble_grid_previous[i][j][1] * 0.4f;
            wobble_grid_forces[i][j][0] += force_x;
            wobble_grid_forces[i][j][1] += force_y;
            wobble_grid_points[i][j][0] += (int)(wobble_grid_forces[i][j][0] * 0.05f);
            wobble_grid_points[i][j][1] += (int)(wobble_grid_forces[i][j][1] * 0.05f);
        }
    }

    GsDraw_QueueFlush();
    Packet_BeginGsRegisters();
    Packet_AddGsRegister(GS_REG_TEXFLUSH, 0xFFFFFFFF);
    Packet_AddGsRegister(GS_REG_ALPHA_1, 0x44);
    Packet_AddGsRegister(GS_REG_TEST_1, 0x30000);
    Packet_AddGsRegister(GS_REG_ZBUF_1, 0x10A000118L);
    Packet_AddGsRegister(GS_REG_TEX0_1, 0x26802ABC0L);
    Packet_AddGsRegister(GS_REG_TEX1_1, 0);
    Packet_EndGsRegisters();

    for (i = 0; i < 7; i++) {
        for (j = 0; j < 10; j++) {
            packet_start = cursor = Packet_Alloc(0x11);
            *cursor++ = 0x1000000F;
            *cursor++ = 0;
            *cursor++ = 0;
            *cursor++ = 0;
            *cursor++ = 0;
            *cursor++ = 0;
            *cursor++ = 0;
            tex0_low = 0xC0AE400000008001L;
            *cursor++ = 0x6C0DC000;
            *(s64 *)cursor = tex0_low;
            cursor += 2;
            *(s64 *)cursor = 0x513513513513L;
            cursor += 2;
            for (k = 0; k < 2; k++) {
                for (corner_col = 0; corner_col < 2; corner_col++) {
                    target_x = wobble_grid_points[i + k][j + corner_col][0];
                    origin_x = wobble_grid_origin[i + k][j + corner_col][0];
                    *cursor++ = origin_x + fptoui((target_x - origin_x) * blend);
                    origin_y = wobble_grid_origin[i + k][j + corner_col][1];
                    *cursor++ = origin_y + fptoui((wobble_grid_points[i + k][j + corner_col][1] - origin_y) * blend);
                    *cursor++ = 0;
                    *cursor++ = 0;
                    *cursor++ = red;
                    *cursor++ = green;
                    *cursor++ = blue;
                    *cursor++ = alpha;
                    *cursor++ = (((j + corner_col) << 6) + 0x6C0) << 4;
                    *cursor++ = (((i + k) << 6) + 0x720) << 4;
                    *cursor++ = 0;
                    *cursor++ = 0;
                }
            }
            *cursor++ = 0;
            *cursor++ = 0;
            *cursor++ = 0;
            *cursor++ = 0x15000276;
            cursor[3] = 0;
            cursor[2] = 0;
            cursor[1] = 0;
            cursor[0] = 0;
            Packet_AppendToChain(packet_start, cursor);
        }
    }

    wobble_grid_reset = 0;
    Packet_BeginGsRegisters();
    Packet_AddGsRegister(GS_REG_ZBUF_1, 0xA000118);
    Packet_EndGsRegisters();
    GsDraw_QueueFlush();
}
