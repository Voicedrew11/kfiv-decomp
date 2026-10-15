#include "common.h"
#include "kf_effect.h"

int Sound_PlayMapAt(u8, void *);

void func_001AEBF8(EffectActor *effect, EffectSlot *spawn_slot) {
    func_001AF928(effect, spawn_slot);
    func_001AF640(effect, spawn_slot);
    Sound_PlayMapAt(*(u8 *)&effect->def->radius_half, &effect->model->position);
}
