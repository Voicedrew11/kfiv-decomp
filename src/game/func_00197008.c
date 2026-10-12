#include "kf_entity.h"

/* Nonzero while any of the three entity-params flags is set. */
int func_00197008(Entity *entity) {
    EntityParams *params = entity->params;

    if (params->unk240[0].unk00 != 0 || params->unk240[1].unk00 != 0 || (u8)params->unk350[0] != 0) {
        return 1;
    }
    return 0;
}
