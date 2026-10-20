#include "player/player.h"
#include "core/game.h"
#include "effect/effect.h"
#include "map/map.h"
#include "gfx/gs.h"
#include "ui/message.h"
#include "gfx/screen.h"
#include "gfx/gs_draw.h"
#include "gfx/packet.h"
#include "input/input.h"
#include "math/vu0.h"
#include "model/motion.h"
#include "player/player_update.h"
#include "save/save_state.h"

/* one of the nine values: the item's base plus what it gained, scaled by LevelRecord.level in percent */
#define ADD_SCALED(n) totals[n] += (def->base_values[n] + record->gained_values[n]) * record->level / 100

void Player_AddItemBonuses(int);
int Sound_PlaySw(int); // match: an int return (the definition is void) keeps $2 busy in Player_BounceWeapon
int Sound_PlayCommon(int);

/* 0x414580 the tint the damage and the statuses fade the screen with (Player_DrawScreenTints draws it) */
PlayerScreenTint player_screen_tint = { 0 };
extern GameStatus game_status;
extern DisplayContext *held_item_model_ptr;
extern int current_map_slot;

/* Works out what the worn items add: clears unkDC to unk10C, lets every worn item add its part, then
   turns the four rates into frame counts. */
void Player_SumWornBonuses(void) {
    player_state.item_bonus_kind1_active = 0;
    player_state.worn_drain_active = 0;
    player_state.hp_regen_period = 0;
    player_state.hp_drain_period = 0;
    player_state.mp_regen_period = 0;
    player_state.mp_drain_period = 0;
    player_state.damage_to_hp_percent = 0;
    player_state.damage_to_mp_percent = 0;
    player_state.movement_halved = 0;
    player_state.ring_of_sight_active = 0;
    player_state.ailment_defence[0] = 0;
    player_state.ailment_defence[1] = 0;
    player_state.ailment_defence[2] = 0;
    player_state.ailment_defence[3] = 0;
    player_state.ailment_defence[4] = 0;
    player_state.ailment_defence[5] = 0;
    Player_AddItemBonuses(player_state.weapon + ITEM_WEAPON_FIRST);
    Player_AddItemBonuses(player_state.equipment[EQUIP_SHIELD] + ITEM_WORN_FIRST);
    Player_AddItemBonuses(player_state.equipment[EQUIP_HELM] + ITEM_WORN_FIRST);
    Player_AddItemBonuses(player_state.equipment[EQUIP_ARMOR] + ITEM_WORN_FIRST);
    Player_AddItemBonuses(player_state.equipment[EQUIP_GAUNTLET] + ITEM_WORN_FIRST);
    Player_AddItemBonuses(player_state.equipment[EQUIP_BOOT] + ITEM_WORN_FIRST);
    Player_AddItemBonuses(player_state.equipment[EQUIP_RING] + ITEM_WORN_FIRST);
    Player_AddItemBonuses(player_state.equipment[EQUIP_BRACELET] + ITEM_WORN_FIRST);
    Player_AddItemBonuses(player_state.equipment[EQUIP_AMULET] + ITEM_WORN_FIRST);
    if (player_state.hp_regen_period != 0) {
        player_state.hp_regen_period = 150 / (s32)player_state.hp_regen_period;
        if (player_state.hp_regen_period == 0) {
            player_state.hp_regen_period = 1;
        }
    }
    if (player_state.hp_drain_period != 0) {
        player_state.hp_drain_period = 150 / (s32)player_state.hp_drain_period;
        if (player_state.hp_drain_period == 0) {
            player_state.hp_drain_period = 1;
        }
    }
    if (player_state.mp_regen_period != 0) {
        player_state.mp_regen_period = 150 / (s32)player_state.mp_regen_period;
        if (player_state.mp_regen_period == 0) {
            player_state.mp_regen_period = 1;
        }
    }
    if (player_state.mp_drain_period != 0) {
        player_state.mp_drain_period = 150 / (s32)player_state.mp_drain_period;
        if (player_state.mp_drain_period == 0) {
            player_state.mp_drain_period = 1;
        }
    }
}

