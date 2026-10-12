#include "common.h"
#include "kf_effect.h"
#include "kf_entity.h"

int func_001C1138(f32, void *);

/* Drops the model onto the entity it is tied to, and clears the motion's flag while it is in reach. */
void func_001ACD18(EffectActor *actor, EffectSlot *spawn_slot) {
    GroundedMotion *motion;
    Entity *target;

    func_001AF928(actor, spawn_slot);
    func_001AF640(actor, spawn_slot);
    motion = (GroundedMotion *)&actor->motion;
    motion->grounded = 0;
    target = actor->owner_entity;
    actor->model->position.y -= target->height;
    func_001C1138(target->height, &actor->model->position);
    if (*(int *)0x70000010 != 0) {
        actor->model->position.y = *(float *)0x70000004;
    } else {
        motion->grounded = 1;
    }
}
