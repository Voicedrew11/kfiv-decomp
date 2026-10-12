#include "kf_effect.h"
#include "kf_entity.h"

#define SCRATCH_VECTOR ((void *)0x70000000)
#define SCRATCH_MATRIX ((int *)0x70000010)

extern char D_00413F80[];

int func_001AFD08(void *, void *, void *);
void KF_Vector_Subtract(void *, void *, void *);
void func_001C1430(f32, void *, void *);
void func_00216778(void *, void *);

/* Starts an effect that homes on an entity picked from its target's setup. */
void func_001A73B0(EffectActor *actor, EffectSlot *spawn_slot) {
    AimingMotion *target_motion = (AimingMotion *)&actor->target->motion;
    AimingMotion *motion = (AimingMotion *)&actor->motion;
    Vector4 offset;
    s32 timer;

    func_001AF928(actor, spawn_slot);
    func_001AF640(actor, spawn_slot);
    func_001AF538(actor->def->speed, &motion->velocity, &actor->model->rotation);
    if (actor->target->def->unkF2 != 0) {
        actor->unk6C = func_001AFD08(actor, D_00413F80, D_00413F80 + 0x10);
        motion->timer = actor->target->def->unkF2;
        if (func_001AF4E0(actor)) {
            func_001A1E28(actor->def->end_effect, &actor->model->position, &actor->model->rotation, actor->owner_entity, actor->effect_param, actor->owner);
            func_001A2A18(actor);
            return;
        }
    } else {
        timer = target_motion->timer - 1;
        actor->unk6C = actor->target->unk6C;
        motion->timer = timer;
    }
    if (actor->unk6C != 0) {
        KF_Vector_Subtract(&offset, &((Entity *)actor->unk6C)->model->position, &actor->model->position);
        offset.y -= ((Entity *)actor->unk6C)->height * 0.5f;
        func_001AFC88(&motion->velocity, &offset, actor->def->speed, actor->def->unkDC);
    }
    motion->flags = 0xFFFF;
    func_001C1430(1.0f, &actor->model->position, motion);
    if (*SCRATCH_MATRIX != 0) {
        func_00216778(&motion->aim, SCRATCH_VECTOR);
        motion->flags = 0;
    }
}
