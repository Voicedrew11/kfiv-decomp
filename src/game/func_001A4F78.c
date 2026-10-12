#include "common.h"
#include "kf_effect.h"
#include "kf_entity.h"

extern char D_00413F80[];

int func_001AFD08(void *, void *, void *);
void func_00216778(void *, void *);
int func_001C0E40(f32, void *);
void KF_Vector_Subtract(void *, void *, void *);

void func_001A4F78(EffectActor *actor, EffectSlot *spawn_slot) {
    LandingMotion *motion;
    LandingMotion *target_motion;

    motion = (LandingMotion *)&actor->motion;
    target_motion = (LandingMotion *)&actor->target->motion;
    func_001AF640(actor, spawn_slot);
    func_001AF928(actor, spawn_slot);
    if (actor->target->def->unkF2 != 0) {
        func_001AF538(actor->def->speed, &motion->velocity, &actor->model->rotation);
        actor->unk6C = func_001AFD08(actor, D_00413F80, D_00413F80 + 0x10);
        motion->unk14 = actor->target->def->unkF2;
    } else {
        func_00216778(motion, target_motion);
        actor->unk6C = actor->target->unk6C;
        motion->unk14 = target_motion->unk14 - 1;
        if (motion->unk14 == 0) {
            actor->frame = actor->def->keyframe_end;
        }
    }
    actor->model->position.y -= actor->def->unkE0 * 0.5f;
    if (func_001C0E40(actor->def->unkE0, &actor->model->position) == 0) {
        actor->frame = actor->def->keyframe_end;
        actor->model->position.y += actor->def->unkE0 * 0.5f;
    } else {
        actor->model->position.y = *(float *)0x70000004;
    }
    motion->state = 0;
    motion->unk10 = actor->def->radius_half;
}
