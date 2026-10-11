#include "kf_entity.h"

void func_00190A30(char *);
void func_0019A310(char *, void (*)(void), int);
void func_00195898(char *, u8);
void func_00181378(void);
void func_001814B0(void);
void func_001816A0(void);

void func_00158E28(Entity *entity) {
    func_00190A30((char *)entity);
    entity->target = NULL;
    ENTITY_FLAG_BITS(entity)->bit10 = 0;
    ENTITY_FLAG_BITS(entity)->bit9 = 0;
    if (ENTITY_FLAG_BITS(entity)->bits36 == 1) {
        if (ENTITY_FLAG_BITS(entity)->bit19) {
            func_0019A310((char *)entity, func_00181378, 0);
        } else {
            func_0019A310((char *)entity, func_001814B0, 0);
        }
    } else if (ENTITY_FLAG_BITS(entity)->bits36 == 2) {
        func_0019A310((char *)entity, func_001816A0, 0);
    }
    func_00195898((char *)entity, 2);
}
