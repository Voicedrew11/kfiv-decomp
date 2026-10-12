#include "kf_effect.h"

extern char D_00413F80[];

void KF_Vector_Subtract(void *, void *, void *);
int func_001B4F90(void *);
int func_001C0E40(f32, void *);
void sceVu0AddVector(void *, void *, void *);

/* Bobs toward its target, then ends once it is grounded or a check fails. */
void func_001AB2C0(EffectActor *actor) {
    BobbingMotion *motion;
    Vector4 offset;
    int check;

    motion = (BobbingMotion *)&actor->motion;
    if (actor->frame < actor->def->max_frames || actor->def->max_frames == 0) {
        KF_Vector_Subtract(&offset, D_00413F80, &actor->model->position);
        func_001AFC88(&motion->velocity, &offset, actor->def->speed, actor->def->unkDC);
        actor->model->position.y -= actor->def->unkE0 * 0.5f;
        motion->velocity.w = 0.0f;
        sceVu0AddVector(&actor->model->position, &actor->model->position, &motion->velocity);
        if (func_001C0E40(actor->def->unkE0, &actor->model->position) == 0) {
            actor->frame = actor->def->keyframe_end;
            actor->model->position.y += actor->def->unkE0 * 0.5f;
        } else {
            actor->model->position.y = *(float *)0x70000004;
        }
    }
    if (motion->ending == 1) {
        func_001A2A18(actor);
    } else if (actor->frame == 0) {
        actor->model->position.y -= 100.0f;
        check = func_001B4F90(actor);
        actor->model->position.y += 100.0f;
        if (check & 0x10) {
            motion->ending = 1;
        } else if (check & 1) {
            func_001A1E28(actor->def->end_effect, &actor->model->position, &actor->model->rotation, actor->owner_entity, actor->effect_param, actor->owner);
            motion->ending = 1;
        }
    }
    actor->frame += 1;
}
