#include "kf_map.h"
#include "kf_message.h"
#include "kf_item.h"
#include "kf_game.h"
#include "kf_entity.h"
#include "kf_map_object.h"
#include "kf_player.h"
#include "kf_item_effect.h"

int Script_IsRunning(int);
int Script_Start(int, int);
void Player_ApplyItem(int);
void Player_ApplyPoisonWater(int);
int func_001188E0(int);
void func_0011A438(int);
void Player_RecomputeStats(void);
void Player_PickChargingMagic(void);
int Player_LearnSpellInRow(int);

extern u8 item_counts[];
extern ItemCommonDef item_common_defs[];
extern GameStatus game_status;
extern EventRunner D_00420D00;
extern PlayerState player_state;
extern void *D_00414188[];
extern int D_00446770;
extern u32 D_00446ED4;
extern u8 spell_levels[6][5];

/* TRUE when the player cannot take one more of the item ("Can't carry any more", func_001C9FB0): a one-of-a-kind
   item already carried, or 99 of another. */
int Player_CannotCarryItem(int idx) {
    ItemCommonDef *entry;

    if (idx < 0) {
        return 0;
    }
    entry = item_common_defs + idx;
    if (entry->flags >> 1 & 1) {
        if (item_counts[idx] != 0) {
            return 1;
        }
    } else {
        if (item_counts[idx] >= 0x63) {
            return 1;
        }
    }
    return 0;
}

void Menu_AddKeyItem(int); // match: one argument is passed (ScriptCmd_AddItem declares three)
/* Adds one of an item to what the player carries: up to 1 for weapons and worn items and for items with flag
   bit 1, up to 99 for the others ("Can't carry any more" past that). */
void Player_AddItem(int index) {
    ItemCommonDef *def;

    if (index >= ITEM_WORN_FIRST) {
        if (item_counts[index] == 0) {
            item_counts[index] = 1;
        }
    } else if (index >= ITEM_WEAPON_FIRST) {
        if (item_counts[index] == 0) {
            item_counts[index] = 1;
        }
    } else if (index >= 0) {
        def = &item_common_defs[index];
        if (def->flags >> 1 & 1) {
            if (item_counts[index] == 0) {
                item_counts[index] = 1;
                if (def->type == 3) {
                    Menu_AddKeyItem(index);
                }
            }
        } else if (item_counts[index] < 99) {
            item_counts[index]++;
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
    if (index < 0x17C && item_counts[index] != 0) {
        item_counts[index]--;
    }
}

/* Uses the item GameStatus.unk20 names once GameStatus.bit6 asks for it: an entity or a map object in front of
   the player may take it for a script of its own; otherwise it does what its type says and one is used up. */
void Player_UseItem(void) {
    int item = game_status.unk20;
    ItemCommonDef *def = &item_common_defs[item];
    Entity *entity;
    MapObject *object;
    MapObject *stood_on;
    u16 timed;

    if (game_status.unk44 > 0) {
        game_status.unk44--;
    }
    if (game_status.bit6) {
        game_status.bit6 = FALSE;
        game_status.unk44 = 1;
        if (item_counts[item] != 0) {
            entity = D_00420D00.unk54;
            if (entity != NULL && entity->spawn_info->unk58 != 0 && entity->group_id == D_00446770) {
                if (entity->spawn_info->unk4E != 0 && Script_IsRunning(entity->spawn_info->unk4E) != 0) {
                    return;
                }
                Script_Start(((Entity *)D_00420D00.unk54)->spawn_info->unk58, 0);
                return;
            }
            object = D_00420D00.current;
            if (object != NULL && object->unk08->unk58 != 0 && (MAP_OBJECT_BYTES(object)->unk00 & 1) == D_00446770) {
                if (object->unk08->unk40 != 0 && Script_IsRunning(object->unk08->unk40) != 0) {
                    return;
                }
                Script_Start(((MapObject *)D_00420D00.current)->unk08->unk58, 0);
                return;
            }
            Player_ApplyItem(item);
            switch (def->type) {
            case 0:
                player_state.hp_now += def->hp_heal;
                if (player_state.hp_now > player_state.hp_max) {
                    player_state.hp_now = player_state.hp_max;
                }
                player_state.mp_now += def->mp_heal;
                if (player_state.mp_now > player_state.mp_max) {
                    player_state.mp_now = player_state.mp_max;
                }
                timed = def->unk08;
                if (timed & 1) {
                    player_state.timed_flags[0] = 0;
                    player_state.flag_timers[0] = 60;
                }
                if (timed & 2) {
                    player_state.timed_flags[1] = 0;
                    player_state.flag_timers[1] = 60;
                }
                if (timed & 4) {
                    player_state.timed_flags[2] = 0;
                    player_state.flag_timers[2] = 60;
                }
                if (timed & 8) {
                    player_state.timed_flags[3] = 0;
                    player_state.flag_timers[3] = 60;
                }
                if (timed & 0x10) {
                    player_state.timed_flags[4] = 0;
                    player_state.flag_timers[4] = 60;
                }
                if (timed & 0x20) {
                    player_state.timed_flags[5] = 0;
                    player_state.flag_timers[5] = 60;
                }
                break;
            case 1:
            case 4:
            case 5:
                if (D_00420D00.current == NULL || !MapObject_UseItem(D_00420D00.current, item)) {
                    Message_ShowNotice(0, 0, 0, 1, 0x2D, MSG_NOTHING_HAPPENED);
                    return;
                }
                break;
            case 2:
                stood_on = D_00414188[0];
                if (stood_on == NULL || stood_on->unk04->kind != MAP_OBJECT_KIND_SYMBOL_OF_PASSAGE || !func_001188E0(item)) {
                    Message_ShowNotice(0, 0, 0, 1, 0x2D, MSG_NOTHING_HAPPENED);
                    return;
                }
                break;
            case 3:
                return;
            case 6:
                func_0011A438(item);
                break;
            case 7:
                break;
            case 8:
                Message_ShowNotice(0, 0, 0, 1, 0x2D, MSG_NOTHING_HAPPENED);
                return;
            default:
                return;
            }
            Player_ApplyPoisonWater(item);
            switch (def->use_mode) {
            case 0:
                item_counts[item]--;
                break;
            case 1:
                break;
            case 2:
                item_counts[item]--;
                item_counts[def->unk02]++;
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
        if (player_state.unk264 == 0) {
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
        if (player_state.unk264 == 0) {
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
        player_state.unk2C4 = D_00446ED4;
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
