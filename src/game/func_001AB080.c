#include "kf_effect.h"

int func_001C0E40(f32, void *);
void func_00216778(void *, void *);
void sceVu0AddVector(void *, void *, void *);

/* Falls under gravity and spawns its end effect when it reaches the ground. */
void func_001AB080(EffectActor *actor) {
    EndingMotion *motion;
    Vector4 spawn_position;

    func_001AEC48(actor);
    motion = (EndingMotion *)&actor->motion;
    func_001AED58(actor);
    func_001AF0B0(actor);
    if (motion->ending != 0) {
        func_001A2A18(actor);
    }
    if (actor->frame < actor->def->max_frames || actor->def->max_frames == 0) {
        motion->velocity.y += actor->def->gravity;
        if (func_001C0E40(motion->velocity.y, &actor->model->position) == 0) {
            sceVu0AddVector(&actor->model->position, &actor->model->position, &motion->velocity);
        } else {
            func_00216778(&spawn_position, &actor->model->position);
            spawn_position.y = *(float *)0x70000004;
            func_001A1E28(actor->def->end_effect, &spawn_position, &actor->model->rotation, actor->owner_entity, actor->effect_param, actor->owner);
            motion->ending = 1;
        }
    }
    actor->frame += 1;
}
