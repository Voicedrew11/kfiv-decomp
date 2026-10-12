#include "kf_entity.h"
#include "kf_player.h"

extern PlayerState D_00413F80;
extern Entity D_003BEDF0[][0x80];
extern Entity D_003D6C70;           /* the last entity of D_003BEDF0 */
extern char D_0043F210[];           /* "g_dmgLassBossCnt[%d]
" */
extern char D_0043F228[];           /* "HP halved!! %d / 2 = %d" in Japanese */
extern s32 D_00446928;              /* g_dmgLassBossCnt: weapon hits the last boss still takes */

int printf(const char *, ...);
int rand(void);
float atan2f(float, float);
void func_00216778(void *, void *);
void func_001B3ED8(EntityParamsUnk240PartUnk00 *, Unk1B1470Request *, Unk1B1470Extra *);
int func_001B49F8(int, int, int, int);
void func_001B3908(Entity *, EntityParamsUnk240PartUnk12 *);
void func_001B4B10(int, int);
void func_001BB260(int);
void func_001BB468(Vector4 *, f32);
void func_001BBC88(int);
void func_001BBEE0(int);
void func_001BBFC8(void);
void func_001BC058(void);
s32 func_0019D0F0(s32);

#define IS_PLAYER(owner) ((owner) == (Entity *)&D_00413F80)
/* rand() scaled to 0-99 (the factor is 2 to the power -31) */
#define ROLL_PERCENT() ((s32)(rand() * 100.0f * 0.0000000004656613f))

#define ENTITY_TYPE_LAST_BOSS 99

/* A hit lands on an entity: work out the damage from the attack's nine strengths and the
   entity's nine resistances, take it off the entity's health and start its reaction.
   The last boss takes no damage here: a weapon hit counts D_00446928 down, throws the
   player back and halves the player's health (PlayerState.unkB4). */
