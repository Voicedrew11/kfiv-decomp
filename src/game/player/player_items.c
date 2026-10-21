#include "map/map.h"
#include "ui/message.h"
#include "item/item.h"
#include "core/game.h"
#include "entity/entity.h"
#include "map_object/map_object.h"
#include "player/player.h"
#include "effect/item_effect.h"
#include "enemy/enemy.h"
#include "player/player_update.h"
#include "script/script.h"
#include "ui/map_viewer.h"
#include "save/save_state.h"
#include "ui/menu_screens.h"

/* ItemCommonDef.type: what using the item does (Player_UseItem); 4, 5 and 7 are left as numbers */
#define ITEM_TYPE_HEAL              0       /* heals hp and mp and clears the ailments its timed_flags name */
#define ITEM_TYPE_USE_ON_OBJECT     1       /* used on the map object in view (MapObject_UseItem) */
#define ITEM_TYPE_WAND              2       /* takes the player to the passage symbol the player stands on */
#define ITEM_TYPE_KEY               3       /* a key, added to the key items */
#define ITEM_TYPE_MAP               6       /* opens the map viewer */
#define ITEM_TYPE_NOTHING_HAPPENS   8       /* shows "Nothing happened" */

/* ItemCommonDef.use_mode: what using the item does to the stock */
#define ITEM_USE_MODE_TAKE          0       /* the item is used up */
#define ITEM_USE_MODE_KEEP          1       /* the item is kept */
#define ITEM_USE_MODE_EXCHANGE      2       /* the item is used up and the player gets def->item */

/* TRUE when the player cannot take one more of the item ("Can't carry any more", MapItem_PickupHold): a one-of-a-kind
   item already carried, or 99 of another. */
int Player_CannotCarryItem(int idx) {
    ItemCommonDef *entry;

    if (idx < 0) {
        return 0;
    }
    entry = item_common_defs + idx;
    if (entry->flags >> 1 & 1) {
        if (item_counts.bytes[idx] != 0) {
            return 1;
        }
    } else {
        if (item_counts.bytes[idx] >= 0x63) {
            return 1;
        }
    }
    return 0;
}

/* Adds one of an item to what the player carries: up to 1 for weapons and worn items and for items with flag
   bit 1, up to 99 for the others ("Can't carry any more" past that). */
void Player_AddItem(int index) {
    ItemCommonDef *def;

    if (index >= ITEM_WORN_FIRST) {
        if (item_counts.bytes[index] == 0) {
            item_counts.bytes[index] = 1;
        }
    } else if (index >= ITEM_WEAPON_FIRST) {
        if (item_counts.bytes[index] == 0) {
            item_counts.bytes[index] = 1;
        }
    } else if (index >= 0) {
        def = &item_common_defs[index];
        if (def->flags >> 1 & 1) {
            if (item_counts.bytes[index] == 0) {
                item_counts.bytes[index] = 1;
                if (def->type == 3) {
                    Menu_AddKeyItem(index);
                }
            }
        } else if (item_counts.bytes[index] < 99) {
            item_counts.bytes[index]++;
            if (def->type == 3) {
                Menu_AddKeyItem(index);
            }
        } else {
            Message_ShowNotice(0, 0, 0, 0, 0x2D, MSG_CANT_CARRY_ANY_MORE);
        }
    }
}

/* Takes one of the item away, if the player has any. */
void Player_RemoveOneItem(u32 index) {
    if (index < 0x17C && item_counts.bytes[index] != 0) {
        item_counts.bytes[index]--;
    }
}

/* Uses the item GameStatus.item_to_use names once GameStatus.bit6 asks for it: an entity or a map object in front of
   the player may take it for a script of its own; otherwise it does what its type says and one is used up. */
