#include "kf_effect.h"

void func_001C1430(f32, void *, void *);
void func_00216778(void *, void *);
float func_001B0178(void *, void *);

/* Drops the effect's model by a probe vector and picks the landing frame from the fall distance. */
void func_001ADCA8(EffectActor *actor, EffectSlot *spawn_slot) {
    Vector4 probe;
    LandingFrameMotion *motion;
    f32 distance;
    f32 velocity;
    f32 fallen;
    int i;

    func_001AF928(actor, spawn_slot);
    func_001AF640(actor, spawn_slot);
    motion = (LandingFrameMotion *)&actor->motion;
    probe.x = 0.0f;
    probe.y = 3000.0f;
    probe.z = 0.0f;
    motion->landing_frame = 0xFFFF;
    func_001C1430(1.0f, &actor->model->position, &probe);
    if (*(int *)0x70000010 != 0) {
        func_00216778(&actor->motion, (void *)0x70000000);
        motion->unk16 = *(u8 *)0x70000018;
        distance = func_001B0178(&actor->motion, &actor->model->position);
        velocity = 0.0f;
        fallen = 0.0f;
        for (i = 0; i < actor->def->keyframe_end; i++) {
            velocity += actor->def->gravity;
            fallen += velocity;
            if (distance < fallen) {
                motion->landing_frame = actor->def->unkF0 + i;
                break;
            }
        }
    }
}
