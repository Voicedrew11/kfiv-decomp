#include "common.h"
#include "kf_message.h"

extern s32 D_00446F38;

/* Address of message message_id: the offset table entry, then the text it points to. */
int func_001D07C8(int message_id) {
    s32 *offset = (s32 *)Message_GetFontText(D_00446F38 + (message_id + 1) * 4);

    return Message_GetFontText(D_00446F38 + *offset);
}
