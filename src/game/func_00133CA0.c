#include "kf_entity.h"

void func_00190A30(char *);
void func_0019A310(char *, void (*)(void), int);
void func_00195898(char *, u8);
void func_00133D60(char *);
void func_00133E40(Entity *);
void func_00133FC8(Entity *);

void func_00133CA0(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    func_00190A30((char *)entity);
    entity->target = NULL;
    entity->flags &= ~0x200;
    if (((s32)(entity->flags >> 36) & 3) == 1) {
        if ((s32)(entity->flags >> 19) & 1) {
            func_0019A310((char *)entity, (void (*)(void))func_00133D60, 0);
        } else {
            func_0019A310((char *)entity, (void (*)(void))func_00133E40, 0);
        }
    } else if (((s32)(entity->flags >> 36) & 3) == 2) {
        func_0019A310((char *)entity, (void (*)(void))func_00133FC8, 0);
    }
    func_00195898((char *)entity, 2);
    unkE0->unk7A = 1;
}
