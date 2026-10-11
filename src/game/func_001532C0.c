#include "kf_entity.h"

void func_00190A30(char *);
void func_0019A310(char *, void (*)(void), int);
void func_00195898(char *, u8);
void func_00153350(Entity *);
void func_00153570(Entity *);

void func_001532C0(Entity *entity) {
    func_00190A30((char *)entity);
    entity->target = NULL;
    ENTITY_FLAG_BITS(entity)->bit10 = 0;
    ENTITY_FLAG_BITS(entity)->bit9 = 0;
    if (ENTITY_FLAG_BITS(entity)->bits36 == 1) {
        func_0019A310((char *)entity, (void (*)(void))func_00153350, 0);
    } else if (ENTITY_FLAG_BITS(entity)->bits36 == 2) {
        func_0019A310((char *)entity, (void (*)(void))func_00153570, 0);
    }
    func_00195898((char *)entity, 2);
}
