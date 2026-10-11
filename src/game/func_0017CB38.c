#include "kf_entity.h"

float func_001954A8(float *, float *);
float func_001957B8(float, float);
float func_00195388(float *, float, float);

extern float D_00413F80[];

void func_0017CB38(Entity *entity) {
    EntityUnkE0Kind1740A8 *unkE0 = (EntityUnkE0Kind1740A8 *)&entity->unkE0;
    EntityModelPart80 *parts = (EntityModelPart80 *)entity->model->unkF8;
    s32 part_index = 2;
    f32 angle;
    f32 limit;

    if (entity->target == NULL) {
        unkE0->unk48 = parts[part_index].unk50[0];
        unkE0->unk4C = parts[part_index].unk50[1];
    } else {
        angle = func_001954A8(entity->model->position, (f32 *)D_00413F80);
        angle = func_001957B8(entity->model->unkD0[1], angle);
        limit = 0.7853982f;
        if (angle > limit) {
            angle = limit;
        } else if (angle < -limit) {
            angle = -limit;
        }
        func_00195388(&unkE0->unk4C, angle, 10.0f);
        parts[part_index].unk50[1] = unkE0->unk4C;
    }
}
