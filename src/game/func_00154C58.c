#include "kf_entity.h"

void func_00190A30(char *);
void func_0019A310(char *, void (*)(void), int);
void func_00195898(char *, u8);
void func_001816A0(void);

void func_00154C58(Entity *entity) {
    if (ENTITY_FLAG_BITS(entity)->bits36 == 2) {
        func_00190A30((char *)entity);
        ENTITY_FLAG_BITS(entity)->bit10 = 0;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->unk5C = 0;
        func_0019A310((char *)entity, func_001816A0, 0);
    }
    func_00195898((char *)entity, 2);
}
