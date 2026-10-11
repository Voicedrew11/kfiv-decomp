#include "kf_entity.h"

void func_00190A30(char *);
void func_00195898(char *, u8);
void func_0019A310(char *, void (*)(void), int);
void func_00169990(void);

/* When the state field in flags is 2, resets the entity and registers func_00169990; then sets state 2. */
void func_00169918(Entity *entity) {
    if ((((u64)entity->flags >> 36) & 3) == 2) {
        func_00190A30((char *)entity);
        entity->target = NULL;
        entity->flags &= ~0x400;
        entity->flags &= ~0x200;
        func_0019A310((char *)entity, func_00169990, 0);
    }
    func_00195898((char *)entity, 2);
}
