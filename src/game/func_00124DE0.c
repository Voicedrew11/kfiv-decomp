#include "kf_entity.h"

float sqrtf(float);
extern float D_00413F80[];

/* Horizontal distance from the entity's spawn point to the player. */
float func_00124DE0(Entity *entity) {
    float delta_x;
    float delta_z;

    delta_x = entity->spawn_info->spawn_x - D_00413F80[0];
    delta_z = entity->spawn_info->spawn_z - D_00413F80[2];
    return sqrtf(delta_x * delta_x + delta_z * delta_z);
}
