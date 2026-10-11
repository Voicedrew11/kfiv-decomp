#include "kf_entity.h"

int func_00192EB0(void *, int);

void func_00154250(Entity *entity) {
    ENTITY_FLAG_BITS(entity)->bit34 = func_00192EB0(entity, ENTITY_FLAG_BITS(entity)->bits11);
}
