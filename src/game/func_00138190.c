#include "kf_entity.h"

int func_00192C98(void *, int, int, u32, int);

void func_00138190(Entity *entity) {
    ENTITY_FLAG_BITS(entity)->bit22 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit23 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit24 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit25 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit21 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit28 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit16 = TRUE;
    func_00192C98(entity, 1, 0, 0, 0);
}
