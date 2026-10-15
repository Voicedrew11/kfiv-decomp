#include "kf_effect.h"
#include "kf_entity.h"

#define SCRATCH_VECTOR ((void *)0x70000000)
#define SCRATCH_MATRIX ((int *)0x70000010)

extern char D_00413F80[];

int EffectHit_FindAimedEntity(void *, void *, void *);
void Vector_Subtract(void *, void *, void *);
void func_001C1430(f32, void *, void *);
void Vector_Copy(void *, void *);

/* Starts an effect that homes on an entity picked from its target's setup. */
void func_001A73B0(EffectActor *actor, EffectSlot *spawn_slot) {
    AimingMotion *target_motion = (AimingMotion *)&actor->target->motion;
    AimingMotion *motion = (AimingMotion *)&actor->motion;
    Vector4 offset;
    s32 timer;

    EffectStart_TurnModel(actor, spawn_slot);
    EffectStart_PlaceModel(actor, spawn_slot);
    EffectAngle_ToVector(actor->def->speed, &motion->velocity, &actor->model->rotation);
    if (actor->target->def->unkF2 != 0) {
        actor->unk6C = EffectHit_FindAimedEntity(actor, D_00413F80, D_00413F80 + 0x10);
        motion->timer = actor->target->def->unkF2;
        if (EffectHit_Check(actor)) {
            Effect_Spawn(actor->def->end_effect, &actor->model->position, &actor->model->rotation, actor->owner_entity, actor->effect_param, actor->owner);
            Effect_Free(actor);
            return;
        }
    } else {
        timer = target_motion->timer - 1;
        actor->unk6C = actor->target->unk6C;
        motion->timer = timer;
    }
    if (actor->unk6C != 0) {
        Vector_Subtract(&offset, &((Entity *)actor->unk6C)->model->position, &actor->model->position);
        offset.y -= ((Entity *)actor->unk6C)->height * 0.5f;
        EffectSteer_Toward(&motion->velocity, &offset, actor->def->speed, actor->def->unkDC);
    }
    motion->flags = 0xFFFF;
    func_001C1430(1.0f, &actor->model->position, motion);
    if (*SCRATCH_MATRIX != 0) {
        Vector_Copy(&motion->aim, SCRATCH_VECTOR);
        motion->flags = 0;
    }
}
