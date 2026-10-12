#include "kf_entity.h"

float func_00195388(float *, float, float);

void func_001374E0(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityModelPart80 *part;
    f32 angle;
    f32 limit;
    s32 i;

    limit = 0.34906584f;
    if (entity->unkC8 & 0x200) {
        angle = limit;
    } else if (entity->unkC8 & 0x100) {
        angle = -limit;
    } else {
        angle = 0.0f;
    }
    limit = 0.34906584f;
    if (angle > limit) {
        angle = limit;
    } else if (angle < -limit) {
        angle = -limit;
    }
    func_00195388(&unkE0->unk48, angle, 10.0f);
    part = (EntityModelPart80 *)entity->model->unkF8;
    for (i = 0; i < ENTITY_MODEL(entity)->part_count; i++) {
        if (i == 7 || i == 12) {
            part->unk50[1] = unkE0->unk48 * 0.5f;
        }
        part++;
    }
}
