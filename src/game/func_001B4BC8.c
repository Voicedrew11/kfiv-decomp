#include "kf_entity.h"
#include "kf_player.h"

extern PlayerState D_00413F80;

int rand(void);
void func_001B4DF8(int idx, int amount);

/* The player was hit for `damage`: what he wears gains experience by how much of his health that was.
   Without a shield everything gains double, and the piece the hit landed on (or one picked by chance) four times. */
void func_001B4BC8(int damage, Unk1B1470Request *request) {
    f32 percent = (f32)damage * 100.0f / (f32)D_00413F80.unk58;
    int amount = damage * 10 / D_00413F80.unk58;
    int part;
    int shield_exp;
    int helmet_exp;
    int armor_exp;
    int gloves_exp;
    int boots_exp;

    if (amount <= 0) {
        amount = 1;
    }
    // match: cleared after the amount is worked out (declared with initialisers, the divide check comes out differently)
    shield_exp = 0;
    helmet_exp = 0;
    armor_exp = 0;
    gloves_exp = 0;
    boots_exp = 0;
    if (request->unk4E != 0) {
        part = request->unk4E;
    } else {
        part = (int)(rand() * 4.0f * 4.656613e-10f) + 1;
    }
    if (!((s32)(D_00413F80.unk1C8 >> 35) & 1) && D_00413F80.equipment[0] != 0) {
        shield_exp = 1;
        if (!(percent < 5.0f)) {
            shield_exp = amount * 2;
            helmet_exp = amount;
            armor_exp = amount;
            gloves_exp = amount;
            boots_exp = amount;
        }
    } else {
        helmet_exp = amount * 2;
        if (part == 1) {
            helmet_exp = amount * 4;
        }
        armor_exp = amount * 2;
        if (part == 2) {
            armor_exp = amount * 4;
        }
        gloves_exp = amount * 2;
        if (part == 3) {
            gloves_exp = amount * 4;
        }
        boots_exp = amount * 2;
        if (part == 4) {
            boots_exp = amount * 4;
        }
    }
    if (D_00413F80.equipment[0] != 0) {
        func_001B4DF8(D_00413F80.equipment[0], shield_exp);
    }
    if (D_00413F80.equipment[1] != 0x1E) {
        func_001B4DF8(D_00413F80.equipment[1], helmet_exp);
    }
    if (D_00413F80.equipment[2] != 0x37) {
        func_001B4DF8(D_00413F80.equipment[2], armor_exp);
    }
    if (D_00413F80.equipment[3] != 0x50) {
        func_001B4DF8(D_00413F80.equipment[3], gloves_exp);
    }
    if (D_00413F80.equipment[4] != 0x69) {
        func_001B4DF8(D_00413F80.equipment[4], boots_exp);
    }
}
