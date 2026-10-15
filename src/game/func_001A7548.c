#include "common.h"
#include "kf_effect.h"

void sceVu0AddVector(Vector4 *dest, Vector4 *a, Vector4 *b);
void EffectAngle_FromVector(Vector4 *rotation, Vector4 *velocity);

void func_001A7548(EffectActor *effect) {
    SelectMotion *motion;

    EffectKeys_ApplyScale(effect);
    motion = (SelectMotion *)&effect->motion;
    EffectKeys_ApplySpin(effect);
    EffectKeys_ApplyColour(effect);
    if (effect->frame == 0) {
        sceVu0AddVector(&effect->model->position, &effect->model->position, &motion->velocity);
        EffectAngle_FromVector(&effect->model->rotation, &motion->velocity);
        if (EffectHit_Check(effect) != 0) {
            Effect_Spawn(effect->def->end_effect, &effect->model->position, &effect->model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
        } else {
            if (motion->unk24 == 0) {
                Effect_Spawn(effect->def->end_effect, &motion->unk10, &effect->model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
            } else {
                u16 effect_id;

                if (motion->unk20 > 0) {
                    effect_id = effect->def->radius_half;
                } else {
                    effect_id = effect->def->unkF0;
                }
                Effect_SpawnFromEffect(effect_id, effect);
            }
        }
    }
    effect->frame++;
}
