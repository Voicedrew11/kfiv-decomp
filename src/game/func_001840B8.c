#include "kf_entity.h"

extern PlayerData D_00413F80;
extern f32 D_00445898;
void func_00184330(Entity *);
float func_001954A8(float *, float *);
float func_001957B8(float, float);
void EntityAi_SetState(char *, u8);
int EntityAi_StartAction(Entity *, void (*)(Entity *), int);

void func_001840B8(Entity *entity, s32 run_now) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 angle;
    f32 limit;
    f32 direction;

    direction = func_001954A8(entity->model->position, D_00413F80.position);
    angle = func_001957B8(entity->model->unkD0[1], direction);
    limit = (f32)ENTITY_CONTROLLER(entity)->unk438 * D_00445898;
    if (0.0f <= angle ? limit < angle : limit < -angle) {
        EntityAi_StartAction(entity, func_00184330, run_now);
    } else {
        unkE0->unk68 = 0x1000;
        entity->unkC8 = 0x1000;
        EntityAi_SetState((char *)entity, 5);
        EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, run_now);
    }
}
