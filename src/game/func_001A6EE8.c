#include "common.h"
#include "kf_effect.h"

extern Entity *D_004467E0;

int func_001B0040(Entity *entity);
float func_001B0178(void *, void *);
float func_001B01B0(float *, float *);

/* Starts an effect that picks the four nearest entities above it and inside a cone, nearest first. */
void func_001A6EE8(EffectActor *effect, EffectSlot *spawn_slot) {
    PickedMotion *motion;
    Entity *entity;
    f32 distance;
    f32 height;
    int list_index;
    int slot;
    int moved;

    motion = (PickedMotion *)&effect->motion;
    func_001AF928(effect, spawn_slot);
    func_001AF640(effect, spawn_slot);
    for (slot = 0; slot < 4; slot++) {
        motion->list_index[slot] = -1;
        motion->entity[slot] = NULL;
        motion->distance[slot] = 65535.0f;
    }
    for (entity = D_004467E0, list_index = 0; entity != NULL; entity = entity->next, list_index++) {
        if (func_001B0040(entity) != 0) {
            continue;
        }
        distance = func_001B0178(&ENTITY_MODEL(entity)->position, &effect->model->position);
        if ((f32)effect->def->interval < distance) {
            continue;
        }
        height = ENTITY_MODEL(entity)->position.y - effect->model->position.y;
        if (height <= 10.0f) {
            continue;
        }
        if (func_001B01B0(&ENTITY_MODEL(entity)->position.x, &effect->model->position.x) / height > 1.0f) {
            continue;
        }
        for (slot = 0; slot < 4; slot++) {
            if (distance < motion->distance[slot]) {
                for (moved = 3; moved > slot; moved--) {
                    motion->list_index[moved] = motion->list_index[moved - 1];
                    motion->distance[moved] = motion->distance[moved - 1];
                    motion->entity[moved] = motion->entity[moved - 1];
                }
                motion->list_index[slot] = list_index;
                motion->distance[slot] = distance;
                motion->entity[slot] = entity;
                break;
            }
        }
    }
}
