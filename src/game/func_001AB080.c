#include "kf_effect.h"

void func_001AEC48(void *);
void func_001AED58(void *);
int func_001AF0B0(void *);
void func_001A2A18(void *);
int func_001C0E40(float, void *);
void func_00216778(void *, void *);
int func_001A1E28(int, int, int, int, u16, int);
void sceVu0AddVector(void *, void *, void *);

void func_001AB080(EffectActor *actor) {
    EffectMotion *motion;
    Vector4 hit;
    u16 max_frames;

    func_001AEC48(actor);
    motion = &actor->motion;
    func_001AED58(actor);
    func_001AF0B0(actor);
    if (motion->unk10[0] != 0) {
        func_001A2A18(actor);
    }
    max_frames = actor->def->max_frames;
    if (actor->frame < max_frames || max_frames == 0) {
        motion->velocity.y += actor->def->gravity;
        if (func_001C0E40(motion->velocity.y, &actor->model->position) == 0) {
            sceVu0AddVector(&actor->model->position, &actor->model->position, &motion->velocity);
        } else {
            func_00216778(&hit, &actor->model->position);
            hit.y = *(float *)0x70000004;
            func_001A1E28(actor->def->end_effect, (int)&hit, (int)&actor->model->rotation, actor->effect_arg, actor->effect_param, (int)actor->owner);
            motion->unk10[0] = 1;
        }
    }
    actor->frame++;
}
