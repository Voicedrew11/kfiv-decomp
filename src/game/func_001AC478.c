#include "common.h"
#include "kf_effect.h"
#include "kf_entity.h"

extern PlayerData D_00413F80;

int func_001B4F90(void *);
void Vector_Subtract(void *, void *, void *);
void sceVu0AddVector(void *, void *, void *);

/* Steers toward the player for the first interval frames, then flies straight until it hits. */
void func_001AC478(EffectActor *actor) {
    Vector4 to_player;
    Vector4 *velocity;

    func_001AEC48(actor);
    velocity = &actor->motion.velocity;
    func_001AED58(actor);
    func_001AF0B0(actor);
    if (actor->frame < actor->def->interval) {
        Vector_Subtract(&to_player, &D_00413F80, &actor->model->position);
        to_player.y -= 110.0f;
        func_001AFC88(velocity, &to_player, actor->def->speed, actor->def->unkDC);
    }
    sceVu0AddVector(&actor->model->position, &actor->model->position, velocity);
    if (func_001B4F90(actor) != 0 && actor->def->end_effect != 0xFFFF) {
        if (*(int *)0x70000018 != 5) {
            Effect_Spawn(actor->def->end_effect, &actor->model->position, &actor->model->rotation, actor->owner_entity, actor->effect_param, actor->owner);
        }
        Effect_Free(actor);
    }
    actor->frame++;
}
