#include "kf_effect.h"

/* Falls under EffectDef.gravity and spawns an effect when it lands. */
void func_001ADDD0(EffectActor *effect) {
    GravityMotion *motion;
    EffectDef *def;
    ModelInstance *model;
    u16 spawn_id;

    func_001AEC48(effect);
    motion = (GravityMotion *)&effect->motion;
    func_001AED58(effect);
    func_001AF0B0(effect);
    def = effect->def;
    if (effect->frame >= def->unkF0) {
        model = effect->model;
        motion->vertical_speed += def->gravity;
        model->position.y += motion->vertical_speed;
        if (effect->frame >= motion->landing_frame) {
            if (motion->landing_mode == 5) {
                spawn_id = def->radius_half;
            } else {
                spawn_id = def->interval;
            }
            Effect_Spawn(spawn_id, &motion->velocity, &model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
            Effect_Free(effect);
        }
    }
    effect->frame++;
}
