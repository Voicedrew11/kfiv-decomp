#include "kf_entity.h"

void func_001243E8(int slot_index);
void func_001A32A8(Entity *);

/* Free every entity in the group from all three lists. */
void KF_Entity_FreeAllInGroup(int group_id) {
    Entity *entity;

    entity = D_004470A8;
    while (entity != NULL) {
        if (entity->group_id == group_id) {
            entity = func_00124C08(entity);
        } else {
            entity = entity->next;
        }
    }

    entity = D_004470B0;
    while (entity != NULL) {
        if (entity->group_id == group_id) {
            entity = func_00124CA0(entity);
        } else {
            entity = entity->next;
        }
    }

    entity = D_004467E0;
    while (entity != NULL) {
        if (entity->group_id == group_id) {
            func_001243E8(entity->unkD3);
            func_00123AB8(entity);
            entity = func_00124D48(entity);
            func_001A32A8(entity);
        } else {
            entity = entity->next;
        }
    }
}
