#include "kf_entity.h"

float func_001957B8(float, float);
void func_00195898(char *, u8);
int EntityAi_StartAction(Entity *, void (*)(Entity *), int);
void func_00184550(Entity *);

void func_00184240(Entity *entity, s32 run_now) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 angle;
    f32 limit;

    angle = func_001957B8(entity->model->unkD0[1], entity->spawn_info->unk14);
    limit = (f32)ENTITY_CONTROLLER(entity)->unk438 * 0.017453292f;
    if (0.0f <= angle ? limit < angle : limit < -angle) {
        EntityAi_StartAction(entity, func_00184550, run_now);
    } else {
        unkE0->unk68 = 0;
        ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
        func_00195898((char *)entity, 0);
        EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, run_now);
    }
}
