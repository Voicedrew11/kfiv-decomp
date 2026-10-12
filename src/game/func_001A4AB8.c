#include "common.h"
#include "kf_effect.h"
#include "kf_entity.h"

#define SCRATCH_VECTOR ((f32 *)0x70000000)

int func_001B4F90(void *);
int func_001C0E40(f32, void *);
void func_00216778(void *, void *);
void KF_Vector_Subtract(void *, void *, void *);
void sceVu0AddVector(void *, void *, void *);

/* Homes on the entity it was aimed at; on a hit it spawns an effect where it landed. */
void func_001A4AB8(EffectActor *actor) {
    ScaledFallMotion *motion;
    Vector4 vec;
    int hit;
    u16 end_effect;

    func_001AEC48(actor);
    motion = (ScaledFallMotion *)&actor->motion;
    func_001AED58(actor);
    func_001AF0B0(actor);
    if (actor->unk6C != 0) {
        if (((u32)(((Entity *)actor->unk6C)->flags >> 2) & 3) == 0) {
            KF_Vector_Subtract(&vec, &((Entity *)actor->unk6C)->model->position, &actor->model->position);
            motion->fall_speed += actor->def->unkE0;
            vec.y -= ((Entity *)actor->unk6C)->height * 0.5f;
            func_001AFC88(&motion->velocity, &vec, actor->def->speed, motion->fall_speed);
        } else {
            actor->unk6C = 0;
        }
    }
    if (actor->frame < actor->def->max_frames || actor->def->max_frames == 0) {
        sceVu0AddVector(&actor->model->position, &actor->model->position, &motion->velocity);
    }
    hit = func_001B4F90(actor);
    if (hit != 0) {
        func_001A2BC8(actor);
        if (hit & 2) {
            func_00216778(&vec, &actor->model->position);
            if (func_001C0E40(350.0f, &actor->model->position) == 1) {
                vec.y = SCRATCH_VECTOR[1];
                end_effect = actor->def->end_effect;
            } else {
                end_effect = actor->def->unkF0;
            }
            func_001A1E28(end_effect, &vec, &actor->model->rotation, actor->owner_entity, actor->effect_param, actor->owner);
        }
        func_001A2A18(actor);
    }
    actor->frame += 1;
}