void Player_UseItem(void) {
    int item = game_status.item_to_use;
    ItemCommonDef *def = &item_common_defs[item];
    Entity *entity;
    MapObject *object;
    MapObject *stood_on;
    u16 timed;

    if (game_status.item_use_cooldown > 0) {
        game_status.item_use_cooldown--;
    }
    if (game_status.bit6) {
        game_status.bit6 = FALSE;
        game_status.item_use_cooldown = 1;
        if (item_counts.bytes[item] != 0) {
            entity = view_ray_status.facing_entity;
            if (entity != NULL && entity->spawn_info->item_script_id != 0 && entity->group_id == current_map_slot) {
                if (entity->spawn_info->talk_script_id != 0 && Script_IsRunning(entity->spawn_info->talk_script_id) != 0) {
                    return;
                }
                Script_Start(((Entity *)view_ray_status.facing_entity)->spawn_info->item_script_id, 0);
                return;
            }
            object = view_ray_status.current;
            if (object != NULL && object->source->item_use_script != 0 && (MAP_OBJECT_BYTES(object)->map_slot & 1) == current_map_slot) {
                if (object->source->use_script != 0 && Script_IsRunning(object->source->use_script) != 0) {
                    return;
                }
                Script_Start(((MapObject *)view_ray_status.current)->source->item_use_script, 0);
                return;
            }
            Player_ApplyItem(item);
            switch (def->type) {
            case ITEM_TYPE_HEAL:
                player_state.hp_now += def->hp_heal;
                if (player_state.hp_now > player_state.hp_max) {
                    player_state.hp_now = player_state.hp_max;
                }
                player_state.mp_now += def->mp_heal;
                if (player_state.mp_now > player_state.mp_max) {
                    player_state.mp_now = player_state.mp_max;
                }
                timed = def->timed_flags;
                if (timed & ITEM_TIMED_FLAG_0) {
                    player_state.timed_flags[0] = 0;
                    player_state.flag_timers[0] = 60;
                }
                if (timed & ITEM_TIMED_FLAG_1) {
                    player_state.timed_flags[1] = 0;
                    player_state.flag_timers[1] = 60;
                }
                if (timed & ITEM_TIMED_FLAG_2) {
                    player_state.timed_flags[2] = 0;
                    player_state.flag_timers[2] = 60;
                }
                if (timed & ITEM_TIMED_FLAG_3) {
                    player_state.timed_flags[3] = 0;
                    player_state.flag_timers[3] = 60;
                }
                if (timed & ITEM_TIMED_FLAG_4) {
                    player_state.timed_flags[4] = 0;
                    player_state.flag_timers[4] = 60;
                }
                if (timed & ITEM_TIMED_FLAG_5) {
                    player_state.timed_flags[5] = 0;
                    player_state.flag_timers[5] = 60;
                }
                break;
            case ITEM_TYPE_USE_ON_OBJECT:
            case 4:
            case 5:
                if (view_ray_status.current == NULL || !MapObject_UseItem(view_ray_status.current, item)) {
                    Message_ShowNotice(0, 0, 0, 1, 0x2D, MSG_NOTHING_HAPPENED);
                    return;
                }
                break;
            case ITEM_TYPE_WAND:
                stood_on = object_under_player[0];
                if (stood_on == NULL || stood_on->definition->kind != MAP_OBJECT_KIND_SYMBOL_OF_PASSAGE || !MapChange_StartPassage(item)) {
                    Message_ShowNotice(0, 0, 0, 1, 0x2D, MSG_NOTHING_HAPPENED);
                    return;
                }
                break;
            case ITEM_TYPE_KEY:
                return;
            case ITEM_TYPE_MAP:
                MapViewer_OpenForItem(item);
                break;
            case 7:
                break;
            case ITEM_TYPE_NOTHING_HAPPENS:
                Message_ShowNotice(0, 0, 0, 1, 0x2D, MSG_NOTHING_HAPPENED);
                return;
            default:
                return;
            }
            Player_ApplyPoisonWater(item);
            switch (def->use_mode) {
            case ITEM_USE_MODE_TAKE:
                item_counts.bytes[item]--;
                break;
            case ITEM_USE_MODE_KEEP:
                break;
            case ITEM_USE_MODE_EXCHANGE:
                item_counts.bytes[item]--;
                item_counts.bytes[def->item]++;
                break;
            }
        }
    }
}

