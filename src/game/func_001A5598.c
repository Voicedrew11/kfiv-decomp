#include "common.h"
#include "kf_effect.h"

int EffectHit_FindAimedEntity(void *, void *, void *);
void Vector_Subtract(void *, void *, void *);
double sqrt(double);

void func_001A5598(EffectActor *actor, EffectSlot *spawn_slot) {
    NearTargetMotion *motion;
    ModelInstance *target;
    float offset[3];
    double remaining;

    motion = (NearTargetMotion *)&actor->motion;
    EffectStart_TurnModel(actor, spawn_slot);
    EffectStart_PlaceModel(actor, spawn_slot);
    actor->model->position.y -= 220.0f;
    EffectAngle_ToVector(actor->def->speed, &motion->velocity, &actor->model->rotation);
    actor->unk6C = EffectHit_FindAimedEntity(actor, &actor->model->position, &actor->model->rotation);
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
