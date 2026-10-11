#include "kf_entity.h"

void func_00190A30(char *);
int func_0019A310(char *, void (*)(void), int); // match: an int result keeps $2 busy after the call
void func_00195898(char *, u8);
void func_00173E98(Entity *);
void func_001740A8(Entity *);

void func_00173E08(Entity *entity) {
    func_00190A30((char *)entity);
    entity->unk5C = 0;
    entity->flags &= ~0x400;
    entity->flags &= ~0x200;
    if (((s32)(entity->flags >> 36) & 3) == 1) {
        func_0019A310((char *)entity, (void (*)(void))func_00173E98, 0);
    } else if (((s32)(entity->flags >> 36) & 3) == 2) {
        func_0019A310((char *)entity, (void (*)(void))func_001740A8, 0);
    }
    func_00195898((char *)entity, 2);
}
