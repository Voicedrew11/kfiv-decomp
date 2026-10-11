#include "kf_entity.h"

void func_001958B8(char *, void (*)(void), int);
void func_0017BA98(Entity *);

extern float D_00413F80[];

void func_0017B958(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    if (entity->unkC6 == 0) {
        if ((-6200.0f <= D_00413F80[0] && D_00413F80[0] <= -1400.0f &&
             -50.0f <= D_00413F80[1] && D_00413F80[1] <= 450.0f &&
             -4200.0f <= D_00413F80[2] && D_00413F80[2] <= -600.0f) ||
            (-6200.0f <= D_00413F80[0] && D_00413F80[0] <= -2600.0f &&
             -50.0f <= D_00413F80[1] && D_00413F80[1] <= 450.0f &&
             -600.0f <= D_00413F80[2] && D_00413F80[2] <= 1000.0f)) {
            func_001958B8((char *)entity, func_0017BA98, unknown);
        } else {
            func_001958B8((char *)entity, (void (*)(void))unkE0->unk18, unknown);
        }
    }
}
