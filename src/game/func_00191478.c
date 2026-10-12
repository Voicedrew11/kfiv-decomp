#include "kf_entity.h"

void func_00191478(EntityModel *model) {
    EntityModelPart80Byte *part;
    ModelInstance *instance;
    s32 i;

    instance = (ModelInstance *)model;
    if (instance->part_count != 0) {
        i = 0;
        do {
            part = (EntityModelPart80Byte *)((i << 7) + (s32)model->unkF8);
            i += 1;
            part->unk6C = 0;
        } while (i < instance->part_count);
    }
}
