#include "kf_entity.h"

float func_00194F80(int, int);

void func_001409D0(Entity *entity) {
    Entity *other_entity = D_004467E0;
    while (other_entity != 0) {
        if (entity->unk0C == other_entity->unk0C) {
            if (*(int *)&other_entity->flags & 1) {
                long other_flags = other_entity->flags;
                if (((int)(other_flags >> 1) & 1) && !((int)(other_flags >> 2) & 3) &&
                    other_entity != entity) {
                    EntityUnkE0 *other_unkE0 = &other_entity->unkE0;
                    if (other_unkE0->unk7A == 0 &&
                        func_00194F80((int)entity->model + 0xC0, (int)other_entity->model + 0xC0) <
                            (float)*(short *)(entity->unk0C + 0x438)) {
                        other_unkE0->unk7A = 1;
                    }
                }
            }
        }
        other_entity = other_entity->next;
    }
}
