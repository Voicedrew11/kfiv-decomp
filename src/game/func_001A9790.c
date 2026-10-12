#include "kf_effect.h"

extern int D_004468E0;

void func_001AF5B0(void *, void *);
void func_00216778(void *, void *);

/* Per-frame update: turns the model's heading by the global counter and wraps it into (-pi, pi]. */
void func_001A9790(EffectActor *effect, EffectSlot *spawn_slot) {
    PhaseMotion *motion;
    Vector4 heading;
    EffectDef *def;
    f32 angle;
    f32 new_y;
    int quotient;

    func_001AF928(effect, spawn_slot);
    func_001AF640(effect, spawn_slot);
    motion = (PhaseMotion *)&effect->motion;
    func_00216778(&heading, &effect->model->rotation);
    def = effect->def;
    heading.y = heading.y + (float)D_004468E0 * def->unkDC;
    angle = heading.y;
    quotient = (int)(angle / 6.28318548f);
    angle = angle - (float)quotient * 6.28318548f;
    if (3.14159274f < angle) {
        angle -= 6.28318548f;
    } else if (angle < -3.14159274f) {
        angle += 6.28318548f;
    }
    heading.y = angle;
    func_001AF538(def->speed, &motion->velocity, &heading);
    func_001AF5B0(&effect->model->rotation, motion);
    new_y = motion->velocity.y - (float)effect->def->radius_half;
    motion->unk18 = 0;
    motion->frame_stamp = D_004468E0;
    motion->velocity.y = new_y;
    motion->phase = D_004468E0 % effect->def->interval;
    D_004468E0 += 1;
}
