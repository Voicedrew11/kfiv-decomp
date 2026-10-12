#include "common.h"
#include "kf_effect.h"

void sceVu0AddVector(Vector4 *dest, Vector4 *a, Vector4 *b);
void func_001AF5B0(Vector4 *rotation, Vector4 *velocity);

void func_001A7548(EffectActor *effect) {
    SelectMotion *motion;

    func_001AEC48(effect);
    motion = (SelectMotion *)&effect->motion;
    func_001AED58(effect);
    func_001AF0B0(effect);
    if (effect->frame == 0) {
        sceVu0AddVector(&effect->model->position, &effect->model->position, &motion->velocity);
        func_001AF5B0(&effect->model->rotation, &motion->velocity);
        if (func_001AF4E0(effect) != 0) {
            func_001A1E28(effect->def->end_effect, &effect->model->position, &effect->model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
        } else {
            if (motion->unk24 == 0) {
                func_001A1E28(effect->def->end_effect, &motion->unk10, &effect->model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
            } else {
                u16 effect_id;

                if (motion->unk20 > 0) {
                    effect_id = effect->def->radius_half;
                } else {
                    effect_id = effect->def->unkF0;
                }
                func_001A2838(effect_id, effect);
            }
        }
    }
    effect->frame++;
}
