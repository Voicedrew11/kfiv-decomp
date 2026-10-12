#include "kf_entity.h"
#include "kf_game.h"
#include "kf_player.h"

extern PlayerState D_00413F80;
extern GameStatus D_0038CC50;
extern Entity D_003BEDF0[][0x80];

void func_001B2C48(void *);
void func_001B3ED8(EntityParamsUnk240PartUnk00 *, Unk1B1470Request *, Unk1B1470Extra *);
int func_001B4AA8(int, int, int);
void func_001B4BC8(int, Unk1B1470Request *);
void func_001BB260(int);
void func_001BB390(void *, int);
void func_001BB4D0(int);
void func_001BD6E0(void);

/* A hit lands on the player: marks the attacking entity, works out the damage from the
   attack's nine strengths against the player's nine defences, takes it off the player's
   health and starts the ailments and the reaction. */
void func_001B29C8(Unk1B1470Request *request, Unk1B1470Extra *extra) {
    EntityParamsUnk240PartUnk00 attack;
    Entity *owner = request->owner;
    s32 blocked;
    s32 percent;
    s32 damage;

    if (owner >= D_003BEDF0[0] && owner < (Entity *)D_003D6DF0) {
        if (extra->unk00 == 0) {
            owner->flags |= 0x8000L << 26;
        } else {
            owner->flags |= 0x8000L << 27;
        }
    }
    if (D_00413F80.unkB4 == 0) {
        return;
    }
    if (D_0038CC50.unk00 == 0) {
        return;
    }
    if (D_0038CC50.unk30 & 1) {
        return;
    }
    blocked = request->unk72;
    if (blocked != 0) {
        return;
    }
    percent = 100;
    if ((u8)(D_00413F80.equipment[0] - 1) <= 1) {
        percent = 120;
    }
    if ((u8)(D_00413F80.equipment[4] - 0x6A) <= 1) {
        percent += 20;
    }
    func_001B3ED8(&attack, request, extra); // match: attack is filled here and never read; the terms below use request->unk50
    damage = func_001B4AA8(request->unk50.unk00[0], D_00413F80.unk90.values[1], D_00413F80.unkAC); // match: values[1] here and on the next line; values[0] is never used
    damage += func_001B4AA8(request->unk50.unk00[1], D_00413F80.unk90.values[1], D_00413F80.unkAC); // match: values[1] a second time
    damage += func_001B4AA8(request->unk50.unk00[2], D_00413F80.unk90.values[2], D_00413F80.unkAC);
    damage += func_001B4AA8(request->unk50.unk00[3], D_00413F80.unk90.values[3], D_00413F80.unkB0) * percent / 100;
    damage += func_001B4AA8(request->unk50.unk00[4], D_00413F80.unk90.values[4], D_00413F80.unkB0);
    damage += func_001B4AA8(request->unk50.unk00[5], D_00413F80.unk90.values[5], D_00413F80.unkB0);
    damage += func_001B4AA8(request->unk50.unk00[6], D_00413F80.unk90.values[6], D_00413F80.unkB0);
    damage += func_001B4AA8(request->unk50.unk00[7], D_00413F80.unk90.values[7], D_00413F80.unkB0);
    damage += func_001B4AA8(request->unk50.unk00[8], D_00413F80.unk90.values[8], D_00413F80.unkB0);
    request->unk74 = 1;
    if (damage <= blocked) { // match: blocked is request->unk72, which is known to be 0 here
        damage = 1;
    }
    func_001BB260(damage);
    func_001B2C48(&request->unk62);
    if (request->unk71 != 0) {
        if (request->unk71 < D_00413F80.unkB8) {
            D_00413F80.unkB8 -= request->unk71;
        } else {
            D_00413F80.unkB8 = 0;
        }
    }
    if (UNK1B1470_REQUEST_UNIT(request) & UNK1B1470_UNIT_UNK4C_3) {
        func_001BB390(request, damage);
    }
    func_001BB4D0(damage);
    func_001B4BC8(damage, request);
    func_001BD6E0();
}
