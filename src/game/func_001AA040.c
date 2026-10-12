#include "common.h"
#include "kf_effect.h"

extern void (*D_00402550[][2])();

int func_001B4F90(void *);
void func_001A9F40(EffectActor *effect, EffectSlot *spawn_slot);
void func_001A9F98(EffectActor *effect, Vector4 *position, int effect_id);
void func_001AA010(EffectActor *effect);
void sceVu0AddVector(void *, void *, void *);

/* Per-frame update of a turning effect: on a hit or at its end frame it starts a sub-effect and restarts. */
void func_001AA040(EffectActor *effect) {
    RestartMotion *motion;
    EffectDef *def;
    u16 max_frames;
    int hits;
    int effect_id;

    motion = (RestartMotion *)&effect->motion;
    motion->countdown--;
    if ((s8)motion->countdown > 0 && motion->started != 0) {
        effect->frame = (s8)motion->start_frame;
        func_001AA010(effect);
        return;
    }
    func_001AA010(effect);
    def = effect->def;
    max_frames = def->max_frames;
    if (effect->frame < max_frames || max_frames == 0) {
        sceVu0AddVector(&effect->model->position, &effect->model->position, &motion->velocity);
    }
    if (effect->frame <= effect->def->unkF0) {
        hits = func_001B4F90(effect);
        if (hits & 2) {
            effect_id = (int)effect->def->unkE0;
            if (effect_id <= 0) {
                effect_id = effect->def->end_effect;
            }
            func_001A9F98(effect, &effect->model->position, effect_id);
        } else if (hits & 4) {
            if (D_00402550[effect->def->unk100][0] == func_001A9F40 && (*(int *)0x70000018 == 3 || *(int *)0x70000018 == 5)) {
                func_001A9F98(effect, &effect->model->position, effect->def->unkF2);
            } else {
                func_001A9F98(effect, &effect->model->position, effect->def->end_effect);
            }
        } else if (effect->frame >= motion->end_frame) {
            func_001A9F98(effect, &motion->aim, motion->end_effect);
        } else if (effect->frame == effect->def->unkF0 && effect->def->radius_half != 0) {
            func_001A2838(effect->def->radius_half, effect);
        }
    }
    effect->frame++;
}
