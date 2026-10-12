#include "common.h"
#include "kf_effect.h"

void func_001AF298(void *, void *);
void func_00216778(void *, void *);
void func_001AF5B0(void *, void *);
void func_0019CE30(u8, void *);
void KF_Vector_Subtract(void *, void *, void *);
void sceVu0AddVector(void *, void *, void *);

extern char D_00413F80[];

/* Advances a chasing effect toward the player and frees it after a fixed window past its end frame. */
void func_001AC8E8(EffectActor *effect) {
    PlayerChaseMotion *motion;
    Vector4 offset;
    Vector4 player_offset;

    func_001AEC48(effect);
    motion = (PlayerChaseMotion *)&effect->motion;
    func_001AED58(effect);
    func_001AF0B0(effect);
    if (effect->frame >= motion->end_frame) {
        if (effect->frame == motion->end_frame) {
            func_001AF538(effect->def->speed, &motion->velocity, &effect->model->rotation);
            func_0019CE30((u8)effect->def->unkE0, &effect->model->position);
        }
        sceVu0AddVector(&effect->model->position, &effect->model->position, &motion->velocity);
        func_001AF298(effect, motion->unk24);
        if (effect->frame > motion->end_frame + 0x3C) {
            func_001A2A18(effect);
        }
    } else if (effect->frame < motion->player_frame) {
        func_00216778(&player_offset, D_00413F80);
        player_offset.y -= 110.0f;
        KF_Vector_Subtract(&offset, &player_offset, &effect->model->position);
        func_001AF5B0(&effect->model->rotation, &offset);
    }
    effect->frame += 1;
}
