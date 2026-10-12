#include "kf_script.h"
#include "kf_entity.h"

Entity *func_001267B0(int);
float func_00194F80(void *, void *);
float func_001954A8(float *, float *);
float func_001957B8(float, float);

/* Script command: each frame, steer the entity toward the point (x, z) until it is within `distance`. */
void func_002156F0(ScriptWalkToFlatCommand *command) {
    f32 point[4];
    Entity *entity;
    f32 heading;
    f32 turn;

    entity = func_001267B0(command->target_id.value);
    if (entity != NULL) {
        point[0] = command->x.value;
        point[2] = command->z.value;
        point[1] = 0.0f;
        point[3] = 0.0f;
        if (func_00194F80(point, entity->model->position) < command->distance.value) {
            command->active = 0;
            return;
        }
        if ((entity->flags & 0xF) == 3 && entity->unk2C != 0) {
            heading = func_001954A8(entity->model->position, point);
            turn = func_001957B8(entity->model->unkD0[1], heading);
            if (ABS(turn) > entity->params->turn_limit) {
                if (turn >= 0.0f) {
                    entity->unkC8 |= 0x200;
                } else {
                    entity->unkC8 |= 0x100;
                }
            }
            entity->unkC8 |= 1;
            if (command->run.value != 0) {
                entity->unkC8 |= 0x40;
            }
            ENTITY_FLAG_BITS(entity)->bits11 = command->mode.value;
            return;
        }
    }
    command->active = 0;
}