/* Adds what wearing item `index` gives (a weapon from ITEM_WEAPON_FIRST, the rest from ITEM_WORN_FIRST) to the player. */
void Player_AddItemBonuses(int index) {
    u8 *kinds;
    u8 *amounts;
    int i;
    u16 *rates;

    if (index >= ITEM_WORN_FIRST) {
        kinds = worn_defs[ITEM_WORN_INDEX(index)].bonus_kinds;
        amounts = worn_defs[ITEM_WORN_INDEX(index)].bonus_amounts;
    } else {
        kinds = weapon_defs[ITEM_WEAPON_INDEX(index)].bonus_kinds;
        amounts = weapon_defs[ITEM_WEAPON_INDEX(index)].bonus_amounts;
    }
    for (i = 0; i < 2; i++) {
        switch (kinds[i]) {
        case 1:
            player_state.item_bonus_kind1_active = 1;
            break;
        case 2:
            player_state.worn_drain_active = 1;
            break;
        case 3:
            player_state.hp_regen_period += amounts[i];
            break;
        case 4:
            player_state.hp_drain_period += amounts[i];
            break;
        case 5:
            player_state.mp_regen_period += amounts[i];
            break;
        case 6:
            player_state.mp_drain_period += amounts[i];
            break;
        case 7:
            player_state.damage_to_hp_percent = amounts[i];
            break;
        case 8:
            player_state.damage_to_mp_percent = amounts[i];
            break;
        case 9:
            player_state.power += amounts[i];
            break;
        case 10:
            player_state.magic += amounts[i];
            break;
        case 0:
        case 11:
            break;
        }
    }
    if (index >= ITEM_WORN_FIRST) {
        rates = worn_defs[ITEM_WORN_INDEX(index)].worn_ailment_defence;
        player_state.ailment_defence[0] += rates[0];
        player_state.ailment_defence[1] += rates[1];
        player_state.ailment_defence[2] += rates[2];
        player_state.ailment_defence[3] += rates[3];
        player_state.ailment_defence[4] += rates[4];
        player_state.ailment_defence[5] += rates[5];
    }
    if (index == ITEM_DEMON_LEGS) {
        player_state.movement_halved = 1;
    }
    if (index == ITEM_RING_OF_SIGHT) {
        player_state.ring_of_sight_active = 1;
    }
}

/* The player takes damage: HP goes down, the screen flashes red and the pad rumbles by how much of half the
   most HP it was. */
void Player_TakeDamage(int damage)
{
    int tint;

    if (Player_LoseHp(damage) != 0) {
        Player_FlashDamage();
        tint = ((damage << 7) / (player_hp_max[0] / 2)) + 0x7F;
        if (tint >= 0x100)
            tint = 0xFF;
        Input_StartRumble(tint, 4, 0);
        recent_damage_frames = 6;
    }
}

/* The player takes damage without the rumble (poison, the ground). */
void Player_TakeDamageNoRumble(int value) {
    Player_LoseHp(value);
    Player_FlashDamage();
}

/* Takes amount off HP, not below zero; FALSE for a non-positive amount or while the game is not being played
   (GameStatus.player_control clear or bit 0 of unk30 set). */
int Player_LoseHp(int amount) {
    if (amount > 0 && game_status.player_control != 0 && !(game_status.update_hold_flags & 1)) {
        if (amount < player_state.hp_now) {
            player_state.hp_now -= amount;
        } else {
            player_state.hp_now = 0;
        }
        return 1;
    }
    return 0;
}

/* Starts the red damage flash (PlayerScreenTint.damage_flash_frames), while the game is being played. */
void Player_FlashDamage(void) {
    if (game_status.player_control != 0 && !(game_status.update_hold_flags & 1)) {
        player_screen_tint.damage_flash_frames = 10;
    }
}

/* Knocks the player back away from source (along x/z), faster the bigger the damage is against the most HP. */
void Player_KnockBackFromHit(void *source, int damage) {
    PlayerState *player;
    float speed;

    if (game_status.player_control != 0) {
        if (!(game_status.update_hold_flags & 1)) {
            player = &player_state;
            if (player->death_timer == 0) {
                if (player->hp_max > 0) {
                    player->aim.x = player->position.x - ((float *)source)[0];
                    player->aim.y = 0;
                    player->aim.z = player->position.z - ((float *)source)[2];
                    Vector_Normalize(&player->aim, &player->aim);
                    speed = ((float)(damage * damage) / (float)player->hp_max) * 99.0f;
                    player->aim_speed = speed;
                    if (speed > 99.0f) {
                        player->aim_speed = 99.0f;
                    }
                }
            }
        }
    }
}