void func_001B2F70(Unk1B1470Request *request, Unk1B1470Extra *extra, Entity *entity, f32 scale) {
    Vector4 position;
    EntityParamsUnk240PartUnk00 attack;
    Entity *owner;
    s32 by_weapon;
    s32 by_magic;
    s32 damage;
    s32 physical_bonus;
    s32 magic_bonus;
    s32 chance;
    f32 percent;

    owner = request->owner;
    by_weapon = FALSE;
    if (IS_PLAYER(owner)) {
        by_weapon = extra->unk00 == 0;
    }
    by_magic = FALSE;
    if (IS_PLAYER(owner)) {
        by_magic = extra->unk00 == 1;
    }

    if (entity->spawn_info->type_id == ENTITY_TYPE_LAST_BOSS) {
        if (!IS_PLAYER(owner)) {
            return;
        }
        if (entity->unk80 != 0) {
            return;
        }
        entity->flags |= 0x8000L << 28;
        entity->unk80 = 15;
        printf(D_0043F210, D_00446928);
        if (!by_weapon) {
            return;
        }
        if (extra->unk04 != 0) {
            return;
        }
        if (--D_00446928 <= 0) {
            return;
        }
        func_001B4B10(extra->unk24, 4);
        func_001BC058();
        func_00216778(&position, owner);
        if (D_00413F80.position.x < entity->model->position[0]) {
            position.x += 100.0f;
            position.z += 100.0f;
        } else {
            position.x -= 100.0f;
            position.z += 100.0f;
        }
        func_001BB468(&position, 200.0f);
        if (D_00413F80.unkB4 < 2) {
            return;
        }
        printf(D_0043F228, D_00413F80.unkB4, D_00413F80.unkB4 / 2);
        func_001BB260(D_00413F80.unkB4 / 2);
        return;
    }

    // match: the range test (not one of the entities) is always true for the player
    if (IS_PLAYER(owner) && (owner < D_003BEDF0[0] || owner > &D_003D6C70)) {
        entity->flags |= 0x8000L << 28;
    }
    if (extra->unk00 == 1) {
        entity->unk82 = request->unk4F;
    }
    if ((s32)(entity->flags >> 18) & 1) {
        return;
    }
    if (!((s32)(entity->flags >> 1) & 1)) {
        return;
    }
    if (entity->unk2C == 0) {
        return;
    }
    if (entity->unk80 != 0) {
        return;
    }

    func_001B3ED8(&attack, request, extra);
    if (ENTITY_FLAG_BITS(entity)->bit35) {
        physical_bonus = entity->params->unkD0;
        magic_bonus = entity->params->unkD1;
    } else {
        physical_bonus = 0;
        magic_bonus = 0;
    }

    damage = 0;
    if (entity->params->unkD4 < 100) {
        damage = func_001B49F8(attack.unk00[0], request->unk50.unk00[0], extra->unk00 == 0 ? extra->unk20 : extra->unk22,
                               entity->params->unkE6 + physical_bonus + entity->params->unkD4);
    }
    if (entity->params->unkD6 < 100) {
        damage += func_001B49F8(attack.unk00[1], request->unk50.unk00[1], extra->unk00 == 0 ? extra->unk20 : extra->unk22,
                                entity->params->unkE6 + physical_bonus + entity->params->unkD6);
    }
    if (entity->params->unkD8 < 100) {
        damage += func_001B49F8(attack.unk00[2], request->unk50.unk00[2], extra->unk00 == 0 ? extra->unk20 : extra->unk22,
                                entity->params->unkE6 + physical_bonus + entity->params->unkD8);
    }
    if (entity->params->unkDA < 100) {
        damage += func_001B49F8(attack.unk00[3], request->unk50.unk00[3], extra->unk22,
                                entity->params->unkE8 + magic_bonus + entity->params->unkDA);
    }
    if (entity->params->unkDC < 100) {
        damage += func_001B49F8(attack.unk00[4], request->unk50.unk00[4], extra->unk22,
                                entity->params->unkE8 + magic_bonus + entity->params->unkDC);
    }
    if (entity->params->unkDE < 100) {
        damage += func_001B49F8(attack.unk00[5], request->unk50.unk00[5], extra->unk22,
                                entity->params->unkE8 + magic_bonus + entity->params->unkDE);
    }
    if (entity->params->unkE0 < 100) {
        damage += func_001B49F8(attack.unk00[6], request->unk50.unk00[6], extra->unk22,
                                entity->params->unkE8 + magic_bonus + entity->params->unkE0);
    }
    if (entity->params->unkE2 < 100) {
        damage += func_001B49F8(attack.unk00[7], request->unk50.unk00[7], extra->unk22,
                                entity->params->unkE8 + magic_bonus + entity->params->unkE2);
    }
    if (entity->params->unkE4 < 100) {
        damage += func_001B49F8(attack.unk00[8], request->unk50.unk00[8], extra->unk22,
                                entity->params->unkE8 + magic_bonus + entity->params->unkE4);
    }

    if (by_weapon) {
        damage = damage * extra->unk02 / 5000;
    }
    damage *= scale;

    if (by_weapon) {
        if (D_00413F80.unk3C == 0x11) {
            if ((entity->spawn_info->type_id >= 0x32 && entity->spawn_info->type_id <= 0x42) ||
                (entity->spawn_info->type_id >= 0x5C && entity->spawn_info->type_id <= 0x61) ||
                entity->spawn_info->type_id == 0x2F || entity->spawn_info->type_id == 0xA4 ||
                entity->spawn_info->type_id == 0xA7 || entity->spawn_info->type_id == 0xA8) {
                damage *= 2;
            }
        }
    }
    if (by_weapon && request->unk6F != 0) {
        chance = request->unk6F - entity->params->unkC9;
        if (ROLL_PERCENT() < chance) {
            damage *= 2;
        }
    }

    if (damage > 0) {
        if (by_weapon) {
            func_001BBEE0(extra->unk24);
        }
        if (by_magic && extra->unk1C == 0) {
            func_001BBC88(extra->unk26);
            extra->unk1C = 1;
        }
    }

    if (by_weapon && extra->unk04 == 0) {
        percent = damage * 100.0f / entity->params->unk0A;
        if (damage <= 0) {
            func_001B4B10(extra->unk24, 4);
            func_001BBFC8();
            return;
        }
        if (percent < 10.0f) {
            func_001B4B10(extra->unk24, 2);
        } else if (percent < 100.0f) {
            func_001B4B10(extra->unk24, 1);
        }
    }

    if (damage <= 0) {
        return;
    }
    if (damage < entity->unk2C) {
        entity->unk2C -= damage;
    } else {
        entity->unk2C = 0;
    }
    entity->unk2E = damage;
    entity->unk80 = 15;

    percent = damage * 100.0f / entity->params->unk0A;
    ENTITY_FLAG_HIT_BITS(entity)->hit_strength = percent < 33.3334f ? 1 : 2;
    ENTITY_FLAG_HIT_BITS(entity)->hit_source = by_weapon ? 1 : by_magic ? 2 : 0; // match: a shift of by_magic in retail too

    // match: three stores to unk6C, one per arm, give retail's single shared call and store
    if (by_weapon && extra->unk04 == 0) {
        entity->unk6C = request->unk38;
    } else if (request->position[0] != request->unk10[0] || request->position[1] != request->unk10[1] ||
               request->position[2] != request->unk10[2]) {
        entity->unk6C = atan2f(request->position[0] - request->unk10[0], request->position[2] - request->unk10[2]);
    } else {
        entity->unk6C = atan2f(entity->model->position[0] - request->position[0], entity->model->position[2] - request->position[2]);
    }

    if (by_weapon || by_magic) {
        if (D_00413F80.unk100 != 0 && D_00413F80.unkB4 > 0) {
            D_00413F80.unkB4 += damage * D_00413F80.unk100 / 100;
            if (D_00413F80.unk58 < D_00413F80.unkB4) {
                D_00413F80.unkB4 = D_00413F80.unk58;
            }
        }
        if (D_00413F80.unk104 != 0) {
            D_00413F80.unkB8 += damage * D_00413F80.unk104 / 100;
            if (D_00413F80.unk5C < D_00413F80.unkB8) {
                D_00413F80.unkB8 = D_00413F80.unk5C;
            }
        }
    }

    func_001B3908(entity, &request->unk62);
    // match: flag bit 40 (still named pad40); the source bit is read as a u8 field
    ENTITY_FLAG_BITS(entity)->pad40 = ((Unk1B1470RequestBits *)request)->unk4C_3;

    if (by_magic && request->unk70 != 0 && damage > 0) {
        chance = request->unk70 - entity->params->unkC9;
        if (ROLL_PERCENT() < chance) {
            entity->unk2C = 0;
        }
    }

    if (by_weapon && extra->unk04 == 0) {
        if (attack.unk00[0] >= attack.unk00[1] && attack.unk00[0] >= attack.unk00[2]) {
            func_0019D0F0(0x32);
        } else if (attack.unk00[1] >= attack.unk00[2]) {
            func_0019D0F0(0x33);
        } else {
            func_0019D0F0(0x34);
        }
    }
}
