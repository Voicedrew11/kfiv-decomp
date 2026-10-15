#include "common.h"
#include "kf_effect.h"
#include "kf_entity.h"

int EffectHit_FindAimedEntity(void *, void *, void *);
void Vector_Subtract(void *, void *, void *);
double sqrt(double);
extern PlayerData D_00413F80;

void func_001A8BA8(EffectActor *actor, EffectSlot *spawn_slot) {
    ChaserMotion *motion;
    Vector4 offset;
    double distance;
    double interval;
    double remaining;

    EffectStart_TurnModel(actor, spawn_slot);
    EffectStart_PlaceModel(actor, spawn_slot);
    motion = (ChaserMotion *)&actor->motion;
    EffectAngle_ToVector(actor->def->speed, &motion->velocity, &actor->model->rotation);
    actor->unk6C = EffectHit_FindAimedEntity(actor, &D_00413F80, (char *)&D_00413F80 + 0x10);
    motion->unk14 = 0;
    if (actor->unk6C) {
        Vector_Subtract(&offset, &ENTITY_MODEL((Entity *)actor->unk6C)->position, &actor->model->position);
        interval = (f32)actor->def->interval;
        distance = sqrt(offset.x * offset.x + offset.z * offset.z);
        remaining = interval - distance;
        motion->unk10 = (f32)(remaining / (double)(actor->def->interval * 2.0f));
        motion->unk10 *= actor->def->unkDC;
    } else {
        motion->unk10 = 0.0f;
    }
}
