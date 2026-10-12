#include "kf_entity.h"

int rand(void);
float cosf(float);
float sinf(float);
int func_001CB0D0(Vector4 *, Vector4 *, int, int, int, int, int);

/* Drops the gold of an entity near it: half of EntityParams.gold for sure, the rest by chance. */
int func_001CB2C8(Entity *entity) {
    Vector4 position;
    Vector4 rotation;
    u16 amount;
    s32 whole;
    s32 half;
    s32 sure;
    f32 distance;
    f32 angle;

    if (entity->params->gold == 0) {
        return; // match: no value, though the function returns what func_001CB0D0 does (retail keeps $2 live here)
    }
    amount = 1;
    if (entity->params->gold >= 2) {
        whole = entity->params->gold;
        half = whole / 2;
        sure = whole - half;
        amount = (s32)((half + 1) * (f32)rand() * 4.656613e-10f) + sure;
    }
    distance = entity->radius * rand() * 4.656613e-10f;
    angle = rand() * 6.2831855f * 4.656613e-10f - 3.1415927f;
    position.x = entity->model->position[0] + cosf(angle) * distance;
    position.y = entity->model->position[1] - 20.0f;
    position.z = entity->model->position[2] + sinf(angle) * distance;
    rotation.x = 0.0f;
    rotation.y = rand() * 6.2831855f * 4.656613e-10f - 3.1415927f;
    rotation.z = 0.0f;
    return func_001CB0D0(&position, &rotation, entity->group_id, amount, 0, TRUE, TRUE);
}
