#include "kf_entity.h"

void func_001914B0(EntityModel *model, s32 first, s32 last, s32 value) {
    EntityModelPart80Byte *part;

    if (first <= last) {
        do {
            part = (EntityModelPart80Byte *)((first << 7) + (s32)model->unkF8);
            first += 1;
            part->unk6C = value;
        } while (first <= last);
    }
}
