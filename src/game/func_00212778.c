#include "kf_model.h"

void *func_001C8AF0(int);
float func_0019B838(float);

/* Turns the model of the owner by one step of the motion, then counts down. */
void func_00212778(RotationMotion *motion) {
    ModelNode *node;
    s32 frames;

    node = func_001C8AF0(motion->owner_id);
    if (node == NULL || motion->frames_left <= 0) {
        motion->active = 0;
    } else {
        node->model->rotation.x = func_0019B838(node->model->rotation.x + motion->delta_x);
        node->model->rotation.y = func_0019B838(node->model->rotation.y + motion->delta_y);
        node->model->rotation.z = func_0019B838(node->model->rotation.z + motion->delta_z);
        frames = motion->frames_left - 1;
        motion->frames_left = frames;
        if (frames == 0) {
            motion->active = 0;
        }
    }
}
