#include "kf_effect.h"
#include "kf_entity.h"

extern char D_00413F80[];

int func_001AFD08(void *, void *, void *);
int func_001C0E40(f32, void *);

/* Starts an effect like func_001A4128, then settles the model onto the ground half a height up. */
void func_001A4248(EffectActor *actor, EffectSlot *spawn_slot) {
    SnappedMotion *motion;
    Entity *source;
    ModelInstance *source_model;

    source = (Entity *)func_001AFD08(actor, D_00413F80, D_00413F80 + 0x10);
    motion = (SnappedMotion *)&actor->motion;
    actor->unk6C = (int)source;
    motion->unk04 = 1;
    if (actor->unk6C == 0) {
        func_001AF640(actor, spawn_slot);
        func_001AF928(actor, spawn_slot);
        motion->unk04 = 0;
    } else {
        source_model = ENTITY_MODEL((Entity *)actor->unk6C);
        actor->model->position.x = source_model->position.x;
        actor->model->position.y = source_model->position.y;
        actor->model->position.z = source_model->position.z;
        actor->model->rotation.x = 0;
        actor->model->rotation.y = 0;
        actor->model->rotation.z = 0;
    }
    actor->model->position.y -= actor->def->unkE0 * 0.5f;
    if (func_001C0E40(actor->def->unkE0, &actor->model->position) == 0) {
        motion->unk04 = 1;
        actor->frame = actor->def->keyframe_end;
        actor->model->position.y += actor->def->unkE0 * 0.5f;
    } else {
        motion->unk04 = 0;
        actor->model->position.y = *(f32 *)0x70000004;
    }
    motion->radius_half = actor->def->radius_half;
}
