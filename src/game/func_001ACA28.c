#include "common.h"
#include "kf_effect.h"

#define TWO_PI 6.2831855f
#define PI 3.1415927f

void func_00216778(void *, void *);
void func_00216C48(void *, void *, void *);
int rand(void);

/* Starts one link of a chain: turns it a random amount and spawns the next link a random length further on. */
void func_001ACA28(EffectActor *effect, EffectSlot *spawn_slot) {
    Vector4 step;
    Vector4 saved_position;
    Vector4 saved_rotation;
    ChainMotion *motion;
    f32 angle;

    func_001AF928(effect, spawn_slot);
    motion = (ChainMotion *)&effect->motion;
    func_001AF640(effect, spawn_slot);
    motion->length = ((rand() & 0xFFFF) * (1.0f / 32768.0f) - 1.0f) * 100.0f + 100.0f;
    if (effect->target->def->unkF2 != 0) {
        motion->links_left = effect->def->interval;
    } else {
        motion->links_left = ((ChainMotion *)&effect->target->motion)->links_left - 1;
    }
    angle = effect->model->rotation.x + ((rand() & 0xFFFF) * (1.0f / 32768.0f) - 1.0f) * effect->def->unkDC;
    angle -= (float)(int)(angle / TWO_PI) * TWO_PI;
    if (PI < angle) {
        angle -= TWO_PI;
    } else if (angle < -PI) {
        angle += TWO_PI;
    }
    effect->model->rotation.x = angle;
    angle = effect->model->rotation.y + ((rand() & 0xFFFF) * (1.0f / 32768.0f) - 1.0f) * effect->def->unkE0;
    angle -= (float)(int)(angle / TWO_PI) * TWO_PI;
    if (PI < angle) {
        angle -= TWO_PI;
    } else if (angle < -PI) {
        angle += TWO_PI;
    }
    effect->model->rotation.y = angle;
    if (motion->links_left != 0) {
        func_00216778(&saved_position, &effect->model->position);
        func_00216778(&saved_rotation, &effect->model->rotation);
        func_001AF538(motion->length, &step, &effect->model->rotation);
        func_00216C48(&effect->model->position, &effect->model->position, &step);
        func_001A2838(effect->def->radius_half, effect);
        func_00216778(&effect->model->position, &saved_position);
        func_00216778(&effect->model->rotation, &saved_rotation);
    }
}
