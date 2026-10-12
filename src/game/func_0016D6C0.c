#include "kf_entity.h"

extern EntityActionWeights3 D_0043EA88;
extern PlayerData D_00413F80;

int rand(void);
int func_001958B8(Entity *, void (*)(Entity *), int);
void func_0016D838(Entity *);
void func_0016DCC0(Entity *);
void func_0016DE50(Entity *);

s32 func_0016D6C0(Entity *entity) {
    EntityActionWeights3 chances = D_0043EA88;
    s32 choice;
    s32 total;
    s32 i;
    s32 roll;
    s32 sum;

    if (entity->unkE0.unk6A > 0) {
        return FALSE;
    }
    if (D_00413F80.position[0] < -16900.0f || D_00413F80.position[0] > -16500.0f || D_00413F80.position[2] < 2500.0f
        || D_00413F80.position[2] > 3600.0f) {
        chances.weights[2] = 90;
    }
    total = chances.weights[0] + chances.weights[1] + chances.weights[2];
    roll = rand() % total;
    choice = 0;
    sum = 0;
    for (i = 0; i < 3; i++) {
        sum += chances.weights[i];
        if (roll < sum) {
            choice = i;
            break;
        }
    }
    if (choice == 0) {
        func_001958B8((char *)entity, (void (*)(void))func_0016DE50, 1);
    } else if (choice == 1) {
        func_001958B8((char *)entity, (void (*)(void))func_0016DCC0, 1);
    } else {
        func_001958B8((char *)entity, (void (*)(void))func_0016D838, 1);
    }
    return TRUE;
}
