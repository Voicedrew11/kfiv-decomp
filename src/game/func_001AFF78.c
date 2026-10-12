#include "common.h"
#include "kf_effect.h"
#include "kf_entity.h"

int func_001B0040(void *);
float func_001B0178(void *, void *);

/* The nearest entity within the effect's range of position, or NULL. */
Entity *func_001AFF78(EffectActor *effect, Vector4 *position, Vector4 *unused_rotation) {
    Entity *entity;
    Entity *nearest = NULL;
    f32 nearest_distance = 65535.0f;
    f32 distance;
    f32 stack_reserve[8]; // match: unused, keeps the retail stack frame size

    for (entity = D_004467E0; entity != NULL; entity = entity->next) {
        if (func_001B0040(entity) != 0) {
            continue;
        }
        distance = func_001B0178(&ENTITY_MODEL(entity)->position, position);
        if ((f32)effect->def->interval < distance) {
            continue;
        }
        if (distance < nearest_distance) {
            nearest_distance = distance;
            nearest = entity;
        }
    }
    return nearest;
}
