#include "kf_entity.h"

void *func_001267B0(int);

void func_00168130(Entity *entity) {
    Entity *other = func_001267B0(entity->unkE0.unk7B);

    if (other != NULL) {
        other->unkE0.unk7A = 1;
    }
}
