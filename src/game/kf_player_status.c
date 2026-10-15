#include "kf_player.h"
#include "kf_game.h"
#include "kf_effect.h"
#include "kf_map.h"
#include "kf_gs.h"
#include "kf_message.h"
#include "kf_screen.h"

/* one of the nine values: the item's base plus what it gained, scaled by LevelRecord.level in percent */
#define ADD_SCALED(n) totals[n] += (def->unk00[n] + record->unk00[n]) * record->level / 100

void Vector_Normalize(void *dst, void *src);
void func_001B57A0(u8, u8, u8);
void func_001B9610(void);
void func_001BB020(int);
void func_001BB2E8(int);
int func_001BB308(int);
void func_001BB360(void);
MapRecordInfo *func_00115CD0(CameraState *, f32, int);
int func_00109098(ModelInstance *model, u8 index, u16 speed);
void func_00109A50(void);
void func_00109A80(int, s64);
void func_00109AD0(void);
void func_0010A4E8(int, int, int, int, int);
int Sound_PlaySw(int); // match: an int return (the definition is void) keeps $2 busy in func_001BBFC8
int Sound_PlayCommon(int);

extern PlayerState D_00413F80;
extern int D_00413FD8[];            /* D_00413F80.hp_max */
extern int D_00414524[];            /* D_00414280 + 0x2A4 */
/* 0x414580 the tint the damage and the statuses fade the screen with (Player_DrawScreenTints draws it) */
PlayerScreenTint D_00414580 = { 0 };
extern PlayerItemDef weapon_defs[];
extern FxTableEntry D_00402DA8[];
extern LevelRecord D_003679A0[70];
extern LevelRecord D_003682F0[];
extern LevelRecordDef worn_defs[];
extern const PlayerLevelStep D_0043E130[];
extern GameStatus D_0038CC50;
extern MapSlot D_0038CD20[];
extern DisplayContext *D_00446978;
extern int D_00446770;
extern s8 D_00446958;
extern int D_0044695C;
extern u8 spell_levels[6][5];
extern u16 spell_experience[6][5];

/* Works out what the worn items add: clears unkDC to unk10C, lets every worn item add its part, then
   turns the four rates into frame counts. */
void func_001BAE88(void) {
    D_00413F80.unkE8 = 0;
    D_00413F80.unkEC = 0;
    D_00413F80.unkF0 = 0;
    D_00413F80.unkF4 = 0;
    D_00413F80.unkF8 = 0;
    D_00413F80.unkFC = 0;
    D_00413F80.unk100 = 0;
    D_00413F80.unk104 = 0;
    D_00413F80.unk108 = 0;
    D_00413F80.unk10C = 0;
    D_00413F80.unkDC[0] = 0;
    D_00413F80.unkDC[1] = 0;
    D_00413F80.unkDC[2] = 0;
    D_00413F80.unkDC[3] = 0;
    D_00413F80.unkDC[4] = 0;
    D_00413F80.unkDC[5] = 0;
    func_001BB020(D_00413F80.weapon + ITEM_WEAPON_FIRST);
    func_001BB020(D_00413F80.equipment[EQUIP_SHIELD] + ITEM_WORN_FIRST);
    func_001BB020(D_00413F80.equipment[EQUIP_HELM] + ITEM_WORN_FIRST);
    func_001BB020(D_00413F80.equipment[EQUIP_ARMOR] + ITEM_WORN_FIRST);
    func_001BB020(D_00413F80.equipment[EQUIP_GAUNTLET] + ITEM_WORN_FIRST);
    func_001BB020(D_00413F80.equipment[EQUIP_BOOT] + ITEM_WORN_FIRST);
    func_001BB020(D_00413F80.equipment[EQUIP_RING] + ITEM_WORN_FIRST);
    func_001BB020(D_00413F80.equipment[EQUIP_BRACELET] + ITEM_WORN_FIRST);
    func_001BB020(D_00413F80.equipment[EQUIP_AMULET] + ITEM_WORN_FIRST);
    if (D_00413F80.unkF0 != 0) {
        D_00413F80.unkF0 = 150 / (s32)D_00413F80.unkF0;
        if (D_00413F80.unkF0 == 0) {
            D_00413F80.unkF0 = 1;
        }
    }
    if (D_00413F80.unkF4 != 0) {
        D_00413F80.unkF4 = 150 / (s32)D_00413F80.unkF4;
        if (D_00413F80.unkF4 == 0) {
            D_00413F80.unkF4 = 1;
        }
    }
    if (D_00413F80.unkF8 != 0) {
        D_00413F80.unkF8 = 150 / (s32)D_00413F80.unkF8;
        if (D_00413F80.unkF8 == 0) {
            D_00413F80.unkF8 = 1;
        }
    }
    if (D_00413F80.unkFC != 0) {
        D_00413F80.unkFC = 150 / (s32)D_00413F80.unkFC;
        if (D_00413F80.unkFC == 0) {
            D_00413F80.unkFC = 1;
        }
    }
}

