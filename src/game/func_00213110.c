#include "kf_model.h"

int func_0011E1F0(int, int);
float func_0019B838(float);
extern int D_00446770;

/* Turns the map object of the owner by one step of the motion, then counts down. */
void func_00213110(RotationMotion *motion) {
    MapObject *map_object;
    s32 frames;

    map_object = (MapObject *)func_0011E1F0(D_00446770, motion->owner_id);
    if (map_object == NULL || motion->frames_left <= 0) {
        motion->active = 0;
    } else {
        map_object->model->rotation.x = func_0019B838(map_object->model->rotation.x + motion->delta_x);
        map_object->model->rotation.y = func_0019B838(map_object->model->rotation.y + motion->delta_y);
        map_object->model->rotation.z = func_0019B838(map_object->model->rotation.z + motion->delta_z);
        frames = motion->frames_left - 1;
        motion->frames_left = frames;
        if (frames <= 0) {
            motion->active = 0;
        }
    }
}
