#include "kf_entity.h"

int Bone_TurnPartToLook(void *, int, int, int, void *, int, float);

int func_001960C0(Entity *entity, int angles, int mode, void *target, int limits, float rate) {
    EntityModel *model = entity->model;
    int part = (int)model->unk138;

    if (part == 0 || !(entity->params->flags & 0x2000)) {
        return 0;
    }
    return Bone_TurnPartToLook(model, part, angles, mode, target, limits, rate);
}
