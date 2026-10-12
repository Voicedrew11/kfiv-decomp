#include "kf_effect.h"

extern int D_004468E8;

void func_00216778(void *, void *);
void func_001AF5B0(void *, void *);

/* Spins the model's heading, then sets the actor's motion from the spin and the definition's radius. */
void func_001AB9C0(EffectActor *effect, EffectSlot *spawn_slot) {
    SpinBurstMotion *motion;
    Vector4 angles;
    f32 heading;

    motion = (SpinBurstMotion *)&effect->motion;
    func_001AF928(effect, spawn_slot);
    func_001AF640(effect, spawn_slot);
    func_00216778(&angles, &effect->model->rotation);
    angles.y += (f32)D_004468E8 * effect->def->unkDC;
    heading = angles.y;
    heading = heading - (f32)(int)(heading / 6.2831855f) * 6.2831855f;
    if (heading > 3.1415927f) {
        heading -= 6.2831855f;
    } else if (heading < -3.1415927f) {
        heading += 6.2831855f;
    }
    angles.y = heading;
    func_001AF538(effect->def->speed, &motion->velocity, &angles);
    func_001AF5B0(&effect->model->rotation, motion);
    motion->velocity.y -= effect->def->radius_half;
    motion->unk18 = 0;
    motion->start_frame = D_004468E8;
    motion->unk14 = 1.0f;
    D_004468E8 = motion->start_frame + 1;
}
