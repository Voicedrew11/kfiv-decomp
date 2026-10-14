#include "kf_player.h"

extern PlayerState D_00413F80;
extern LevelDef item_name_defs[0x17C];
extern PlayerItemDef weapon_defs[];
extern LevelRecord D_003679A0[70];

void *memset(void *, s32, u32);
void func_001BAE88(void);
void func_001BB728(u16 *, u32);

/* one of the nine values: the item's row for its level plus what it gained, scaled by LevelRecord.level in percent */
#define SET_SCALED(n)     D_00413F80.unk60.values[n] = (item_def->unk00[record->unk14].values[n] + record->unk00[n]) * record->level / 100

/* Works the player's derived numbers out again from the base ones, what is worn and the magic in effect. */
void func_001BA310(void) {
    PlayerItemDef *item_def = &weapon_defs[D_00413F80.weapon];
    LevelRecord *record = &D_003679A0[D_00413F80.weapon];
    s32 unused_third;
    s32 unused_fifth;
    s32 unused_fifteenth;

    if (D_00413F80.equipped_magic != 0) {
        // match: three divisions whose results are never used; only their divide checks are left in retail,
        // so what was divided is not known
        unused_third = D_00413F80.hp_base / 3;
        unused_fifth = D_00413F80.hp_base / 5;
        unused_fifteenth = D_00413F80.hp_base / 15;
    }
    D_00413F80.hp_max = D_00413F80.hp_base;
    D_00413F80.mp_max = D_00413F80.mp_base;
    D_00413F80.power = D_00413F80.power_base;
    D_00413F80.magic = D_00413F80.magic_base;
    func_001BAE88();
    D_00413F80.unkA8 = D_00413F80.power * 3 + 80;
    D_00413F80.unkA8 = (f32)D_00413F80.unkA8 * 2.2046;
    D_00413F80.weight = item_name_defs[D_00413F80.weapon + ITEM_WEAPON_FIRST].unk3E;
    D_00413F80.weight += item_name_defs[D_00413F80.equipment[EQUIP_SHIELD] + ITEM_WORN_FIRST].unk3E;
    D_00413F80.weight += item_name_defs[D_00413F80.equipment[EQUIP_HELM] + ITEM_WORN_FIRST].unk3E;
    D_00413F80.weight += item_name_defs[D_00413F80.equipment[EQUIP_ARMOR] + ITEM_WORN_FIRST].unk3E;
    D_00413F80.weight += item_name_defs[D_00413F80.equipment[EQUIP_GAUNTLET] + ITEM_WORN_FIRST].unk3E;
    D_00413F80.weight += item_name_defs[D_00413F80.equipment[EQUIP_BOOT] + ITEM_WORN_FIRST].unk3E;
    if (PLAYER_WEARS_ANCIENT_SET(&D_00413F80)) {
        D_00413F80.weight /= 3;
    }
    D_00413F80.weight += item_name_defs[D_00413F80.equipment[EQUIP_RING] + ITEM_WORN_FIRST].unk3E;
    D_00413F80.weight += item_name_defs[D_00413F80.equipment[EQUIP_BRACELET] + ITEM_WORN_FIRST].unk3E;
    D_00413F80.weight += item_name_defs[D_00413F80.equipment[EQUIP_AMULET] + ITEM_WORN_FIRST].unk3E;
    SET_SCALED(0);
    SET_SCALED(1);
    SET_SCALED(2);
    SET_SCALED(3);
    SET_SCALED(4);
    SET_SCALED(5);
    SET_SCALED(6);
    SET_SCALED(7);
    SET_SCALED(8);
    D_00413F80.unk60.values[0] += D_00413F80.unk11C[0];
    D_00413F80.unk60.values[1] += D_00413F80.unk11C[1];
    D_00413F80.unk60.values[2] += D_00413F80.unk11C[2];
    D_00413F80.unk60.values[3] += D_00413F80.unk122[0];
    D_00413F80.unk60.values[4] += D_00413F80.unk122[1];
    D_00413F80.unk60.values[5] += D_00413F80.unk122[2];
    D_00413F80.unk60.values[6] += D_00413F80.unk122[3];
    D_00413F80.unk60.values[7] += D_00413F80.unk122[4];
    D_00413F80.unk60.values[8] += D_00413F80.unk122[5];
    D_00413F80.unk72 = item_def->unk36;
    D_00413F80.unk72.values[0] += D_00413F80.unk110[0];
    D_00413F80.unk72.values[1] += D_00413F80.unk110[1];
    D_00413F80.unk72.values[2] += D_00413F80.unk110[2];
    D_00413F80.unk72.values[3] += D_00413F80.unk110[3];
    D_00413F80.unk72.values[4] += D_00413F80.unk110[4];
    D_00413F80.unk72.values[5] += D_00413F80.unk110[5];
    memset(&D_00413F80.unk7E, 0, sizeof(PlayerShotStats));
    func_001BB728((u16 *)D_00413F80.unk7E.values, D_00413F80.equipment[EQUIP_SHIELD]);
    func_001BB728((u16 *)D_00413F80.unk7E.values, D_00413F80.equipment[EQUIP_HELM]);
    func_001BB728((u16 *)D_00413F80.unk7E.values, D_00413F80.equipment[EQUIP_ARMOR]);
    func_001BB728((u16 *)D_00413F80.unk7E.values, D_00413F80.equipment[EQUIP_GAUNTLET]);
    func_001BB728((u16 *)D_00413F80.unk7E.values, D_00413F80.equipment[EQUIP_BOOT]);
    func_001BB728((u16 *)D_00413F80.unk7E.values, D_00413F80.equipment[EQUIP_RING]);
    func_001BB728((u16 *)D_00413F80.unk7E.values, D_00413F80.equipment[EQUIP_BRACELET]);
    func_001BB728((u16 *)D_00413F80.unk7E.values, D_00413F80.equipment[EQUIP_AMULET]);
    memset(&D_00413F80.unk90, 0, sizeof(PlayerShotStats));
    func_001BB728((u16 *)D_00413F80.unk90.values, D_00413F80.equipment[EQUIP_SHIELD]);
    func_001BB728((u16 *)D_00413F80.unk90.values, D_00413F80.equipment[EQUIP_HELM]);
    func_001BB728((u16 *)D_00413F80.unk90.values, D_00413F80.equipment[EQUIP_ARMOR]);
    func_001BB728((u16 *)D_00413F80.unk90.values, D_00413F80.equipment[EQUIP_GAUNTLET]);
    func_001BB728((u16 *)D_00413F80.unk90.values, D_00413F80.equipment[EQUIP_BOOT]);
    func_001BB728((u16 *)D_00413F80.unk90.values, D_00413F80.equipment[EQUIP_RING]);
    func_001BB728((u16 *)D_00413F80.unk90.values, D_00413F80.equipment[EQUIP_BRACELET]);
    func_001BB728((u16 *)D_00413F80.unk90.values, D_00413F80.equipment[EQUIP_AMULET]);
    D_00413F80.unk90.values[0] += D_00413F80.unk13A[0];
    D_00413F80.unk90.values[1] += D_00413F80.unk13A[1];
    D_00413F80.unk90.values[2] += D_00413F80.unk13A[2];
    D_00413F80.unk90.values[3] += D_00413F80.unk140[0];
    D_00413F80.unk90.values[4] += D_00413F80.unk140[1];
    D_00413F80.unk90.values[5] += D_00413F80.unk140[2];
    D_00413F80.unk90.values[6] += D_00413F80.unk140[3];
    D_00413F80.unk90.values[7] += D_00413F80.unk140[4];
    D_00413F80.unk90.values[8] += D_00413F80.unk140[5];
    D_00413F80.unkDC[0] += D_00413F80.unk12E[0];
    D_00413F80.unkDC[1] += D_00413F80.unk12E[1];
    D_00413F80.unkDC[2] += D_00413F80.unk12E[2];
    D_00413F80.unkDC[3] += D_00413F80.unk12E[3];
    D_00413F80.unkDC[4] += D_00413F80.unk12E[4];
    D_00413F80.unkDC[5] += D_00413F80.unk12E[5];
    if (D_00413F80.unkEC != 0 || (u16)D_00413F80.timed_flags[3] != 0) { // match: read unsigned here
        D_00413F80.power /= 2;
        D_00413F80.magic /= 2;
    }
    if (D_00413F80.unk2EC > 0) {
        D_00413F80.power += D_00413F80.unk2F0;
    }
    if (D_00413F80.power_seed_timer != 0) {
        D_00413F80.power += D_00413F80.power / 10;
    }
    if (D_00413F80.widda_needle_timer != 0) {
        D_00413F80.magic += D_00413F80.magic / 10;
    }
    if (D_00413F80.unk2EC > 0) {
        D_00413F80.magic = 1;
    }
    if (D_00413F80.equipment[EQUIP_GAUNTLET] == ITEM_WORN_INDEX(ITEM_SACRED_GLOVES)) {
        s32 item_weight = item_name_defs[D_00413F80.weapon + ITEM_WEAPON_FIRST].unk3E;

        D_00413F80.weight -= item_weight / 4;
    }
    if (D_00413F80.equipment[EQUIP_GAUNTLET] == ITEM_WORN_INDEX(ITEM_BONE_HANDS)) {
        D_00413F80.weight += item_name_defs[D_00413F80.weapon + ITEM_WEAPON_FIRST].unk3E;
    }
    if (D_00413F80.equipment[EQUIP_GAUNTLET] == ITEM_WORN_INDEX(ITEM_LORD_MYUS_GLOVES)) {
        s32 item_weight = item_name_defs[D_00413F80.weapon + ITEM_WEAPON_FIRST].unk3E;

        D_00413F80.weight -= item_weight / 4;
    }
    if (D_00413F80.equipment[EQUIP_GAUNTLET] == ITEM_WORN_INDEX(ITEM_SILVERY_GLOVES)) {
        D_00413F80.unk60.values[0] += D_00413F80.unk60.values[0] / 10;
        D_00413F80.unk60.values[1] += D_00413F80.unk60.values[1] / 10;
        D_00413F80.unk60.values[2] += D_00413F80.unk60.values[2] / 10;
    }
    if (D_00413F80.equipment[EQUIP_GAUNTLET] == ITEM_WORN_INDEX(ITEM_VENOM_GLOVES)) {
        D_00413F80.unk72.values[0] += 70;
    }
    if (D_00413F80.equipment[EQUIP_RING] == ITEM_WORN_INDEX(ITEM_WIDDAS_RING)) {
        D_00413F80.hp_max += D_00413F80.hp_max / 5;
    }
    if (D_00413F80.equipment[EQUIP_RING] == ITEM_WORN_INDEX(ITEM_RING_OF_FIRE)) {
        D_00413F80.unk60.values[3] += 20;
    }
    if (D_00413F80.equipment[EQUIP_RING] == ITEM_WORN_INDEX(ITEM_RING_OF_EARTH)) {
        D_00413F80.unk60.values[4] += 20;
    }
    if (D_00413F80.equipment[EQUIP_RING] == ITEM_WORN_INDEX(ITEM_RING_OF_WIND)) {
        D_00413F80.unk60.values[5] += 20;
    }
    if (D_00413F80.equipment[EQUIP_RING] == ITEM_WORN_INDEX(ITEM_RING_OF_WATER)) {
        D_00413F80.unk60.values[6] += 20;
    }
    if (D_00413F80.equipment[EQUIP_BRACELET] == ITEM_WORN_INDEX(ITEM_POWER_BRACELET)) {
        D_00413F80.unk60.values[0] += D_00413F80.unk60.values[0] / 10;
        D_00413F80.unk60.values[1] += D_00413F80.unk60.values[1] / 10;
        D_00413F80.unk60.values[2] += D_00413F80.unk60.values[2] / 10;
    }
    if (D_00413F80.equipment[EQUIP_BRACELET] == ITEM_WORN_INDEX(ITEM_FEATHER_BRACELET)) {
        D_00413F80.weight = D_00413F80.weight * 3 / 4;
    }
    if (D_00413F80.equipment[EQUIP_HELM] == ITEM_WORN_INDEX(ITEM_ANGELIC_CROWN)) {
        D_00413F80.mp_max += D_00413F80.mp_max / 5;
    }
    if (D_00413F80.equipment[EQUIP_BRACELET] == ITEM_WORN_INDEX(ITEM_CLARITY_BRACELET)) {
        D_00413F80.mp_max *= 5;
    }
    if (D_00413F80.hp_max >= 1000) {
        D_00413F80.hp_max = 999;
    }
    if (D_00413F80.mp_max >= 1000) {
        D_00413F80.mp_max = 999;
    }
    if (D_00413F80.unk72.values[0] > 100) {
        D_00413F80.unk72.values[0] = 100;
    }
    if (D_00413F80.unk72.values[1] > 100) {
        D_00413F80.unk72.values[1] = 100;
    }
    if (D_00413F80.unk72.values[2] > 100) {
        D_00413F80.unk72.values[2] = 100;
    }
    if (D_00413F80.unk72.values[3] > 100) {
        D_00413F80.unk72.values[3] = 100;
    }
    if (D_00413F80.unk72.values[4] > 100) {
        D_00413F80.unk72.values[4] = 100;
    }
    if (D_00413F80.unk72.values[5] > 100) {
        D_00413F80.unk72.values[5] = 100;
    }
    if (D_00413F80.unkDC[0] > 100) {
        D_00413F80.unkDC[0] = 100;
    }
    if (D_00413F80.unkDC[1] > 100) {
        D_00413F80.unkDC[1] = 100;
    }
    if (D_00413F80.unkDC[2] > 100) {
        D_00413F80.unkDC[2] = 100;
    }
    if (D_00413F80.unkDC[3] > 100) {
        D_00413F80.unkDC[3] = 100;
    }
    if (D_00413F80.unkDC[4] > 100) {
        D_00413F80.unkDC[4] = 100;
    }
    if (D_00413F80.unkDC[5] > 100) {
        D_00413F80.unkDC[5] = 100;
    }
}
