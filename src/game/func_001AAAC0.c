#include "common.h"
#include "kf_effect.h"
#include "kf_player.h"

extern PlayerState D_00413F80;

float func_001B0178(void *, void *);
void func_00216778(void *, void *);

/* Update: fades the effect's scale by the player's distance inside the velocity band. */
void func_001AAAC0(EffectActor *effect) {
    Vector4 *velocity;
    float distance_to_player;
    float fade;

    func_001AEC48(effect);
    velocity = &effect->motion.velocity;
    func_001AED58(effect);
    func_001AF0B0(effect);
    distance_to_player = func_001B0178(&D_00413F80, &effect->model->position);
    if (velocity->y < distance_to_player) {
        fade = 1.0f;
    } else {
        if (distance_to_player < velocity->x) {
            fade = 0.0f;
        } else {
            fade = (distance_to_player - velocity->x) / (velocity->y - velocity->x);
        }
    }
    *(float *)&effect->unk0C = *(float *)&effect->unk0C * fade;
    func_00216778(&effect->model->position, effect->start_position);
    func_00216778(&effect->model->rotation, effect->start_rotation);
    effect->frame += 1;
}