/* What using item `kind` does to the player itself: the Power Seed and the Widda Needle start a timer, the
   fruits raise unk2C ("Magic Power UP") or unk28 ("Physical UP"), the crystals raise unk24 and teach a magic. */
void Player_ApplyItem(int kind) {
    int marked;

    switch (kind) {
    case ITEM_POWER_SEED:
        player_state.power_seed_timer = 1800;
        break;
    case ITEM_WIDDA_NEEDLE:
        player_state.widda_needle_timer = 1800;
        break;
    case ITEM_WISE_MANS_FRUIT:
        if (player_state.magic_base < 999) {
            Message_ShowNotice(0, 0, 0, 1, 0x2D, MSG_MAGIC_POWER_UP);
            player_state.magic_base++;
        } else {
            Message_ShowNotice(0, 0, 0, 1, 0x2D, MSG_NOTHING_HAPPENED);
        }
        break;
    case ITEM_FOOLS_FRUIT:
        if (player_state.power_base < 999) {
            player_state.power_base++;
            Message_ShowNotice(0, 0, 0, 1, 0x2D, MSG_PHYSICAL_UP);
        } else {
            Message_ShowNotice(0, 0, 0, 1, 0x2D, MSG_NOTHING_HAPPENED);
        }
        break;
    case ITEM_MAGIC_CRYSTAL:
        player_state.mp_base++;
        if (player_state.mp_base > 999) {
            player_state.mp_base = 999;
        }
        Player_RecomputeStats();
        ItemEffect_Start(kind, 0);
        break;
    case ITEM_FIRE_CRYSTAL:
    case ITEM_EARTH_CRYSTAL:
    case ITEM_WIND_CRYSTAL:
    case ITEM_WATER_CRYSTAL:
    case ITEM_LIGHT_CRYSTAL:
    case ITEM_DARK_CRYSTAL:
        marked = Player_LearnSpellInRow(kind - ITEM_FIRE_CRYSTAL);
        player_state.mp_base++;
        if (player_state.mp_base > 999) {
            player_state.mp_base = 999;
        }
        Player_RecomputeStats();
        if (player_state.charging_magic == 0) {
            Player_PickChargingMagic();
        }
        ItemEffect_Start(kind, marked);
        break;
    case ITEM_FIRE_CRYSTAL_2:
    case ITEM_EARTH_CRYSTAL_2:
    case ITEM_WIND_CRYSTAL_2:
    case ITEM_WATER_CRYSTAL_2:
    case ITEM_LIGHT_CRYSTAL_2:
    case ITEM_DARK_CRYSTAL_2:
        marked = Player_LearnSpellInRow(kind - ITEM_FIRE_CRYSTAL_2);
        player_state.mp_base += 5;
        if (player_state.mp_base > 999) {
            player_state.mp_base = 999;
        }
        Player_RecomputeStats();
        if (player_state.charging_magic == 0) {
            Player_PickChargingMagic();
        }
        ItemEffect_Start(kind, marked);
        break;
    }
}

/* Item 18 (Poison Water) poisons the player (timed_flags[0]) for 1800 frames. */
void Player_ApplyPoisonWater(int kind) {
    if (kind == 0x12) {
        player_state.timed_flags[0] = 1800;
        player_state.ailment0_start_frame = frame_counter;
    }
}

/* Learns the next magic of a spell row: the first of its five slots at level 0 goes to 1. FALSE if all are learned. */
int Player_LearnSpellInRow(int row) {
    int i;

    for (i = 0; i < 5; i++) {
        if (spell_levels[row][i] == 0) {
            spell_levels[row][i] = 1;
            return TRUE;
        }
    }
    return FALSE;
}
