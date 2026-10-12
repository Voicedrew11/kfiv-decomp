#include "kf_entity.h"
#include "kf_player.h"

extern PlayerState D_00413F80;
extern u8 D_00369830[380];

/* Sets the entity's byte at 0xA2 from base and a delay worked out from the player's values and the flags at D_00369830. */
void func_0019A290(Entity *entity, s32 base) {
    s32 delay;

    delay = (D_00413F80.unkB4 * 100) / D_00413F80.unk58;
    delay = (D_00369830[6] == 0) ? delay : delay + 5;
    delay = (D_00369830[7] != 0) ? delay + 0xF : delay;
    delay = (D_00369830[8] != 0) ? delay + 0x1E : delay;
    delay = (D_00369830[0xD] != 0) ? delay + 0x32 : delay;
    entity->unkA2 = base + (delay * 0x1E) / 200;
}
