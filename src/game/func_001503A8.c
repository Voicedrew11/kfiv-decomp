#include "kf_entity.h"

void *func_001267B0(int);

s32 func_001503A8(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    s32 count = 0;
    s32 i;

    unkE0->unk6A = 0;
    for (i = 0; i < 6; i++) {
        Entity *other = func_001267B0(i + 10);
        if (other != NULL && (other->flags & 3) == 3 && ((s32)(other->flags >> 2) & 3) < 3) {
            unkE0->unk6A |= 1 << i;
            count++;
        }
    }
    unkE0->unk7A = count;
    if (unkE0->unk7A < 5) {
        if (unkE0->unk68 < 1) {
            return TRUE;
        }
    }
    return FALSE;
}
