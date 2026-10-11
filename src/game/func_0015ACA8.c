#include "kf_entity.h"

extern PlayerData D_00413F80;

float func_00194F80(void *, void *);

void func_0015ACA8(Entity *entity) {
    Entity *other;
    EntityUnkE0 *unkE0;

    for (other = D_004467E0; other != NULL; other = other->next) {
        if (entity->params == other->params && ((s32)other->flags & 1) && ((s32)(other->flags >> 1) & 1)
            && ((s32)(other->flags >> 2) & 3) == 0 && other != entity) {
            unkE0 = &other->unkE0;
            if (unkE0->unk68 == 0
                && func_00194F80(entity->model->position, other->model->position) < (s16)entity->params->unk434
                && func_00194F80(other->spawn_info, &D_00413F80) < other->spawn_info->unk4C) {
                unkE0->unk7A = unkE0->unk68 = 1;
            }
        }
    }
}
