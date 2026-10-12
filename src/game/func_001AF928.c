#include "common.h"
#include "kf_effect.h"

#define TWO_PI 6.2831855f
#define PI 3.1415927f

void func_00216778(void *, void *);
int rand(void);

/* Wraps an angle into -pi..pi. */
static inline f32 wrap_angle(f32 angle) {
    angle = angle - (float)(int)(angle / TWO_PI) * TWO_PI;
    if (angle > PI) {
        angle -= TWO_PI;
    } else if (angle < -PI) {
        angle += TWO_PI;
    }
    return angle;
}

/* Sets the model's rotation for a new effect: the start rotation or the slot's, plus the definition's
   base rotation with a random spread. */
void func_001AF928(EffectActor *effect, EffectSlot *spawn_slot) {
    if (EFFECT_DEF_FLAGS(effect->def) & 0x10000) {
        func_00216778(&effect->model->rotation, effect->start_rotation);
    } else {
        func_00216778(&effect->model->rotation, &spawn_slot->unk10);
    }
    if (!(EFFECT_DEF_FLAGS(effect->def) & 0x40000)) {
        effect->model->rotation.x = 0.0f;
    }
    if (EFFECT_DEF_FLAGS(effect->def) & 0x800000) {
        effect->model->rotation.z = 0.0f;
    }
    effect->model->rotation.x += effect->def->rotation_base[0] + ((rand() & 0xFFFF) * (1.0f / 32768.0f) - 1.0f) * effect->def->rotation_spread[0];
    effect->model->rotation.y += effect->def->rotation_base[1] + ((rand() & 0xFFFF) * (1.0f / 32768.0f) - 1.0f) * effect->def->rotation_spread[1];
    effect->model->rotation.z += effect->def->rotation_base[2] + ((rand() & 0xFFFF) * (1.0f / 32768.0f) - 1.0f) * effect->def->rotation_spread[2];
    effect->model->rotation.x = wrap_angle(effect->model->rotation.x);
    effect->model->rotation.y = wrap_angle(effect->model->rotation.y);
    effect->model->rotation.z = wrap_angle(effect->model->rotation.z);
}
