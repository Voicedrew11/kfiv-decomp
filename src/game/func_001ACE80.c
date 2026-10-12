#include "kf_effect.h"

void func_001AEC48(void *);
void func_001AED58(void *);
int func_001AF0B0(void *);
void func_001AF298(void *, void *);
void func_001AF538(float *, float *, float);
void func_00216778(void *, void *);
void sceVu0AddVector(void *, void *, void *);

void func_001ACE80(EffectActor *actor) {
    AttachedMotion *motion;

    func_001AEC48(actor);
    motion = (AttachedMotion *)&actor->motion;
    func_001AED58(actor);
    func_001AF0B0(actor);
    if (actor->frame < actor->def->unkF2) {
        func_00216778(&actor->model->position, actor->start_position);
        func_00216778(&actor->model->rotation, actor->start_rotation);
    } else if (actor->def->unkF2 == actor->frame) {
        func_001AF538(&motion->velocity.x, &(*motion->anchor)->rotation.x, -actor->def->speed);
    } else {
        if (actor->frame < actor->def->max_frames || actor->def->max_frames == 0) {
            sceVu0AddVector(&actor->model->position, &actor->model->position, &motion->velocity);
        }
        func_001AF298(actor, motion->unk14);
    }
    actor->frame++;
}
