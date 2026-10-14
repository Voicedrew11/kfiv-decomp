#include "kf_effect.h"

extern float D_00413F80[];

void sceVu0AddVector(void *, void *, void *);
void func_001AF298(void *, void *);
void func_001AF5B0(void *, void *);
void Vector_Subtract(void *, void *, void *);
void Vector_Normalize(void *, void *);

/* Advances the effect a frame: before its radius frame it aims a point at the model, from then on it moves the model by its velocity. */
void func_001A8468(EffectActor *actor) {
    ScaledFallMotion *motion;
    char unused[0x10];  /* unused, but retail reserves the 0x10 bytes */
    Vector4 offset;

    motion = (ScaledFallMotion *)&actor->motion;
    if (actor->frame >= actor->def->radius_half) {
        if (actor->frame == actor->def->radius_half) {
            func_001AF538(actor->def->speed, &motion->velocity, &actor->model->rotation);
        }
        sceVu0AddVector(&actor->model->position, &actor->model->position, &motion->velocity);
        func_001AF298(actor, &motion->unk1C[8]);
    } else {
        Vector_Subtract(&offset, D_00413F80, &actor->model->position);
        offset.y -= 200.0f;
        func_001AF5B0(&actor->model->rotation, &offset);
        Vector_Normalize(&motion->velocity, &offset);
    }
    actor->frame++;
}
