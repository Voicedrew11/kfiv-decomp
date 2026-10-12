#include "common.h"
#include "kf_entity.h"

int func_0018E3D0(void *, int, int, int, void *, int, float);

/* Forwards to func_0018E3D0 when the entity model's unk134 is set. */
int func_00196200(Entity *entity, int value_a, int value_b, void *value_c, int value_d, float scale) {
    EntityModel *model = entity->model;

    if (model->unk134 == 0) {
        return 0;
    }
    return func_0018E3D0(model, model->unk134, value_a, value_b, value_c, value_d, scale);
}
