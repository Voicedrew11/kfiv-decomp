#include "kf_entity.h"

void func_00190A30(char *);
void func_00195898(char *, u8);
void func_0019A310(char *, void (*)(void), int);
extern void func_001816A0(void);

void func_0013EBD0(Entity *entity) {
    u64 flags;

    func_00190A30((char *)entity);
    entity->target = NULL;
    entity->flags &= ~0x400;
    entity->flags &= ~0x200;
    flags = entity->flags;
    if (((flags >> 36) & 3) == 2) {
        func_0019A310((char *)entity, func_001816A0, 0);
    }
    func_00195898((char *)entity, 2);
}
