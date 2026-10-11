#include "kf_entity.h"

extern int D_00413F80[];

int func_0018EAC0(void *, void *);
s16 func_00190F90(float, void *, int);
void func_001958B8(char *, void (*)(void), int);

void func_00183150(Entity *entity, int extra) {
    EntityUnkE0 *sub = &entity->unkE0;
    s64 buf;
    int result;

    if (ENTITY_CONTROLLER(entity)->flags & 0x100) {
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
    } else {
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
    }
    ENTITY_FLAG_BITS(entity)->bit10 = 0;
    ENTITY_FLAG_BITS(entity)->bit9 = 0;
    if (entity->unkA0 == 0) {
        entity->target = D_00413F80;
    }
    result = func_0018EAC0(entity, &buf);
    if (result > 0) {
        entity->unkC8 = 0x201;
    } else if (result < 0) {
        entity->unkC8 = 0x101;
    } else {
        entity->unkC8 = func_00190F90(50.0f, entity, 0);
    }
    func_001958B8((char *)entity, sub->unk18, extra);
}
