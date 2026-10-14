#include "kf_effect.h"
#include "kf_entity.h"

extern f32 D_00413F80[];

void func_0010C9E8(ModelInstance *);
char *func_0010CAB8(char *, u32);
float func_001B0178(void *, void *);
void Vector_Copy(void *, void *);

/* Copies its source entity's pose and scales itself by how far the source is from the camera. */
void func_001AA940(EffectActor *actor) {
    float distance;
    float fade;
    FadingMotion *motion;
    char *bone;

    func_001AEC48(actor);
    motion = (FadingMotion *)&actor->motion;
    func_001AED58(actor);
    func_001AF0B0(actor);
    func_0010C9E8(ENTITY_MODEL(actor->owner_entity));
    bone = func_0010CAB8((char *)actor->owner_entity->model, ENTITY_CONTROLLER(actor->owner_entity)->unkC5);
    Vector_Copy(&actor->model->position, bone);
    Vector_Copy(&actor->model->rotation, &ENTITY_MODEL(actor->owner_entity)->rotation);
    distance = func_001B0178(D_00413F80, &actor->model->position);
    if (motion->range[1] < distance) {
        fade = 1.0f;
    } else if (distance < motion->range[0]) {
        fade = 0.0f;
    } else {
        fade = (distance - motion->range[0]) / (motion->range[1] - motion->range[0]);
    }
    actor->frame++;
    actor->scale *= fade;
    if (actor->frame >= actor->def->keyframe_end) {
        actor->frame = 0;
    }
}
