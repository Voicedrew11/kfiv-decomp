#include "kf_entity.h"

void func_00190A30(char *);
void func_0019A310(char *, void (*)(void), int);
void func_00195898(char *, u8);
void func_0017F408(Entity *);
void func_0017F5A8(Entity *);

void func_0017F378(Entity *entity) {
    func_00190A30((char *)entity);
    entity->target = NULL;
    ENTITY_FLAG_BITS(entity)->bit10 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
    if (ENTITY_FLAG_BITS(entity)->bits36 == 1) {
        func_0019A310((char *)entity, func_0017F408, 0);
    } else if (ENTITY_FLAG_BITS(entity)->bits36 == 2) {
        func_0019A310((char *)entity, func_0017F5A8, 0);
    }
    func_00195898((char *)entity, 2);
}
