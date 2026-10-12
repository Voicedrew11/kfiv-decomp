#include "kf_effect.h"
#include "kf_entity.h"

extern char D_00413F80[];

void func_001AF928(void *, int);
void func_001AF640(void *, int);
int func_001AFD08(void *, void *, void *);

/* Starts an effect: sits on the entity func_001AFD08 picks, or runs the default start when there is none. */
void func_001A4128(EffectActor *actor, int frame) {
    SnappedMotion *motion;
    Entity *source;
    ModelInstance *source_model;

    source = (Entity *)func_001AFD08(actor, D_00413F80, D_00413F80 + 0x10);
    motion = (SnappedMotion *)&actor->motion;
    actor->unk6C = (int)source;
    if (source == NULL) {
        func_001AF640(actor, frame);
        func_001AF928(actor, frame);
    } else {
        source_model = ENTITY_MODEL(source);
        actor->model->position.x = source_model->position.x;
        actor->model->position.y = source_model->position.y - source->height * 0.5f;
        actor->model->position.z = source_model->position.z;
        actor->model->rotation.x = 0;
        actor->model->rotation.y = 0;
        actor->model->rotation.z = 0;
    }
    motion->radius_half = actor->def->radius_half;
    motion->unk04 = 0;
}
