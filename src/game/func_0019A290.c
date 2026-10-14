#include "kf_entity.h"
#include "kf_player.h"

extern PlayerState D_00413F80;
extern u8 item_counts[ITEM_COUNT];

/* Sets the entity's byte at 0xA2 from base and a delay worked out from the player's values and the healing items carried. */
void func_0019A290(Entity *entity, s32 base) {
    s32 delay;

    delay = (D_00413F80.hp_now * 100) / D_00413F80.hp_max;
    delay = (item_counts[ITEM_HERB] == 0) ? delay : delay + 5;
    delay = (item_counts[ITEM_HERBAL_LIQUID] != 0) ? delay + 0xF : delay;
    delay = (item_counts[ITEM_FOREST_FRUIT] != 0) ? delay + 0x1E : delay;
    delay = (item_counts[ITEM_DRAGON_FLOWER] != 0) ? delay + 0x32 : delay;
    entity->unkA2 = base + (delay * 0x1E) / 200;
}
