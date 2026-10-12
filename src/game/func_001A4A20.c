#include "common.h"
#include "kf_effect.h"
#include "kf_entity.h"

int func_001AFD08(EffectActor *effect, Vector4 *position, Vector4 *reference);
extern PlayerData D_00413F80;

// Lifts the model 220 units, starts its velocity and aims it at the player.
void func_001A4A20(EffectActor *effect, EffectSlot *spawn_slot) {
    PlayerHomingMotion *motion = (PlayerHomingMotion *)&effect->motion;
    ModelInstance *model;

    func_001AF928(effect, spawn_slot);
    func_001AF640(effect, spawn_slot);
    model = effect->model;
    model->position.y -= 220.0f;
    func_001AF538(effect->def->speed, &motion->velocity, &model->rotation);
    effect->unk6C = func_001AFD08(effect, (Vector4 *)&D_00413F80, (Vector4 *)((char *)&D_00413F80 + 0x10));
    motion->unkDC = effect->def->unkDC;
}
