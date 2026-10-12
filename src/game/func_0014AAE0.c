#include "kf_entity.h"

int func_00192C98(void *, int, int, u32, int);

void func_0014AAE0(Entity *entity) {
    if (entity->unkC8 & 1) {
        func_00192C98(entity, 3, 0, entity->params->unkC4, 0);
    } else {
        func_00192C98(entity, 0, 0, entity->params->unkC4, 0);
    }
}
