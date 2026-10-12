#include "common.h"
#include "kf_effect.h"
#include "kf_entity.h"

int func_001AFD08(void *, void *, void *);
void KF_Vector_Subtract(void *, void *, void *);
double sqrt(double);
extern PlayerData D_00413F80;

void func_001A8BA8(EffectActor *actor, EffectSlot *spawn_slot) {
    ChaserMotion *motion;
    Vector4 offset;
    double distance;
    double interval;
    double remaining;

    func_001AF928(actor, spawn_slot);
    func_001AF640(actor, spawn_slot);
    motion = (ChaserMotion *)&actor->motion;
    func_001AF538(actor->def->speed, &motion->velocity, &actor->model->rotation);
    actor->unk6C = func_001AFD08(actor, &D_00413F80, (char *)&D_00413F80 + 0x10);
    motion->unk14 = 0;
    if (actor->unk6C) {
        KF_Vector_Subtract(&offset, &ENTITY_MODEL((Entity *)actor->unk6C)->position, &actor->model->position);
        interval = (f32)actor->def->interval;
        distance = sqrt(offset.x * offset.x + offset.z * offset.z);
        remaining = interval - distance;
        motion->unk10 = (f32)(remaining / (double)(actor->def->interval * 2.0f));
        motion->unk10 *= actor->def->unkDC;
    } else {
        motion->unk10 = 0.0f;
    }
}
