#include "kf_entity.h"

/* Returns 1 if any of the first three 0x3D8 records of the entity params is set. */
int func_00197040(Entity *entity) {
    EntityParams *params = entity->params;

    if (params->unk3D8[0].record.unk00 != 0 || ENTITY_PARAMS_UNK3D8(params, 1)->unk00 != 0 || ENTITY_PARAMS_UNK3D8(params, 2)->unk00 != 0) {
        return 1;
    }
    return 0;
}