/* Adds what wearing item `index` gives (a weapon from ITEM_WEAPON_FIRST, the rest from ITEM_WORN_FIRST) to the player. */
void func_001BB020(int index) {
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
            D_00413F80.unkE8 = 1;
            break;
        case 2:
            D_00413F80.unkEC = 1;
            break;
        case 3:
            D_00413F80.unkF0 += amounts[i];
            break;
        case 4:
            D_00413F80.unkF4 += amounts[i];
            break;
        case 5:
            D_00413F80.unkF8 += amounts[i];
            break;
        case 6:
            D_00413F80.unkFC += amounts[i];
            break;
        case 7:
            D_00413F80.unk100 = amounts[i];
            break;
        case 8:
            D_00413F80.unk104 = amounts[i];
            break;
        case 9:
            D_00413F80.power += amounts[i];
            break;
        case 10:
            D_00413F80.magic += amounts[i];
            break;
        case 0:
        case 11:
            break;
        }
    }
    if (index >= ITEM_WORN_FIRST) {
        rates = worn_defs[ITEM_WORN_INDEX(index)].unk12;
        D_00413F80.unkDC[0] += rates[0];
        D_00413F80.unkDC[1] += rates[1];
        D_00413F80.unkDC[2] += rates[2];
        D_00413F80.unkDC[3] += rates[3];
        D_00413F80.unkDC[4] += rates[4];
        D_00413F80.unkDC[5] += rates[5];
    }
    if (index == ITEM_DEMON_LEGS) {
        D_00413F80.unk108 = 1;
    }
    if (index == ITEM_RING_OF_SIGHT) {
        D_00413F80.unk10C = 1;
    }
}

void func_001BB260(int damage)
{
    int tint;

    if (func_001BB308(damage) != 0) {
        func_001BB360();
        tint = ((damage << 7) / (D_00413FD8[0] / 2)) + 0x7F;
        if (tint >= 0x100)
            tint = 0xFF;
        func_001B57A0(tint, 4, 0);
        D_0044695C = 6;
    }
}

void func_001BB2E8(int value) {
    func_001BB308(value);
    func_001BB360();
}

/* Takes amount from the counter, clamped at zero; fails for a non-positive amount or while the block is locked. */
int func_001BB308(int amount) {
    if (amount > 0 && D_0038CC50.unk00 != 0 && !(D_0038CC50.unk30 & 1)) {
        if (amount < D_00413F80.hp_now) {
            D_00413F80.hp_now -= amount;
        } else {
            D_00413F80.hp_now = 0;
        }
        return 1;
    }
    return 0;
}

void func_001BB360(void) {
    if (D_0038CC50.unk00 != 0 && !(D_0038CC50.unk30 & 1)) {
        D_00414580.unk00 = 10;
    }
}

