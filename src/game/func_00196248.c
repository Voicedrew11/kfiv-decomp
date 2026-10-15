#include "kf_entity.h"
#include "kf_player.h"

extern PlayerState D_00413F80;

void Vector_Copy(void *, void *);
int Bone_TurnPartToLook(void *, int, int, int, void *, int, float);

int func_00196248(Entity *entity, int angles, int mode, int limits, float rate) {
    f32 target[2]; // match: the frame has room for two floats only, though Vector_Copy writes four
    EntityModel *model;

    Vector_Copy(target, &D_00413F80);
    target[1] -= 115.0f;
    model = entity->model;
    if (model->unk134 == 0) {
        return 0;
    }
    return Bone_TurnPartToLook(model, model->unk134, angles, mode, target, limits, rate);
}
