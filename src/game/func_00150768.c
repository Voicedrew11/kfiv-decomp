#include "kf_entity.h"

void func_00190A30(char *);
void func_00195898(char *, u8);
void func_0019A310(char *, void (*)(void), int);
extern void func_00181378(void);
extern void func_00150818(void);
extern void func_00150A98(void);

void func_00150768(Entity *entity) {
    u64 flags;
    u64 kind;

    func_00190A30((char *)entity);
    entity->target = NULL;
    entity->flags &= ~0x400;
    entity->flags &= ~0x200;
    flags = entity->flags;
    kind = (flags >> 36) & 3;
    if (kind == 1) {
        if ((int)(flags >> 19) & 1) {
            func_0019A310((char *)entity, func_00181378, 0);
        } else {
            func_0019A310((char *)entity, func_00150818, 0);
        }
    } else if (kind == 2) {
        func_0019A310((char *)entity, func_00150A98, 0);
    }
    func_00195898((char *)entity, 2);
}
