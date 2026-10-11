#include "kf_entity.h"

float func_001954A8(float *, float *);
float func_001957B8(float, float);

/* Returns 0 with no target, 1 if the turn to it is within the limit, else 0x101/0x201 by turn direction. */
int func_00172BB8(Entity *entity) {
    float angle;
    float limit;
    int result;

    if (entity->target == NULL) {
        return 0;
    }
    result = 1;
    angle = func_001954A8(&ENTITY_MODEL(entity)->position.x, entity->target);
    angle = func_001957B8(ENTITY_MODEL(entity)->rotation.y, angle);
    limit = ENTITY_CONTROLLER(entity)->turn_limit;
    if (0.0f <= angle ? limit < angle : limit < -angle) {
        result = 0x101;
        if (0.0f <= angle) {
            result = 0x201;
        }
    }
    return result;
}
