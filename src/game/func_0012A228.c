#include "kf_entity.h"

void func_00190A30(char *);
void func_00195898(char *, u8);
void func_0019A310(char *, void (*)(void), int);
void func_001816A0(Entity *);

void func_0012A228(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    func_00190A30((char *)entity);
    entity->flags &= ~0x200;
    func_0019A310((char *)entity, (void (*)(void))func_001816A0, 0);
    func_00195898((char *)entity, 2);
    unkE0->unk7A = 1;
}
