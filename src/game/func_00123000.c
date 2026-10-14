#include "common.h"
#include "kf_message.h"
#include "kf_item.h"

/* Shows "... Key used" when the item a door or chest needed is one of the keys. */
void MapObject_ShowKeyUsed(int item) {
    if (item >= ITEM_KEY_FIRST && item <= ITEM_KEY_LAST) {
        Message_ShowNotice(0, 0, 0, 0, 0x2D, item + (MSG_KEY_USED_FIRST - ITEM_KEY_FIRST));
    }
}
