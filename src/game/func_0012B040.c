#include "kf_entity.h"

float func_001954A8(float *, float *);
float func_001957B8(float, float);
float func_00195088(float *, float, float);

/* Bends the five neck joints (model parts 30 to 34) towards the target. Facing it: each joint
   moves to where the joint before it was, the first back to straight. Otherwise the turn is
   spread over the joints, each taking up to 15 degrees, and the joints past that straighten. */
void func_0012B040(void *entity_arg) {
    Entity *entity = entity_arg;
    EntityUnkE0Kind12ACA0 *unkE0 = (EntityUnkE0Kind12ACA0 *)&entity->unkE0;
    EntityModelPart80 *part;
    f32 previous[5];
    f32 limit = 0.2617994f;
    f32 angle;
    f32 step;
    f32 remaining;
    f32 moved;
    s32 done;
    s32 i;
    f32 *previous_joint;
    s32 part_index;

    part = (EntityModelPart80 *)entity->model->unkF8;
    if (entity->unkD1 != 0 && entity->unkD1 != 3 && entity->unkD1 != 4 && entity->unkD1 != 0xB &&
        entity->unkD1 != 0xC) {
        unkE0->unk40[2] = unkE0->unk40[3] = unkE0->unk40[4] = unkE0->unk40[5] = unkE0->unk40[6] = 0.0f;
        return;
    }
    if (entity->target != NULL) {
        angle = func_001954A8(entity->model->position, entity->target);
        angle = func_001957B8(entity->model->unkD0[1], angle);
        previous[0] = unkE0->unk40[2];
        previous[1] = unkE0->unk40[3];
        previous[2] = unkE0->unk40[4];
        previous[3] = unkE0->unk40[5];
        previous[4] = unkE0->unk40[6];
        step = entity->params->turn_limit;
        if (angle >= -0.0001f && angle <= 0.0001f) {
            if (unkE0->unk40[2] != 0.0f) {
                moved = func_00195088(&unkE0->unk40[2], 0.0f, step);
                moved = ABS(moved);
                if (moved < step) {
                    step -= moved;
                } else {
                    step = 0.0f;
                }
            }
            previous_joint = previous;
            for (i = 0; i < 4; i++, previous_joint++) {
                if (unkE0->unk40[i + 3] != *previous_joint) {
                    moved = func_00195088(&unkE0->unk40[i + 3], *previous_joint, step);
                    moved = ABS(moved);
                    if (moved < step) {
                        step -= moved;
                    } else {
                        step = 0.0f;
                    }
                }
            }
        } else {
            remaining = unkE0->unk1C;
            done = FALSE;
            for (i = 2; i < 7; i++) {
                if (!done) {
                    unkE0->unk40[i] += remaining;
                    if (unkE0->unk40[i] > limit) {
                        remaining = unkE0->unk40[i] - limit;
                        unkE0->unk40[i] = limit;
                    } else if (unkE0->unk40[i] < -limit) {
                        remaining = unkE0->unk40[i] + limit;
                        unkE0->unk40[i] = -limit;
                    } else {
                        done = TRUE;
                    }
                } else if (unkE0->unk40[i] != 0.0f) {
                    moved = func_00195088(&unkE0->unk40[i], 0.0f, step);
                    moved = ABS(moved);
                    if (moved < step) {
                        step -= moved;
                    } else {
                        step = 0.0f;
                    }
                }
            }
        }
    }
    part_index = 30;
    for (i = 0; i < 5; i++) {
        part[part_index++].unk50[1] = -unkE0->unk40[i + 2];
    }
}

