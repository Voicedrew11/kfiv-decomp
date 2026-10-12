#include "common.h"

s32 func_001CFC58(s32);

extern s32 D_00446F38;

/* Address of message message_id: the offset table entry, then the text it points to. */
int func_001D07C8(int message_id) {
    s32 *offset = (s32 *)func_001CFC58(D_00446F38 + (message_id + 1) * 4);

    return func_001CFC58(D_00446F38 + *offset);
}