/* Knocks the player back away from target (along x/z) at the speed given (PlayerState.aim and aim_speed). */
void Player_KnockBack(Vector4 *target, f32 speed) {
    player_state.aim.x = player_state.position.x - target->x;
    player_state.aim.y = 0.0f;
    player_state.aim.z = player_state.position.z - target->z;
    Vector_Normalize(&player_state.aim, &player_state.aim);
    player_state.aim_speed = speed;
}

/* A hit of at least a tenth of the most HP throws the view back (PlayerState.view_tilt_pitch grows) for frames that
   grow with the damage and shrink with power. */
void Player_TiltViewFromHit(int damage) {
    PlayerState *player;
    int frames;

    if (game_status.player_control != 0 && !(game_status.update_hold_flags & 1)) {
        player = &player_state;
        if (!((float)damage * 100.0f / (float)player->hp_max < 10.0f)) {
            frames = (damage * 500 / (player->hp_max + 50)) / player->power;
            if (player->view_tilt_frames < frames) {
                float angle = player->view_tilt_pitch;

                if (DEG_TO_RAD(15) > angle) {
                    player->view_tilt_frames = frames;
                }
            }
        }
    }
}

int Sound_StopVoice(int); // match: an int return (the definition is void) keeps -1 in $3

/* Stops the last footstep sound (footstep_voice), if one plays. */
void Player_StopFootstepSound(void) {
    if (footstep_voice >= 0) {
        Sound_StopVoice(footstep_voice);
        footstep_voice = -1;
    }
}

/* Kills the player: HP and the drawn bar to 0, and the dying starts. */
void Player_Kill(void) {
    if (player_state.death_timer == 0) {
        player_state.hp_now = 0;
        player_state.hp_bar_level = 0;
        Player_UpdateDying();
    }
}

/* Picks the magic the magic gauge charges for (PlayerState.charging_magic): the equipped magic, else a magic shortcut,
   else the weapon's magic, else the first learned magic cast from the menu (fx_table unk102 == 1). */
void Player_PickChargingMagic(void) {
    PlayerItemDef *def;
    int i;
    int j;
    int effect_id;

    if (player_state.equipped_magic != 0) {
        player_state.charging_magic = player_state.equipped_magic;
        return;
    }
    if (player_state.shortcut_type == 2 && player_state.shortcut != 0) {
        player_state.charging_magic = player_state.shortcut;
        return;
    }
    def = &weapon_defs[player_state.weapon];
    if (def->action_handler_index != 2) {
        if (def->first_magic_id != 0xFFFF) {
            player_state.charging_magic = def->first_magic_id;
            return;
        }
        // match: the pointer form puts the sum in the index register
        if (def->second_magic_id != 0xFFFF && (weapon_level_records + player_state.weapon)->level_index >= 2) {
            player_state.charging_magic = def->second_magic_id;
            return;
        }
    }
    for (i = 0; i < 6; i++) {
        for (j = 0; j < 5; j++) {
            effect_id = 15 * i + 3 * j + spell_levels[i][j];
            if (spell_levels[i][j] != 0 && fx_table[effect_id].item_menu_magic == 1) {
                player_state.charging_magic = effect_id;
                return;
            }
        }
    }
}

/* Adds the item's nine values (base plus what was gained), scaled by its level in percent, to the totals. */
void Player_AddWornItemValues(u16 *totals, u32 index) {
    LevelRecordDef *def;
    LevelRecord *record;

    if (index < 160) {
        def = &worn_defs[index];
        record = &worn_item_level_records[index];
        ADD_SCALED(0);
        ADD_SCALED(1);
        ADD_SCALED(2);
        ADD_SCALED(3);
        ADD_SCALED(4);
        ADD_SCALED(5);
        ADD_SCALED(6);
        ADD_SCALED(7);
        ADD_SCALED(8);
    }
}

