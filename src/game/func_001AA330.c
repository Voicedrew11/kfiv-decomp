#include "kf_effect.h"

void func_001AF928(void *, int);
void func_001AF640(void *, int);
void func_001AF538(float *, float *, float);
s16 func_001B0208(float, void *, void *, void *);

/* Per-frame update for an actor: sets the sub-state at +0x10 from its definition at +0x54. */
void func_001AA330(EffectActor *actor, int frame) {
    TurningMotion *motion;
    EffectDef *def;
    s16 angle;

    motion = (TurningMotion *)&actor->motion;
    func_001AF928(actor, frame);
    func_001AF640(actor, frame);
    func_001AF538(&motion->velocity.x, &actor->model->rotation.x, actor->def->speed);
    angle = func_001B0208((float)actor->def->radius_half, actor, motion, motion->unk10);
    motion->unk24 = 0;
    motion->unk22 = angle;
    def = actor->def;
    motion->unk20 = def->end_effect;
    if (def->unkF2 != 0) {
        actor->flags |= (s64)0x8000 << 43;
    }
}
