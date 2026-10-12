#include "common.h"
#include "kf_effect.h"
#include "kf_player.h"

extern PlayerState D_00413F80;
extern f32 D_00431AF0[16];

void sceVu0RotMatrixY(void *, void *, float);
void sceVu0ApplyMatrix(void *, void *, void *);
void func_00216C48(void *, void *, void *);
void func_001C1430(f32, void *, void *);
int rand(void);

/* Scratchpad words the ground probe leaves its result in. */
#define SCRATCHPAD_FLOAT_04 (*(float *)0x70000004)
#define SCRATCHPAD_INT_10 (*(int *)0x70000010)

/* Starts an effect at a random spot around the player, resting on the ground below; ends it where there is none. */
void func_001ADA60(EffectActor *effect) {
    Vector4 offset;
    f32 matrix[16];
    EffectMotion *motion;

    sceVu0RotMatrixY(matrix, D_00431AF0, D_00413F80.rotation.y);
    sceVu0ApplyMatrix(&offset, matrix, effect->def->offset_base);
    motion = &effect->motion;
    func_00216C48(&effect->model->position, &D_00413F80, &offset);
    effect->model->position.x += ((rand() & 0xFFFF) * (1.0f / 32768.0f) - 1.0f) * effect->def->offset_spread[0];
    effect->model->position.z += ((rand() & 0xFFFF) * (1.0f / 32768.0f) - 1.0f) * effect->def->offset_spread[2];
    effect->model->rotation.x = ((rand() & 0xFFFF) * (1.0f / 32768.0f) - 1.0f) * 3.1415927f;
    effect->model->rotation.y = ((rand() & 0xFFFF) * (1.0f / 32768.0f) - 1.0f) * 3.1415927f;
    effect->model->rotation.z = 0.0f;
    effect->motion.velocity.x = 0.0f;
    motion->velocity.y = effect->def->speed;
    motion->velocity.z = 0.0f;
    effect->model->position.y -= 1500.0f;
    offset.y = 3000.0f;
    offset.x = 0.0f;
    offset.z = 0.0f;
    // match: argument order as (position, direction, scale) schedules the argument moves as retail does
    ((void (*)(void *, void *, f32))func_001C1430)(&effect->model->position, &offset, 1.0f);
    if (SCRATCHPAD_INT_10 != 0) {
        effect->model->position.y = SCRATCHPAD_FLOAT_04 - (f32)effect->def->radius_half;
    } else {
        func_001A2A18(effect);
    }
}
