#include "kf_entity.h"

float func_00194F80(int, int);

void func_001409D0(Entity *entity) {
    Entity *other_entity = D_004467E0;
    while (other_entity != NULL) {
        if (entity->params == other_entity->params) {
            if (*(int *)&other_entity->flags & 1) {
                u64 other_flags = other_entity->flags;
                if (((int)(other_flags >> 1) & 1) && !((int)(other_flags >> 2) & 3) &&
                    other_entity != entity) {
                    EntityUnkE0 *other_unkE0 = &other_entity->unkE0;
                    if (other_unkE0->unk7A == 0 &&
                        func_00194F80((int)entity->model->position, (int)other_entity->model->position) <
                            (float)entity->params->unk438) {
                        other_unkE0->unk7A = 1;
                    }
                }
            }
        }
        other_entity = other_entity->next;
    }
}
