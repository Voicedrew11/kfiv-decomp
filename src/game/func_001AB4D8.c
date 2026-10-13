#include "common.h"
#include "kf_effect.h"

extern Vector4 D_00413F80;

void func_001AF298(EffectActor *effect, void *unk10);
void func_001AF5B0(void *rotation, void *direction);
void Vector_Subtract(void *out, void *a, void *b);
void sceVu0AddVector(void *out, void *a, void *b);

/* Homing effect: steers its velocity toward the player and moves while frame < max_frames. */
void func_001AB4D8(EffectActor *effect) {
    Vector4 to_player;
    EffectMotion *motion = &effect->motion;
    EffectDef *def;
    u16 max_frames;

    func_001AEC48(effect);
    func_001AED58(effect);
    func_001AF0B0(effect);
    Vector_Subtract(&to_player, &D_00413F80, &effect->model->position);
    def = effect->def;
    if (effect->frame < def->radius_half) {
        to_player.y -= 220.0f;
        func_001AFC88(&motion->velocity, &to_player, def->speed, def->unkDC);
        func_001AF5B0(&effect->model->rotation, &motion->velocity);
        def = effect->def;
    }
    max_frames = def->max_frames;
    if (effect->frame < max_frames || max_frames == 0) {
        sceVu0AddVector(&effect->model->position, &effect->model->position, &motion->velocity);
    }
    func_001AF298(effect, &motion->unk10);
    effect->frame += 1;
}
