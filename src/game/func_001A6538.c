#include "common.h"
#include "kf_effect.h"

extern char D_00413F80[];
extern int D_004468C8;

int func_001B4F90(void *);
int func_001C0E40(f32, void *);
char func_0019C8D0(int index, int first, int second, int third, int fourth);
void func_001A6498(EffectActor *effect, int effect_id, float angle);
void func_00216C48(void *, void *, void *);

/* Scratchpad words shared with the other effect kinds. */
#define SCRATCHPAD_FLOAT_04 (*(float *)0x70000004)

/* Per-frame update of an effect that runs along the ground, spawning an effect to either side every other frame. */
void func_001A6538(EffectActor *effect) {
    f32 unused_matrix[16]; // match: retail reserves 0x60 bytes of stack it never uses
    Vector4 unused_offset;
    Vector4 unused_position;
    ProbeFlatMotion *motion;
    int hits;
    int result;

    motion = (ProbeFlatMotion *)&effect->motion;
    if ((s8)motion->on_ground != 0) {
        func_001A2A18(effect);
        return;
    }
    func_001AEC48(effect);
    func_001AED58(effect);
    func_001AF0B0(effect);
    motion->velocity.y = -(motion->radius * 0.5f);
    motion->velocity.w = 0.0f;
    func_00216C48(&effect->model->position, &effect->model->position, &motion->velocity);
    if (effect->frame & 1) {
        func_001A6498(effect, effect->def->radius_half, (f32)effect->def->unkF0);
        func_001A6498(effect, effect->def->interval, (f32)-effect->def->unkF2);
    }
    if (func_001C0E40(motion->radius, &effect->model->position) == 0) {
        effect->model->position.y += motion->radius * 0.5f;
        effect->frame = effect->def->keyframe_end;
    } else {
        effect->model->position.y += motion->radius * 0.5f;
        if (effect->model->position.y < SCRATCHPAD_FLOAT_04) {
            motion->unk14 += effect->def->gravity;
            effect->model->position.y += motion->unk14;
        } else {
            motion->unk14 = 0.0f;
            effect->model->position.y = SCRATCHPAD_FLOAT_04;
        }
    }
    effect->model->position.y -= 110.0f;
    hits = func_001B4F90(effect);
    effect->model->position.y += 110.0f;
    if (hits & 0x14) {
        func_001A2A18(effect);
        return;
    }
    if (effect->frame == 0) {
        result = func_0019C8D0(1, 10, 0, (int)D_00413F80, EFFECT_FLAG_PARTS(effect)->unk46);
        EFFECT_FLAG_PARTS(effect)->unk46 = result;
        if ((s8)result > 0) {
            D_004468C8 += 1;
        }
    }
    effect->frame += 1;
}
