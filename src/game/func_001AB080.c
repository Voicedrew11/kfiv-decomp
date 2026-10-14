#include "kf_effect.h"

int func_001C0E40(f32, void *);
void Vector_Copy(void *, void *);
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
        Effect_Free(actor);
    }
    if (actor->frame < actor->def->max_frames || actor->def->max_frames == 0) {
        motion->velocity.y += actor->def->gravity;
        if (func_001C0E40(motion->velocity.y, &actor->model->position) == 0) {
            sceVu0AddVector(&actor->model->position, &actor->model->position, &motion->velocity);
        } else {
            Vector_Copy(&spawn_position, &actor->model->position);
            spawn_position.y = *(float *)0x70000004;
            Effect_Spawn(actor->def->end_effect, &spawn_position, &actor->model->rotation, actor->owner_entity, actor->effect_param, actor->owner);
            motion->ending = 1;
        }
    }
    actor->frame += 1;
}
