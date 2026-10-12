#include "kf_entity.h"

extern float D_00413F80[];

float atan2f(float, float);
float func_00195388(float *, float, float);
float func_001954A8(float *, float *);
float func_001957B8(float, float);

/* turns two model parts towards the target: pitch from the player's height, yaw from the bearing */
void func_00146AF0(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityModelPart80 *parts = (EntityModelPart80 *)entity->model->unkF8;
    s32 first = entity->params->unk436;
    s32 second = entity->params->unk438;
    f32 angle;
    f32 bearing;
    f32 limit;

    if (entity->target == NULL || entity->unkC6 != 0) {
        unkE0->unk48 = parts[first].unk50[0];
        unkE0->unk4C = parts[first].unk50[1];
        unkE0->unk50 = parts[second].unk50[0];
        unkE0->unk54 = parts[second].unk50[1];
    } else {
        angle = atan2f(D_00413F80[1] - entity->model->position[1], entity->distance_to_player);
        limit = 0.5235988f;
        if (angle > limit) {
            angle = limit;
        } else if (angle < -limit) {
            angle = -limit;
        }
        func_00195388(&unkE0->unk48, angle * 0.5f, 10.0f);
        func_00195388(&unkE0->unk50, angle * 0.5f, 10.0f);
        parts[first].unk50[0] = unkE0->unk48;
        parts[second].unk50[0] = unkE0->unk50;
        bearing = func_001954A8(entity->model->position, entity->target);
        angle = func_001957B8(entity->model->unkD0[1], bearing);
        limit = 0.6981317f;
        if (angle > limit) {
            angle = limit;
        } else if (angle < -limit) {
            angle = -limit;
        }
        func_00195388(&unkE0->unk4C, angle * 0.5f, 10.0f);
        func_00195388(&unkE0->unk54, angle * 0.5f, 10.0f);
        parts[first].unk50[1] = unkE0->unk4C;
        parts[second].unk50[1] = unkE0->unk54;
    }
}
