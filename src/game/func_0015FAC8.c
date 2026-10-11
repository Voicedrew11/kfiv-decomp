#include "kf_entity.h"

void func_00190A30(char *);
void func_0019A310(char *, void (*)(void), int);
void func_00195898(char *, u8);
void func_00181378(void);
void func_00161550(Entity *);
void func_001616D8(Entity *);

void func_0015FAC8(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    func_00190A30((char *)entity);
    entity->flags &= ~0x200;
    if (((s32)(entity->flags >> 36) & 3) == 1) {
        if ((s32)(entity->flags >> 19) & 1) {
            func_0019A310((char *)entity, func_00181378, 0);
        } else {
            func_0019A310((char *)entity, (void (*)(void))func_00161550, 0);
        }
    } else if (((s32)(entity->flags >> 36) & 3) == 2) {
        func_0019A310((char *)entity, (void (*)(void))func_001616D8, 0);
    }
    func_00195898((char *)entity, 2);
    unkE0->unk68 = 0;
    unkE0->unk6A = entity->params->unk438;
    entity->unkD6 = 0x80;
}
