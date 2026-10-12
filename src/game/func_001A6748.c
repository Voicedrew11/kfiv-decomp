#include "common.h"
#include "kf_effect.h"

void func_00216778(void *, void *);
int func_001C0E40(f32, void *);

/* Starts an effect that falls toward the ground in front of it. */
void func_001A6748(EffectActor *effect, EffectSlot *spawn_slot) {
    SpeedMotion *motion;
    Vector4 position;

    motion = (SpeedMotion *)&effect->motion;
    func_001AF640(effect, spawn_slot);
    func_001AF928(effect, spawn_slot);
    func_001AF538(effect->def->speed, &motion->velocity, &effect->model->rotation);
    motion->unk14 = 0;
    motion->speed = effect->def->unkDC;
    func_00216778(&position, &effect->model->position);
    position.y -= motion->speed * 0.5f;
    if (func_001C0E40(motion->speed, &position.x) == 0) {
        effect->frame = effect->def->keyframe_end;
    } else {
        effect->model->position.y = *(float *)0x70000004;
    }
    motion->flag18 = 0;
}
