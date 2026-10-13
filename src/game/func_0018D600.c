#include "common.h"
#include "kf_effect.h"

char *func_0010CAB8(char *, u32);

/* Starts effect effect_id at point point_id of the entity's model, turned by rotation. */
void func_0018D600(char **entity, int effect_id, u32 point_id, int rotation) {
    char *point;

    point = func_0010CAB8(*entity, point_id);
    if (point != 0) {
        Effect_StartSpawner(effect_id, point, (void *)rotation, entity);
    }
}
