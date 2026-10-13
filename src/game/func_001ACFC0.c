#include "kf_effect.h"
#include "kf_game.h"
#include "kf_player.h"

extern GameStatus D_0038CC50;
extern u16 D_00413DA8[];
extern u16 D_0044693C;
extern PlayerState D_00413F80;

void func_00216778(void *, void *);

/* Holds the model still for a few frames after a hit, then ends; otherwise copies its start pose and reacts to the player. */
void func_001ACFC0(EffectActor *actor) {
    ShockMotion *motion;

    motion = (ShockMotion *)&actor->motion;
    if (motion->countdown != 0) {
        actor->model->unkE0 = 0;
        actor->model->unkE4 = 0;
        actor->model->unkE8 = 0;
        motion->countdown--;
        if (motion->countdown <= 0) {
            KF_Effect_Free(actor);
        }
        return;
    }
    func_001AEC48(actor);
    func_001AED58(actor);
    func_001AF0B0(actor);
    func_00216778(&actor->model->position, actor->start_position);
    func_00216778(&actor->model->rotation, actor->start_rotation);
    if (D_0038CC50.unk00 != 0) {
        if (!(D_0038CC50.unk30 & 1)) {
            if ((D_00413DA8[0] & D_0044693C) && D_00413F80.unk263 == 0) {
                return;
            }
            if (D_00413F80.unk298 != 0 && D_00413F80.unk263 == 0) {
                KF_Effect_SpawnFromEffect(actor->def->radius_half, actor);
                motion->scale = actor->def->speed * ((float)D_00413F80.unkBC / 5000.0f) + 1.0f;
            }
            motion->countdown = 3;
        }
    }
}
