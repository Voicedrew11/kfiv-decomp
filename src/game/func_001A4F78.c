#include "common.h"
#include "kf_effect.h"
#include "kf_entity.h"

extern char D_00413F80[];

int EffectHit_FindAimedEntity(void *, void *, void *);
void Vector_Copy(void *, void *);
int func_001C0E40(f32, void *);
void Vector_Subtract(void *, void *, void *);

void func_001A4F78(EffectActor *actor, EffectSlot *spawn_slot) {
    LandingMotion *motion;
    LandingMotion *target_motion;

    motion = (LandingMotion *)&actor->motion;
    target_motion = (LandingMotion *)&actor->target->motion;
    EffectStart_PlaceModel(actor, spawn_slot);
    EffectStart_TurnModel(actor, spawn_slot);
    if (actor->target->def->unkF2 != 0) {
        EffectAngle_ToVector(actor->def->speed, &motion->velocity, &actor->model->rotation);
        actor->unk6C = EffectHit_FindAimedEntity(actor, D_00413F80, D_00413F80 + 0x10);
        motion->unk14 = actor->target->def->unkF2;
    } else {
        Vector_Copy(motion, target_motion);
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
