#include "kf_effect.h"
#include "kf_entity.h"

#define SCRATCH_FLAG (*(s32 *)0x70000010)

void func_001AF298(void *, void *);
void func_001AF5B0(void *, void *);
float func_001B0178(void *, void *);
void func_001C1430(f32, void *, void *);
void KF_Vector_Subtract(void *, void *, void *);
void sceVu0AddVector(void *, void *, void *);

/* Steers the effect toward the entity it chases (pulled by gravity after radius_half) and moves it. */
void func_001AD318(EffectActor *actor) {
    FollowerMotion *motion;
    Vector4 delta;
    char unused[0x10]; /* unused, but retail reserves the bytes */

    motion = (FollowerMotion *)&actor->motion;
    if (motion->unk20 != 0) {
        func_001A2A18(actor);
        return;
    }
    func_001AEC48(actor);
    func_001AED58(actor);
    func_001AF0B0(actor);
    if (actor->def->unkF0 < actor->frame) {
        actor->unk6C = 0;
    }
    if (actor->frame < actor->def->max_frames || actor->def->max_frames == 0) {
        if (actor->unk6C != 0) {
            KF_Vector_Subtract(&delta, ((Entity *)actor->unk6C)->model->position, &actor->model->position);
            delta.y -= ((Entity *)actor->unk6C)->height * 0.5f;
            func_001AFC88(&motion->velocity, &delta, motion->speed, actor->def->unkDC);
        }
        if (actor->def->radius_half < actor->frame) {
            motion->velocity.y += actor->def->gravity;
        }
        if (motion->unk20 == 0) {
            func_001C1430(1.0f, &actor->model->position, motion);
            if (SCRATCH_FLAG != 0) {
                motion->unk20 = 1;
                func_001B0178((void *)0x70000000, &actor->model->position);
                return;
            }
        }
        sceVu0AddVector(&actor->model->position, &actor->model->position, motion);
        func_001AF5B0(&actor->model->rotation, motion);
    }
    func_001AF298(actor, &motion->unk20);
    actor->frame++;
}
