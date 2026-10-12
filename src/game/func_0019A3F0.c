#include "kf_entity.h"

u16 func_0019A418(ModelInstance *model, int table_index);

int func_0019A3F0(Entity *entity, s32 index) {
    return func_0019A418(ENTITY_MODEL(entity), entity->params->unk60[index]);
}
