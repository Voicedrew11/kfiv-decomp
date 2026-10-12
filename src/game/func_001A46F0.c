#include "common.h"
#include "kf_effect.h"
#include "kf_entity.h"

int rand(void);
void func_001AF5B0(void *, void *);
Entity *func_001AFF78(EffectActor *, void *, void *);
float func_001B0970(void *, void *);
int func_001B4F90(void *);
void sceVu0AddVector(void *, void *, void *);
void KF_Vector_Subtract(void *, void *, void *);

/* Chases the entity it picked (wobbling once hit), moves, and spawns its end effects on a hit. */
void func_001A46F0(EffectActor *actor) {
    HomingMotion *motion;
    Vector4 delta;
    int hit;
    int mode;
    u16 effect_id;

    func_001AEC48(actor);
    motion = (HomingMotion *)&actor->motion;
    func_001AED58(actor);
    func_001AF0B0(actor);
    if (motion->unk24 != 0) {
        func_001A2A18(actor);
        return;
    }
    if (actor->unk6C != 0) {
        KF_Vector_Subtract(&delta, ((Entity *)actor->unk6C)->model->position, &actor->model->position);
        if (motion->unk18 == 0) {
            motion->unkDC += actor->def->unkE0;
            delta.y -= ((Entity *)actor->unk6C)->height * 0.8f;
        } else {
            if (actor->frame & 1) {
                motion->unk1C = (((rand() & 0xFFFF) * 0.000030517578f) - 1.0f) * 200.0f;
                motion->unk20 = (((rand() & 0xFFFF) * 0.000030517578f) - 1.0f) * 200.0f;
            }
            delta.y -= ((Entity *)actor->unk6C)->height;
            if (func_001B0970(&delta, motion) < 0.0f) {
                delta.x += motion->unk1C;
                delta.y -= 50.0f;
                delta.z += motion->unk20;
            }
        }
        func_001AFC88(&motion->velocity, &delta, actor->def->speed, motion->unkDC);
        func_001AF5B0(&actor->model->rotation, motion);
        if (ENTITY_FLAG_BITS((Entity *)actor->unk6C)->bits2 > 0) {
            actor->unk6C = (int)func_001AFF78(actor, &actor->model->position, &actor->model->rotation);
        }
    }
    if (actor->frame < actor->def->max_frames || actor->def->max_frames == 0) {
        sceVu0AddVector(&actor->model->position, &actor->model->position, motion);
        func_001A1E28(actor->def->unkF2, &actor->model->position, &actor->model->rotation, actor->owner_entity, actor->effect_param, actor->owner);
    }
    motion->unk14--;
    *(int *)0x70000018 = 6;
    hit = func_001B4F90(actor);
    if (hit != 0) {
        if (hit & 2) {
            if (motion->unk14 < 0) {
                motion->unkDC = actor->def->unkF0;
                func_001A1E28(actor->def->radius_half, &actor->model->position, &actor->model->rotation, actor->owner_entity, actor->effect_param, actor->owner);
                motion->unk18 = 1;
                motion->unk14 = 15;
            }
        } else {
            mode = *(int *)0x70000018;
            if (mode != 3) {
                effect_id = 0x14;
                if (mode != 5) {
                    effect_id = actor->def->end_effect;
                }
            } else {
                effect_id = 0x14;
            }
            func_001A1E28(effect_id, &actor->model->position, &actor->model->rotation, actor->owner_entity, actor->effect_param, actor->owner);
            motion->unk24 = 1;
        }
    }
    actor->frame++;
}