/* Finds the map record the player stands in and notes its index. */
void Player_FindMapRecord(void) {
    PlayerState *player = &player_state;
    MapRecordInfo *record;

    record = Map_FindRecordAtInSlot((f32 *)player, 100.0f, current_map_slot);
    if (record != NULL) {
        // match: retail shifts by 14, not by 7 (the records are 0x80 bytes each)
        player->map_index = (u32)((char *)record - map_slots[current_map_slot].records) >> 14;
        player->record_flag_block = record->flag_block_bit;
    }
}

/* Steps the screen fade amount (unk2A8): stepping onto a map record whose byte 0x4C is lower than the last one
   starts a quick dip toward black; the amount then eases back to 0. */
void Player_UpdateScreenFade(void) {
    PlayerState *player;
    MapSlot *slot;
    char area;
    int step;
    float fade;
    float next;

    slot = &map_slots[current_map_slot];
    player = &player_state;
    area = *(char *)((player->map_index << 7) + (int)slot->records + 0x4C); // match: summed as integers, index first
    player->fade_area = area;
    if (area - player_previous_fade_level[0] < 0) {
        player->screen_fade_step = -0x30;
    }
    step = player->screen_fade_step;
    if (step != 0) {
        next = player->screen_fade + (float)step;
        player->screen_fade_step = step / 2;
        player->screen_fade = next;
        return;
    }
    fade = player->screen_fade;
    if (fade < 0.0f) {
        next = fade + 0.5f;
        player->screen_fade = next;
        if (next >= 0.0f) {
            player->screen_fade = 0.0f;
        }
    } else if (fade > 0.0f) {
        next = fade - 4.0f;
        player->screen_fade = next;
        if (next <= 0.0f) {
            player->screen_fade = 0.0f;
        }
    }
}

/* Fades the screen toward white (player_state.screen_fade > 0) or black (< 0) by that amount. */
void Player_DrawScreenFade(void) {
    if (player_state.screen_fade != 0.0f) {
        Packet_BeginGsRegisters();
        Packet_AddGsRegister(GS_REG_TEST_1, 0x30000);
        Packet_AddGsRegister(GS_REG_ZBUF_1, 0x10A000118L);
        Packet_EndGsRegisters();
        if (player_state.screen_fade > 0.0f) {
            GsDraw_FillScreen(0xFF, 0xFF, 0xFF, (int)player_state.screen_fade, 0x48);
        } else if (player_state.screen_fade < 0.0f) {
            GsDraw_FillScreen(0, 0, 0, (int)-player_state.screen_fade, 0x44);
        }
        Packet_BeginGsRegisters();
        Packet_AddGsRegister(GS_REG_ZBUF_1, 0xA000118L);
        Packet_EndGsRegisters();
    }
}

/* Adds experience (twice as much while Fortune is worn) and takes every level it now reaches ("Level UP"). */
void Player_AddExperience(int amount) {
    int level;

    if (player_state.equipment[EQUIP_BRACELET] == ITEM_WORN_INDEX(ITEM_FORTUNE)) {
        amount *= 2;
    }
    player_state.experience += amount;
    if (player_state.experience > 999998) {
        player_state.experience = 999999;
    }
    for (level = player_state.level + 1; level < 100; level++) {
        if (player_state.experience >= player_level_steps[level].experience) {
            player_state.level++;
            player_state.hp_base += player_level_steps[level].hp_gain;
            player_state.mp_base += player_level_steps[level].mp_gain;
            player_state.power_base += player_level_steps[level].power_gain;
            player_state.magic_base += player_level_steps[level].magic_gain;
            Message_ShowNotice(0, 0, 0, 0, 0x2D, MSG_LEVEL_UP);
        }
    }
}

/* Counts a use of the magic: its slot's use count rises (by 3 for some), and at 50 and 150 uses the slot's
   level goes up ("Magic Level UP"). Every 100th call also raises PlayerState.magic_base ("Magic Power UP"). */
