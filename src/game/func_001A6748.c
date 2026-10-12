#include "kf_effect.h"

void func_001AF640(void *, int);
void func_001AF928(void *, int);
void func_001AF538(float *, float *, float);
void func_00216778(void *, void *);
int func_001C0E40(float, void *);

/* Starts an effect that rests on the ground: sets its range, and restarts at the frame the definition names if the ground is out of reach. */
void func_001A6748(EffectActor *actor, int frame) {
    LandingMotion *motion;
    Vector4 ground;

    motion = (LandingMotion *)&actor->motion;
    func_001AF640(actor, frame);
    func_001AF928(actor, frame);
    func_001AF538(&motion->velocity.x, &actor->model->rotation.x, actor->def->speed);
    motion->range = actor->def->unkDC;
    motion->unk14 = 0;
    func_00216778(&ground, &actor->model->position);
    ground.y -= motion->range * 0.5f;
    if (func_001C0E40(motion->range, &ground) == 0) {
        actor->frame = actor->def->restart_frame;
    } else {
        actor->model->position.y = *(float *)0x70000004;
    }
    motion->landed = 0;
}
