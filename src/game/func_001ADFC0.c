#include "kf_effect.h"

/* Counts down, and every reload_frames frames spawns an effect once frame reaches keyframe_end. */
void func_001ADFC0(EffectActor *effect) {
    RepeaterMotion *motion;
    EffectDef *def;
    u16 reload_frames;
    int countdown;

    func_001AEC48(effect);
    motion = (RepeaterMotion *)&effect->motion;
    func_001AED58(effect);
    func_001AF0B0(effect);
    countdown = motion->countdown - 1;
    motion->countdown = countdown;
    if (countdown <= 0) {
        def = effect->def;
        reload_frames = motion->reload_frames;
        effect->frame += 1;
        motion->countdown = reload_frames;
        if (effect->frame >= def->keyframe_end) {
            func_001A1E28(def->radius_half, &effect->model->position, &effect->model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
            effect->frame = 0;
        }
    }
}
