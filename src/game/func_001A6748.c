#include "common.h"
#include "kf_effect.h"

void Vector_Copy(void *, void *);
int func_001C0E40(f32, void *);

/* Starts an effect that falls toward the ground in front of it. */
void func_001A6748(EffectActor *effect, EffectSlot *spawn_slot) {
    SpeedMotion *motion;
    Vector4 position;

    motion = (SpeedMotion *)&effect->motion;
    EffectStart_PlaceModel(effect, spawn_slot);
    EffectStart_TurnModel(effect, spawn_slot);
    EffectAngle_ToVector(effect->def->speed, &motion->velocity, &effect->model->rotation);
    motion->unk14 = 0;
    motion->speed = effect->def->unkDC;
    Vector_Copy(&position, &effect->model->position);
    position.y -= motion->speed * 0.5f;
    if (func_001C0E40(motion->speed, &position.x) == 0) {
        effect->frame = effect->def->keyframe_end;
    } else {
        effect->model->position.y = *(float *)0x70000004;
    }
    motion->flag18 = 0;
}
