#include "kf_player.h"

extern PlayerState D_00413F80;
extern LevelDef D_004147F8[0x17C];
extern PlayerItemDef D_0041B9B8[];
extern LevelRecord D_003679A0[70];

void *memset(void *, s32, u32);
void func_001BAE88(void);
void func_001BB728(u16 *, u32);

/* one of the nine values: the item's row for its level plus what it gained, scaled by LevelRecord.level in percent */
#define SET_SCALED(n)     D_00413F80.unk60.values[n] = (item_def->unk00[record->unk14].values[n] + record->unk00[n]) * record->level / 100

/* Works the player's derived numbers out again from the base ones, what is worn and the magic in effect. */
void func_001BA310(void) {
    PlayerItemDef *item_def = &D_0041B9B8[D_00413F80.unk3C];
    LevelRecord *record = &D_003679A0[D_00413F80.unk3C];
    s32 unused_third;
    s32 unused_fifth;
    s32 unused_fifteenth;

    if (D_00413F80.unk3D != 0) {
        // match: three divisions whose results are never used; only their divide checks are left in retail,
        // so what was divided is not known
        unused_third = D_00413F80.unk20 / 3;
        unused_fifth = D_00413F80.unk20 / 5;
        unused_fifteenth = D_00413F80.unk20 / 15;
    }
    D_00413F80.unk58 = D_00413F80.unk20;
    D_00413F80.unk5C = D_00413F80.unk24;
    D_00413F80.unkAC = D_00413F80.unk28;
    D_00413F80.unkB0 = D_00413F80.unk2C;
    func_001BAE88();
    D_00413F80.unkA8 = D_00413F80.unkAC * 3 + 80;
    D_00413F80.unkA8 = (f32)D_00413F80.unkA8 * 2.2046;
    D_00413F80.unkA4 = D_004147F8[D_00413F80.unk3C + 150].unk3E;
    D_00413F80.unkA4 += D_004147F8[D_00413F80.equipment[0] + 220].unk3E;
    D_00413F80.unkA4 += D_004147F8[D_00413F80.equipment[1] + 220].unk3E;
    D_00413F80.unkA4 += D_004147F8[D_00413F80.equipment[2] + 220].unk3E;
    D_00413F80.unkA4 += D_004147F8[D_00413F80.equipment[3] + 220].unk3E;
    D_00413F80.unkA4 += D_004147F8[D_00413F80.equipment[4] + 220].unk3E;
    if (D_00413F80.unk3C == 0x2B && D_00413F80.equipment[0] == 0x19 && D_00413F80.equipment[1] == 0x35 && D_00413F80.equipment[2] == 0x4D
        && D_00413F80.equipment[3] == 0x66 && D_00413F80.equipment[4] == 0x80) {
        D_00413F80.unkA4 /= 3;
    }
    D_00413F80.unkA4 += D_004147F8[D_00413F80.equipment[5] + 220].unk3E;
    D_00413F80.unkA4 += D_004147F8[D_00413F80.equipment[6] + 220].unk3E;
    D_00413F80.unkA4 += D_004147F8[D_00413F80.equipment[7] + 220].unk3E;
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
    func_001BB728((u16 *)D_00413F80.unk7E.values, D_00413F80.equipment[0]);
    func_001BB728((u16 *)D_00413F80.unk7E.values, D_00413F80.equipment[1]);
    func_001BB728((u16 *)D_00413F80.unk7E.values, D_00413F80.equipment[2]);
    func_001BB728((u16 *)D_00413F80.unk7E.values, D_00413F80.equipment[3]);
    func_001BB728((u16 *)D_00413F80.unk7E.values, D_00413F80.equipment[4]);
    func_001BB728((u16 *)D_00413F80.unk7E.values, D_00413F80.equipment[5]);
    func_001BB728((u16 *)D_00413F80.unk7E.values, D_00413F80.equipment[6]);
    func_001BB728((u16 *)D_00413F80.unk7E.values, D_00413F80.equipment[7]);
    memset(&D_00413F80.unk90, 0, sizeof(PlayerShotStats));
    func_001BB728((u16 *)D_00413F80.unk90.values, D_00413F80.equipment[0]);
    func_001BB728((u16 *)D_00413F80.unk90.values, D_00413F80.equipment[1]);
    func_001BB728((u16 *)D_00413F80.unk90.values, D_00413F80.equipment[2]);
    func_001BB728((u16 *)D_00413F80.unk90.values, D_00413F80.equipment[3]);
    func_001BB728((u16 *)D_00413F80.unk90.values, D_00413F80.equipment[4]);
    func_001BB728((u16 *)D_00413F80.unk90.values, D_00413F80.equipment[5]);
    func_001BB728((u16 *)D_00413F80.unk90.values, D_00413F80.equipment[6]);
    func_001BB728((u16 *)D_00413F80.unk90.values, D_00413F80.equipment[7]);
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
        D_00413F80.unkAC /= 2;
        D_00413F80.unkB0 /= 2;
    }
    if (D_00413F80.unk2EC > 0) {
        D_00413F80.unkAC += D_00413F80.unk2F0;
    }
    if (D_00413F80.unk19C != 0) {
        D_00413F80.unkAC += D_00413F80.unkAC / 10;
    }
    if (D_00413F80.unk1A0 != 0) {
        D_00413F80.unkB0 += D_00413F80.unkB0 / 10;
    }
    if (D_00413F80.unk2EC > 0) {
        D_00413F80.unkB0 = 1;
    }
    if (D_00413F80.equipment[3] == 0x5D) {
        s32 item_weight = D_004147F8[D_00413F80.unk3C + 150].unk3E;

        D_00413F80.unkA4 -= item_weight / 4;
    }
    if (D_00413F80.equipment[3] == 0x5E) {
        D_00413F80.unkA4 += D_004147F8[D_00413F80.unk3C + 150].unk3E;
    }
    if (D_00413F80.equipment[3] == 0x67) {
        s32 item_weight = D_004147F8[D_00413F80.unk3C + 150].unk3E;

        D_00413F80.unkA4 -= item_weight / 4;
    }
    if (D_00413F80.equipment[3] == 0x63) {
        D_00413F80.unk60.values[0] += D_00413F80.unk60.values[0] / 10;
        D_00413F80.unk60.values[1] += D_00413F80.unk60.values[1] / 10;
        D_00413F80.unk60.values[2] += D_00413F80.unk60.values[2] / 10;
    }
    if (D_00413F80.equipment[3] == 0x64) {
        D_00413F80.unk72.values[0] += 70;
    }
    if (D_00413F80.equipment[5] == 0x85) {
        D_00413F80.unk58 += D_00413F80.unk58 / 5;
    }
    if (D_00413F80.equipment[5] == 0x87) {
        D_00413F80.unk60.values[3] += 20;
    }
    if (D_00413F80.equipment[5] == 0x88) {
        D_00413F80.unk60.values[4] += 20;
    }
    if (D_00413F80.equipment[5] == 0x89) {
        D_00413F80.unk60.values[5] += 20;
    }
    if (D_00413F80.equipment[5] == 0x8A) {
        D_00413F80.unk60.values[6] += 20;
    }
    if (D_00413F80.equipment[6] == 0x8F) {
        D_00413F80.unk60.values[0] += D_00413F80.unk60.values[0] / 10;
        D_00413F80.unk60.values[1] += D_00413F80.unk60.values[1] / 10;
        D_00413F80.unk60.values[2] += D_00413F80.unk60.values[2] / 10;
    }
    if (D_00413F80.equipment[6] == 0x90) {
        D_00413F80.unkA4 = D_00413F80.unkA4 * 3 / 4;
    }
    if (D_00413F80.equipment[1] == 0x30) {
        D_00413F80.unk5C += D_00413F80.unk5C / 5;
    }
    if (D_00413F80.equipment[6] == 0x8D) {
        D_00413F80.unk5C *= 5;
    }
    if (D_00413F80.unk58 >= 1000) {
        D_00413F80.unk58 = 999;
    }
    if (D_00413F80.unk5C >= 1000) {
        D_00413F80.unk5C = 999;
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
