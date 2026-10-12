#include "common.h"
#include "kf_effect.h"
#include "kf_player.h"

extern PlayerState D_00413F80;

int func_001AFD08(EffectActor *effect, Vector4 *position, Vector4 *reference);
void func_001A8FF0(EffectActor *actor, Vector4 *target);
void func_00216778(void *, void *);
void func_00216C48(void *, void *, void *);
int func_001C0E40(f32, void *);
double sin(double);
double cos(double);

/* Scratchpad word the ground probe leaves the ground height in. */
#define SCRATCHPAD_FLOAT_04 (*(float *)0x70000004)

/* Starts a sweeping effect aimed at its target, or at the ground ahead of the player when it has none. */
void func_001A90A0(EffectActor *effect, EffectSlot *spawn_slot) {
    Vector4 aim;
    Vector4 rotated;
    SweepMotion *motion;
    f32 sine;
    f32 cosine;
    f32 distance;

    motion = (SweepMotion *)&effect->motion;
    func_001AF928(effect, spawn_slot);
    func_001AF640(effect, spawn_slot);
    motion->arrived = 0;
    motion->on_ground = 0;
    motion->hit_spawned = 0;
    effect->unk6C = func_001AFD08(effect, &D_00413F80.position, &D_00413F80.rotation);
    motion->level_step = effect->def->max_level - EFFECT_SLOT_FLAG_PARTS(spawn_slot)->level;
    if (effect->unk6C != 0) {
        func_001A8FF0(effect, &((EffectTarget *)effect->unk6C)->model->position);
        func_00216778(&aim, &((EffectTarget *)effect->unk6C)->model->position);
        aim.y -= 50.0f;
        if (func_001C0E40(100.0f, &aim) == 1) {
            motion->on_ground = 1;
        }
        motion->has_aim = 1;
    } else {
        sine = sin(-spawn_slot->unk10.y);
        cosine = cos(-spawn_slot->unk10.y);
        aim.x = 0.0f;
        aim.y = 0.0f;
        aim.w = 0.0f;
        distance = effect->def->unkF2;
        rotated.x = cosine * aim.x - sine * distance;
        rotated.z = sine * aim.x + cosine * distance;
        aim.z = rotated.z;
        aim.x = rotated.x;
        func_00216C48(&aim, &aim, &D_00413F80.position);
        aim.y -= 300.0f;
        if (func_001C0E40(2000.0f, &aim) == 1) {
            aim.y = SCRATCHPAD_FLOAT_04;
            motion->has_aim = 1;
            motion->on_ground = 1;
        } else {
            motion->has_aim = 0;
            aim.y += 300.0f;
        }
        func_001A8FF0(effect, &aim);
    }
    if (EFFECT_SLOT_FLAG_PARTS(spawn_slot)->level == 1) {
        motion->first_level = 1;
    }
}
