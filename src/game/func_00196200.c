#include "common.h"
#include "kf_entity.h"

int Bone_TurnPartToLook(void *, int, int, int, void *, int, float);

/* Forwards to Bone_TurnPartToLook when the entity model's unk134 is set. */
int func_00196200(Entity *entity, int value_a, int value_b, void *value_c, int value_d, float scale) {
    EntityModel *model = entity->model;

    if (model->unk134 == 0) {
        return 0;
    }
    return Bone_TurnPartToLook(model, model->unk134, value_a, value_b, value_c, value_d, scale);
}
