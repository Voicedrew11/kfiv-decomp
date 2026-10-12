#include "kf_effect.h"

void func_00216778(void *, void *);
int func_001C0E40(f32, void *);

/* Follows its target and counts down, then ends at the target's frame count. */
void func_001AB188(EffectActor *actor, EffectSlot *spawn_slot) {
    FollowCountMotion *motion;
    FollowCountMotion *target_motion;

    motion = (FollowCountMotion *)&actor->motion;
    target_motion = (FollowCountMotion *)&actor->target->motion;
    func_001AF640(actor, spawn_slot);
    func_001AF928(actor, spawn_slot);
    if (actor->target->def->unkF2 != 0) {
        motion->countdown = actor->target->def->unkF2;
        func_001AF538(actor->def->speed, &motion->velocity, &actor->model->rotation);
    } else {
        func_00216778(&motion->velocity, target_motion);
        motion->countdown = target_motion->countdown - 1;
        if (motion->countdown == 0) {
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
    motion->unk11 = 0;
}
