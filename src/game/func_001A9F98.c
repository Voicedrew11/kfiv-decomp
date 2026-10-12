#include "kf_effect.h"

/* Starts a sub-effect from the frame that the definition's unkF0 and interval give. */
void func_001A9F98(EffectActor *effect, Vector4 *position, int effect_id) {
    StartFrameMotion *motion;
    EffectDef *def;
    int start_frame;

    motion = (StartFrameMotion *)&effect->motion;
    def = effect->def;
    start_frame = def->unkF0;
    if (effect->frame < def->interval) {
        start_frame += def->interval - effect->frame;
    }
    motion->unk26 = start_frame;
    effect->frame = (s8)start_frame;
    func_001A2928(effect_id, effect, position);
    motion->unk25 = 1;
}
