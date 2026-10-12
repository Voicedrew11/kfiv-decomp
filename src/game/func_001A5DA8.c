#include "common.h"
#include "kf_effect.h"

extern int D_004468DC;

int func_001B4F90(void *);
void sceVu0AddVector(void *, void *, void *);
void func_00216778(void *, void *);
int func_001C0E40(f32, void *);

/* Scratchpad words shared with the other effect kinds. */
#define SCRATCHPAD_FLOAT_04 (*(float *)0x70000004)
#define SCRATCHPAD_WORD_18 (*(int *)0x70000018)

/* Per-frame update of a timed effect that spawns its children at intervals. */
void func_001A5DA8(EffectActor *effect) {
    TurningMotion *motion;
    Vector4 point;
    int hits;
    int spawn_count;
    int spawn_id;

    motion = (TurningMotion *)&effect->motion;
    func_001AEC48(effect);
    func_001AED58(effect);
    func_001AF0B0(effect);
    sceVu0AddVector(&effect->model->position, &effect->model->position, &motion->velocity);
    if ((s8)motion->unk25 != 0) { // match: retail loads unk25 as a signed byte
        func_001A2A18(effect);
    } else {
        motion->unk22 -= 1;
        hits = func_001B4F90(effect);
        if (hits != 0) {
            if ((hits & 2) && effect->def->unkF2 != 0) {
                func_001A1E28(effect->def->end_effect, &effect->model->position, &effect->model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
                func_001A1E28(0x33, &point, &effect->model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
                motion->unk25 = 1;
            } else if ((hits & 8) && motion->unk22 < 0) {
                func_00216778(&point, &effect->model->position);
                point.y -= 100.0f;
                point.y = (func_001C0E40(200.0f, &point) == 1) ? SCRATCHPAD_FLOAT_04 : effect->model->position.y;
                if (SCRATCHPAD_WORD_18 == 5) {
                    if (3.0f < effect->model->unkE8) {
                        spawn_id = 0x19;
                        D_004468DC = 0;
                    } else {
                        spawn_id = 0x16;
                    }
                    func_001A1E28(spawn_id, &point, &effect->model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
                    spawn_count = D_004468DC + 1;
                    D_004468DC = spawn_count;
                    if (spawn_count & 1) {
                        func_001A1E28(0x122, &point, &effect->model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
                    }
                } else {
                    func_001A1E28(effect->def->end_effect, &point, &effect->model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
                    D_004468DC += 1;
                    if (3.0f < effect->model->unkE8) {
                        D_004468DC = 0;
                    }
                    if ((s8)motion->unk24 == 0 || (D_004468DC & 1) == 0) {
                        func_001A1E28(0x33, &point, &effect->model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
                        motion->unk24 = 1;
                    }
                }
                motion->unk22 = effect->def->radius_half;
            }
        }
    }
    effect->frame += 1;
}
