#include "common.h"
#include "kf_effect.h"

void func_001AF298(void *, void *);
void sceVu0AddVector(void *, void *, void *);

void func_001A8ED0(EffectActor *actor) {
    EndCountdownMotion *motion;
    s8 spawn_flag = 0;

    func_001AEC48(actor);
    motion = (EndCountdownMotion *)&actor->motion;
    func_001AED58(actor);
    func_001AF0B0(actor);
    if (motion->countdown > 0) {
        motion->countdown--;
        if (motion->countdown <= 0) {
            KF_Effect_Free(actor);
            return;
        }
    } else {
        if (actor->frame < actor->def->max_frames || actor->def->max_frames == 0) {
            sceVu0AddVector(&actor->model->position, &actor->model->position, motion);
        }
        if (actor->frame >= motion->end_frame) {
            KF_Effect_Spawn(actor->def->end_effect, &motion->unk10, &actor->model->rotation, actor->owner_entity, actor->effect_param, actor->owner);
            spawn_flag = 1;
        } else {
            func_001AF298(actor, &spawn_flag);
        }
        if (spawn_flag) {
            KF_Effect_FreeSpawnersOf(actor);
            motion->countdown = actor->def->interval;
        }
    }
    actor->frame++;
}