void Player_CountMagicUse(int effect_id) {
    int slot;
    int row;
    int column;
    u32 uses;
    u16 clamped_uses;

    slot = (effect_id - 1) / 3;
    row = slot / 5;
    column = slot % 5;
    if ((u32)row < 6 && column >= 0 && column < 5 && spell_experience[row][column] < 150) {
        if (fx_table[effect_id].item_menu_magic == 0) {
            spell_experience[row][column] += 1;
        } else if ((u32)(effect_id - 0x10) < 3) {
            spell_experience[row][column] += 1;
        } else if ((u32)(effect_id - 0x37) < 3) {
            spell_experience[row][column] += 1;
        } else {
            spell_experience[row][column] += 3;
        }
        uses = spell_experience[row][column];
        if (uses > 150) {
            spell_experience[row][column] = 150;
            uses = 150;
        }
        clamped_uses = uses;
        if ((clamped_uses >= 50 && spell_levels[row][column] < 2)
            || (clamped_uses >= 150 && spell_levels[row][column] < 3)) {
            spell_levels[row][column]++;
            Message_ShowNotice(0, 0, 0, 0, 0x2D, MSG_MAGIC_LEVEL_UP);
            if (player_state.equipped_magic == effect_id) {
                player_state.equipped_magic = effect_id + 1;
            } else if (player_state.shortcut_type == 2 && player_state.shortcut == effect_id) {
                player_state.shortcut++;
            }
        }
    }
    player_state.magic_use_count = (player_state.magic_use_count + 1) % 100;
    if (player_state.magic_use_count == 0 && player_state.magic_base < 999) {
        player_state.magic_base++;
        Message_ShowNotice(0, 0, 0, 0, 0x2D, MSG_MAGIC_POWER_UP);
    }
}

/* Counts a use of the weapon in hand (its row of weapon_level_records): at 180 and 480 uses its level goes up ("Weapon Level UP").
   Every 100th call also raises PlayerState.power_base ("Physical UP"). */
void Player_CountWeaponUse(int weapon) {
    LevelRecord *record = &weapon_level_records[weapon];
    u16 uses;
    int new_uses;
    int count;
    PlayerState *player;

    uses = *(u16 *)&record->use_count; // match: read as unsigned
    if (uses < 0x1E0) {
        new_uses = uses + 1;
        record->use_count = new_uses;
        if ((u16)new_uses == 0xB4 || (u16)new_uses == 0x1E0) {
            if (record->level_index < 2) {
                record->level_index++;
                Message_ShowNotice(0, 0, 0, 0, 0x2D, MSG_WEAPON_LEVEL_UP);
            }
        }
    }
    player = &player_state;
    count = (player->weapon_use_count + 1) % 100;
    player->weapon_use_count = count;
    if (count == 0) {
        if (player->power_base < 0x3E7) {
            player->power_base++;
            Message_ShowNotice(0, 0, 0, 0, 0x2D, MSG_PHYSICAL_UP);
        }
    }
}

/* The weapon bounces off: unless it already did this swing, its sound stops, sound 1 plays and the weapon
   model goes back to motion 0; the swing hits nothing more (unk261). */
void Player_BounceWeapon(void) {
    if (player_state.weapon_bounced == 0 && weapon_defs[player_state.weapon].action_handler_index != 2) {
        Sound_StopVoice(2);
        Sound_PlaySw(1);
        player_state.weapon_bounced = 1;
        ModelMotion_BlendInto((ModelInstance *)held_item_model_ptr, 0, held_item_model_ptr->settings->blend_speed);
    }
}

/* Player_BounceWeapon without the sound (the last boss's hits). */
void Player_BounceWeaponQuietly(void) {
    DisplayContext *display;

    if (player_state.weapon_bounced == 0 && weapon_defs[player_state.weapon].action_handler_index != 2) {
        Sound_StopVoice(2);
        display = held_item_model_ptr;
        player_state.weapon_bounced = 1;
        ModelMotion_BlendInto((ModelInstance *)display, 0, display->settings->blend_speed);
    }
}

/* The swing struck a map object it cannot break: notes it for the swing (unk262), with sound 0x23 when asked. */
void Player_StrikeSolidObject(int with_sound) {
    PlayerState *player = &player_state;

    if (player->struck_solid_object == 0) {
        if (with_sound) {
            Sound_PlayCommon(0x23);
        }
        player->struck_solid_object = 1;
    }
}

/* While a swing runs, keeps it from hitting anything more (unk263; a map change). */
void Player_EndSwingHits(void) {
    if (player_state.item_action_step != 0) {
        player_state.swing_ended = 1;
    }
}
