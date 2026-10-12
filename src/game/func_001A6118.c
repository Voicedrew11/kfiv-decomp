#include "common.h"
#include "kf_effect.h"

extern char D_00413F80[];
extern int D_004468C8;

void func_001AF298(void *, void *);
int func_001B4F90(void *);
void sceVu0AddVector(void *, void *, void *);
void KF_Vector_Scale(void *, void *, float);
int func_001C0E40(f32, void *);
char func_0019C8D0(int index, int first, int second, int third, int fourth);
double sin(double);

/* Scratchpad words shared with the other effect kinds. */
#define SCRATCHPAD_FLOAT_04 (*(float *)0x70000004)

/* Per-frame update of a swaying effect that drops and bounces. */
void func_001A6118(EffectActor *effect) {
    SwayMotion *motion;
    Vector4 offset;
    float sine;
    float angle;
    int hits;
    int result;

    motion = (SwayMotion *)&effect->motion;
    if (motion->done != 0) {
        func_001A2A18(effect);
        return;
    }
    func_001AEC48(effect);
    func_001AED58(effect);
    func_001AF0B0(effect);
    motion->velocity.y = -(motion->unk10 * 0.5f);
    motion->velocity.w = 0.0f;
    offset.y = 0.0f;
    offset.x = -motion->velocity.z;
    offset.z = effect->motion.velocity.x;
    motion->unk1C += effect->def->unkE8;
    motion->angle += motion->unk1C;
    angle = motion->unk18 * motion->angle;
    angle -= (float)(int)(angle / 6.2831855f) * 6.2831855f;
    if (3.1415927f < angle) {
        angle -= 6.2831855f;
    } else if (angle < -3.1415927f) {
        angle += 6.2831855f;
    }
    motion->angle = angle;
    sine = (float)sin((double)angle);
    KF_Vector_Scale(&offset, &offset, sine);
    sceVu0AddVector(&effect->model->position, &effect->model->position, &offset);
    sceVu0AddVector(&effect->model->position, &effect->model->position, &motion->velocity);
    if (func_001C0E40(motion->unk10, &effect->model->position) == 0) {
        effect->model->position.y += motion->unk10 * 0.5f;
        effect->frame = effect->def->keyframe_end;
    } else {
        effect->model->position.y += motion->unk10 * 0.5f;
        if (effect->model->position.y < SCRATCHPAD_FLOAT_04) {
            motion->vertical_speed += effect->def->gravity;
            effect->model->position.y += motion->vertical_speed;
        } else {
            motion->vertical_speed = 0.0f;
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
