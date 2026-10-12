#include "kf_entity.h"
#include "kf_map.h"
#include "kf_player.h"

extern PlayerState D_00413F80;
extern MapSlot D_0038CD20[];

void func_001B3ED8(EntityParamsUnk240PartUnk00 *, Unk1B1470Request *, Unk1B1470Extra *);
int func_001B49F8(int, int, int, int);
void func_001B4B10(int, int);
void func_001BBFC8(void);
void func_001BC0E0(int with_sound);
void func_0011E228(MapObject *object);

#define IS_PLAYER(owner) ((owner) == (Entity *)&D_00413F80)

/* A hit lands on a map object: one that can break does when the damage reaches its toughness (a map's own
   hit always breaks it); otherwise the player's weapon bounces off. */
void func_001B3AC0(Unk1B1470Request *request, Unk1B1470Extra *extra, MapObject *object) {
    EntityParamsUnk240PartUnk00 attack;
    s32 by_weapon;
    s32 damage;

    if (object->unk08->unk30 == 0x201) {
        if (!IS_PLAYER(request->owner)) {
            return;
        }
        if (extra->unk00 != 0) {
            return;
        }
        if (extra->unk04 != 0) {
            return;
        }
        if (D_00413F80.unk3C == 0x2E) {
            func_0011E228(object);
            return;
        }
        func_001B4B10(extra->unk24, 4);
        func_001BBFC8();
        return;
    }
    if (request->owner == (Entity *)&D_0038CD20[0] || request->owner == (Entity *)&D_0038CD20[1]) {
        func_0011E228(object);
        return;
    }
    if (!IS_PLAYER(request->owner)) {
        return;
    }
    by_weapon = extra->unk00 == 0;
    if (!(MAP_OBJECT_DEF_WORD(object) & 0x800)) {
        if (extra->unk00 != 0) {
            return;
        }
        if (extra->unk04 != 0) {
            return;
        }
        func_001B4B10(extra->unk24, 4);
        func_001BBFC8();
        return;
    }

    func_001B3ED8(&attack, request, extra);
    damage = 0;
    if (object->unk04->unk52[0] < 200) {
        damage = func_001B49F8(attack.unk00[0], request->unk50.unk00[0], extra->unk00 == 0 ? extra->unk20 : extra->unk22,
                               object->unk04->unk52[0]);
    }
    if (object->unk04->unk52[1] < 200) {
        damage += func_001B49F8(attack.unk00[1], request->unk50.unk00[1], extra->unk00 == 0 ? extra->unk20 : extra->unk22,
                                object->unk04->unk52[1]);
    }
    if (object->unk04->unk52[2] < 200) {
        damage += func_001B49F8(attack.unk00[2], request->unk50.unk00[2], extra->unk00 == 0 ? extra->unk20 : extra->unk22,
                                object->unk04->unk52[2]);
    }
    if (object->unk04->unk52[3] < 200) {
        damage += func_001B49F8(attack.unk00[3], request->unk50.unk00[3], extra->unk22, object->unk04->unk52[3]);
    }
    if (object->unk04->unk52[4] < 200) {
        damage += func_001B49F8(attack.unk00[4], request->unk50.unk00[4], extra->unk22, object->unk04->unk52[4]);
    }
    if (object->unk04->unk52[5] < 200) {
        damage += func_001B49F8(attack.unk00[5], request->unk50.unk00[5], extra->unk22, object->unk04->unk52[5]);
    }
    if (object->unk04->unk52[6] < 200) {
        damage += func_001B49F8(attack.unk00[6], request->unk50.unk00[6], extra->unk22, object->unk04->unk52[6]);
    }
    if (object->unk04->unk52[7] < 200) {
        damage += func_001B49F8(attack.unk00[7], request->unk50.unk00[7], extra->unk22, object->unk04->unk52[7]);
    }
    if (object->unk04->unk52[8] < 200) {
        damage += func_001B49F8(attack.unk00[8], request->unk50.unk00[8], extra->unk22, object->unk04->unk52[8]);
    }

    if (by_weapon) {
        damage = damage * extra->unk02 / 5000;
    }
    if ((MAP_OBJECT_DEF_WORD(object) & 0x2000) && by_weapon) {
        damage *= request->unk6E;
    }
    if (damage >= MAP_OBJECT_DEF_TOUGHNESS(object->unk04)) {
        func_0011E228(object);
        return;
    }
    if (D_00413F80.unk262 != 0) {
        return;
    }
    if (extra->unk00 != 0) {
        return;
    }
    if (extra->unk04 != 0) {
        return;
    }
    func_001B4B10(extra->unk24, 4);
    func_001BC0E0(object->unk08->unk30 != 0x1C && object->unk08->unk30 != 0xFF);
}
