#include "kf_entity.h"

extern float D_00413F80[];

float func_00124DE0(void *);
void func_001243E8(char);
void func_00123AB8(void *);
void func_00124D48(void *);
void func_00124BC8(void *);
void func_001A32A8(void *);

/* Free entities that are out of range of the player. */
void KF_Entity_DespawnOutOfRange(void) {
    Entity *entity = D_004467E0;
    while (entity != 0) {
        EntitySpawnInfo *spawn_info;
        float distance;
        float height_difference;
        int out_of_range;
        Entity *despawned;

        distance = func_00124DE0(entity);
        entity->distance_to_player = distance;
        spawn_info = entity->spawn_info;
        height_difference = D_00413F80[1] - spawn_info->spawn_y;
        if (!(0.0f <= height_difference)) {
            height_difference = -height_difference;
        }
        out_of_range = 0;
        if ((float)spawn_info->despawn_range + 300.0f < distance ||
            (spawn_info->vertical_despawn_range != 0 &&
             (float)spawn_info->vertical_despawn_range + 150.0f < height_difference)) {
            out_of_range = 1;
        }
        despawned = entity;
        if (out_of_range != 0) {
            entity = entity->next;
            func_001243E8(despawned->unkD3);
            func_00123AB8(despawned);
            func_00124D48(despawned);
            func_00124BC8(despawned);
            func_001A32A8(despawned);
        } else {
            entity = entity->next;
        }
    }
}
