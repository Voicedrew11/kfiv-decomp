#include "kf_entity.h"

void func_001958B8(char *, void (*)(void), int);
void func_0017A260(Entity *);

extern char D_00349690[0x2000];

void func_0017A1E8(Entity *entity, s32 unknown) {
    EntityUnkE0Kind1740A8 *unkE0 = (EntityUnkE0Kind1740A8 *)&entity->unkE0;

    if (entity->unkC6 == 0) {
        s32 index = entity->params->unk432;

        index += entity->params->unk430 * 64;
        if (((u8 *)D_00349690)[index] != 0) {
            func_001958B8((char *)entity, func_0017A260, unknown);
            return;
        }
        func_001958B8((char *)entity, unkE0->unk18, unknown);
    }
}