void func_001BB390(void *source, int damage) {
    PlayerState *player;
    float speed;

    if (D_0038CC50.unk00 != 0) {
        if (!(D_0038CC50.unk30 & 1)) {
            player = &D_00413F80;
            if (player->unk1A8 == 0) {
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

/* Sets the player aim vector (+0x220) to the flattened, normalised direction from target, and its speed. */
void func_001BB468(Vector4 *target, f32 speed) {
    D_00413F80.aim.x = D_00413F80.position.x - target->x;
    D_00413F80.aim.y = 0.0f;
    D_00413F80.aim.z = D_00413F80.position.z - target->z;
    Vector_Normalize(&D_00413F80.aim, &D_00413F80.aim);
    D_00413F80.aim_speed = speed;
}

void func_001BB4D0(int damage) {
    PlayerState *player;
    int frames;

    if (D_0038CC50.unk00 != 0 && !(D_0038CC50.unk30 & 1)) {
        player = &D_00413F80;
        if (!((float)damage * 100.0f / (float)player->hp_max < 10.0f)) {
            frames = (damage * 500 / (player->hp_max + 50)) / player->power;
            if (player->unk234 < frames) {
                float angle = player->unk238;

                if (0.2617994f > angle) {
                    player->unk234 = frames;
                }
            }
        }
    }
}

int Sound_StopVoice(int); // match: an int return (the definition is void) keeps -1 in $3

/* Stops the sound slot D_00446958 holds, if any. */
void func_001BB590(void) {
    if (D_00446958 >= 0) {
        Sound_StopVoice(D_00446958);
        D_00446958 = -1;
    }
}

void func_001BB5C0(void) {
    if (D_00413F80.unk1A8 == 0) {
        D_00413F80.hp_now = 0;
        D_00413F80.unk1A4 = 0;
        func_001B9610();
    }
}

/* Picks PlayerState.unk264: unk3D, else unk46, else what the current PlayerItemDef names, else the first
   marked slot of spell_levels whose effect is ready. */
void func_001BB5E8(void) {
    PlayerItemDef *def;
    int i;
    int j;
    int effect_id;

    if (D_00413F80.equipped_magic != 0) {
        D_00413F80.unk264 = D_00413F80.equipped_magic;
        return;
    }
    if (D_00413F80.shortcut_type == 2 && D_00413F80.shortcut != 0) {
        D_00413F80.unk264 = D_00413F80.shortcut;
        return;
    }
    def = &weapon_defs[D_00413F80.weapon];
    if (def->unk63 != 2) {
        if (def->unk42 != 0xFFFF) {
            D_00413F80.unk264 = def->unk42;
            return;
        }
        // match: the pointer form puts the sum in the index register
        if (def->unk52 != 0xFFFF && (D_003679A0 + D_00413F80.weapon)->unk14 >= 2) {
            D_00413F80.unk264 = def->unk52;
            return;
        }
    }
    for (i = 0; i < 6; i++) {
        for (j = 0; j < 5; j++) {
            effect_id = 15 * i + 3 * j + spell_levels[i][j];
            if (spell_levels[i][j] != 0 && D_00402DA8[effect_id].unk102 == 1) {
                D_00413F80.unk264 = effect_id;
                return;
            }
        }
    }
}

/* Adds the item's nine values (base plus what was gained), scaled by its level in percent, to the totals. */
void func_001BB728(u16 *totals, u32 index) {
    LevelRecordDef *def;
    LevelRecord *record;

    if (index < 160) {
        def = &worn_defs[index];
        record = &D_003682F0[index];
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
void func_001BB908(void) {
    PlayerState *player = &D_00413F80;
    MapRecordInfo *record;

    record = func_00115CD0((CameraState *)player, 100.0f, D_00446770);
    if (record != NULL) {
        // match: retail shifts by 14, not by 7 (the records are 0x80 bytes each)
        player->map_index = (u32)((char *)record - D_0038CD20[D_00446770].records) >> 14;
        player->unk250 = record->unk38;
    }
}

void func_001BB978(void) {
    PlayerState *player;
    MapSlot *slot;
    char area;
    int step;
    float fade;
    float next;

    slot = &D_0038CD20[D_00446770];
    player = &D_00413F80;
    area = *(char *)((player->map_index << 7) + (int)slot->records + 0x4C); // match: summed as integers, index first
    *(s32 *)player->pad2A4 = area;
    if (area - D_00414524[0] < 0) {
        *(s32 *)player->pad2AC = -0x30;
    }
    step = *(s32 *)player->pad2AC;
    if (step != 0) {
        next = player->unk2A8 + (float)step;
        *(s32 *)player->pad2AC = step / 2;
        player->unk2A8 = next;
        return;
    }
    fade = player->unk2A8;
    if (fade < 0.0f) {
        next = fade + 0.5f;
        player->unk2A8 = next;
        if (next >= 0.0f) {
            player->unk2A8 = 0.0f;
        }
    } else if (fade > 0.0f) {
        next = fade - 4.0f;
        player->unk2A8 = next;
        if (next <= 0.0f) {
            player->unk2A8 = 0.0f;
        }
    }
}

/* Fades the screen toward white (D_00413F80.unk2A8 > 0) or black (< 0) by that amount. */
void func_001BBA58(void) {
    if (D_00413F80.unk2A8 != 0.0f) {
        func_00109A50();
        func_00109A80(GS_REG_TEST_1, 0x30000);
        func_00109A80(GS_REG_ZBUF_1, 0x10A000118L);
        func_00109AD0();
        if (D_00413F80.unk2A8 > 0.0f) {
            func_0010A4E8(0xFF, 0xFF, 0xFF, (int)D_00413F80.unk2A8, 0x48);
        } else if (D_00413F80.unk2A8 < 0.0f) {
            func_0010A4E8(0, 0, 0, (int)-D_00413F80.unk2A8, 0x44);
        }
        func_00109A50();
        func_00109A80(GS_REG_ZBUF_1, 0xA000118L);
        func_00109AD0();
    }
}

/* Adds experience (twice as much while Fortune is worn) and takes every level it now reaches ("Level UP"). */
void Player_AddExperience(int amount) {
    int level;

    if (D_00413F80.equipment[EQUIP_BRACELET] == ITEM_WORN_INDEX(ITEM_FORTUNE)) {
        amount *= 2;
    }
    D_00413F80.experience += amount;
    if (D_00413F80.experience > 999998) {
        D_00413F80.experience = 999999;
    }
    for (level = D_00413F80.level + 1; level < 100; level++) {
        if (D_00413F80.experience >= D_0043E130[level].experience) {
            D_00413F80.level++;
            D_00413F80.hp_base += D_0043E130[level].hp_gain;
            D_00413F80.mp_base += D_0043E130[level].mp_gain;
            D_00413F80.power_base += D_0043E130[level].power_gain;
            D_00413F80.magic_base += D_0043E130[level].magic_gain;
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
        if (D_00402DA8[effect_id].unk102 == 0) {
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
            if (D_00413F80.equipped_magic == effect_id) {
                D_00413F80.equipped_magic = effect_id + 1;
            } else if (D_00413F80.shortcut_type == 2 && D_00413F80.shortcut == effect_id) {
                D_00413F80.shortcut++;
            }
        }
    }
    D_00413F80.magic_use_count = (D_00413F80.magic_use_count + 1) % 100;
    if (D_00413F80.magic_use_count == 0 && D_00413F80.magic_base < 999) {
        D_00413F80.magic_base++;
        Message_ShowNotice(0, 0, 0, 0, 0x2D, MSG_MAGIC_POWER_UP);
    }
}

/* Counts a use of the weapon in hand (its row of D_003679A0): at 180 and 480 uses its level goes up ("Weapon Level UP").
   Every 100th call also raises PlayerState.power_base ("Physical UP"). */
void Player_CountWeaponUse(int weapon) {
    LevelRecord *record = &D_003679A0[weapon];
    u16 uses;
    int new_uses;
    int count;
    PlayerState *player;

    uses = *(u16 *)&record->unk16; // match: read as unsigned
    if (uses < 0x1E0) {
        new_uses = uses + 1;
        record->unk16 = new_uses;
        if ((u16)new_uses == 0xB4 || (u16)new_uses == 0x1E0) {
            if (record->unk14 < 2) {
                record->unk14++;
                Message_ShowNotice(0, 0, 0, 0, 0x2D, MSG_WEAPON_LEVEL_UP);
            }
        }
    }
    player = &D_00413F80;
    count = (player->weapon_use_count + 1) % 100;
    player->weapon_use_count = count;
    if (count == 0) {
        if (player->power_base < 0x3E7) {
            player->power_base++;
            Message_ShowNotice(0, 0, 0, 0, 0x2D, MSG_PHYSICAL_UP);
        }
    }
}

void func_001BBFC8(void) {
    if (D_00413F80.unk261 == 0 && weapon_defs[D_00413F80.weapon].unk63 != 2) {
        Sound_StopVoice(2);
        Sound_PlaySw(1);
        D_00413F80.unk261 = 1;
        func_00109098((ModelInstance *)D_00446978, 0, D_00446978->settings->unk82);
    }
}

/* Kick off a request once the flag at 0x261 is clear and the table entry at 0x63 is not 2. */
void func_001BC058(void) {
    DisplayContext *display;

    if (D_00413F80.unk261 == 0 && weapon_defs[D_00413F80.weapon].unk63 != 2) {
        Sound_StopVoice(2);
        display = D_00446978;
        D_00413F80.unk261 = 1;
        func_00109098((ModelInstance *)display, 0, display->settings->unk82);
    }
}

void func_001BC0E0(int with_sound) {
    PlayerState *player = &D_00413F80;

    if (player->unk262 == 0) {
        if (with_sound) {
            Sound_PlayCommon(0x23);
        }
        player->unk262 = 1;
    }
}

void func_001BC128(void) {
    if (D_00413F80.unk258 != 0) {
        D_00413F80.unk263 = 1;
    }
}
