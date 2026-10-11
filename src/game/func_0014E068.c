#include "kf_entity.h"

void func_00190A30(char *);
void func_00195898(char *, u8);
void func_0019A310(char *, void (*)(void), int);
extern void func_001816A0(void);

void func_0014E068(Entity *entity) {
    EntityUnkE0 *sub = &entity->unkE0;
    u64 flags;
    u64 state;

    func_00190A30((char *)entity);
    entity->flags &= ~0x200;
    flags = entity->flags;
    state = (flags >> 36) & 3;
    if (state == 2) {
        func_0019A310((char *)entity, func_001816A0, 0);
    }
    func_00195898((char *)entity, 2);
    sub->unk68 = 0;
}
