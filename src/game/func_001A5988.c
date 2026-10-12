#include "common.h"
#include "kf_effect.h"
#include "kf_collision.h"

int func_001B4F90(void *);
void func_001C1430(f32, void *, void *);
void func_001AF298(void *, void *);
void sceVu0AddVector(void *, void *, void *);

/* Runs along the ground: moves level, follows the ground up at once and down by falling, and
   between frames radius_half and interval checks for hits hit_offset below itself. On a
   surface of kind 3 or 5 and on a hit it ends, on such a surface in effect unkF0 and effect 0x123. */
void func_001A5988(EffectActor *effect) {
    Vector4 unused; /* unused, but retail reserves the bytes */
    GroundRunMotion *motion = (GroundRunMotion *)&effect->motion;

    if (motion->ended) {
        func_001A2A18(effect);
        return;
    }
    func_001AEC48(effect);
    func_001AED58(effect);
    func_001AF0B0(effect);
    if (effect->def->unkF2 == 0) {
        if (effect->frame < effect->def->max_frames || effect->def->max_frames == 0) {
            motion->velocity.w = 0;
            sceVu0AddVector(&effect->model->position, &effect->model->position, motion);
            effect->model->position.y -= motion->height * 0.5f;
            func_001C1430(1.0f, &effect->model->position, &motion->probe);
            if (COLLISION_RESULT->hit == 0) {
                effect->model->position.y += motion->height * 0.5f;
                func_001A2A18(effect);
                return;
            }
            if (COLLISION_RESULT->unk18 == 3 || COLLISION_RESULT->unk18 == 5) {
                func_001A1E28(effect->def->unkF0, &effect->model->position, &effect->model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
                func_001A1E28(0x123, &effect->model->position, &effect->model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
                func_001A2A18(effect);
                return;
            }
            effect->model->position.y += motion->height * 0.5f;
            if (effect->model->position.y < COLLISION_RESULT->point[1]) {
                motion->fall_speed += effect->def->gravity;
                effect->model->position.y += motion->fall_speed;
            } else if (COLLISION_RESULT->point[1] < effect->model->position.y - 5.0f) {
                effect->model->position.y -= 5.0f;
            } else {
                motion->fall_speed = 0;
                effect->model->position.y = COLLISION_RESULT->point[1];
            }
        }
        if (effect->frame >= effect->def->radius_half && effect->frame <= effect->def->interval) {
            effect->model->position.y -= motion->hit_offset;
            if (func_001B4F90(effect) & 0x14) {
                if (COLLISION_RESULT->unk18 == 3 || COLLISION_RESULT->unk18 == 5) {
                    func_001A1E28(effect->def->unkF0, &effect->model->position, &effect->model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
                    func_001A1E28(0x123, &effect->model->position, &effect->model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
                }
                func_001A2A18(effect);
            }
            effect->model->position.y += motion->hit_offset;
        }
    } else {
        func_001AF298(effect, &motion->ended);
    }
    effect->frame += 1;
}
