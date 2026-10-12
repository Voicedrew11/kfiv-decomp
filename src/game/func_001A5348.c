#include "common.h"
#include "kf_effect.h"
#include "kf_collision.h"

int func_001B4F90(void *);
int func_001C0E40(f32, void *);
void sceVu0AddVector(void *, void *, void *);
void func_001AF5B0(void *, void *);
void func_00216778(void *, void *);

/* Falls under gravity (no faster than 45 a frame) until its hit check reports something. On a
   surface of kind 3 or 5 it ends in effect 0x14 (and 0x16 first on kind 5); otherwise a hit
   with bit 8 ends in the end effect if there is ground within reach, any other in radius_half. */
void func_001A5348(EffectActor *effect) {
    Vector4 unused[5];
    Vector4 position;
    EffectMotion *motion;
    int hit;

    func_001AEC48(effect);
    motion = &effect->motion;
    func_001AED58(effect);
    func_001AF0B0(effect);
    if (effect->frame < effect->def->max_frames || effect->def->max_frames == 0) {
        motion->velocity.y += effect->def->gravity;
        if (motion->velocity.y > 45.0f) {
            motion->velocity.y = 45.0f;
        }
        sceVu0AddVector(&effect->model->position, &effect->model->position, motion);
        func_001AF5B0(&effect->model->rotation, motion);
    }
    hit = func_001B4F90(effect);
    if (hit & 8) {
        if (COLLISION_RESULT->unk18 == 3 || COLLISION_RESULT->unk18 == 5) {
            if (COLLISION_RESULT->unk18 == 5) {
                func_001A1E28(0x16, &effect->model->position, &effect->model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
            }
            func_001A1E28(0x14, &effect->model->position, &effect->model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
        } else {
            func_00216778(&position, &effect->model->position);
            position.y -= effect->def->unkDC;
            if (func_001C0E40(effect->def->unkDC + effect->def->unkDC, &position) == 1) {
                effect->model->position.y = COLLISION_RESULT->point[1];
                func_001A1E28(effect->def->end_effect, &effect->model->position, &effect->model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
            }
        }
        func_001A2A18(effect);
    } else if (hit != 0) {
        if (COLLISION_RESULT->unk18 == 3 || COLLISION_RESULT->unk18 == 5) {
            if (COLLISION_RESULT->unk18 == 5) {
                func_001A1E28(0x16, &effect->model->position, &effect->model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
            }
            func_001A1E28(0x14, &effect->model->position, &effect->model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
        } else {
            func_001A1E28(effect->def->radius_half, &effect->model->position, &effect->model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
        }
        func_001A2A18(effect);
    }
    effect->frame += 1;
}
