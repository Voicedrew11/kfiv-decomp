#include "kf_entity.h"

int func_00192C98(void *, int, int, u32, int);

void func_0017A4F8(Entity *entity) {
    func_00192C98(entity, 0, 0, entity->params->unkC4, 0);
    ENTITY_FLAG_BITS(entity)->bits11 = 0;
}
