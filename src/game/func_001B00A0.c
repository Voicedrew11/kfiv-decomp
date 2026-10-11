#include "kf_entity.h"

extern Entity *D_004467E0;

float func_001B0178(void *, void *);
void func_00216778(float *, void *);

/* Nonzero if the point is within radius of any live entity centre. */
int func_001B00A0(float radius, void *point) {
    float position[4] __attribute__((aligned(16)));
    Entity *entity;

    for (entity = D_004467E0; entity != NULL; entity = entity->next) {
        if (((int)(entity->flags >> 1) & 1) == 0) continue;
        if (((int)(entity->flags >> 2) & 3) > 0) continue;
        func_00216778(position, &ENTITY_MODEL(entity)->position);
        position[1] -= entity->height * 0.5f;
        if (func_001B0178(point, position) <= radius + entity->radius) return TRUE;
    }
    return FALSE;
}
