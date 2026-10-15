#include "common.h"
#include "kf_effect.h"
#include "kf_collision.h"

void func_001C1430(f32, void *, void *);

/* Starts an effect that runs along the ground: launches along the heading and settles on the ground found
   within def->unkDC below half that height; marks it ended where there is none. */
void func_001A5880(EffectActor *effect, EffectSlot *spawn_slot) {
    GroundRunMotion *motion = (GroundRunMotion *)&effect->motion;
    ModelInstance *model;
    f32 saved_y;
    f32 height;
    f32 half_height;

    EffectStart_PlaceModel(effect, spawn_slot);
    EffectStart_TurnModel(effect, spawn_slot);
    if (effect->def->unkF2 == 0) {
        EffectAngle_ToVector(effect->def->speed, &motion->velocity, &effect->model->rotation);
        height = effect->def->unkDC;
        half_height = height * 0.5f;
        motion->ended = 0;
        motion->height = height;
        motion->fall_speed = 0;
        motion->probe.x = 0;
        motion->probe.y = effect->def->unkDC;
        motion->probe.z = 0;
        motion->probe.w = 0;
        model = effect->model;
        saved_y = model->position.y;
        model->position.y -= half_height;
        func_001C1430(1.0f, &model->position, &motion->probe);
        effect->model->position.y = saved_y;
        if (COLLISION_RESULT->hit == 0) {
            motion->ended = 1;
        } else {
            effect->model->position.y = COLLISION_RESULT->point[1];
        }
        motion->hit_offset = effect->def->unkE0;
    }
}
