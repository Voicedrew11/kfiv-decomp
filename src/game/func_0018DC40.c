#include "kf_entity.h"
#include "kf_entity_world.h"
#include "kf_effect.h"
char *func_0010CAB8(char *, u32);

/* Respawns the entity, sets its flags for a fresh fight, and may start a bone effect. */
void func_0018DC40(Entity *entity) {
    char *bone;

    KF_Entity_ResetToSpawn(entity);
    entity->flags = ((entity->flags | 2) & ~0xC) | 0x100;
    KF_Effect_FreeForOwner(entity);
    if (entity->params->unkC6 != 0) {
        if (entity->params->unkC6 != 0xFFFF) {
            bone = func_0010CAB8((char *)entity->model, entity->params->unkC5);
            KF_Effect_StartSpawner(entity->params->unkC6, bone, (char *)entity->model + 0xD0, entity);
        }
    }
}
