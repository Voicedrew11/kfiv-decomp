#include "kf_player.h"
#include "kf_effect.h"
#include "kf_message.h"

extern PlayerState D_00413F80;
extern u16 spell_experience[6][5];
extern u8 spell_levels[6][5];
extern FxTableEntry D_00402DA8[];

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
