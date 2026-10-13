#include "common.h"
#include "kf_effect.h"

int func_001AFD08(void *, void *, void *);
void Vector_Subtract(void *, void *, void *);
double sqrt(double);

void func_001A5598(EffectActor *actor, EffectSlot *spawn_slot) {
    NearTargetMotion *motion;
    ModelInstance *target;
    float offset[3];
    double remaining;

    motion = (NearTargetMotion *)&actor->motion;
    func_001AF928(actor, spawn_slot);
    func_001AF640(actor, spawn_slot);
    actor->model->position.y -= 220.0f;
    func_001AF538(actor->def->speed, &motion->velocity, &actor->model->rotation);
    actor->unk6C = func_001AFD08(actor, &actor->model->position, &actor->model->rotation);
    motion->near_target = 0;
    if (actor->unk6C != 0) {
        target = *(ModelInstance **)actor->unk6C;
        Vector_Subtract(offset, &target->position, &actor->model->position);
        remaining = (double)(float)actor->def->interval - sqrt((double)(offset[0] * offset[0] + offset[2] * offset[2]));
        motion->fraction = remaining / (double)((float)actor->def->interval + (float)actor->def->interval);
        motion->fraction *= actor->def->unkDC;
    } else {
        motion->fraction = 0;
    }
}
