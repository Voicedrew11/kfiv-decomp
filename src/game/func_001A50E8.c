#include "common.h"
#include "kf_effect.h"
#include "kf_entity.h"

float func_001B0178(void *, void *);
void KF_Vector_Subtract(void *, void *, void *);
void func_00216778(void *, void *);
int func_001C0E40(f32, void *);
void sceVu0AddVector(void *, void *, void *);

/* Lands the effect next to its target entity, then spawns the end effect once it is close */
void func_001A50E8(EffectActor *actor) {
    LandingMotion *motion;
    Entity *target;
    Vector4 offset;

    motion = (LandingMotion *)&actor->motion;
    if (actor->frame < actor->def->max_frames || actor->def->max_frames == 0) {
        target = (Entity *)actor->unk6C;
        if (target != NULL) {
            if (((int)(target->flags >> 2) & 3) == 0) { // match: the int cast gives retail's dsll/dsra32
                KF_Vector_Subtract(&offset, &target->model->position, &actor->model->position);
                func_001AFC88(&motion->velocity, &offset, actor->def->speed, actor->def->unkDC);
            } else {
                actor->unk6C = 0;
            }
        }
        actor->model->position.y -= actor->def->unkE0 * 0.5f;
        motion->velocity.w = 0;
        sceVu0AddVector(&actor->model->position, &actor->model->position, &motion->velocity);
        if (func_001C0E40(actor->def->unkE0, &actor->model->position) == 0) {
            actor->frame = actor->def->keyframe_end;
            actor->model->position.y += actor->def->unkE0 * 0.5f;
        } else {
            actor->model->position.y = *(float *)0x70000004;
        }
    }
    if (motion->state == 1) {
        func_001A2A18(actor);
    } else if (actor->frame == 0) {
        func_00216778(&offset, &actor->model->position);
        if (actor->unk6C != 0) {
            target = (Entity *)actor->unk6C;
            if (func_001B0178(&offset, &target->model->position) < 100.0f) {
                func_001A1E28(actor->def->end_effect, &actor->model->position, &actor->model->rotation, actor->owner_entity, actor->effect_param, actor->owner);
                motion->state = 1;
            }
        }
    }
    actor->frame += 1;
}
