#include "kf_entity.h"

extern EntityIdTable D_0043E890;

void *func_001267B0(int);

s32 func_0013C018(Entity *entity, f32 limit) {
    EntityIdTable table = D_0043E890;
    s32 i;
    Entity *other;
    EntityUnkE0 *other_unkE0;

    for (i = 0; i < 5; i++) {
        other = func_001267B0(table.ids[i]);
        if (other != NULL && (other->flags & 0xD) == 1 && other != entity) {
            other_unkE0 = &other->unkE0;
            if (other_unkE0->unk7A == 5 && other_unkE0->unk54 < limit) {
                return FALSE;
            }
        }
    }
    return TRUE;
}
